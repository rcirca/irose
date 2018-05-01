/*
	$Header: /Client/CCamera.h 17    04-03-31 9:49p Jeddli $
*/
#ifndef	__CCAMERA_H
#define	__CCAMERA_H
//-------------------------------------------------------------------------------------------------

// #define	__USE_CAMERA

class CCamera {
private :
	static	CCamera *m_pInstance;

	HNODE		m_hNODE;

#ifndef	_DEBUG
	D3DVECTOR	m_Angle;
	D3DVECTOR	m_PosAT;
	D3DVECTOR	m_PosEYE;
#endif

	
	


	CCamera ();
	~CCamera ();

public  :
	static CCamera* Instance ();
	void   Destroy ();

	float		m_fDistance;		// distace eye~at
	float		m_fYAW;
	float		m_fPITCH;
	bool		m_bFollowMode;


	HNODE		m_hMotion;


#ifdef	_DEBUG
	D3DVECTOR	m_Angle;
	D3DVECTOR	m_PosAT;
	D3DVECTOR	m_PosEYE;
#endif

	///
	/// sight info table
	///
	
	void		SetSightInfo( int iInfoIdx );

	///
	/// Reset other camera.. and set avatar camera
	///
	void		ResetToAvatarCamera();

	
	void		Init (HNODE hNODE);
	HNODE		GetZHANDLE ()	 const							{	return m_hNODE;	}


	void		Set_Distance (float fDistance)					{	m_fDistance = fDistance;	}
	void		Add_Distance (float fDistance);
//#ifdef	_DEBUG
	D3DVECTOR	Get_Position (void);
//#else
//	D3DVECTOR	Get_Position (void)								{	return this->m_PosAT;		}
//#endif
	void		Set_Position (D3DVECTOR &Pos)					{	Set_Position(Pos.x, Pos.y, Pos.z);	}
	void		Set_Position (float fX, float fY, float fZ);
	void		Set_YPosition (float fY)						{	m_PosAT.y = fY;				}

	void		LookAt( float eyeX, float eyeY, float eyeZ,
						float centerX, float centerY, float centerZ,
						float upX, float upY, float upZ );

	void		Move (float fDeviationX, float fDeviationY, float fDeviationZ)	{	::moveCamera ( m_hNODE, fDeviationX, fDeviationY, fDeviationZ );	}
	void		Attach( HNODE hModel );
	void		Detach()				{	::cameraDetachTarget(m_hNODE);		}
	void		Update()				{	::cameraUpdate(m_hNODE);			}
	void		Add_YAW(short nMovement);
	void		Add_PITCH (short nMovement);

	bool		Toggle_FloowMODE();



	//////////////////////////////////////////////////////////////////////////////////////////
	/// Not attached to avt camera

	/// set motion to camera
	void		SetMotion( const char* strMotion );

	/// set camera property
	//void		SetCameraProperty( D3DVECTOR
} ;

extern CCamera	*g_pCamera;

//-------------------------------------------------------------------------------------------------
#endif
