/** 
 * @file zz_particle_event_sequence.h
 * @brief particle event_sequence class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    30-apr-2003
 *
 * $Header: /engine/include/zz_particle_event_sequence.h 6     04-11-07 5:56p Choo0219 $
 * $History: zz_particle_event_sequence.h $
 * 
 * *****************  Version 6  *****************
 * User: Choo0219     Date: 04-11-07   Time: 5:56p
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-08-31   Time: 4:46p
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-08-03   Time: 9:59a
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-04-28   Time: 2:03p
 * Updated in $/engine/include
 * ocean vs. camera test more precisely
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-20   Time: 3:52p
 * Updated in $/engine/include
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
 * *****************  Version 16  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_PARTICLE_EVENT_SEQUENCE_H__
#define __ZZ_PARTICLE_EVENT_SEQUENCE_H__

#ifndef __ZZ_PARTICLE_H__
#include "zz_particle.h"
#endif

#ifndef __ZZ_RECYCLING_ARRAY_H__
#include "zz_recycling_array.h"
#endif

#ifndef __ZZ_MINMAX_H__
#include "zz_minmax.h"
#endif

#include <vector>

class zz_particle_event;
class zz_texture;
class zz_vfs;

///--------------------------------------------------------------------------------
class zz_particle_event_sequence
{
public:

	// update coordinates with [world or local]
	enum {
		ZZ_UPDATE_COORD_WORLD = 0, // rotation is updated by parent node, and position is not updated by parent node. (position is updated by world)
		ZZ_UPDATE_COORD_LOCAL_WORLD = 1, // rotation is not update by parent node, and position is updated by parent node. (local rotation == world rotation)
		ZZ_UPDATE_COORD_LOCAL = 2, // rotation and position is updated by parent node. 
	};

	zz_particle_event_sequence();
	virtual ~zz_particle_event_sequence();

	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pDev );
	HRESULT InvalidateDeviceObjects();


	int Reset(void);

	// returns false if no particles are alive and no new particles are to be created
	bool update (zz_time diff_time, vec3 m_vPartSysPos);

	bool Render(LPDIRECT3DVERTEXBUFFER9 pVB, int iVBSize );


	void CreateNewParticle(vec3 m_vPartSysPos);
	void CreateFadeLists();


	void SortEvents(void);
	void NailDownRandomTimes(void);


	static zz_particle_event *zz_particle_event_sequence::EventFactory( int iEventType );

	
	// Attributes

	zz_minmax<float>		GetLifetime(void) const { return(m_Lifetime); }
	void				SetLifetime(const zz_minmax<float> data) { m_Lifetime = data; }

	int					GetSrcBlendMode(void) const { return(m_iSrcBlendMode); }
	void				SetSrcBlendMode(const int data) { m_iSrcBlendMode = data; }

	int					GetDestBlendMode(void) const { return(m_iDestBlendMode); }
	void				SetDestBlendMode(const int data) { m_iDestBlendMode = data; }

	int					GetBlendOperation(void) const { return(m_iBlendOperation); }
	void				SetBlendOperation(const int data) { m_iBlendOperation = data; }

	zz_minmax<float>		GetEmitRate(void) const { return(m_EmitRate); }
	void				SetEmitRate(const zz_minmax<float> data) { m_EmitRate = data; }

	zz_minmax<vec3> GetEmitRadius(void) const { return(m_vEmitRadius); }
	void				SetEmitRadius(const zz_minmax<vec3> &data) { m_vEmitRadius = data; }

	zz_minmax<vec3> GetGravity(void) const { return(m_vGravity); }
	void				SetGravity(const zz_minmax<vec3> &data) { m_vGravity = data; }

	std::string			GetName(void) const { return(m_strName); }
	void				SetName(const std::string &data) { m_strName = data; }
	  
	int					GetNumActiveParticles(void)
	{
		int num = (m_Particles) ? m_Particles->GetNumUsedElements() : 0;
		num = (m_WorldParticles) ? m_WorldParticles->GetNumUsedElements() : num;
		return num;
	}

	int					GetNumParticles(void) const { return(m_iNumParticles); }
	void				SetNumParticles(const int data) { m_iNumParticles = data; }

	int					GetLoops(void) { return(m_Loops); }
	void				SetLoops(const int data) { m_Loops = data; }

	zz_minmax<vec3> GetSpawnDir(void) const { return(m_vSpawnDir); }
	void				SetSpawnDir(const zz_minmax<vec3> &data) { m_vSpawnDir = data; }

	std::string			GetTextureFilename(void) const { return(m_strTexFilename); }
	zz_texture *	    GetTexture(void) const { return(m_texParticle); }
	void				SetTexture(const char *strTexFilename);

	int					GetTextureSizeWidth(void) const { return (m_iTextureSizeWidth); }
	int					GetTextureSizeHeight(void) const { return (m_iTextureSizeHeight); }

	int					GetImplementType(void) const { return (m_iImplementType); }
	int					GetUpdateCoord(void) const { return (m_iUpdateCoord); }
	
	void RunEvents( zz_particle &part );
	int DeleteAllParticles(void)
	{
		if (m_Particles) m_Particles->DeleteAll();
		if (m_WorldParticles) m_WorldParticles->DeleteAll();
		m_iTotalParticleLives = 0; return 0;
	}

	bool reset_render_state ();
	void set_world_tm(const mat4 * world_tm_in) { world_tm = world_tm_in; }
	void set_scale_world(float scale_in) { world_scale = scale_in; }
	void before_render ();
	//void after_render ();

private:
	std::vector<zz_particle_event *>		m_Events;
	zz_recycling_array<zz_particle>		*m_Particles;
	zz_recycling_array<zz_particle_world> *m_WorldParticles;

	zz_minmax<float>						m_Lifetime;
	zz_minmax<float>						m_EmitRate; // in particles/sec
	int m_Loops;
	zz_minmax<vec3>				m_vSpawnDir;
	zz_minmax<vec3>				m_vEmitRadius;
	zz_minmax<vec3>				m_vGravity;

	std::string							m_strTexFilename;

	zz_texture *                        m_texParticle;
	LPDIRECT3DDEVICE9					m_pd3dDevice;
	const mat4 *						world_tm; // world transform
	float								world_scale; // globally applied scale


	int									m_iNumParticles;
	int									m_iDestBlendMode;
	int									m_iSrcBlendMode;
	int									m_iBlendOperation; // added

	int									m_iAlignType;	// 0 : Normal billboard, 1 : World mesh(non-billboard, non-aligned), 2 :  Z Axis aligned
	int									m_iTextureSizeWidth;
	int									m_iTextureSizeHeight;

	long int							m_iTotalParticleLives;
	
	// use PointSprite when does not use billboarding, non-texture animation, 
	int									m_iImplementType; // 0 : PointSprite, 1 : Billboard
	
	// problem(applying gravity and adapting billboard)
	int									m_iUpdateCoord; // ZZ_UPDATE_COORD_WORLD | LOCAL

	std::string							m_strName;
	  
	float								m_fNumNewPartsExcess;

public:
	bool Save( zz_vfs * fs );
	bool Load( zz_vfs * fs );
};

#endif // __ZZ_PARTICLE_EVENT_SEQUENCE_H__