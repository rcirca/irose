/*
	$Header: /Client/CViewMSG.h 4     04-07-08 6:10p Jeddli $
*/
#ifndef	__CViewMSG_H
#define	__CViewMSG_H


#include "GameProc/ItemInfoView.h"


#define	MAX_VIEWMSG_OBJCNT		( MAX_CLIENT_OBJECTS / 2 )
class CViewMSG {
private:
	short	m_nRegObjCNT;
	short	m_nRegObjIDX[ MAX_VIEWMSG_OBJCNT ];


	//----------------------------------------------------------------------------------------------------	
	/// @brief 아이템 정보
	//----------------------------------------------------------------------------------------------------
	CItemInfoView				m_ItemInfoView;



	static CViewMSG	*m_pInstance;
	CViewMSG ();
	~CViewMSG ();

public :
	static CViewMSG *Instance ();
	void Destroy ()				{	SAFE_DELETE( m_pInstance );		}

	void	Clear ();
	void	AddObjIDX( short nCliObjIDX );
	void	Draw ();
	short	GetRegObjCnt(){ return m_nRegObjCNT; }
	short	GetRegObjIdx( int i){ return m_nRegObjIDX[i]; }


	//----------------------------------------------------------------------------------------------------	
	/// @brief 아이템 정보
	//----------------------------------------------------------------------------------------------------
	void	ResetItemInfoView();
	void	AddItemToInfoView( CObjITEM* pItem );
	void	DrawItemInfoView();
} ;

extern CViewMSG *g_pViewMSG;

//-------------------------------------------------------------------------------------------------
#endif
