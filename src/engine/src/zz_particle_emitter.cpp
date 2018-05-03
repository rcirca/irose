/** 
* @file zz_particle_emitter.cpp
* @brief particle_emitter class.
* @author Jiho Choi (zho@korea.com)
* @version 1.0
* @date    30-apr-2003
*
* $Header: /engine/src/zz_particle_emitter.cpp 25    04-12-16 4:19p Choo0219 $
*/

#include "zz_tier0.h"
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include "zz_algebra.h"
#include "zz_misc.h"
#include "zz_mem.h"
#include "zz_particle_emitter.h"
#include "zz_vfs.h"
#include "zz_vfs.h"
#include "zz_profiler.h"
#include "zz_system.h"
#include "zz_renderer.h"
#include "zz_renderer_d3d.h"
#include "zz_motion.h"

ZZ_IMPLEMENT_DYNCREATE(zz_particle_emitter, zz_animatable)

const int MAX_PARTICLE_NUM = 1000;
LPDIRECT3DDEVICE9 zz_particle_emitter::m_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 zz_particle_emitter::m_vbParticles = NULL;
LPDIRECT3DVERTEXBUFFER9 zz_particle_emitter::m_vbParticles_point = NULL;
int zz_particle_emitter::vb_ref_count = 0;

zz_particle_emitter::zz_particle_emitter() : pos_channel(NULL), zz_animatable()
{
	// default particle system configuration - boring!
	Init();
}

void zz_particle_emitter::Init()
{
	m_vPos = vec3( 0.0f, 0.0f, 0.0f );

	for (std::vector<zz_particle_event_sequence *>::iterator i = m_Sequences.begin(); i != m_Sequences.end(); i++) 
	{
		zz_delete (*i);
	}	  
	m_Sequences.clear();

	SetPosRange( zz_minmax<vec3>(vec3(0.0f,0.0f,0.0f),vec3(0.0f,0.0f,0.0f)) );
	  
	SetVBSize( 600 );
	stop();

	if (!res.get_ready())
		res.make_ready();
}


zz_particle_emitter::~zz_particle_emitter()
{
	Init();
	invalidate_device_objects();
}


bool zz_particle_emitter::load (zz_vfs * fs, bool loadnow)
{
	Init();

	uint32 iSequenceCnt = 0;
	fs->read_uint32( iSequenceCnt );

	for( uint32 i = 0; i < iSequenceCnt; i++ )
	{
		zz_particle_event_sequence* pParticleEventSequence = zz_new zz_particle_event_sequence();
		pParticleEventSequence->Load( fs );
				
		m_Sequences.push_back( pParticleEventSequence );
	}	

	set_bvolume_type(ZZ_BV_OBB);
	reset_bvolume();
	set_collision_level(ZZ_CL_NONE);
	set_cast_shadow(false);

	if (loadnow) {
		restore_device_objects();
	}

	return true;
}

bool zz_particle_emitter::load (const char* fname, bool loadnow)
{
	zz_vfs fs;

	if( fs.open( fname, zz_vfs::ZZ_VFS_READ ) == false )
	{
		return false;
	}

	return load(&fs, loadnow);
}

bool zz_particle_emitter::restore_device_objects ()
{  
	HRESULT hr;

	m_pd3dDevice = ((zz_renderer_d3d *)(znzin->renderer))->get_device();

	zz_render_state * state = znzin->get_rs();

	DWORD usage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;

	if (!state->use_hw_vertex_processing_support) {
		usage |= D3DUSAGE_SOFTWAREPROCESSING;
	}

	// create vertex buffer
	if (m_vbParticles_point == NULL) {
		if(FAILED(hr = m_pd3dDevice->CreateVertexBuffer(MAX_PARTICLE_NUM * sizeof( VERTEX_PARTICLE_POINT ),
			usage | D3DUSAGE_POINTS, 
			D3DFVF_PARTICLE_POINT, D3DPOOL_DEFAULT, &m_vbParticles_point, NULL ))) 
		{
			return false;
		}
	}
	if (m_vbParticles == NULL) {
		if(FAILED(hr = m_pd3dDevice->CreateVertexBuffer(MAX_PARTICLE_NUM * 6 * sizeof( VERTEX_PARTICLE ),
			usage, 
			D3DFVF_PARTICLE, D3DPOOL_DEFAULT, &m_vbParticles, NULL ))) 
		{
			return false;
		}
	}

	// restore device objects for each event sequence
	for (std::vector<zz_particle_event_sequence *>::iterator i = m_Sequences.begin(); i != m_Sequences.end(); i++) 
	{
		(*i)->RestoreDeviceObjects( m_pd3dDevice );
	}

	// add ref
	vb_ref_count++;

	//ZZ_LOG("particle_emitter: restore_device_objects(%s, %d)\n", this->get_name(), vb_ref_count);
	assert( m_vbParticles || m_vbParticles_point );

	if (!res.get_created())
		res.make_created();
	return true;
}

bool zz_particle_emitter::invalidate_device_objects ()
{
	if (!res.get_created()) return true;

	// invalidate device objects for each event sequence
	for (std::vector<zz_particle_event_sequence *>::iterator i = m_Sequences.begin(); i != m_Sequences.end(); i++) 
	{
		(*i)->InvalidateDeviceObjects();
	}

	vb_ref_count--;
	if (vb_ref_count <= 0) {
		vb_ref_count = 0;
		SAFE_RELEASE(m_vbParticles);
		SAFE_RELEASE(m_vbParticles_point);
		res.make_uncreated();
	}
	//ZZ_LOG("particle_emitter: invalidate_device_objects(%s, %d)\n", this->get_name(), vb_ref_count);
	return true;
}

void zz_particle_emitter::update_animation (bool recursive, zz_time diff_time)
{
	zz_animatable::update_animation(false, diff_time); // non-recursive
}

void zz_particle_emitter::render (bool recursive)
{
//	zz_assert(res.get_created());             //test  particel error 잠시 주석처리..
	assert(m_vbParticles || m_vbParticles_point);

	if (znzin->renderer->get_render_where() == ZZ_RW_SHADOWMAP) return;

	//ZZ_LOG("particle_emitter: render(%s)\n", this->get_name());
	if (IsRunning()) 
	{
		mat4 modelviewTM, modelview_worldTM;
		this->get_modelviewTM(modelviewTM);
		znzin->renderer->set_modelview_matrix(modelviewTM);
		for (std::vector<zz_particle_event_sequence *>::iterator i = m_Sequences.begin(); i != m_Sequences.end(); i++) 
		{
			if ((*i)->GetImplementType() == 0) {
				assert(m_vbParticles_point);
				(*i)->Render( m_vbParticles_point, m_iVBSize );
			}
			else {
				assert(m_vbParticles);
				(*i)->Render( m_vbParticles, m_iVBSize );
			}
		}
	}
}

void zz_particle_emitter::attach_motion (zz_motion * motion_to_attach)
{
	if (!motion_to_attach) return; // no motion

	zz_animatable::attach_motion(motion_to_attach);

	pos_channel = static_cast<zz_channel_position*>(motion_to_attach->channels[0]);
}

bool zz_particle_emitter::apply_motion (void)
{
	if (!motion) return false; // no motion

	zz_time time = motion_controller.get_current_time();

	int fps = motion->get_fps();
	if (pos_channel) pos_channel->get_by_time(time, fps, &this->position);
	return true;
}

void zz_particle_emitter::build_mesh_minmax (vec3& mesh_min_out, vec3& mesh_max_out)
{
	mesh_min_out = m_vPosRange.m_min;
	mesh_max_out = m_vPosRange.m_max;
}

// modified version for zz_animatable::update_time()
void zz_particle_emitter::update_time (bool recursive, zz_time diff_time)
{
	// default particle does not care about motion_controller.
	// but motion attached paricle emittor can be motion_contrelled.
	if (!motion_controller.update(diff_time)) {
		motion_controller.stop(); // not same as stop()
	}

	if (apply_motion()) { // do apply motion no relation with motion_controller
		invalidate_transform();
	}

	// like update_animation
	m_vPos = this->get_worldTM().get_position();

	vec3 world_scale;

	bool still_alive = false;
	if (IsRunning())
	{
		for (std::vector<zz_particle_event_sequence *>::iterator i = m_Sequences.begin(); i != m_Sequences.end(); i++) 
		{
			(*i)->set_world_tm(&this->get_worldTM());
			this->get_scale_world(world_scale);
			(*i)->set_scale_world(world_scale.x);
			if ((*i)->update(diff_time, m_vPos)) 
				still_alive = true; // any sequence is still alive
		}
		if (still_alive == false) { // no sequences are still alive
			// not same as stop();
			pause();
			DeleteAllParticles();
		}
	}

	zz_visible::update_time(recursive, diff_time);
}

void zz_particle_emitter::before_render ()
{
	if (!res.get_created()) {
		restore_device_objects();
	}

	if (!IsRunning()) return; // do nothing

	for (std::vector<zz_particle_event_sequence *>::iterator i = m_Sequences.begin(); i != m_Sequences.end(); i++) 
	{
		(*i)->before_render();
	}
}

