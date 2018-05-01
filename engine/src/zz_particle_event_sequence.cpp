/** 
 * @file zz_particle_event_sequence.cpp
 * @brief particle_event_sequence class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    30-apr-2003
 *
 * $Header: /engine/src/zz_particle_event_sequence.cpp 21    06-12-10 10:07p Choo0219 $
 * $History: zz_particle_event_sequence.cpp $
 * 
 * *****************  Version 21  *****************
 * User: Choo0219     Date: 06-12-10   Time: 10:07p
 * Updated in $/engine/src
 * 
 * *****************  Version 20  *****************
 * User: Choo0219     Date: 06-06-23   Time: 11:40a
 * Updated in $/engine/src
 * 
 * *****************  Version 19  *****************
 * User: Choo0219     Date: 05-12-24   Time: 10:23p
 * Updated in $/engine/src
 * 
 * *****************  Version 18  *****************
 * User: Zho          Date: 04-10-14   Time: 2:05p
 * Updated in $/engine/src
 * 
 * *****************  Version 17  *****************
 * User: Zho          Date: 04-08-31   Time: 4:46p
 * Updated in $/engine/src
 * 
 * *****************  Version 16  *****************
 * User: Zho          Date: 04-08-11   Time: 6:20p
 * Updated in $/engine/src
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 04-08-03   Time: 9:59a
 * Updated in $/engine/src
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 04-07-10   Time: 4:31p
 * Updated in $/engine/src
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 04-06-24   Time: 2:12p
 * Updated in $/engine/src
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-06-22   Time: 9:51a
 * Updated in $/engine/src
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-04-28   Time: 2:03p
 * Updated in $/engine/src
 * ocean vs. camera test more precisely
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-04-26   Time: 5:20p
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-02-11   Time: 5:06p
 * Updated in $/engine/src
 * fTimeDelta converted diff/1000 -> ZZ_TIME_TO_SEC(diff)
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-02-06   Time: 1:26p
 * Updated in $/engine/src
 * Xform coordinate system
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-01-20   Time: 3:56p
 * Updated in $/engine/src
 * Added RendererCachedInfo.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-29   Time: 1:15a
 * Updated in $/engine/src
 * "not converting time to msec" bug fixed in update()
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-24   Time: 6:53p
 * Updated in $/engine/src
 * not to delayed load texture in particle
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-11   Time: 10:38p
 * Updated in $/engine/src
 * zfunc added
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 34  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_mem.h"
#include "zz_type.h"
#include "zz_system.h"
#include "zz_profiler.h"
#include "zz_manager.h"
#include "zz_renderer.h"
#include "zz_renderer_d3d.h"
#include "zz_vfs.h"
#include "zz_texture.h"
#include "zz_camera.h"
#include "zz_log.h"
#include "zz_particle_event.h"
#include "zz_particle_event_sequence.h"

#include <algorithm>
#include <D3DX9.h>

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

using namespace std;

const DWORD BlendingModes[ 11 ] = 
{
	D3DBLEND_ZERO,
	D3DBLEND_ONE,
	D3DBLEND_SRCCOLOR,
	D3DBLEND_INVSRCCOLOR,
	D3DBLEND_SRCALPHA,
	D3DBLEND_INVSRCALPHA,
	D3DBLEND_DESTALPHA,
	D3DBLEND_INVDESTALPHA,
	D3DBLEND_DESTCOLOR,
	D3DBLEND_INVDESTCOLOR,
	D3DBLEND_SRCALPHASAT,
};


zz_particle_event_sequence::zz_particle_event_sequence()
{
	m_pd3dDevice = NULL;
	m_texParticle = NULL;
	m_WorldParticles = NULL;
	m_Particles = NULL;
	Reset();
}

zz_particle_event_sequence::~zz_particle_event_sequence()
{
	Reset();
}

zz_particle_event *zz_particle_event_sequence::EventFactory( int iEventType )
{
	zz_particle_event *pEvent = NULL;

	switch( iEventType )
	{
	case EVENT_SIZE:			pEvent = zz_new zz_particle_event_size(); break;			
	case EVENT_EVENTTIMER:		pEvent = zz_new zz_particle_event_event_timer(); break;
	case EVENT_REDCOLOR:		pEvent = zz_new zz_particle_event_red_color(); break;						
	case EVENT_GREENCOLOR:		pEvent = zz_new zz_particle_event_green_color(); break;	
	case EVENT_BLUECOLOR:		pEvent = zz_new zz_particle_event_blue_color(); break;	
	case EVENT_ALPHA:			pEvent = zz_new zz_particle_event_alpha(); break;
	case EVENT_COLOR:			pEvent = zz_new zz_particle_event_color(); break;
	case EVENT_VELOCITYX:		pEvent = zz_new zz_particle_event_velocityx(); break;	
	case EVENT_VELOCITYY:		pEvent = zz_new zz_particle_event_velocityy(); break;
	case EVENT_VELOCITYZ:		pEvent = zz_new zz_particle_event_velocityz(); break;
	case EVENT_VELOCITY:		pEvent = zz_new zz_particle_event_velocity(); break;			
	case EVENT_TEXTURE:			pEvent = zz_new zz_particle_event_texture(); break;
	case EVENT_ROTATION:        pEvent = zz_new zz_particle_event_rotation(); break; 
	}	

	return pEvent;
}

int zz_particle_event_sequence::Reset()
{
	m_iImplementType = 0;
	m_iTextureSizeWidth = 1;
	m_iTextureSizeHeight = 1;
	
	m_iTotalParticleLives = 0;
	m_Loops = -1;

	memset( &m_vSpawnDir, 0, sizeof( m_vSpawnDir ) );

	m_iNumParticles = 100;
	m_fNumNewPartsExcess = 0.0f;
	m_vGravity = zz_minmax<vec3>(vec3(0.0f, 0.0f, 0.0f),vec3(0.0f, 0.0f, 0.0f));
	m_vEmitRadius = zz_minmax<vec3>(vec3(0.0f, 0.0f, 0.0f),vec3(0.0f, 0.0f, 0.0f));

	m_iBlendOperation = D3DBLENDOP_ADD;
	m_iUpdateCoord = ZZ_UPDATE_COORD_WORLD; // world

	for (std::vector<zz_particle_event *>::iterator i = m_Events.begin(); i != m_Events.end(); i++) 
	{
		// can't use safe zz_delete here, because it will set i = NULL.
		if (*i) zz_delete (*i);
	}
	m_Events.clear();

	ZZ_SAFE_DELETE(m_Particles);
	ZZ_SAFE_DELETE(m_WorldParticles);
	//ZZ_SAFE_RELEASE(m_texParticle);
	if (m_texParticle) {
		znzin->textures->kill(m_texParticle);
	}

	m_Particles = NULL;
	m_WorldParticles = NULL;
	m_texParticle = NULL;
	
	this->world_tm = NULL;
	this->world_scale = 1.0f;

	return 0;
}

void zz_particle_event_sequence::SortEvents(void)
{
	// sort the events by the time they occur (initial = 1st, final = last)
	// a testament to the power of STL: one line to accomplish this.  Pretty sweet, huh?
	zz_particle_event_compare_func cf;
	std::sort(m_Events.begin(), m_Events.end(), cf);

//#ifdef _DEBUG
//	// spit out the list of events, for debugging purposes.
//	OutputDebugString("\n\nSorted Events:");
//	for (std::vector<zz_particle_event *>::iterator i = m_Events.begin(); i != m_Events.end(); i++) 
//	{
//		char buf[256];
//		_snprintf(buf, sizeof(buf), "\nTime: %0.2f Type: %20s Fade: %c",
//		(*i)->GetActualTime(), typeid(*(*i)).name(), (*i)->IsFade() ? 'Y' : 'N');
//		OutputDebugString(buf);
//	}
//
//#endif
}

void zz_particle_event_sequence::CreateFadeLists()
{
	// for each event,
	for (std::vector<zz_particle_event *>::iterator i = m_Events.begin(); i != m_Events.end(); i++) 
	{
		// try to find the next fade event of the same type.
		for (std::vector<zz_particle_event *>::iterator j = i; j != m_Events.end(); j++) 
		{
			if (j != i )
			{
				if( (*j)->type == (*i)->type  )
				{
					if( (*j)->IsFade() ) 
					{
						// we've found a fade event further in the future.  make a note that
						// this event needs to be linked to this future fade event (so that we
						// can calculate the deltas later).
						(*i)->m_NextFadeEvent = (*j);
						break;
					}
				}
			}
		}
	}
}

void zz_particle_event_sequence::NailDownRandomTimes(void)
{
	for (std::vector<zz_particle_event *>::iterator i = m_Events.begin(); i != m_Events.end(); i++) 
	{
		float fTime = (*i)->GetTimeRange().get_random_num_in_range();
		(*i)->SetActualTime( fTime );
	}
}

HRESULT zz_particle_event_sequence::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pDev )
{
	m_pd3dDevice = pDev;

	SetTexture(m_strTexFilename.c_str()); // trick ourselves into loading the texture

	// restore device objects for each event in this sequence
	for (std::vector<zz_particle_event *>::iterator i = m_Events.begin(); i != m_Events.end(); i++) 
	{
		(*i)->RestoreDeviceObjects(pDev);
	}

	return S_OK;
}

HRESULT zz_particle_event_sequence::InvalidateDeviceObjects()
{
	//ZZ_SAFE_DELETE(m_Particles);
	//ZZ_SAFE_DELETE(m_WorldParticles);
	//ZZ_SAFE_RELEASE(m_texParticle);
	  
	// invalidate device objects for each event in this sequence
	for (std::vector<zz_particle_event *>::iterator i = m_Events.begin(); i != m_Events.end(); i++) 
	{
		(*i)->InvalidateDeviceObjects();
	}

	return S_OK;
}

void zz_particle_event_sequence::SetTexture(const char *strTexFilename)
{
//	m_strTexFilename = strTexFilename;
	std::string string_tex_filename(strTexFilename);
	copy(string_tex_filename.begin(), string_tex_filename.end(), m_strTexFilename.begin());
	if (m_pd3dDevice) 
	{
		//ZZ_SAFE_RELEASE(m_texParticle);
		if (m_texParticle) {
			znzin->textures->kill(m_texParticle);
		}

		m_texParticle = (zz_texture*)znzin->textures->find(strTexFilename);

		if (!m_texParticle) {
			m_texParticle = (zz_texture*)znzin->textures->spawn(strTexFilename, ZZ_RUNTIME_TYPE(zz_texture), false /* not to autoload */);
			assert(m_texParticle);
			m_texParticle->set_path(strTexFilename);
		}
	
		assert(m_texParticle);

		// add reference count
		m_texParticle->addref();
		//znzin->textures->load(m_texParticle);
		znzin->textures->flush_entrance(m_texParticle);
	}
}

void zz_particle_event_sequence::CreateNewParticle( vec3 m_vPartSysPos )
{			
	zz_particle * part = NULL;
	if (this->GetUpdateCoord() == ZZ_UPDATE_COORD_WORLD) {
		if (m_WorldParticles) {
			part = m_WorldParticles->New();
		}
	}
	else {
		if (m_Particles) {
			part = m_Particles->New();
		}
	}

	if (!part) {
		ZZ_LOG("particle_event_sequence: failed to create particle\n");
		return;
	}

	part->m_fLifetime = m_Lifetime.get_random_num_in_range();
	
	// get local_pos by m_vEmitRadius
	vec3 local_pos = m_vEmitRadius.get_random_num_in_range();

	if (GetUpdateCoord() == ZZ_UPDATE_COORD_WORLD) {
		zz_particle_world * world_part = reinterpret_cast<zz_particle_world*>(part);
		// set particle's world-rotation matrix by this->world_tm
		world_part->rot_world = world_tm->get_rot(world_part->rot_world);

		// set paritcle initial position
		mult(world_part->m_vPos, world_part->rot_world, local_pos);
		world_part->m_vPos += m_vPartSysPos; // set by world
	
		// set gravity in world coordsys
		vec3 gravity = m_vGravity.get_random_num_in_range();

		// get inverse world matrix
		mat3 inv_world = world_part->rot_world.transpose();
        // set gravity in particle's local coordsys
		mult(world_part->gravity_local, inv_world, gravity);
	}
	else {
		part->m_vPos = local_pos; // go local
	}

	// process any initial events
	std::vector<zz_particle_event *>::iterator i;
	for ( i = m_Events.begin(); i != m_Events.end() && !(*i)->GetActualTime(); i++) 
	{
		(*i)->DoItToIt(*part);
	}
	part->m_CurEvent = i;
	m_iTotalParticleLives++;
}

void zz_particle_event_sequence::RunEvents( zz_particle &part )
{
	// apply any other events to this particle
	std::vector<zz_particle_event *>::iterator i;
	for (i = part.m_CurEvent; 
		i != m_Events.end() && (*i)->GetActualTime() <= part.m_fEventTimer; i++) 
	{
		float oldeventtimer = part.m_fEventTimer;
		(*i)->DoItToIt(part);

		if (part.m_fEventTimer != oldeventtimer) 
		{
			// event timer has changed, we need to recalc m_CurEvent.
			std::vector<zz_particle_event *>::iterator RecalcIter;
			for (RecalcIter = m_Events.begin(); 
				RecalcIter != m_Events.end() && (*RecalcIter)->GetActualTime() < part.m_fEventTimer; 
				RecalcIter++);

			// set our main iterator to the recalculated iterator
			// the -1 just compensates for the i++ in the main for loop
			i = RecalcIter-1; 
		}
	}
	part.m_CurEvent = i;
}

bool zz_particle_event_sequence::update (zz_time diff_time, vec3 m_vPartSysPos )
{
	float fTimeDelta = ZZ_TIME_TO_SEC(float(diff_time));
	if (!m_Particles && !m_WorldParticles) return false;

	int num_current_active = 0;
	int q;
	if (this->GetUpdateCoord() == ZZ_UPDATE_COORD_WORLD) {
		// update existing particles
		for (q=0 ; q < m_WorldParticles->GetTotalElements() ; q++ ) {
			if ( m_WorldParticles->IsAlive(q) ) 
			{
				zz_particle_world &part = m_WorldParticles->GetAt(q);

				if (!part.update(diff_time)) 
				{
					m_WorldParticles->Delete(&part);
				}
				else 
				{
					// run all the particle's events
					RunEvents(part);
					num_current_active++;
				}
			} // is alive
		} // next particle
	}
	else {
		for (q=0 ; q < m_Particles->GetTotalElements() ; q++ ) 
		{
			if ( m_Particles->IsAlive(q) ) 
			{
				zz_particle &part = m_Particles->GetAt(q);

				if (!part.update(diff_time)) 
				{
					m_Particles->Delete(&part);
				}
				else 
				{
                    // apply gravity to this particle.
					part.m_vDir += fTimeDelta * m_vGravity.get_random_num_in_range();
					
					// run all the particle's events
					RunEvents(part);
					num_current_active++;
				}
			} // is alive
		} // next particle
	}
	
	float fEmitRateThisFrame = m_EmitRate.get_random_num_in_range();
	int iNumNewParts = (int)(fEmitRateThisFrame * fTimeDelta);
	m_fNumNewPartsExcess += (float)( fEmitRateThisFrame * fTimeDelta ) - iNumNewParts;
	  
	if (m_fNumNewPartsExcess > 1.0f) 
	{
		iNumNewParts += (int)m_fNumNewPartsExcess;
		m_fNumNewPartsExcess -= (int)m_fNumNewPartsExcess;
	}

	//int total_elements = (GetUpdateCoord() == ZZ_UPDATE_COORD_WORLD) ? m_WorldParticles->GetTotalElements() : m_Particles->GetTotalElements();

	bool is_running = true;
	if (m_Loops > 0 && m_iTotalParticleLives + iNumNewParts > m_Loops * m_iNumParticles ) 
	{
		iNumNewParts = ( m_Loops * m_iNumParticles ) - m_iTotalParticleLives;
		if (iNumNewParts <= 0) 
		{
			iNumNewParts = 0;
		}
		if (iNumNewParts == 0 && num_current_active == 0) {
			is_running = false;
		}
	}
	else if (m_Loops > 0 && m_iTotalParticleLives == m_Loops * m_iNumParticles) {
		if (num_current_active == 0) {
			is_running = false;
		}
	}
	int free_elements;
	free_elements = (GetUpdateCoord() == ZZ_UPDATE_COORD_WORLD) ? m_WorldParticles->GetNumFreeElements() : m_Particles->GetNumFreeElements();
	for (q=0; q < iNumNewParts && free_elements; q++) 
	{
		try 
		{
			CreateNewParticle(m_vPartSysPos);
		} catch(...) 
		{ 
			q = iNumNewParts; 
		}
		free_elements = (GetUpdateCoord() == ZZ_UPDATE_COORD_WORLD) ? m_WorldParticles->GetNumFreeElements() : m_Particles->GetNumFreeElements();
	}

	return is_running;
}

bool zz_particle_event_sequence::reset_render_state ()
{
	// Reset render state
	m_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	return true;
}

void zz_particle_event_sequence::before_render ()
{
	if (m_texParticle)
		m_texParticle->flush_device(true);
}

bool zz_particle_event_sequence::Render( LPDIRECT3DVERTEXBUFFER9 pVB, int iVBSize )
{
	//ZZ_PROFILER_INSTALL(Pparticle_render);

	HRESULT hr;

	//ZZ_LOG("particle_event_sequence: Render(). implement_type(%d)\n", m_iImplementType);

	// Set up the vertex buffer to be rendered
	if (this->m_iImplementType == 0) {
		// Set the render states for using point sprites
		m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
		m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MAX, FtoDW(128.0f) );
		m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
		m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
		m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );
		m_pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof(VERTEX_PARTICLE_POINT) );
		m_pd3dDevice->SetFVF( D3DFVF_PARTICLE_POINT );
	}
	else {
		m_pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof(VERTEX_PARTICLE) );
		m_pd3dDevice->SetFVF( D3DFVF_PARTICLE );
	}

	zz_renderer_cached_info& cached = znzin->renderer->get_cached_info();
	cached.invalidate(zz_renderer_cached_info::VERTEX_BUFFER);

	znzin->renderer->set_vertex_shader(ZZ_HANDLE_NULL);
	znzin->renderer->set_pixel_shader(ZZ_HANDLE_NULL);

	znzin->renderer->enable_zbuffer(true);	
	znzin->renderer->set_zfunc(ZZ_CMP_LESSEQUAL);
	znzin->renderer->enable_alpha_blend(true, ZZ_BT_NORMAL);
	znzin->renderer->enable_alpha_test(false);
	m_pd3dDevice->SetRenderState(D3DRS_BLENDOP, GetBlendOperation());
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, GetSrcBlendMode()); // SRCALPHA
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, GetDestBlendMode()); // DESTALPHA
	znzin->renderer->enable_zwrite(false);
  
	// set up texture stages for simple texture stage copy
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);

	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE); 
	 
	// set to stage 0
	m_texParticle->flush_device(false);
	m_texParticle->set(0);

	VERTEX_PARTICLE *pVertices;
	VERTEX_PARTICLE_POINT *pVerticesPoint; // for point sprite version

	DWORD dwNumParticlesToRender = 0;

	// Lock the vertex buffer.  We fill the vertex buffer in small
	// chunks, using D3DLOCK_NOOVERWRITE.  When we are done filling
	// each chunk, we call DrawPrim, and lock the next chunk.  When
	// we run out of space in the vertex buffer, we start over at
	// the beginning, using D3DLOCK_DISCARD.
	if (this->m_iImplementType == 0) {
		if(FAILED(hr = pVB->Lock( 0, iVBSize * sizeof(VERTEX_PARTICLE_POINT),
			(void **) &pVerticesPoint, D3DLOCK_DISCARD)))
		{
			return reset_render_state();
		}
	}
	else {
		assert((iVBSize%6) == 0); // must be a multiple of 6
		if(FAILED(hr = pVB->Lock( 0, iVBSize * sizeof(VERTEX_PARTICLE),
			(void **) &pVertices, D3DLOCK_DISCARD)))
		{
			return reset_render_state();
		}
	}

	int i;
	vec3 pos[6];
	vec2 uv[6];
	vec2 texture_index;
	vec2 texture_size((float)this->GetTextureSizeWidth(), (float)this->GetTextureSizeHeight());
	mat3 rot;
	float co, si, x, y;
    float x2, y2;

	bool rotate_particle = false;

	static float theta = ZZ_TO_RAD;
	
	if (this->m_iImplementType == 1) {
		zz_camera * cam = znzin->get_camera();
		switch (m_iAlignType) {
			case 0:
				cam->get_rotation_facing(rot);
				break;
			case 1:
				rot = mat3_id;
				break;
			case 2:
				cam->get_rotation_facing_axis(rot, 2);
				break;
		}
	}

	//ZZ_LOG("particle_event_sequence: render(). num_particles(%d)\n", m_iNumParticles);

	// Render each particle
	for (int q=0; q < m_iNumParticles; q++) 
	{
		// Render each particle a bunch of times to get a blurring effect
		if ((m_Particles && m_Particles->IsAlive(q)) || (m_WorldParticles && m_WorldParticles->IsAlive(q)))
		{
			zz_particle * part;
			if (m_Particles)
				part = &m_Particles->GetAt(q);
			else part = &m_WorldParticles->GetAt(q);

			vec3 part_pos;
			switch (GetUpdateCoord())
			{
			case ZZ_UPDATE_COORD_WORLD:
				part_pos = part->m_vPos;
				break;
			case ZZ_UPDATE_COORD_LOCAL:
				{
					vec4 part_world_pos;
					mult(part_world_pos, *world_tm, vec4(part->m_vPos.x, part->m_vPos.y, part->m_vPos.z, 1.0f));
					part_pos.x = part_world_pos.x;
					part_pos.y = part_world_pos.y;
					part_pos.z = part_world_pos.z;
				}
				break;
			case ZZ_UPDATE_COORD_LOCAL_WORLD:
				part_pos = part->m_vPos + world_tm->get_position();
				break;
			}
	
			if (this->m_iImplementType == 0) {
				pVerticesPoint->position = part_pos;
				pVerticesPoint->pointsize = part->m_vSize.x * world_scale;
				pVerticesPoint->color = (DWORD)D3DXCOLOR(part->m_Color.r, part->m_Color.g, part->m_Color.b, part->m_Color.a);
				pVerticesPoint++;
			}
			else {
				texture_index.set(float(int(part->m_fTextureIdx)%int(texture_size.x)), float(int(part->m_fTextureIdx)/int(texture_size.y)));
				
				if (part->m_bAngleOnOff) {
					// rotate theta(radian) around z-axis
					co = cosf(theta*part->m_fAngle);
					si = sinf(theta*part->m_fAngle);
					x = (co*part->m_vSize.x) * world_scale; 
					y = (si*part->m_vSize.x) * world_scale; 
					x2 = (-si*part->m_vSize.y)*world_scale;
					y2 = (co*part->m_vSize.y)*world_scale;

					// assumes COUNTER-CLOCK-WISE polygon welding (CW-CULL)
					pos[0].set(-x - x2, -y - y2, 0); // bottom-left
					pos[1].set(x - x2, y - y2, 0);  // bottom-right
					pos[2].set(-x + x2, -y + y2, 0);  // top-left
					pos[3] = pos[2];       // top-left
					pos[4] = pos[1];       // bottom-right
					pos[5].set(x + x2, y + y2, 0);   // top-right
				}
				else {
					x = part->m_vSize.x * world_scale;
					y = part->m_vSize.y * world_scale;
					pos[0].set(-x, -y, 0); // left-bottom
					pos[1].set(x, -y, 0);  // right-bottom
					pos[2].set(-x, y, 0);  // left-top
					pos[3] = pos[2];       // left-top
					pos[4] = pos[1];       // right-bottom
					pos[5].set(x, y, 0);   // right-top
				}

				uv[0].set((texture_index.x + 0)/texture_size.x, (texture_index.y + 1)/texture_size.y); // (0, 1)
				uv[1].set((texture_index.x + 1)/texture_size.x, (texture_index.y + 1)/texture_size.y); // (1, 1)
				uv[2].set((texture_index.x + 0)/texture_size.x, (texture_index.y + 0)/texture_size.y); // (0, 0)

				uv[3].set((texture_index.x + 0)/texture_size.x, (texture_index.y + 0)/texture_size.y); // (0, 0)
				uv[4].set((texture_index.x + 1)/texture_size.x, (texture_index.y + 1)/texture_size.y); // (1, 1)
				uv[5].set((texture_index.x + 1)/texture_size.x, (texture_index.y + 0)/texture_size.y); // (1, 0)

				for (i = 0; i < 6; i++) {
					if (m_iAlignType == 1) { // non-billboard
						pVertices->position = pos[i] + part_pos;
					}
					else {
						mult(pVertices->position, rot, pos[i]);
						pVertices->position += part_pos;
					}

					pVertices->color = (DWORD)D3DXCOLOR(part->m_Color.r, part->m_Color.g, part->m_Color.b, part->m_Color.a);
					pVertices->uv = uv[i];
					pVertices++;
				}
			}

			++dwNumParticlesToRender;
			if( dwNumParticlesToRender == ((m_iImplementType == 0) ? iVBSize : iVBSize/6))
			{
				// Done filling this chunk of the vertex buffer.  Lets unlock and
				// draw this portion so we can begin filling the next chunk.

				pVB->Unlock();

				try {
					if (m_iImplementType == 0) {
						if(FAILED(hr = m_pd3dDevice->DrawPrimitive( 
							D3DPT_POINTLIST, 0, dwNumParticlesToRender)))
							return reset_render_state();
						if(FAILED(hr = pVB->Lock(0, iVBSize * sizeof(VERTEX_PARTICLE_POINT),
							(void **) &pVerticesPoint, D3DLOCK_DISCARD)))
							return reset_render_state();
					}
					else {
						if(FAILED(hr = m_pd3dDevice->DrawPrimitive( 
							D3DPT_TRIANGLELIST, 0, dwNumParticlesToRender*2)))
							return reset_render_state();

						// Lock the next chunk of the vertex buffer.  If we are at the 
						// end of the vertex buffer, DISCARD the vertex buffer and start
						// at the beginning.  Otherwise, specify NOOVERWRITE, so we can
						// continue filling the VB while the previous chunk is drawing.
						if(FAILED(hr = pVB->Lock(0, iVBSize * sizeof(VERTEX_PARTICLE),
							(void **) &pVertices, D3DLOCK_DISCARD)))
							return reset_render_state();
					}
				}
				catch (...) {
					// do nothing
					// maybe device lost state
					ZZ_LOG("particel: render(). dp exception\n");
				}

				dwNumParticlesToRender = 0;
			}
		}
	}

	// Unlock the vertex buffer
	pVB->Unlock();

	try {
		// Render any remaining particles
		if (m_iImplementType == 0) {
			// Render any remaining particles
			if( dwNumParticlesToRender )
			{
				if(FAILED(hr = m_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 0, dwNumParticlesToRender )))
					return reset_render_state();
			}
			// Reset render states
			m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
			m_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
		}
		else {
			if( dwNumParticlesToRender )
			{
				if(FAILED(hr = m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumParticlesToRender*2 )))
					return reset_render_state();
			}
		}
	}
	catch (...) {
		// no nothing
		// maybe device lost state
		ZZ_LOG("particle: render(). dp exception. 2\n");
	}
	
	return reset_render_state();
}

bool zz_particle_event_sequence::Save( zz_vfs * fs )
{
	return false;
}

bool zz_particle_event_sequence::Load( zz_vfs * fs )
{
	int iLength;
	char Buf[255];
	// name	
	fs->read((char *)&(iLength), sizeof( int ) );
	fs->read( Buf, sizeof( char )*iLength );	
	Buf[ iLength ] = NULL;
	m_strName = std::string( Buf );

	// Lifetime
	fs->read_float((m_Lifetime.m_min));
	fs->read_float((m_Lifetime.m_max));	

	// EmitRate
	fs->read_float((m_EmitRate.m_min));
	fs->read_float((m_EmitRate.m_max));	

	// Loop count
	fs->read_uint32( (m_Loops) );

	// SpawnDir
	fs->read_float( (m_vSpawnDir.m_min.x) );
	fs->read_float( (m_vSpawnDir.m_min.y) );
	fs->read_float( (m_vSpawnDir.m_min.z) );
	fs->read_float( (m_vSpawnDir.m_max.x) );
	fs->read_float( (m_vSpawnDir.m_max.y) );
	fs->read_float( (m_vSpawnDir.m_max.z) );
	m_vSpawnDir.m_min.x *= ZZ_SCALE_IN;
	m_vSpawnDir.m_min.y *= ZZ_SCALE_IN;
	m_vSpawnDir.m_min.z *= ZZ_SCALE_IN;
	m_vSpawnDir.m_max.x *= ZZ_SCALE_IN;
	m_vSpawnDir.m_max.y *= ZZ_SCALE_IN;
	m_vSpawnDir.m_max.z *= ZZ_SCALE_IN;

	// m_vEmitRadius
	fs->read_float( (m_vEmitRadius.m_min.x) );
	fs->read_float( (m_vEmitRadius.m_min.y) );
	fs->read_float( (m_vEmitRadius.m_min.z) );
	fs->read_float( (m_vEmitRadius.m_max.x) );
	fs->read_float( (m_vEmitRadius.m_max.y) );
	fs->read_float( (m_vEmitRadius.m_max.z) );		
	m_vEmitRadius.m_min.x *= ZZ_SCALE_IN;
	m_vEmitRadius.m_min.y *= ZZ_SCALE_IN;
	m_vEmitRadius.m_min.z *= ZZ_SCALE_IN;
	m_vEmitRadius.m_max.x *= ZZ_SCALE_IN;
	m_vEmitRadius.m_max.y *= ZZ_SCALE_IN;
	m_vEmitRadius.m_max.z *= ZZ_SCALE_IN;

	// m_vGravity
	fs->read_float( (m_vGravity.m_min.x) );
	fs->read_float( (m_vGravity.m_min.y) );
	fs->read_float( (m_vGravity.m_min.z) );
	fs->read_float( (m_vGravity.m_max.x) );
	fs->read_float( (m_vGravity.m_max.y) );
	fs->read_float( (m_vGravity.m_max.z) );
	m_vGravity.m_min.x *= ZZ_SCALE_IN;
	m_vGravity.m_min.y *= ZZ_SCALE_IN;
	m_vGravity.m_min.z *= ZZ_SCALE_IN;
	m_vGravity.m_max.x *= ZZ_SCALE_IN;
	m_vGravity.m_max.y *= ZZ_SCALE_IN;
	m_vGravity.m_max.z *= ZZ_SCALE_IN;

	// texfilename length	
	fs->read_uint32( iLength );	  	  	
	fs->read( Buf, sizeof( char )*iLength );
	Buf[ iLength ] = NULL;
	//m_strTexFilename = std::string("particles/") + std::string( Buf );
	m_strTexFilename = std::string( Buf );
	
	// Particle number
	fs->read_uint32( m_iNumParticles );

	// align type
	fs->read_uint32( m_iAlignType );

	// update coordinates
	// refer to [zz_particle_event_sequence.h]
	fs->read_uint32( m_iUpdateCoord );
	
	// sub texture size width & height
	fs->read_uint32( m_iTextureSizeWidth );
	fs->read_uint32( m_iTextureSizeHeight );

	fs->read_uint32( m_iImplementType );

	// Destination blend mode
	fs->read_uint32( m_iDestBlendMode);

	// Source blend mode
	fs->read_uint32( m_iSrcBlendMode );

	// blend operation
	fs->read_uint32( m_iBlendOperation );

	// Event count
	int iEventCount; 
	fs->read_uint32( iEventCount );

	for (std::vector<zz_particle_event *>::iterator i = m_Events.begin(); i != m_Events.end(); i++) {
		// can't use safe zz_delete here, because it will set i = NULL.
		if (*i) zz_delete (*i);
	}
	m_Events.clear();

	int iEventType = 0;
	for( int j = 0; j < iEventCount; j++ )
	{
		fs->read_uint32( iEventType );

		zz_particle_event* pEvent = EventFactory( iEventType );

		assert(pEvent);
		
		pEvent->Load( fs );

		m_Events.push_back( pEvent );
	}	

	NailDownRandomTimes();
	SortEvents();
	CreateFadeLists();

	if (this->GetUpdateCoord() == ZZ_UPDATE_COORD_WORLD) {
		if (!m_WorldParticles) {
			m_WorldParticles = zz_new zz_recycling_array<zz_particle_world>(m_iNumParticles);
		}
	}
	else {
		if (!m_Particles) {
			m_Particles = zz_new zz_recycling_array<zz_particle>(m_iNumParticles);
		}
	}

	return true;
}


