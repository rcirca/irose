#include "stdafx.h"
#include ".\cupgradedlg.h"
#include "subclass/CUpgradeDlgStateNormal.h"
#include "subclass/CUpgradeDlgStateWait.h"
#include "subclass/CUpgradeDlgStateResult.h"
#include "../../GameData/Event/CTEventUpgrade.h"
#include "../../GameData/CUpgrade.h"
#include "../../GameCommon/Item.h"
#include "../CDragItem.h"
#include "../command/dragcommand.h"
#include "../../object.h"


CUpgradeDlg::CUpgradeDlg( int iType )
{
	m_pStates[STATE_NORMAL] = new CUpgradeDlgStateNormal(this);
	m_pStates[STATE_WAIT]	= new CUpgradeDlgStateWait;
	m_pStates[STATE_RESULT]	= new CUpgradeDlgStateResult(this);

	m_pCurrState = m_pStates[STATE_NORMAL];

	POINT pt;

	pt.x = 169;pt.y = 99;

	m_pDragItemTarget = new CDragItem;
	CTCommand* pCmd = new CTCmdDragTakeoutTargetItemFromUpgradeDlg;
	m_pDragItemTarget->AddTarget( CDragItem::TARGET_ALL, pCmd );
	m_pDragItemTarget->AddTarget( iType, NULL );

	m_TargetItemSlot.SetOffset( pt );
	m_TargetItemSlot.SetParent( iType );
	m_TargetItemSlot.SetDragAvailable();
	m_TargetItemSlot.SetDragItem( m_pDragItemTarget );


	pt.x = 169; pt.y = 172;
	m_pDragItemMaterial = new CDragItem;;

	pCmd = new CTCmdDragTakeoutMaterialItemFromUpgradeDlg;
	m_pDragItemMaterial->AddTarget( CDragItem::TARGET_ALL, pCmd );
	m_pDragItemMaterial->AddTarget( iType, NULL );

	
	for(int i = 0 ; i < 3; ++i )
	{
		m_MaterialSlots[i].SetOffset( pt );
		m_MaterialSlots[i].SetParent( iType );
		m_MaterialSlots[i].SetDragAvailable();
		m_MaterialSlots[i].SetDragItem( m_pDragItemMaterial );

		pt.y += 46;
	}

	SetDialogType( iType );

}

CUpgradeDlg::~CUpgradeDlg(void)
{
	SAFE_DELETE( m_pDragItemTarget );
	SAFE_DELETE( m_pDragItemMaterial );


	SAFE_DELETE( m_pStates[STATE_NORMAL] );
	SAFE_DELETE( m_pStates[STATE_WAIT] );
	SAFE_DELETE( m_pStates[STATE_RESULT] );

	m_pCurrState = NULL;
}

void CUpgradeDlg::ChangeState( int iID )
{
	assert( iID >= 0 && iID < STATE_MAX );
	if(  iID >= 0 && iID < STATE_MAX   )
	{
		m_pCurrState->Leave();
		m_pCurrState = m_pStates[iID];
		m_pCurrState->Enter();
	}
}

void CUpgradeDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );


	if( CUpgrade::GetInstance().GetType() == CUpgrade::TYPE_NPC )
	{
		WORD wNpcSvrIdx = CUpgrade::GetInstance().GetNpcSvrIdx();
		CObjCHAR* pNpc = g_pObjMGR->Get_ClientCharOBJ( wNpcSvrIdx, false );
		if( !( pNpc && g_pAVATAR->IsInRANGE( pNpc, AVT_CLICK_EVENT_RANGE )) )
		{
			CUpgrade::GetInstance().RemoveTargetItem();
			Hide();
			return;
		}
	}

	m_pCurrState->Update( ptMouse );

	m_TargetItemSlot.Update( ptMouse );
	for( int i = 0 ; i < 3; ++i )
		m_MaterialSlots[i].Update( ptMouse );

}

void CUpgradeDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();
	m_pCurrState->Draw();

	m_TargetItemSlot.Draw();
	for( int i = 0 ; i < 3; ++i )
		m_MaterialSlots[i].Draw();


	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );


	CUpgrade& Upgrade = CUpgrade::GetInstance();
	if( Upgrade.GetTargetItemName() )
	{
		RECT rcTargetItemName = { 40, 122, 40 + 110, 122 + 18 };
		drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcTargetItemName ,g_dwWHITE,DT_CENTER, Upgrade.GetTargetItemName() );
	}

	RECT  rcDraw = { 27, 189, 27  + 88, 189 + 18 };
	RECT  rcDrawCount = { 115, 189, 145, 205 };


	for( int i = 0; i < 3; ++i )
	{
		if( Upgrade.GetRequireMaterialName( i ) )
		{
			drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_CENTER, Upgrade.GetRequireMaterialName( i ) );
			drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDrawCount ,DT_RIGHT, "%d", Upgrade.GetRequireMaterialCount( i ) );
			
		}
		rcDraw.top		+= 47;
		rcDraw.bottom	+= 47;
		rcDrawCount.top += 47;
		rcDrawCount.bottom += 47;
	}

	///Text Draw
	SetRect( &rcDraw, 79,379,79+34,379+15 );
	drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_RIGHT,"%d", Upgrade.GetSuccessProb() );

	SetRect( &rcDraw, 176,379,176+34,379+15 );
	switch( Upgrade.GetType() )
	{
	case CUpgrade::TYPE_SKILL:
		drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_RIGHT,"%d", Upgrade.GetRequireMp() );
		break;
	case CUpgrade::TYPE_NPC:
		drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_RIGHT,"%d", Upgrade.GetRequireMoney() );
		break;
	default:
		break;
	}
	
}

unsigned CUpgradeDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	return m_pCurrState->Process( uiMsg, wParam, lParam );
}

void CUpgradeDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );
	if( pObj == NULL ) return;

	assert( strcmp(pObj->toString(),"Upgrade") == 0 );
	if( strcmp(pObj->toString(),"Upgrade") ) return;

	CTEventUpgrade* pEvent = (CTEventUpgrade*)pObj;
	switch( pEvent->GetID() )
	{
	case CTEventUpgrade::EID_REMOVE_MATERIALITEM:
		{
			int iIndex = pEvent->GetIndex();
			assert( iIndex >= 0 && iIndex < 3 );
			if( iIndex >= 0 && iIndex < 3 )
			{
				assert( m_MaterialSlots[iIndex].GetIcon() );
				m_MaterialSlots[iIndex].DetachIcon();
			}
			break;
		}
	case CTEventUpgrade::EID_REMOVE_TARGETITEM:
		{
			assert( m_TargetItemSlot.GetIcon() );
			m_TargetItemSlot.DetachIcon();
			break;
		}
	case CTEventUpgrade::EID_SET_MATERIALiTEM:
		{
			int iIndex = pEvent->GetIndex();
			assert( iIndex >= 0 && iIndex < 3 );
			if( iIndex >= 0 && iIndex < 3 )
			{
				assert( m_MaterialSlots[iIndex].GetIcon() == NULL );
				m_MaterialSlots[iIndex].AttachIcon( pEvent->GetItem()->CreateItemIcon() );
			}
			break;
		}
	case  CTEventUpgrade::EID_SET_TARGETITEM:
		{
			assert( m_TargetItemSlot.GetIcon() == NULL );
			if( m_TargetItemSlot.GetIcon() == NULL )
				m_TargetItemSlot.AttachIcon( pEvent->GetItem()->CreateItemIcon() );

			break;
		}
	case CTEventUpgrade::EID_RECEIVE_RESULT:
		{
			ChangeState( STATE_RESULT );
			break;
		}
	default:
		break;
	}
}
void CUpgradeDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	m_TargetItemSlot.MoveWindow( m_sPosition );
	int i = 0;
	for( i = 0 ; i < 3; ++i )
		m_MaterialSlots[i].MoveWindow( m_sPosition );

	for( i = 0 ; i < STATE_MAX; ++i )
		m_pStates[i]->MoveWindow( m_sPosition );;
}

void CUpgradeDlg::Show()
{
	CTDialog::Show();
	switch( CUpgrade::GetInstance().GetType() )
	{
	case CUpgrade::TYPE_SKILL:
		HideChild( IID_TEXT_COST );
		break;
	default:
		break;
	}
}

void CUpgradeDlg::Hide()
{
	CTDialog::Hide();
	CUpgrade::GetInstance().RemoveTargetItem();
}