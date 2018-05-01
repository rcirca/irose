#include "stdafx.h"
#include "CQuestDlg.h"
#include "CDialogDlg.h"

#include "../../Game.h"
#include "../../Object.h"
#include "../../Network/CNetwork.h"
#include "../../Common/IO_QUEST.h"
#include "../../Util/Localizing.h"
#include "../CTDrawImpl.h"
#include "../it_MGR.h"
#include "../CToolTipMgr.h"
#include "../IO_ImageRes.h"

#include "../Command/UICommand.h"

#include "subclass/CQuestListItem.h"

#include "SplitHangul.h"
#include "ZListBox.h"
#include "TScrollBar.h"
#include "TPane.h"
#include "TListBox.h"
#include "ResourceMgr.h"

const short QUEST_SLOT_WIDTH  = 40;
const short QUEST_SLOT_HEIGHT = 40;
const short QUEST_SLOT_MAX    = QUEST_ITEM_PER_QUEST;
const POINT QUEST_SLOT_POSITION[QUEST_SLOT_MAX] = { 
	{ 10,347 - 171},{ 51,347 - 171},{ 92, 347 - 171},{ 133, 347- 171},{ 174, 347 - 171},{ 211 , 347 - 171}
};



CQuestDlg::CQuestDlg()
{
	m_pSelectedQuestItem		= NULL;
	m_iPrevSelectedQuestID		= 0;
	m_bMinimize					= false;
}

CQuestDlg::~CQuestDlg()
{
	SAFE_DELETE( m_pSelectedQuestItem );
}

unsigned int CQuestDlg::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision() ) return 0;

	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			if(On_LButtonUP(iProcID))
				return uiMsg;
			break;
		case WM_LBUTTONDOWN:
			if(On_LButtonDN(iProcID))
				return uiMsg;
			break;
		}
		return uiMsg;
	}
	return 0;
}

bool CQuestDlg::On_LButtonUP( unsigned ID )
{
	switch( ID )
	{
	case IID_BTN_MINIMIZE:
		{
			Minimize();
			break;
		}
	case IID_BTN_MAXIMIZE:
		{
			Maximize();
			break;
		}
	case IID_BTN_ICONIZE:
		g_itMGR.AddDialogIcon( 	GetDialogType() );
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;
	case IID_BTN_ABANDON:
		{
			int iSelectQuestIdx = GetSelectQuestIdx();
			if( iSelectQuestIdx < 0 )
				break;
		
			int iQuestID      = g_pAVATAR->m_Quests.m_QUEST[iSelectQuestIdx].GetID();

			if( iQuestID >= 1 )
			{

				CTCmdAbandonQuest* pOkCmd = new CTCmdAbandonQuest( iSelectQuestIdx, iQuestID );
				g_itMGR.OpenMsgBox( CStr::Printf(F_STR_QUERY_ABANDON_QUEST, QUEST_NAME( iQuestID) ),
					CMsgBox::BT_OK | CMsgBox::BT_CANCEL, 
					true,
					0,
					pOkCmd,
					NULL );



			}
		}
		break;
	default:
		break;
	}
	return false;
}

bool CQuestDlg::On_LButtonDN( unsigned  ID )
{
	switch( ID )
	{
	case IID_ZLIST_ITEM:
		{
			UpdateQuestDescription();
			break;
		}
	default:
		break;
	}
	return true;
}

void CQuestDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return ;
	CTDialog::Update( ptMouse );
	
	if( m_pSelectedQuestItem )
		m_pSelectedQuestItem->Update( ptMouse );

	RECT rcSlot = {0,0,0,0};
	tagITEM* pItem = NULL;
	CSplitHangul Spliter;

	int iSelectQuestIdx = GetSelectQuestIdx();
	if( iSelectQuestIdx < 0 )
		return;

	///NPC 대화창이 열려있는 동안에는 포기를 할수 없다.
	CDialogDlg* pDlg = (CDialogDlg*)g_itMGR.FindDlg( DLG_TYPE_DIALOG );
	CWinCtrl* pCtrl = FindChildInPane(IID_PANE_QUESTINFO, IID_BTN_ABANDON);
	if( pDlg && pDlg->IsVision() )
	{
		if( pCtrl && pCtrl->IsVision() )
			pCtrl->SetEnable( false );
	}
	else
	{
		if( pCtrl && pCtrl->IsVision() )
			pCtrl->SetEnable( true );
	}


	///이하 툴팁 표시
	CTDialog* pTDlg = CTDialog::GetProcessMouseOverDialog();
	if( pTDlg && pTDlg != (CTDialog*)this )
			return ;

	CWinCtrl* pPaneQuestInfo = Find( IID_PANE_QUESTINFO );
	assert( pPaneQuestInfo );
	int iOffsetY = 0;
	if( pPaneQuestInfo )
		iOffsetY = pPaneQuestInfo->GetOffset().y;

	for( int i = 0 ; i < QUEST_SLOT_MAX; ++i )
	{
		rcSlot.left = m_sPosition.x + QUEST_SLOT_POSITION[i].x;
		rcSlot.top  = m_sPosition.y + QUEST_SLOT_POSITION[i].y + iOffsetY;
		rcSlot.right  = rcSlot.left + QUEST_SLOT_WIDTH;
		rcSlot.bottom = rcSlot.top  + QUEST_SLOT_HEIGHT;
		if( PtInRect(&rcSlot, ptMouse ))
		{
			pItem = g_pAVATAR->m_Quests.m_QUEST[iSelectQuestIdx].GetSlotITEM( i );
			if( pItem && !pItem->IsEmpty() )
			{
				CInfo Info;
				Info.Clear();

				POINT ptTemp = ptMouse;
				ptTemp.x += 30;
				Info.SetPosition( ptTemp );
				Info.AddString( ITEM_NAME( pItem->GetTYPE(), pItem->GetItemNO() ));
				
				Spliter.Clear();
				Spliter.DoSplit( (char*)ITEM_DESC( pItem->GetTYPE(), pItem->GetItemNO()), 30 ,CLocalizing::GetSingleton().GetCurrentCodePageNO());
				for( int k = 0; k < Spliter.GetLineCount() ; ++k )
					Info.AddString( Spliter.GetString( k ) );

				
				CToolTipMgr::GetInstance().RegistInfo( Info );
				return;
			}
		}
	}

	//WINCTRL_LIST_ITOR iter;
	//for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter)
	//{
	//	pCtrl = *iter;
	//	if( pCtrl->GetControlType() == CTRL_IMAGE )
	//		continue;

	//	if( pCtrl->IsInside(ptMouse.x, ptMouse.y ) )
	//	{
	//		CToolTipMgr::GetInstance().RegUIInfo( (short)ptMouse.x, (short)ptMouse.y, GetDialogType(), pCtrl->GetControlID());
	//		break;
	//	}
	//}
}

void CQuestDlg::Draw()
{
	if( !IsVision() ) return ;
	CTDialog::Draw();
	if( m_pSelectedQuestItem )
		m_pSelectedQuestItem->Draw();
	DrawItems();
}

void CQuestDlg::UpdateQuestDescription()
{

	CWinCtrl* pAbandonBtn = FindChildInPane(IID_PANE_QUESTINFO, IID_BTN_ABANDON );
	assert( pAbandonBtn );
	if( pAbandonBtn == NULL ) return;

	CWinCtrl* pCtrl = FindChildInPane(IID_PANE_QUESTINFO, IID_LIST_QUESTINFO );
	if( pCtrl == NULL || pCtrl->GetControlType() != CTRL_LISTBOX )
	{
		LogString( LOG_DEBUG,"Not Found ListBox : CQuestDlg::SetQuestDescption() \n");
		pAbandonBtn->Hide();
		return;
	}
	CTListBox* pList = (CTListBox*)pCtrl;
	pList->ClearText();
	if( m_pSelectedQuestItem )
		SAFE_DELETE( m_pSelectedQuestItem );


	int iSelectQuestIdx = GetSelectQuestIdx();
	if( iSelectQuestIdx < 0 )
	{
		pAbandonBtn->Hide();
		return;
	}

	int iQuestID      = g_pAVATAR->m_Quests.m_QUEST[iSelectQuestIdx].GetID();

	if( iQuestID < 1  )
	{
		pAbandonBtn->Hide();
		return;
	}
	
	pList->AppendText( QUEST_DESC( iQuestID ),D3DCOLOR_RGBA(255,255,255,255) );
	pList->SetValue( 0 );

	m_iPrevSelectedQuestID = iQuestID;
	m_pSelectedQuestItem = new CQuestListItem( iQuestID );
	
	if( CWinCtrl* pPaneCtrl = Find( IID_PANE_QUESTINFO ) )
		m_pSelectedQuestItem->SetOffset( 15, pPaneCtrl->GetOffset().y + 34 );

	m_pSelectedQuestItem->MoveWindow( m_sPosition );
	pAbandonBtn->Show();
}

void CQuestDlg::Show() 
{
	CTDialog::Show();

	CWinCtrl* pCtrl;
	if( m_bMinimize )
	{
		if( pCtrl = FindChildInPane( IID_PANE_QUESTLIST, IID_BTN_MINIMIZE ) )
			pCtrl->Hide();
	}
	else
	{
		if( pCtrl = FindChildInPane( IID_PANE_QUESTLIST, IID_BTN_MAXIMIZE ) )
			pCtrl->Hide();
	}


	if( m_pSelectedQuestItem )
		m_pSelectedQuestItem->Show();

	UpdateQuestList();
}
void CQuestDlg::UpdateQuestList()
{
	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_QUESTLIST, IID_ZLIST_QUEST );
	if( pCtrl == NULL )
	{
		LogString( LOG_DEBUG,"Not Found ListBox : CQuestDlg::SetQuestDescption() \n");
		return;
	}

	CZListBox* pList = (CZListBox*)pCtrl;
	pList->Clear();
	memset( m_iList2QuestIdx , -1 ,sizeof( m_iList2QuestIdx ) );
	CQuestListItem* pItem = NULL;
	int iQuestIdx	= 0;
	int iSelectItemIndex = -1;
	int iCount		= 0;
	for( int i = 0; i < QUEST_PER_PLAYER; ++i )
	{
		if( iQuestIdx = g_pAVATAR->m_Quests.m_QUEST[i].GetID() )
		{
			pItem = new CQuestListItem( iQuestIdx );
			pItem->SetControlID( IID_ZLIST_ITEM );
			pItem->Show();
			pList->Add( pItem );
			m_iList2QuestIdx[ iCount ] = i;
			if( m_iPrevSelectedQuestID && m_iPrevSelectedQuestID == iQuestIdx )
				iSelectItemIndex = iCount;
			iCount++;
		}
	}

	///이전 선택되어진 QuestIndex가 리스트에 없다면 맨 밑에 있는 퀘스트로 이동
	if( iSelectItemIndex == -1 )
		iSelectItemIndex = pList->GetSize() - 1;

	if( pList->GetSize() )///1개 이상 등록된 Quest가 있을때만
	{
		///스크롤바와 리스트를 힘께 이동시키기 위해서
		pCtrl = FindChildInPane( IID_PANE_QUESTLIST, IID_ZLIST_SCROLLBAR );
		if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
		{
			CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
			pScrollBar->SetValue( iSelectItemIndex );
		}
		pList->SetSelected( iSelectItemIndex );
	}
	UpdateQuestDescription();
}
void CQuestDlg::DrawItems()
{
	CTDrawImpl* pDrawObj = g_itMGR.GetDrawImplPtr();

	POINT ptDlg = GetPosition();
	tagITEM* pItem = NULL;
	POINT ptDraw = {0,0};
	int iItemNo = 0;

	int iSelectQuestIdx = GetSelectQuestIdx();
	if( iSelectQuestIdx < 0 )
		return;


	CWinCtrl* pPaneQuestInfo = Find( IID_PANE_QUESTINFO );
	assert( pPaneQuestInfo );
	int iOffsetY = 0;
	if( pPaneQuestInfo )
		iOffsetY = pPaneQuestInfo->GetOffset().y;

	for( int i = 0 ; i < QUEST_SLOT_MAX; ++i )
	{
		pItem = g_pAVATAR->m_Quests.m_QUEST[iSelectQuestIdx].GetSlotITEM( i );
		if( pItem && !pItem->IsEmpty() )
		{
			ptDraw.x = ptDlg.x + QUEST_SLOT_POSITION[i].x;
			ptDraw.y = ptDlg.y + QUEST_SLOT_POSITION[i].y + iOffsetY ;

			iItemNo = ITEM_ICON_NO( pItem->GetTYPE(), pItem->GetItemNO() );
			pDrawObj->Draw( ptDraw.x ,ptDraw.y ,IMAGE_RES_ITEM, iItemNo );
			if( pItem->IsEnableDupCNT() )
			{
				int iImageIndex =  CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"ID_BLACK_PANEL");
				char szCount[32]= {0};
				itoa( pItem->GetQuantity(), szCount, 10 );

				HNODE hFont = g_GameDATA.m_hFONT[ FONT_NORMAL ];
				SIZE size = getFontTextExtent (  hFont, szCount );
				int iWidth = (int)size.cx+4;		
				g_DrawImpl.Draw( (int)ptDraw.x , (int)ptDraw.y, iWidth , IMAGE_RES_UI, iImageIndex ,D3DCOLOR_ARGB(128,255,255,255));
				drawFont( hFont, true, 2, 2 ,g_dwYELLOW, szCount );
			}
		}
	}

	if( g_pAVATAR->m_Quests.m_QUEST[iSelectQuestIdx].GetExpirationTIME() )
	{
		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
		::setTransformSprite( mat );
		RECT rc = { 38,392 - 171 + iOffsetY, 135, 412 - 171 + iOffsetY };
		if( g_pAVATAR->m_Quests.m_QUEST[iSelectQuestIdx].GetRemainTIME() > 0 )
		{
			drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rc, g_dwWHITE, DT_LEFT, 
				CStr::Printf("%d%s %d%s", 
				g_pAVATAR->m_Quests.m_QUEST[iSelectQuestIdx].GetRemainTIME() / 6 ,
				STR_MINUTE, 
				g_pAVATAR->m_Quests.m_QUEST[iSelectQuestIdx].GetRemainTIME() % 6 * 10 ,
				STR_SECOND )
			);
		}
		else
		{
			drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, (int)rc.left, (int)rc.top, g_dwWHITE, STR_TIMEOUT );
		}
	}
	

}

int  CQuestDlg::GetSelectQuestIdx()
{
	CWinCtrl* pCtrl = FindChildInPane(IID_PANE_QUESTLIST, IID_ZLIST_QUEST );
	if( pCtrl == NULL )
		return -1;

	CZListBox* pList = (CZListBox*)pCtrl;
	int iListIdx = pList->GetSelectedItemIndex();

	if( iListIdx < 0 || iListIdx >= QUEST_PER_PLAYER )
		return -1;

	return m_iList2QuestIdx[ iListIdx ];
}

void CQuestDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );

	if( m_pSelectedQuestItem )
		m_pSelectedQuestItem->MoveWindow( pt );
}
void CQuestDlg::Hide()
{
	CTDialog::Hide();
	if( m_pSelectedQuestItem )
		m_pSelectedQuestItem->Hide();

	// 홍근.	
	ActiveListBoxMoveBegin();
}

CWinCtrl* CQuestDlg::FindChildInPane( unsigned uiPaneID, unsigned uiChildID )
{
	CWinCtrl* pCtrl = Find( uiPaneID );
	if( pCtrl && pCtrl->GetControlType() == CTRL_PANE )
	{
		CTPane* pPane = (CTPane*)pCtrl;
		return pPane->FindChild( uiChildID );
	}
	return NULL;
}

void CQuestDlg::Minimize()
{
	CWinCtrl* pCtrl = NULL;
	
	m_bMinimize = true;

	if( pCtrl = FindChildInPane( IID_PANE_QUESTLIST, IID_BTN_MINIMIZE ) )
		pCtrl->Hide();

	if( pCtrl = FindChildInPane( IID_PANE_QUESTLIST, IID_BTN_MAXIMIZE ) )
		pCtrl->Show();


	if( pCtrl = Find( IID_PANE_QUESTINFO ) )
	{
		pCtrl->SetOffset( 0, 56 );
		SetHeight( pCtrl->GetHeight() + 56 );
		if( m_pSelectedQuestItem )
			m_pSelectedQuestItem->SetOffset( 15, pCtrl->GetOffset().y + 34 );

	}

	MoveWindow( m_sPosition );
}

void CQuestDlg::Maximize()
{
	CWinCtrl* pCtrl = NULL;

	m_bMinimize = false;

	if( pCtrl = FindChildInPane( IID_PANE_QUESTLIST, IID_BTN_MINIMIZE ) )
		pCtrl->Show();

	if( pCtrl = FindChildInPane( IID_PANE_QUESTLIST, IID_BTN_MAXIMIZE ) )
		pCtrl->Hide();

	if( pCtrl = Find( IID_PANE_QUESTINFO ) )
	{
		pCtrl->SetOffset( 0, 171 );
		SetHeight( pCtrl->GetHeight() + 171 );
		if( m_pSelectedQuestItem )
			m_pSelectedQuestItem->SetOffset( 15, pCtrl->GetOffset().y + 34 );
	}

	MoveWindow( m_sPosition );
}

//*-------------------------------------------------------------------------------------*/
/// 홍근
/// @brief 현재 활성화된(선택된탭) 리스트박스를 시작으로 이동시킨다
//*-------------------------------------------------------------------------------------*/
void CQuestDlg::ActiveListBoxMoveBegin()
{

#ifdef _WORK

	CWinCtrl* pCtrl = Find( IID_ZLIST_QUEST );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		pListBox->SetValue( 0 );

		pCtrl = Find( IID_ZLIST_SCROLLBAR );
		if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
		{
			CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
			pScrollBar->SetValue( 0 );
		}
	}

#endif
}