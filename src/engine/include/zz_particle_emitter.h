/** 
 * @file zz_particle_emitter.h
 * @brief particle emitter class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    30-apr-2003
 *
 * $Header: /engine/include/zz_particle_emitter.h 11    04-08-31 4:46p Zho $
 * $History: zz_particle_emitter.h $
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-08-31   Time: 4:46p
 * Updated in $/engine/include
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-08-16   Time: 4:20p
 * Updated in $/engine/include
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-07-19   Time: 8:42p
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-06-30   Time: 6:57p
 * Updated in $/engine/include
 * 6.30.4498
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-06-29   Time: 10:09a
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-04-07   Time: 5:25p
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-11   Time: 11:35a
 * Updated in $/engine/include
 * void apply_motion() -> bool apply_motion()
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-15   Time: 6:12p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-15   Time: 4:56p
 * Updated in $/engine/include
 * refrence count removed. is it ok?
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 18  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 * 
 * *****************  Version 17  *****************
 * User: Zho          Date: 03-11-27   Time: 4:50p
 * Updated in $/znzin11/engine/include
 * - removed zz_resource class, added full-support of resource management
 * into zz_node, and cleaned up some codes.
 */

#ifndef __ZZ_PARTICLE_EMITTER_H__
#define __ZZ_PARTICLE_EMITTER_H__

#include <map>
#include <string>
#include <algorithm>
#include <functional>

#include "d3dx9.h"

#ifndef __ZZ_ANIMATABLE_H__
#include "zz_animatable.h"
#endif

#ifndef __ZZ_CHANNEL_POSITION_H__
#include "zz_channel_position.h"
#endif

#ifndef __ZZ_CHANNEL_ROTATION_H__
#include "zz_channel_rotation.h"
#endif

#ifndef __ZZ_PARTICLE_H__
#include "zz_particle.h"
#endif

#ifndef __ZZ_PARTICLE_EVENT_SEQUENCE_H__
#include "zz_particle_event_sequence.h"
#endif

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

#ifndef __ZZ_DEVICE_RESOURCE_H__
#include "zz_device_resource.h"
#endif

class zz_particle_emitter : public zz_animatable {
public:
	std::string m_strLastError;

	zz_particle_emitter();
	virtual ~zz_particle_emitter();

	virtual void update_time (bool recursive, zz_time diff_time);
	virtual void update_animation (bool recursive, zz_time diff_time);
	virtual void render (bool recursive = false);

	virtual void set_load_weight (int weight_in) { }
	virtual int get_load_weight () const { return 0; }

	virtual bool restore_device_objects ();
	virtual bool invalidate_device_objects ();
    
	virtual void Init();
	virtual bool load (const char* fname, bool loadnow = false);
	virtual bool load (zz_vfs * fs, bool loadnow = false);

	virtual void play();
	virtual void pause();
	virtual void stop();
	virtual bool IsRunning()
	{
		return (m_bIsRunning && motion_controller.is_playing());
	}
	
	int DeleteAllParticles(void) 
	{
		std::for_each( m_Sequences.begin(), m_Sequences.end(), std::mem_fun< int, zz_particle_event_sequence > ( &zz_particle_event_sequence::DeleteAllParticles) );		
		return 0;
	}

	// [attributes] Get, Set method  
	zz_minmax<vec3> GetPosRange(void) const { return(m_vPosRange); }
	void	SetPosRange(const zz_minmax<vec3> &data) { m_vPosRange = data; }

	vec3	GetPos(void) const { return(m_vPos); }
	void	SetPos(const vec3 &data) { m_vPos = data; }  
	
	int GetVBSize(void) const { return(m_iVBSize); }
	void SetVBSize(const int data) { m_iVBSize = data; } // must be 6 times

	int Reset(void) 
	{
		std::for_each( m_Sequences.begin(), m_Sequences.end(), std::mem_fun< int, zz_particle_event_sequence > ( &zz_particle_event_sequence::Reset) );		
		return 0;
	}

	int GetNumActiveParticles(void) 
	{
		int count=0;
		for (std::vector<zz_particle_event_sequence *>::iterator i = m_Sequences.begin(); i != m_Sequences.end(); i++) 
		{
			count += (*i)->GetNumActiveParticles();
		}
		return(count);
	}

	virtual bool apply_motion (void);

	virtual void attach_motion (zz_motion * motion_to_attach);

protected:
	// sub-function of the reset_bvolume()
	virtual void build_mesh_minmax (vec3& mesh_min_out, vec3& mesh_max_out);

private:  
  
	// particle system attributes
	zz_minmax<vec3> m_vPosRange; 
	vec3	m_vPos; // actual position of emitter
	bool use_pointsprite; // whether this use pointsprite, default is true

	std::vector<zz_particle_event_sequence *>	m_Sequences;

	int m_iMaxParticles;
	bool m_bIsRunning;

	int m_iVBSize; // size of the vertex buffer (can and should be less than NUMPARTICLES)

	int total_particles; // affects on lifetime

	zz_channel_position * pos_channel;

	// all particles share these
	static LPDIRECT3DDEVICE9	m_pd3dDevice;
	static LPDIRECT3DVERTEXBUFFER9 m_vbParticles;
	static LPDIRECT3DVERTEXBUFFER9 m_vbParticles_point;
	static int vb_ref_count;
	zz_device_resource res;

	virtual void before_render ();
	//virtual void after_render ();

	ZZ_DECLARE_DYNAMIC(zz_particle_emitter)
};

inline void zz_particle_emitter::play()
{
	m_bIsRunning = true;
	motion_controller.play();
}

inline void zz_particle_emitter::pause()
{
	m_bIsRunning = false;
	motion_controller.pause();
}

inline void zz_particle_emitter::stop()
{
	pause();
	DeleteAllParticles();
	motion_controller.stop();
}

#endif // __ZZ_PARTICLE_EMITTER_H__