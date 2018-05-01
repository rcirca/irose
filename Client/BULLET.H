/*
	$Header: /Client/BULLET.H 29    04-11-25 6:26p Jeddli $
*/
#ifndef	__BULLET_H
#define	__BULLET_H
#include "IO_Basic.h"
#include "IO_Effect.h"
#include "Object.h"
//-------------------------------------------------------------------------------------------------


#define	BULLET_MOVE_DIRECTION	1
#define	BULLET_MOVE_PARABOLA	2
#define	BULLET_MOVE_IMMEDIATE	3
#define	BULLET_MOVE_GUIDED		4

///
/// CBullet class, inherited from CEffect
/// 
class CBullet : public CEffect 
{
private:
public :
	CBullet ()	{	m_dwHitTIME = 0;	}


	short			m_nBulletNO;
	short			m_nSourOBJ;
	short			m_nDestOBJ;
	DWORD			m_dwHitTIME;
	float			m_fSpeed;

	D3DXVECTOR3		m_PosCUR;
	D3DXVECTOR3		m_PosFROM;
	D3DXVECTOR3		m_PosTO;

	bool			m_bUnlinkAll;

	/// is skill bullet?
	int				m_iSkillIDX;

	/// is Dummy bullet?
	bool			m_bDummyBullet;

	/// 스킬사용으로 발생한 총알의 경우는.. 스킬번호를 세팅해준다..
	void			SetSkillIDX( int iSkillIDX ){ m_iSkillIDX = iSkillIDX; }

	

	char *			Make_ZNAME(int iCreateOrder, short nPartIdx)	{	return CStr::Printf (NAME_BULLET, iCreateOrder, nPartIdx);	}

	virtual bool	Init ( CObjCHAR *pSour, CObjCHAR *pDest, int iEffectIDX, bool bLinkToPoint = false, int iPointNO = 0, bool bDummy = false, int iSkillIDX = 0 )=0{ *(int*)0 = 10; };
	virtual bool	Init ( CObjCHAR *pSour, D3DXVECTOR3 PosTo, int iEffectIDX, bool bLinkToPoint = false, int iPointNO = 0, bool bDummy = false, int iSkillIDX = 0 )=0{ *(int*)0 = 10; };
	virtual int		Proc ()=0{ *(int*)0 = 10; };
} ;

///
/// 직선 이동 총알
///
class CBulletDIRECTION : public CBullet
{
private:
	void			Cal_MoveVEC (CObjCHAR *pDest);

public :
	D3DXVECTOR3		m_MoveVEC;
	int				m_iDistance;

	bool			Init ( CObjCHAR *pSour, CObjCHAR *pDest, int iEffectIDX, bool bLinkToLeftHand = false, int iPointNO = 0, bool bDummy = false, int iSkillIDX = 0 );
	bool			Init ( CObjCHAR *pSour, D3DXVECTOR3 PosTo, int iEffectIDX, bool bLinkToLeftHand = false, int iPointNO = 0, bool bDummy = false, int iSkillIDX = 0 ){ return 0; };
	int				Proc ();
} ;


///
/// 직선 이동 지역 타겟
///
class CBulletToPosition : public CBullet
{
private:
	void			Cal_MoveVEC ( D3DXVECTOR3 TargetPos );

public :
	D3DXVECTOR3		m_MoveVEC;
	int				m_iDistance;

	bool			Init ( CObjCHAR *pSour, CObjCHAR *pDest, int iEffectIDX, bool bLinkToLeftHand = false, int iPointNO = 0, bool bDummy = false, int iSkillIDX = 0 ){return 0;};
	bool			Init ( CObjCHAR *pSour, D3DXVECTOR3 TargetPos, int iEffectIDX, bool bLinkToLeftHand = false, int iPointNO = 0, bool bDummy = false, int iSkillIDX = 0 );
	int				Proc ();
} ;


///
/// 직격 총알
///
class CBulletIMMEDIATE : public CBullet
{
private:
public:
} ;



///
/// 포물선 이동 총알
///
class CBulletPARABOLA : public CBullet
{
private:
	float			m_vZ0;
	float			m_vZ;
	
	D3DXVECTOR3		m_DisplacementVec;
	float			m_fDistance;
	float			m_fArrivedTime;
	DWORD			m_dwElapsedTime;



	void			Cal_MoveVEC ( CObjCHAR *pDest );

public :
	CBulletPARABOLA();
	~CBulletPARABOLA();


	D3DXVECTOR3		m_MoveVEC;
	int				m_iDistance;

	bool			Init ( CObjCHAR *pSour, CObjCHAR *pDest, int iEffectIDX, bool bLinkToLeftHand = false, int iPointNO = 0, bool bDummy = false, int iSkillIDX = 0 );
	bool			Init ( CObjCHAR *pSour, D3DXVECTOR3 PosTo, int iEffectIDX, bool bLinkToLeftHand = false, int iPointNO = 0, bool bDummy = false, int iSkillIDX = 0 );
	int				Proc ();

	
} ;



///
/// Bullet Manager class
///
class CBulletMANAGER
{
private:
	DWORD	m_dwCurTime;
	DWORD	m_dwPassTime;

	int		m_iCreateOrder;

	classDLLIST< CBullet* >	m_LIST;

	static CBulletMANAGER* m_pInstance;
	CBulletMANAGER ();
	~CBulletMANAGER ();

public :
	static CBulletMANAGER* Instance ();
	void   Destroy ();

	DWORD  GetCurTime ()		{	return	m_dwCurTime;	}
	DWORD  GetPassTime ()		{	return	m_dwPassTime;	}

	CBullet*Add_BULLET( CObjCHAR *pSour, CObjCHAR *pDest, int iEffectIDX,  bool bLinkToPoint = false, int iPointNO = 0, D3DXVECTOR3 PosTo = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), bool bUseTargetPos = false, bool bDummy = false, int iSkillIDX = 0 );
	void	ProcBULLET ();
} ;
extern CBulletMANAGER *g_pBltMGR;

//-------------------------------------------------------------------------------------------------
#endif
