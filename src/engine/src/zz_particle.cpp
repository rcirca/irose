/** 
 * @file zz_particle.cpp
 * @brief particle class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    20-mar-2003
 *
 * $Header: /engine/src/zz_particle.cpp 6     05-12-24 10:23p Choo0219 $
 * $History: zz_particle.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Choo0219     Date: 05-12-24   Time: 10:23p
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-11   Time: 6:01p
 * Updated in $/engine/src
 * Reverted to fTimeDelta/1000.0f. After converting particle file, change
 * the code.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-11   Time: 5:42p
 * Updated in $/engine/src
 * reverted to original bug version.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 5:05p
 * Updated in $/engine/src
 * fTimeDelta converted diff/1000 -> ZZ_TIME_TO_SEC(diff)
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
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
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_particle.h"

zz_particle::zz_particle() 
{ 
	m_fWeight = 1.0f;
	m_fLifetime = 1.0f; m_fAge = 0.0f; m_fEventTimer = 0.0f; 
	m_Color = vec4(1.0f,1.0f,1.0f,1.0f);
	m_ColorStep = vec4(0.0f,0.0f,0.0f,0.0f);
	m_vDir = vec3(0.0f,0.0f,0.0f);
	m_vDirStep = vec3(0.0f,0.0f,0.0f);
	m_vPos = vec3(0.0f,0.0f,0.0f);
	m_vSize = vec3(10.0f, 10.0f, 0.f);
	m_vSizeStep = vec3(0, 0, 0);
	m_fTextureIdx = 0.0f;
	m_fTextureIdxStep = 0.0f;
    m_fAngle = 0.0f;
	m_fAngleStep = 0.0f;
    m_bAngleOnOff = false;
}

zz_particle::~zz_particle()
{ 
}

bool zz_particle::update (zz_time diff_time)
{
	//float fTimeDelta = ZZ_TIME_TO_SEC(float(diff_time));
	float fTimeDelta = float(diff_time)/1000.0f; // TODO: change by upper code
	
	// age the particle
	m_fAge += fTimeDelta;
	m_fEventTimer += fTimeDelta;

	// if this particle's age is greater than it's lifetime, it dies.
	if (m_fAge >= m_fLifetime) {
		return(false); // dead!
	}

	m_vPos += fTimeDelta * m_vDir;
	m_Color += fTimeDelta * m_ColorStep;
	m_vDir += fTimeDelta * m_vDirStep;
	m_vSize += fTimeDelta * m_vSizeStep;
	m_fAngle += fTimeDelta * m_fAngleStep;
   
	if(m_fAngle > 360.0f)
		m_fAngle -= 360.0f;
	
	m_fTextureIdx += fTimeDelta * m_fTextureIdxStep;

	return(true); // particle stays alive
}

zz_particle_world::zz_particle_world() 
{ 
	rot_world = mat3_id;
	gravity_local = vec3_null;
}

zz_particle_world::~zz_particle_world()
{ 
}

bool zz_particle_world::update (zz_time diff_time)
{
	//float fTimeDelta = ZZ_TIME_TO_SEC(float(diff_time));
	float fTimeDelta = float(diff_time)/1000.0f; // TODO: change by upper code

	// age the particle
	m_fAge += fTimeDelta;
	m_fEventTimer += fTimeDelta;

	// if this particle's age is greater than it's lifetime, it dies.
	if (m_fAge >= m_fLifetime) {
		return(false); // dead!
	}

	// world_dir and gravity_local is updated in zz_particle_event_sequence::CreateNewParticle()
	vec3 world_dir;
	mult(world_dir, this->rot_world, m_vDir); // transform m_vDir(local) to world
	m_vPos += fTimeDelta * world_dir;
	m_vDir += fTimeDelta * gravity_local; // gravity_local is the gravity vector in local coordsys

	m_Color += fTimeDelta * m_ColorStep;
	m_vDir += fTimeDelta * m_vDirStep;
	m_vSize += fTimeDelta * m_vSizeStep;
	
	m_fTextureIdx += fTimeDelta * m_fTextureIdxStep;

	return(true); // particle stays alive
}