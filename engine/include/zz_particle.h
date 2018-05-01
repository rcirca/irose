/** 
 * @file zz_particle.h
 * @brief particle class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    20-mar-2003
 *
 * $Header: /engine/include/zz_particle.h 3     05-12-24 10:23p Choo0219 $
 * $History: zz_particle.h $
 * 
 * *****************  Version 3  *****************
 * User: Choo0219     Date: 05-12-24   Time: 10:23p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-11   Time: 5:05p
 * Updated in $/engine/include
 * fTimeDelta converted diff/1000 -> ZZ_TIME_TO_SEC(diff)
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
 * User: Zho          Date: 03-11-30   Time: 7:53p
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
 * *****************  Version 10  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_PARTICLE_H__
#define __ZZ_PARTICLE_H__

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif

#include <vector>

#include "d3dx9.h"
#include "zz_type.h"

// d3d point sprite version
typedef struct 
{
    vec3 position;        
	float       pointsize;
	D3DCOLOR    color;
} VERTEX_PARTICLE_POINT;
#define D3DFVF_PARTICLE_POINT (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_PSIZE)

// billboard version (non pointsprite)
typedef struct 
{
    vec3 position;        
	D3DCOLOR    color;
	vec2 uv;
} VERTEX_PARTICLE;
#define D3DFVF_PARTICLE (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

class zz_particle_event;

class zz_particle
{
public:
	zz_particle();
	virtual ~zz_particle();

	std::vector< zz_particle_event * >::iterator	m_CurEvent;

	float						m_fWeight;
	  
	vec3						m_vSize;
	vec3						m_vSizeStep;

	float						m_fLifetime; // in seconds
	float						m_fAge; // in seconds
	  
	float						m_fEventTimer; // different from age

	float m_fTextureIdx;    // texture index
	float m_fTextureIdxStep; // texture index step
	  
	vec4 m_Color;
	vec4 m_ColorStep;
	  
	vec3 m_vPos; // current position of particle
	  
	vec3 m_vDir; // position per 1 second
	vec3 m_vDirStep;

	float m_fAngle;
	float m_fAngleStep;
	bool m_bAngleOnOff;
	
	// @diff_time diff time in system timer's metric
	virtual bool update (zz_time diff_time);
};

// world aligned particle
class zz_particle_world : public zz_particle
{
public:
	zz_particle_world();
	virtual ~zz_particle_world();

	vec3 gravity_local; // gravity in local coordsys. set in CreateNewParticle
	mat3 rot_world; // ratation_to_world. set in CreateNewParticle

	// @diff_time diff time in system timer's metric
	virtual bool update (zz_time diff_time);  
};

#endif // __ZZ_PARTICLE_H__
