#include "stdafx.h"
#include ".\selecteventdlg.h"
#include "../IO_ImageRes.h"
#include "subclass/CDialogNpcScriptAnswerItemNew.h"
#include "subclass/CDialogNpcScriptAnswerItem.h"

#include "../../Object.h"
#include "../../Country.h"
#include "../it_mgr.h"


#include "tgamectrl/resourcemgr.h"
#include "tgamectrl/zlistbox.h"
#include "tgamectrl/tcaption.h"
#include "tgamectrl/timage.h"
#include "tgamectrl/tbutton.h"


const int BG_IMAGE_HEIGHT = 150;

CSelectEventDlg::CSelectEventDlg(void)
{
	m_nTargetClientIdx = 0;
}

CSelectEventDlg::~CSelectEventDlg(void)
{

}

void CSelectEventDlg::SetTitle( const char* pszTitle )
{
	assert( pszTitle );
	if( pszTitle )
		m_strTitle = pszTitle;
}

void CSelectEventDlg::AddEvent( char* pszScript, int iEventID, void (*fpEventHandle)(int iEventID) )
{
	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{
		CWinCtrl* pCtrl = Find( IID_ZLISTBOX_EVENT );
		assert( pCtrl ); if( pCtrl == NULL ) return;
		assert( pszScript ); if( pszScript == NULL ) return;
		CZListBox* pListBox = (CZListBox*)pCtrl;
		int iItemCount = pListBox->GetSize();

		int iWidth	 = g_pCApp->GetWIDTH();

		CDialogNpcScriptAnswerItemNew* pAnswerItem = new CDialogNpcScriptAnswerItemNew( 
															iItemCount + 1, pszScript, iEventID, fpEventHandle, iWidth );

		pListBox->Add( pAnswerItem );
		pListBox->SetExtent( iItemCount + 1 );
		pListBox->SetValue( 0 );
		pListBox->SetHeight( pListBox->GetHeight() + pAnswerItem->GetHeight() );
		pListBox->SetWidth( iWidth );

		SetHeight( GetHeight() + pAnswerItem->GetHeight() );
	}
	else
	{
		assert( pszScript );
		if( pszScript == NULL ) return;

		CWinCtrl* pCtrl = Find( IID_ZLISTBOX_EVENT );
		assert( pCtrl ); if( pCtrl == NULL ) return;



		CZListBox* pListBox = (CZListBox*)pCtrl;
		if( pListBox->GetSize() < 2 )
			return ;

		int iImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "EVENT_DIALOG_UPSIDE_MIDDLE" );
		int iHeight  = 21;
		int iWidth	 = 244;
		CDialogNpcScriptAnswerItem* pAnswerItem = new CDialogNpcScriptAnswerItem( 
			pListBox->GetSize() - 1, pszScript, iEventID, fpEventHandle, 
															iImageID, iHeight , iWidth );

		pListBox->InsertItem( pListBox->GetSize() - 2, pAnswerItem );
		pListBox->SetExtent( pListBox->GetExtent() + 1 );

		pListBox->SetValue( 0 );
		pListBox->SetHeight( pListBox->GetHeight() + pAnswerItem->GetHeight() );

		SetHeight( GetHeight() + pAnswerItem->GetHeight() );
	}
}

void CSelectEventDlg::SetTargetClientObjectIndex( short iIndex )
{
	m_nTargetClientIdx = iIndex;
}

void CSelectEventDlg::Hide()
{
	CTDialog::Hide();
	if( CWinCtrl* pCtrl = Find( IID_ZLISTBOX_EVENT ) )
	{
		CZListBox* pListBox = (CZListBox*)pCtrl;
		pListBox->Clear();
	}

	CTCaption* pCaption = GetCaption();
	if( pCaption )
		SetHeight( pCaption->GetHeight() );

	if( CCountry::GetSingleton().IsApplyNewVersion() )
		g_itMGR.ChangeState( IT_MGR::STATE_NORMAL );

	//m_strTitle.clear();

}

void CSelectEventDlg::Show()
{
	///NPC Object가 없거나 있는데 거리가 일정 거리 이상이라면 

	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{

		CTDialog::Show();
		POINT pt = { 0, g_pCApp->GetHEIGHT() * 2 / 5 - GetHeight()  };
		MoveWindow( pt );
		SetWidth( g_pCApp->GetWIDTH() );


		CGameOBJ* pObj = g_pObjMGR->Get_OBJECT( m_nTargetClientIdx );
		if( pObj == NULL )
			return;
		else if( g_pAVATAR->Get_DISTANCE( pObj->Get_CurPOS() ) >= 1000 )
			return;

		g_itMGR.ChangeState( IT_MGR::STATE_NPCDIALOG );

	}
	else
	{


		CWinCtrl* pCtrl = Find( IID_ZLISTBOX_EVENT );
		assert( pCtrl ); if( pCtrl == NULL ) return;


		CZListBox* pListBox = (CZListBox*)pCtrl;
		pListBox->SetExtent( 0 );
		pListBox->SetHeight(0);
		///UPSIDE BOTTOM
		int iImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "EVENT_DIALOG_UPSIDE_BOTTOM" );
		int iHeight  = 12;
		int iWidth	 = 244;
		CDialogNpcScriptAnswerItem* pAnswerItem = new CDialogNpcScriptAnswerItem( 
			0, NULL, 0, NULL, iImageID, iHeight , iWidth );

		pListBox->Add( pAnswerItem );
		pListBox->SetExtent( pListBox->GetExtent() + 1 );

		pListBox->SetValue( 0 );
		pListBox->SetHeight( pListBox->GetHeight() + pAnswerItem->GetHeight() );
		SetHeight( GetHeight() + pAnswerItem->GetHeight() );


		///FOOTER
		iImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "EVENT_DIALOG_FOOT" );
		iHeight  = 3;

		pAnswerItem = new CDialogNpcScriptAnswerItem( 
			0, NULL, 0, NULL, iImageID, iHeight , iWidth );

		pListBox->Add( pAnswerItem );
		pListBox->SetExtent( pListBox->GetExtent() + 1 );

		pListBox->SetValue( 0 );
		pListBox->SetHeight( pListBox->GetHeight() + pAnswerItem->GetHeight() );
		SetHeight( GetHeight() + pAnswerItem->GetHeight() );
		CTDialog::Show();
	}

}
unsigned CSelectEventDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		if( uiProcID == IID_BTN_CLOSE && uiMsg == WM_LBUTTONUP )
			Hide();

		return uiMsg;
	}
	return 0;
}
void CSelectEventDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();


	DWORD text_color = g_dwBLACK;
	int	  font       = FONT_NORMAL_BOLD;
	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{
		text_color	=	g_dwWHITE;
		font		= FONT_OUTLINE_11_BOLD;
	}

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	RECT rcDrawText = { 80, 63, 80 + 85, 63 + 22 };
	drawFont( g_GameDATA.m_hFONT[ font ], true, &rcDrawText, text_color, DT_CENTER, m_strTitle.c_str() );		

}
void CSelectEventDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	CTDialog::Update( ptMouse );
	CGameOBJ* pObj = g_pObjMGR->Get_OBJECT( m_nTargetClientIdx );

	///NPC Object가 없거나 있는데 거리가 일정 거리 이상이라면 
	if( pObj == NULL )
		Hide();
	else if( g_pAVATAR->Get_DISTANCE( pObj->Get_CurPOS() ) >= 1000 )
		Hide();
}


bool CSelectEventDlg::Create( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{
		if( CCountry::GetSingleton().IsApplyNewVersion() )
		{
			CWinCtrl* pCtrl = Find( IID_BG_IMAGE );
			if( NULL != pCtrl && CTRL_IMAGE == pCtrl->GetControlType() )
			{
				CTImage* pImage = (CTImage*)pCtrl;
				pImage->SetScaleWidth( g_pCApp->GetWIDTH() / pImage->GetWidth() );
			}

			pCtrl = Find( IID_BTN_CLOSE );
			if( NULL != pCtrl  && CTRL_BUTTON == pCtrl->GetControlType() )
			{
				CTButton* pBtn = (CTButton*)pCtrl;
				POINT pt = pBtn->GetOffset();
				pt.x = g_pCApp->GetWIDTH() - pBtn->GetWidth() - 10;
				pBtn->SetOffset( pt );
	//			pBtn->MoveWindow( m_sPosition );
			}

			pCtrl = Find( IID_ZLISTBOX_EVENT );
			if( NULL != pCtrl )
			{
				pCtrl->SetOffset( 20, GetHeight() + 40);
	//			pCtrl->MoveWindow( m_sPosition );
			}

			POINT pt = { 0, g_pCApp->GetHEIGHT() * 2 / 5 - GetHeight()  };

			MoveWindow( pt );
			SetWidth( g_pCApp->GetWIDTH() );
		}
		return true;
	}
	return false;
}