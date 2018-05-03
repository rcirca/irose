/*
	$Header: /Client/IO_Effect.h 40    04-11-01 9:55p Jeddli $
*/
#ifndef	__IO_EFFECT_H
#define	__IO_EFFECT_H

//#include "CObjCHAR.h"

#include "util\DLLIST.h"
#include "util\classHASH.h"

typedef unsigned int HNODE;


/////////////////////////////////////////////////////////////////////////////////////
/// CParticle
/// Link( m_hVisible <- m_hParticle )
/////////////////////////////////////////////////////////////////////////////////////
class CParticle 
{
private:
	HNODE	m_hVisible;
	HNODE	m_hParticle;

public :

	CParticle ()		{	m_hVisible=NULL,m_hParticle=NULL;	}
	~CParticle ();

	HNODE GetPARTICLE()	{	return m_hParticle;		}
	HNODE GetVISIBLE ()	{	return m_hVisible;		}

	bool RegisterToNZIN (char *szName, char *szParticleFile);
	void UnregisterFromNZINE (void);
} ;



/////////////////////////////////////////////////////////////////////////////////////
/// CMeshEffect
/// Link( m_hVisible <- m_hAnimatable <- m_hMorpher )
/////////////////////////////////////////////////////////////////////////////////////
class CMeshEffect {
private:
	HNODE	m_hVisible;
	HNODE	m_hAnimatable;
	HNODE	m_hMorpher;

public :
	CMeshEffect ()			{	m_hVisible=NULL,m_hAnimatable=NULL,m_hMorpher=NULL;	}
	~CMeshEffect ();				

	HNODE GetANIMATABLE()	{	return m_hAnimatable;	} 
	HNODE GetMORPHER()		{	return m_hMorpher;		}
	HNODE GetVISIBLE ()		{	return m_hVisible;		}


	bool RegisterToNZIN (HNODE hMesh, HNODE hMotion, HNODE hMaterial);
	void UnregisterFromNZINE (void);
} ;

/////////////////////////////////////////////////////////////////////////////////////
/// tagEFFECT_PARTICLE
/// Add in attribute for particle
/////////////////////////////////////////////////////////////////////////////////////
struct tagEFFECT_PARTICLE 
{
	CStrVAR			m_FileName;
	bool			m_bUseAniFile;
	t_HASHKEY		m_HashBoneANI;

	D3DVECTOR		m_Transform;
	D3DXQUATERNION	m_Rotate;

	int				m_iAniLoopCnt;
	int				m_iDelay;
	int				m_iIsLink;
} ;

/////////////////////////////////////////////////////////////////////////////////////
/// tagEFFECT_MESHANI
/// Add in attribute for Mesh effect
/////////////////////////////////////////////////////////////////////////////////////
struct tagEFFECT_MESHANI 
{
	t_HASHKEY		m_HashMESH;
	t_HASHKEY		m_HashMeshANI;
	t_HASHKEY		m_HashMAT;

	bool			m_bUseAniFile;
	t_HASHKEY		m_HashBoneANI;

	int				m_iZTest;
	int				m_iAlphaTest;
	int				m_iZWrite;

	int				m_iBlendSour;
	int				m_iBlendDest;
	int				m_iBlendOP;

	D3DVECTOR		m_Transform;
	D3DXQUATERNION	m_Rotate;

	int				m_iAniLoopCnt;
	int				m_iDelay;
	int				m_iRepeatCNT;
	int				m_iIsLink;
} ;



/////////////////////////////////////////////////////////////////////////////////////
/// CEffectDATA
/// 실제로 데이터를 보관하고 있고,
/// 생성은 CFileList 메카니즘에 의해 생성된다.
/////////////////////////////////////////////////////////////////////////////////////
class CEffectDATA 
{
public :
	CStrVAR		m_Name;

	t_HASHKEY	m_SoundHash;

	int			m_iSoundLoopCNT;	// 0 -> loop !!
	bool		m_bUseSoundFile;

	int			m_iParticleCNT;
	int			m_iMeshAniCNT;

	tagEFFECT_PARTICLE	*m_pParticle;
	tagEFFECT_MESHANI	*m_pMeshAni;

	CEffectDATA ();
	~CEffectDATA ();

	bool Load (char *szFileName);
	void Free (void);
} ;


class CObjCHAR;
/////////////////////////////////////////////////////////////////////////////////////
/// CEffect
/////////////////////////////////////////////////////////////////////////////////////
class CEffect 
{
private:
	HNODE		 m_hRootNODEs[ 2 ];	// 0 linked effects, 1 unlinked effects
	bool		 m_bIsVisible;

	HNODE		 m_hParent;
	CObjCHAR*	 m_pParent;			// 링크가 걸린 부모..

	bool		 m_bNightEffect;	/// 밤에만 나오는 이펙트 인가?

	static	int	 m_iEffectCnt;		/// 생성된 전체 이펙트의 개수

public:
	void		SetParentCHAR( CObjCHAR* pParent ){ m_pParent = pParent; }
	CObjCHAR*	GetParentCHAR(){ return m_pParent; }

protected:
	CEffectDATA *m_pEffectDATA;
	CParticle   *m_pParticle;
	CMeshEffect *m_pMeshEffect;

public :
	classDLLNODE< CEffect* > *m_pListNODE;

	CEffect ();
	CEffect ( CEffectDATA *pEffectDATA );
	~CEffect ();

	HNODE	GetZNODE ()							{	return m_hRootNODEs[ 0 ];	}
	HNODE	GetParentNODE()						{   return m_hParent; }
	bool	Create (CEffectDATA *pEffectDATA);

	bool	IsIntersect (HNODE hMODEL);
	void	InsertToScene ();
	void	RemoveFromScene ();
	
	void	StartEffect();
	void	StopEffect();

	void	SetNightEffect( bool bNightEffect ){ m_bNightEffect = bNightEffect; }


	void	LinkBONE (HNODE hMODEL, short nBone)
	{	
		::linkBone( hMODEL, m_hRootNODEs[ 0 ], nBone );
		::linkBone( hMODEL, m_hRootNODEs[ 1 ], nBone );
		::unlinkVisibleWorld ( m_hRootNODEs[ 1 ] );

		m_hParent = hMODEL;
	}

	void	LinkNODE (HNODE hNODE)
	{	
		::linkNode( hNODE, m_hRootNODEs[ 0 ] );
		::linkNode( hNODE, m_hRootNODEs[ 1 ] );
		::unlinkVisibleWorld ( m_hRootNODEs[ 1 ] );

		StartEffect();

		m_hParent = hNODE;
	}

	void	LinkDUMMY (HNODE hNODE, int iPointNO )
	{	
		::linkDummy( hNODE, m_hRootNODEs[ 0 ], iPointNO );
		::linkDummy( hNODE, m_hRootNODEs[ 1 ], iPointNO );
		::unlinkVisibleWorld ( m_hRootNODEs[ 1 ] );

		m_hParent = hNODE;
	}

	void	Transform (D3DVECTOR &Position)	
	{	
		::setPosition( m_hRootNODEs[ 0 ], Position.x, Position.y, Position.z );
		::setPosition( m_hRootNODEs[ 1 ], Position.x, Position.y, Position.z );
	}

	void	Rotation ( D3DXQUATERNION &quat )	
	{	
		::setQuaternion( m_hRootNODEs[ 0 ], quat.w, quat.x, quat.y, quat.z  );
		::setQuaternion( m_hRootNODEs[ 1 ], quat.w, quat.x, quat.y, quat.z );
	}
	void    Rotation( D3DXVECTOR3 &vec );


	void	Scale ( D3DVECTOR &vecScale )	
	{	
		::setScale( m_hRootNODEs[ 0 ], vecScale.x, vecScale.y, vecScale.z  );		
		::setScale( m_hRootNODEs[ 1 ], vecScale.x, vecScale.y, vecScale.z  );		
	}

	void	UnlinkNODE (void)					
	{	
		::unlinkNode( m_hRootNODEs[ 0 ] );				
		StopEffect();
	}
	void	LinkVisibleWorld (HNODE hNODE)		{	::linkVisibleWorld( hNODE, m_hRootNODEs[ 0 ] );	}
	void	UnlinkVisibleWorld (void)			{	::unlinkVisibleWorld ( m_hRootNODEs[ 0 ] );		}

	void	SetPOSITION (D3DVECTOR &Position)	{	::setPosition( m_hRootNODEs[ 0 ], Position.x, Position.y, Position.z );	}
	int		GetPOSITION (D3DVECTOR* Position)	{	return ::getPosition( m_hRootNODEs[ 0 ], (float*)Position ); } 

	bool	IsFinish (void);
} ;



/////////////////////////////////////////////////////////////////////////////////////
/// CEffectLIST
/// 직접 hash 테이블을 관리하고, CFileList 의 기능을 대신한다.
/////////////////////////////////////////////////////////////////////////////////////
class CEffectLIST 
{
private :
	classHASH  < CEffectDATA* >	m_HASH;
	classDLLIST< CEffect* >		m_LIST;

	short		m_nFileEffectCNT;
	t_HASHKEY  *m_pHashKEY;						/// FILE_EFFECT.STb 의 리스트를 위한 배열이다.

public  :
	CEffectLIST (char *szFileEFFECT);
	~CEffectLIST ();

	t_HASHKEY	 Add_EffectFILE(char *szEffectFile);
	CEffectDATA *Get_EffectDATA( t_HASHKEY uiHashKEY );

	CEffect		*Add_EFFECT (t_HASHKEY uiHashKEY, bool bAdd2LIST=false);
	void		 Del_EFFECT(CEffect *pCEffect, bool bUpdateParent = true );

	CEffect		*Add_EffectWithIDX (short nIndex, bool bAdd2LIST=false);
	t_HASHKEY	 Get_IndexHashKEY (short nIndex);

	CEffect		*Get_EffectClassWithHashKEY( t_HASHKEY uiHashKEY );

	short		 GetFileCNT ()	{	return m_nFileEffectCNT;	}

	int  GetCount ()	{	return m_LIST.GetNodeCount ();	}
	void Proc ();
} ;


extern CEffectLIST	*g_pEffectLIST;

//-------------------------------------------------------------------------------------------------
#endif
