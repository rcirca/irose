/*
	$Header: /Client/CViewMSG.cpp 21    04-09-19 8:13p Zho $
*/
#include "stdAFX.h"
#include "CViewMSG.h"
#include "Game.h"
#include "Object.h"
#include "Interface\\CUIMediator.h"
#include "Interface/CTDrawImpl.h"
#include "Interface/it_mgr.h"
#include "interface/IO_ImageRes.h"

CViewMSG* CViewMSG::m_pInstance = NULL;

//-------------------------------------------------------------------------------------------------
CViewMSG::CViewMSG()
{
	m_nRegObjCNT = 0;
}

CViewMSG::~CViewMSG ()
{
}

CViewMSG* CViewMSG::Instance ()
{
	if ( !m_pInstance ) 
		m_pInstance = new CViewMSG;

	return m_pInstance;
}

void CViewMSG::Clear ()
{
	m_nRegObjCNT = 0;
}

void CViewMSG::AddObjIDX (short nCliObjIDX)
{
	if ( m_nRegObjCNT >= MAX_VIEWMSG_OBJCNT )
		return;

	m_nRegObjIDX[ m_nRegObjCNT++ ] = nCliObjIDX;
}


//-------------------------------------------------------------------------------------------------
/// 캐릭터 아이디를 표시한다..
void CViewMSG::Draw ()
{	

	// do not disable zwirte.
	// if disable zwrite, characte id can be overlapped other id rect.
	//reinterpret_cast<LPD3DDEVICE>( getDevice () )->SetRenderState( D3DRS_ZWRITEENABLE, false );

	CObjCHAR *pCharOBJ;
	
	for (short nI=0; nI<m_nRegObjCNT; nI++) 
	{
		pCharOBJ = (CObjCHAR*)g_pObjMGR->Get_OBJECT( m_nRegObjIDX[ nI ] );
		if ( !pCharOBJ )
			continue;		

		switch( pCharOBJ->Get_TYPE() )
		{
			case OBJ_AVATAR:				
			case OBJ_USER:
			case OBJ_NPC:
			case OBJ_MOB:
				g_UIMed.DrawAvataName( pCharOBJ );
				break;			
			
			/*else
			{
				D3DVECTOR   PosSCR;
				pCharOBJ->GetScreenPOS (PosSCR);	
				CTDrawImpl* pDrawObj = g_itMGR.GetDrawImplPtr();
				pDrawObj->Draw( PosSCR.x , PosSCR.y , PosSCR.z,IMAGE_RES_UI, 32 );
			}*/
		}
	}

	Clear ();

	DrawItemInfoView();

	// no need to zwrite true. it's default
	//reinterpret_cast<LPD3DDEVICE>( getDevice () )->SetRenderState( D3DRS_ZWRITEENABLE, true );
}
//-------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------	
/// @brief 아이템 정보
//----------------------------------------------------------------------------------------------------

void CViewMSG::ResetItemInfoView()
{
	m_ItemInfoView.Reset();
}

void CViewMSG::AddItemToInfoView( CObjITEM* pItem )
{
	m_ItemInfoView.PushDropItemInfo( pItem );
}

void CViewMSG::DrawItemInfoView()
{
	m_ItemInfoView.Draw();
}