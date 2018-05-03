/** 
 * @file zz_particle_event.cpp
 * @brief particle_event class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    30-apr-2003
 *
 * $Header: /engine/src/zz_particle_event.cpp 7     06-06-23 11:40a Choo0219 $
 * $History: zz_particle_event.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Choo0219     Date: 06-06-23   Time: 11:40a
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Choo0219     Date: 05-12-24   Time: 10:23p
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-05-19   Time: 8:14p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-04-26   Time: 5:20p
 * Updated in $/engine/src
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-06   Time: 1:26p
 * Updated in $/engine/src
 * Xform coordinate system
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
#include "zz_particle_event.h"
#include "zz_log.h"
#include "zz_vfs.h"

//////////////////////////////////////////////////////////////////////////////
// zz_particle_event
//////////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------
// Save, Load method..
bool zz_particle_event::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event::Load( zz_vfs * fs )
{
	fs->read_float( (m_TimeRange.m_min) );
	fs->read_float( (m_TimeRange.m_max) );	
	char temp;
	fs->read_char( temp ); //fs->ReadBool( &(m_bFade) );
	m_bFade = (temp == 0x00) ? false : true;

	return true;
}

bool zz_particle_event_texture::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event_texture::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_TextureIdx.m_min) );
	fs->read_float( (m_TextureIdx.m_max) );

	return true;
}	

void zz_particle_event_texture::DoItToIt( zz_particle &part )
{
	if (!IsFade()) {
		part.m_fTextureIdx = m_TextureIdx.get_random_num_in_range();
	}
	if ( m_NextFadeEvent )
	{
		float newvalue = static_cast<zz_particle_event_texture *>(m_NextFadeEvent)->GetTextureIdx().get_random_num_in_range();
		float timedelta = m_NextFadeEvent->GetActualTime() - GetActualTime();
		if (timedelta == 0) timedelta = 1; // prevent divide by zero errors
		part.m_fTextureIdxStep = (newvalue - part.m_fTextureIdx) / timedelta;
	}
	//ZZ_LOG("particle_event_texture: (%f) - DoItToIt(%x, %d)\n", part.m_fEventTimer, &part, (int)(part.m_fTextureIdx));
}

//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_size
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Save, Load method..
bool zz_particle_event_size::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event_size::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_Size.m_min.x) );
	fs->read_float( (m_Size.m_min.y) );
	m_Size.m_min.x *= ZZ_SCALE_IN;
	m_Size.m_min.y *= ZZ_SCALE_IN;

	fs->read_float( (m_Size.m_max.x) );
	fs->read_float( (m_Size.m_max.y) );
	m_Size.m_max.x *= ZZ_SCALE_IN;
	m_Size.m_max.y *= ZZ_SCALE_IN;

	return true;
}

void zz_particle_event_size::DoItToIt( zz_particle &part )
{
	if (!IsFade()) part.m_vSize = m_Size.get_random_num_in_range();

	if ( m_NextFadeEvent )
	{
		vec3 newsize = static_cast< zz_particle_event_size * >(m_NextFadeEvent)->GetSize().get_random_num_in_range();
		float timedelta = m_NextFadeEvent->GetActualTime() - GetActualTime();
		if (timedelta == 0) timedelta = 1; // prevent divide by zero errors
		part.m_vSizeStep.x = (newsize.x - part.m_vSize.x) / timedelta;
		part.m_vSizeStep.y = (newsize.y - part.m_vSize.y) / timedelta;
		part.m_vSizeStep.z = (newsize.z - part.m_vSize.z) / timedelta;
		//part.m_vSizeStep.x = newsize.x - part.m_vSize.x / timedelta;
		//part.m_vSizeStep.y = newsize.y - part.m_vSize.y / timedelta;
		//part.m_vSizeStep.z = newsize.z - part.m_vSize.z / timedelta;
	}
}

//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_color
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Save, Load method..
bool zz_particle_event_color::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event_color::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_Color.m_min.r) );
	fs->read_float( (m_Color.m_min.g) );
	fs->read_float( (m_Color.m_min.b) );
	fs->read_float( (m_Color.m_min.a) );   

	fs->read_float( (m_Color.m_max.r) );
	fs->read_float( (m_Color.m_max.g) );
	fs->read_float( (m_Color.m_max.b) );
	fs->read_float( (m_Color.m_max.a) );

	return true;
}

void zz_particle_event_color::DoItToIt(zz_particle &part)
{
	if (!IsFade()) 
		part.m_Color = m_Color.get_random_num_in_range();

	if (m_NextFadeEvent) 
	{
		vec4 newcolor = static_cast<zz_particle_event_color *>(m_NextFadeEvent)->GetColor().get_random_num_in_range();
		float timedelta = m_NextFadeEvent->GetActualTime() - GetActualTime();
		if (timedelta == 0) timedelta = 1; // prevent divide by zero errors
		// calculate color deltas to get us to the next fade event.
		part.m_ColorStep.r = (newcolor.r - part.m_Color.r) / timedelta;
		part.m_ColorStep.g = (newcolor.g - part.m_Color.g) / timedelta;
		part.m_ColorStep.b = (newcolor.b - part.m_Color.b) / timedelta;
		part.m_ColorStep.a = (newcolor.a - part.m_Color.a) / timedelta;
		//part.m_ColorStep.r = newcolor.r - part.m_Color.r / timedelta;
		//part.m_ColorStep.g = newcolor.g - part.m_Color.g / timedelta;
		//part.m_ColorStep.b = newcolor.b - part.m_Color.b / timedelta;
		//part.m_ColorStep.a = newcolor.a - part.m_Color.a / timedelta;
	}
}




//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_red_color
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Save, Load method..
bool zz_particle_event_red_color::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event_red_color::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_RedColor.m_min) );
	fs->read_float( (m_RedColor.m_max) );
	return true;
}

void zz_particle_event_red_color::DoItToIt( zz_particle &part )
{
	if (!IsFade()) part.m_Color.r = m_RedColor.get_random_num_in_range();

	if (m_NextFadeEvent) 
	{
		float newvalue = static_cast<zz_particle_event_red_color *>(m_NextFadeEvent)->GetRedColor().get_random_num_in_range();
		float timedelta = m_NextFadeEvent->GetActualTime() - GetActualTime();
		if (timedelta == 0) timedelta = 1; // prevent divide by zero errors
		part.m_ColorStep.r = (newvalue - part.m_Color.r) / timedelta;
		// part.m_ColorStep.r = newvalue - part.m_Color.r / timedelta;
	}
}



//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_green_color
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Save, Load method..
bool zz_particle_event_green_color::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event_green_color::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_GreenColor.m_min) );
	fs->read_float( (m_GreenColor.m_max) );	

	return true;
}

void zz_particle_event_green_color::DoItToIt( zz_particle &part )
{
	if (!IsFade()) part.m_Color.g = m_GreenColor.get_random_num_in_range();

	if (m_NextFadeEvent) 
	{
		float newvalue = static_cast<zz_particle_event_green_color *>(m_NextFadeEvent)->GetGreenColor().get_random_num_in_range();
		float timedelta = m_NextFadeEvent->GetActualTime() - GetActualTime();
		if (timedelta == 0) timedelta = 1; // prevent divide by zero errors
		part.m_ColorStep.g = (newvalue - part.m_Color.g) / timedelta;
		//part.m_ColorStep.g = newvalue - part.m_Color.g / timedelta;
	}
}



//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_blue_color
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Save, Load method..
bool zz_particle_event_blue_color::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event_blue_color::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_BlueColor.m_min) );
	fs->read_float( (m_BlueColor.m_max) );	

	return true;
}

void zz_particle_event_blue_color::DoItToIt( zz_particle &part )
{
	if (!IsFade()) part.m_Color.b = m_BlueColor.get_random_num_in_range();

	if (m_NextFadeEvent) 
	{
		float newvalue = static_cast<zz_particle_event_blue_color *>(m_NextFadeEvent)->GetBlueColor().get_random_num_in_range();
		float timedelta = m_NextFadeEvent->GetActualTime() - GetActualTime();
		if (timedelta == 0) timedelta = 1; // prevent divide by zero errors
		part.m_ColorStep.b = (newvalue - part.m_Color.b) / timedelta;
		//part.m_ColorStep.b = newvalue - part.m_Color.b / timedelta;
	}
}



//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_alpha
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Save, Load method..
bool zz_particle_event_alpha::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event_alpha::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_Alpha.m_min) );
	fs->read_float( (m_Alpha.m_max) );		  

	return true;
}

void zz_particle_event_alpha::DoItToIt( zz_particle &part )
{
	if (!IsFade()) part.m_Color.a = m_Alpha.get_random_num_in_range();

	if (m_NextFadeEvent) 
	{
		float newvalue = static_cast<zz_particle_event_alpha *>(m_NextFadeEvent)->GetAlpha().get_random_num_in_range();
		float timedelta = m_NextFadeEvent->GetActualTime() - GetActualTime();
		if (timedelta == 0) timedelta = 1; // prevent divide by zero errors
		part.m_ColorStep.a = (newvalue - part.m_Color.a) / timedelta;
		//part.m_ColorStep.a = newvalue - part.m_Color.a / timedelta;
	}
}


//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_velocity
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Save, Load method..
bool zz_particle_event_velocity::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event_velocity::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_Velocity.m_min.x) );
	fs->read_float( (m_Velocity.m_min.y) );
	fs->read_float( (m_Velocity.m_min.z) );
	fs->read_float( (m_Velocity.m_max.x) );
	fs->read_float( (m_Velocity.m_max.y) );
	fs->read_float( (m_Velocity.m_max.z) );

	m_Velocity.m_min.x *= ZZ_SCALE_IN;
	m_Velocity.m_min.y *= ZZ_SCALE_IN;
	m_Velocity.m_min.z *= ZZ_SCALE_IN;

	m_Velocity.m_max.x *= ZZ_SCALE_IN;
	m_Velocity.m_max.y *= ZZ_SCALE_IN;
	m_Velocity.m_max.z *= ZZ_SCALE_IN;

	return true;
}

void zz_particle_event_velocity::DoItToIt( zz_particle &part )
{
	if (!IsFade()) 
		part.m_vDir = m_Velocity.get_random_num_in_range();

	if (m_NextFadeEvent) 
	{
		vec3 newvalue = static_cast<zz_particle_event_velocity *>(m_NextFadeEvent)->GetVelocity().get_random_num_in_range();
		float timedelta = m_NextFadeEvent->GetActualTime() - GetActualTime();
		if (timedelta == 0) timedelta = 1; // prevent divide by zero errors
		part.m_vDirStep.x = (newvalue.x - part.m_vDir.x) / timedelta;
		part.m_vDirStep.y = (newvalue.y - part.m_vDir.y) / timedelta;
		part.m_vDirStep.z = (newvalue.z - part.m_vDir.z) / timedelta;
		//part.m_vDirStep.x = newvalue.x - part.m_vDir.x / timedelta;
		//part.m_vDirStep.y = newvalue.y - part.m_vDir.y / timedelta;
		//part.m_vDirStep.z = newvalue.z - part.m_vDir.z / timedelta;
	}
}

//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_velocityx
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Save, Load method..
bool zz_particle_event_velocityx::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event_velocityx::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_VelocityX.m_min) );
	fs->read_float( (m_VelocityX.m_max) );	  

	m_VelocityX.m_min *= ZZ_SCALE_IN;
	m_VelocityX.m_max *= ZZ_SCALE_IN;  

	return true;
}

void zz_particle_event_velocityx::DoItToIt( zz_particle &part )
{
	if (!IsFade()) part.m_vDir.x = m_VelocityX.get_random_num_in_range();

	if (m_NextFadeEvent) 
	{
		float newvalue = static_cast<zz_particle_event_velocityx *>(m_NextFadeEvent)->GetValue().get_random_num_in_range();
		float timedelta = m_NextFadeEvent->GetActualTime() - GetActualTime();
		if (timedelta == 0) timedelta = 1; // prevent divide by zero errors
		part.m_vDirStep.x = (newvalue - part.m_vDir.x) / timedelta;
		//part.m_vDirStep.x = newvalue - part.m_vDir.x / timedelta;
	}
}

//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_velocityy
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Save, Load method..
bool zz_particle_event_velocityy::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event_velocityy::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_VelocityY.m_min) );
	fs->read_float( (m_VelocityY.m_max) );	  

	m_VelocityY.m_min *= ZZ_SCALE_IN;
	m_VelocityY.m_max *= ZZ_SCALE_IN;

	return true;
}

void zz_particle_event_velocityy::DoItToIt( zz_particle &part )
{
	if (!IsFade()) part.m_vDir.y = m_VelocityY.get_random_num_in_range();

	if (m_NextFadeEvent) 
	{
		float newvalue = static_cast<zz_particle_event_velocityy *>(m_NextFadeEvent)->GetValue().get_random_num_in_range();
		float timedelta = m_NextFadeEvent->GetActualTime() - GetActualTime();
		if (timedelta == 0) timedelta = 1; // prevent divide by zero errors
		part.m_vDirStep.y = (newvalue - part.m_vDir.y) / timedelta;
		//part.m_vDirStep.y = newvalue - part.m_vDir.y / timedelta;
	}
}



//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_velocityz
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Save, Load method..
bool zz_particle_event_velocityz::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event_velocityz::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_VelocityZ.m_min) );
	fs->read_float( (m_VelocityZ.m_max) );

	m_VelocityZ.m_min *= ZZ_SCALE_IN;
	m_VelocityZ.m_max *= ZZ_SCALE_IN;

	return true;
}

void zz_particle_event_velocityz::DoItToIt( zz_particle &part )
{
  if (!IsFade()) part.m_vDir.z = m_VelocityZ.get_random_num_in_range();

  if (m_NextFadeEvent) 
  {
    float newvalue = static_cast<zz_particle_event_velocityz *>(m_NextFadeEvent)->GetValue().get_random_num_in_range();
    float timedelta = m_NextFadeEvent->GetActualTime() - GetActualTime();
    if (timedelta == 0) timedelta = 1; // prevent divide by zero errors
	part.m_vDirStep.z = (newvalue - part.m_vDir.z) / timedelta;
    //part.m_vDirStep.z = newvalue - part.m_vDir.z / timedelta;
  }
}



//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_event_timer
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Save, Load method..
bool zz_particle_event_event_timer::Save( zz_vfs * fs )
{
	return true;
}

bool zz_particle_event_event_timer::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_EventTimer.m_min) );
	fs->read_float( (m_EventTimer.m_max) );	  	  	

	return true;
}

void zz_particle_event_event_timer::DoItToIt(zz_particle &part)
{
	part.m_fEventTimer = m_EventTimer.get_random_num_in_range();
}

bool zz_particle_event_rotation::Save( zz_vfs * fs )
{
	return true;
}


bool zz_particle_event_rotation::Load( zz_vfs * fs )
{
	zz_particle_event::Load( fs );

	fs->read_float( (m_Rotation.m_min) );
	fs->read_float( (m_Rotation.m_max) );
	
	return true;
}

void zz_particle_event_rotation::DoItToIt( zz_particle &part )
{
  if (!IsFade()) part.m_fAngleStep = m_Rotation.get_random_num_in_range();

  if (m_NextFadeEvent) 
  {
    float newvalue = static_cast<zz_particle_event_rotation *>(m_NextFadeEvent)->GetRotation().get_random_num_in_range();
    float timedelta = m_NextFadeEvent->GetActualTime() - GetActualTime();
    if (timedelta == 0) timedelta = 1; // prevent divide by zero errors
	part.m_fAngleStep = (newvalue - part.m_fAngle) / timedelta;
    //part.m_vDirStep.z = newvalue - part.m_vDir.z / timedelta;
  }
  part.m_bAngleOnOff = true;

}