/** 
 * @file zz_particle_event.h
 * @brief particle event class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    30-apr-2003
 *
 * $Header: /engine/include/zz_particle_event.h 2     05-12-24 10:23p Choo0219 $
 * $History: zz_particle_event.h $
 * 
 * *****************  Version 2  *****************
 * User: Choo0219     Date: 05-12-24   Time: 10:23p
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
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_PARTICLE_EVENT_H__
#define __ZZ_PARTICLE_EVENT_H__

#ifndef __ZZ_PARTICLE_H__
#include "zz_particle.h"
#endif

#ifndef __ZZ_MINMAX_H__
#include "zz_minmax.h"
#endif

#include <algorithm>
#include <functional>
#include <stdio.h>

class zz_vfs;

enum EVENT_TYPE
{
	EVENT_NONE = 0,
	EVENT_SIZE = 1,
	EVENT_EVENTTIMER = 2,
	EVENT_REDCOLOR = 3,
	EVENT_GREENCOLOR = 4,
	EVENT_BLUECOLOR = 5,
	EVENT_ALPHA = 6,
	EVENT_COLOR = 7,
	EVENT_VELOCITYX = 8,
	EVENT_VELOCITYY = 9,
	EVENT_VELOCITYZ = 10,
	EVENT_VELOCITY = 11,
	EVENT_TEXTURE = 12,
    EVENT_ROTATION =13,
};

#define MAKE_STR( X ) #X

//--------------------------------------------------------------------------------
class zz_particle_event
{
public:
	int type;

	zz_particle_event() : type(EVENT_NONE) { m_NextFadeEvent = NULL; }
	virtual ~zz_particle_event() { }

	virtual void RestoreDeviceObjects( LPDIRECT3DDEVICE9 pDev ){};
	virtual void InvalidateDeviceObjects(){};

	
	// Attributes
	zz_minmax<float>	GetTimeRange(void) const { return(m_TimeRange); }
	void			SetTimeRange(const zz_minmax<float> data) { m_TimeRange = data; }

	float			GetActualTime(void) const { return(m_ActualTime); }
	void			SetActualTime(const float data) { m_ActualTime = data; }

	bool			IsFade(void) const { return(m_bFade); }
	void			SetFade(const bool data = true) { m_bFade = data; }
	    

	virtual void	DoItToIt( zz_particle &part ) = 0;
	virtual bool	FadeAllowed() = 0;

	
	zz_particle_event *m_NextFadeEvent;


  //----------------------------------------------------------------------------
  // Save, Load method..
  virtual bool Save( zz_vfs * fs );
  virtual bool Load( zz_vfs * fs );
  


protected:  
	zz_minmax<float>		m_TimeRange;
	float				m_ActualTime;
	bool				m_bFade;
};




//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_compare_func
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_compare_func : public std::binary_function<zz_particle_event *, zz_particle_event *, bool> 
{
public:
	bool operator() (const zz_particle_event* lhs, const zz_particle_event* rhs) const 
	{
		return lhs->GetActualTime() < rhs->GetActualTime();
	}
};


//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_size
//////////////////////////////////////////////////////////////////////////////
// Texture Event
class zz_particle_event_texture : public zz_particle_event
{
public:
	zz_particle_event_texture() : zz_particle_event() { type = EVENT_TEXTURE; }

	bool FadeAllowed() { return(true); }
	void DoItToIt(zz_particle &part);
	
	zz_minmax<float> GetTextureIdx(void) const { return( m_TextureIdx ); }
	void SetTextureIdx(const zz_minmax<float>& TextureIdx ) { m_TextureIdx = TextureIdx; }

	//----------------------------------------------------------------------------
	// Save, Load method..
	virtual bool Load( zz_vfs * fs );
	virtual bool Save( zz_vfs * fs );

protected:
	zz_minmax<float> m_TextureIdx;
};


//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_size
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_size : public zz_particle_event
{
public:
	zz_particle_event_size() : zz_particle_event() { type = EVENT_SIZE; }

	bool			FadeAllowed() { return(true); }
	void			DoItToIt(zz_particle &part);
	
	zz_minmax<vec3>	GetSize(void) const { return(m_Size); }
	void			SetSize(const zz_minmax<vec3> data) { m_Size = data; }

	//----------------------------------------------------------------------------
	// Save, Load method..
	virtual bool Save( zz_vfs * fs );
	virtual bool Load( zz_vfs * fs );
	

protected:
	zz_minmax<vec3>		m_Size;   
};




//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_color
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_color : public zz_particle_event
{
public:
	zz_particle_event_color() : zz_particle_event() { type = EVENT_COLOR; }

	bool				FadeAllowed() { return(true); }
	void				DoItToIt(zz_particle &part);
	
	zz_minmax<vec4>	GetColor(void) const { return(m_Color); }
	void				SetColor(const zz_minmax<vec4> &data) { m_Color = data; }

	//----------------------------------------------------------------------------
	// Save, Load method..
	virtual bool Save( zz_vfs * fs );
	virtual bool Load( zz_vfs * fs );	

protected:
	zz_minmax<vec4>			m_Color;
};




//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_red_color
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_red_color : public zz_particle_event
{
public:
	zz_particle_event_red_color() : zz_particle_event() { type = EVENT_REDCOLOR; }

	bool				FadeAllowed() { return(true); }
	void				DoItToIt(zz_particle &part);
	
	zz_minmax<float>		GetRedColor(void) const { return(m_RedColor); }
	void				SetRedColor(const zz_minmax<float> data) { m_RedColor = data; }

	//----------------------------------------------------------------------------
	// Save, Load method..
	virtual bool Save( zz_vfs * fs );
	virtual bool Load( zz_vfs * fs );	


protected:
	zz_minmax<float>			m_RedColor;
};




//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_green_color
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_green_color : public zz_particle_event
{
public:
	zz_particle_event_green_color() : zz_particle_event() { type = EVENT_GREENCOLOR; }

	bool						FadeAllowed() { return(true); }
	void						DoItToIt(zz_particle &part);
	  
	zz_minmax<float>			GetGreenColor(void) const { return(m_GreenColor); }
	void						SetGreenColor(const zz_minmax<float> data) { m_GreenColor = data; }

	//----------------------------------------------------------------------------
	// Save, Load method..
	virtual bool Save( zz_vfs * fs );
	virtual bool Load( zz_vfs * fs );

protected:
	zz_minmax<float>				m_GreenColor;
};



//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_blue_color
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_blue_color : public zz_particle_event
{
public:
	zz_particle_event_blue_color() : zz_particle_event() { type = EVENT_BLUECOLOR; }

	bool						FadeAllowed() { return(true); }
	void						DoItToIt(zz_particle &part);
	  
	zz_minmax<float>			GetBlueColor(void) const { return(m_BlueColor); }
	void						SetBlueColor(const zz_minmax<float> data) { m_BlueColor = data; }


  //----------------------------------------------------------------------------
  // Save, Load method..
	virtual bool Save( zz_vfs * fs );
	virtual bool Load( zz_vfs * fs );

protected:
	zz_minmax<float>			m_BlueColor;
};

//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_alpha
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_alpha : public zz_particle_event
{
public:
	zz_particle_event_alpha() : zz_particle_event() { type = EVENT_ALPHA; }

	bool						FadeAllowed() { return(true); }
	void						DoItToIt(zz_particle &part);
	  
	zz_minmax<float>			GetAlpha(void) const { return(m_Alpha); }
	void						SetAlpha(const zz_minmax<float> data) { m_Alpha = data; }


	//----------------------------------------------------------------------------
	// Save, Load method..
	virtual bool Save( zz_vfs * fs );
	virtual bool Load( zz_vfs * fs );
	  

protected:
	zz_minmax<float>				m_Alpha;
};


//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_velocity
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_velocity : public zz_particle_event
{
public:
	zz_particle_event_velocity() : zz_particle_event() { type = EVENT_VELOCITY; }

	bool							FadeAllowed() { return(true); }
	void							DoItToIt(zz_particle &part);
	  
	zz_minmax<vec3>			GetVelocity(void) const { return(m_Velocity); }
	void							SetVelocity(const zz_minmax<vec3> &data) { m_Velocity = data; }

	//----------------------------------------------------------------------------
	// Save, Load method..
	virtual bool Save( zz_vfs* fs );
	virtual bool Load( zz_vfs * fs );
	  

protected:
	zz_minmax<vec3>		m_Velocity;
};


//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_velocityx
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_velocityx : public zz_particle_event
{
public:
	zz_particle_event_velocityx() : zz_particle_event() { type = EVENT_VELOCITYX; }

	bool							FadeAllowed() { return(true); }
	void							DoItToIt(zz_particle &part);
	  
	zz_minmax<float>				GetValue(void) const { return(m_VelocityX); }
	void							SetValue(const zz_minmax<float> &data) { m_VelocityX = data; }


	//----------------------------------------------------------------------------
	// Save, Load method..
	virtual bool Save( zz_vfs * fs );
	virtual bool Load( zz_vfs * fs );  

protected:
	zz_minmax<float>				m_VelocityX;
};



//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_velocityy
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_velocityy : public zz_particle_event
{
public:
	zz_particle_event_velocityy() : zz_particle_event() { type = EVENT_VELOCITYY; }

	bool						FadeAllowed() { return(true); }
	void						DoItToIt(zz_particle &part);
	
	zz_minmax<float>				GetValue(void) const { return(m_VelocityY); }
	void						SetValue(const zz_minmax<float> &data) { m_VelocityY = data; }

	//----------------------------------------------------------------------------
	// Save, Load method..
	virtual bool Save( zz_vfs * fs );
	virtual bool Load( zz_vfs * fs );
	
protected:
	zz_minmax<float>				m_VelocityY;
};



//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_velocityz
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_velocityz : public zz_particle_event
{
public:
	zz_particle_event_velocityz() : zz_particle_event() { type = EVENT_VELOCITYZ; }

	bool						FadeAllowed() { return(true); }
	void						DoItToIt(zz_particle &part);
	
	zz_minmax<float>				GetValue(void) const { return(m_VelocityZ); }
	void						SetValue(const zz_minmax<float> &data) { m_VelocityZ = data; }

	//----------------------------------------------------------------------------
	// Save, Load method..
	virtual bool Save( zz_vfs * fs );	
	virtual bool Load( zz_vfs * fs );
	
protected:
	zz_minmax<float>				m_VelocityZ;
};



//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_event_timer
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_event_timer : public zz_particle_event
{
public:
	zz_particle_event_event_timer() : zz_particle_event() { type = EVENT_EVENTTIMER; }

	bool						FadeAllowed() { return(false); }
	void						DoItToIt(zz_particle &part);
	
	zz_minmax<float>				GetEventTimer(void) const { return(m_EventTimer); }
	void						SetEventTimer(const zz_minmax<float> &data) { m_EventTimer = data; }

	//----------------------------------------------------------------------------
	// Save, Load method..
	virtual bool Save( zz_vfs * fs );
	virtual bool Load( zz_vfs * fs );	

protected:
	zz_minmax<float>				m_EventTimer;
};

//////////////////////////////////////////////////////////////////////////////
// zz_particle_event_rotation
//////////////////////////////////////////////////////////////////////////////

class zz_particle_event_rotation : public zz_particle_event
{
public:
	zz_particle_event_rotation() : zz_particle_event() { type = EVENT_ROTATION; }

	bool							FadeAllowed() { return(true); }
	void							DoItToIt(zz_particle &part);
	  
	zz_minmax<float>			GetRotation(void) const { return(m_Rotation); }
	void						SetRotation(const zz_minmax<float> &data) { m_Rotation = data; }

	//----------------------------------------------------------------------------
	// Save, Load method..
	virtual bool Save( zz_vfs* fs );
	virtual bool Load( zz_vfs * fs );
	  

protected:
	zz_minmax<float>		m_Rotation;
};


#endif // __ZZ_PARTICLE_EVENT_H__