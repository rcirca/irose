/*
	$Header: /Client/CObjFIXED.h 22    04-08-26 12:11p Jeddli $
*/
#ifndef	__COBJFIXED_H
#define	__COBJFIXED_H
#include "CGameOBJ.h"
#include "IO_Model.h"
#include "IO_Effect.h"

///
/// 건물, 나무, 풀, 바위, 총알.....
/// inherited from CGameOBJ 
///

class CObjFIXED : public CGameOBJ
{
protected:
	CMODEL<CFixedPART>		*m_pMODEL;
	HNODE					*m_pHNODES;

	t_HASHKEY				*m_pLightMapMaterial;
	CEffect					**m_ppEFFECT;


	///
	/// ..txt 파일에서 부모, 차일드. .. 등.. 정렬된 라이트맵 데이터에 따른 순서에 입각하여, 실제 파트를 구한다.
	///
	short					GetPartIndex (short nPartSeq);
	bool					CreatePart (int iCreateOrder, D3DVECTOR &Position, short nPartIdx);


public   :
	CObjFIXED ()		
	{	
		m_pMODEL=NULL, m_pHNODES=NULL, m_bIsVisible=false;
		m_pLightMapMaterial=NULL,m_ppEFFECT=NULL;	
	}
	virtual ~CObjFIXED ()		{	this->Delete ();			}



	static DWORD			m_dwCreateOrder;


	union 
	{
		short	m_nCnstIdx;
		short	m_nTreeIdx;
	} ;


	
	/////////////////////////////////////////////////////////////////////////////////////////
	/// < Inherited form GameObj virtual functions	

	/// 현재 view frustum 안에 있는가?
	/*override*/ virtual bool			IsInViewFrustum();
	/*override*/ virtual void			InsertToScene (void);
	/*override*/ virtual void			RemoveFromScene (bool bIncludeEFFECT=true);

	/// < end
	//////////////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////////////////////
	/// > item 이나 기타 하위 객체들에서 뭔가 추가적으로 할일이 있다.
	///
	virtual char*			Make_ZNAME(int iCreateOrder, short nPartIdx)=0{ *(int*)0 = 10; };

	virtual void			Delete ();

	/// > end
	//////////////////////////////////////////////////////////////////////////////////////////




	/// 루트 노드 핸들을 구한다.
	HNODE					GetRootZNODE ()		{	return m_pHNODES[ m_pMODEL->m_nRootPART ];	}
	
	void					LinkNODE (HNODE hTarget, CPointPART *pDummyPoint);
	void					UnlinkVisibleWorld (void);
	void					LinkToModel( HNODE hModel );

	void					GetScreenPOS ( D3DVECTOR &PosSCR);
	

	void					Scale (D3DVECTOR &Scale);
	void					Rotate (D3DXQUATERNION &Rotate);
	bool					IsIntersect (float &fCurDistance);
	bool					IsIntersectForCamera(float &fCurDistance );

	/// 라이트맵 세팅..
	void					SetLightMap (short nPartIdx, char *szLightMapFile, int iXPos, int iYPos, int iWidth, int iHeight);


	/// 이벤트 오브젝트등은 오버라이딩 한다.
	virtual void			SetPOSITION (D3DVECTOR &Position);
	virtual bool			Create (CMODEL<CFixedPART> *pMODEL, D3DVECTOR &Position, bool bCreateEffect=false);
} ;

//-------------------------------------------------------------------------------------------------
#endif	

