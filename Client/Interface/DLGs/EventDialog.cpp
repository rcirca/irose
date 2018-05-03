#include "stdafx.h"
#include ".\eventdialog.h"
#include "../../Object.h"
#include "../it_MGR.h"
#include "subclass/CDialogNpcScriptItem.h"
#include "subclass/CDialogNpcScriptAnswerItem.h"
#include "subclass/CDialogNpcScriptAnswerItemNew.h"
#include "../IO_ImageRes.h"
#include "../../Country.h"

#include "tgamectrl/resourcemgr.h"
#include "tgamectrl/tcaption.h"
#include "tgamectrl/zlistbox.h"
#include "tgamectrl/timage.h"
#include "tgamectrl/tbutton.h"

const int BG_IMAGE_HEIGHT = 150;
CEventDialog::CEventDialog(void)
{
	m_hNpcFace = NULL;

	m_heightNpcFace = 0;
	m_widthNpcFace  = 0;
}

CEventDialog::~CEventDialog(void)
{
}
void CEventDialog::SetScript( char* pszScript )
{
	assert( pszScript ); if( pszScript == NULL ) return;

	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{
		SetHeight( BG_IMAGE_HEIGHT );	
		m_strTempScript.clear();	
		m_strTempScript = pszScript;

		///답변 예제를 담을 ListBox의 위치를 조정한다.
		CWinCtrl* pCtrl = Find( IID_ZLISTBOX_ANSWER_EXAMPLE );
		assert( pCtrl ); if( pCtrl == NULL ) return;

		CZListBox* pListBox = (CZListBox*)pCtrl;
		pListBox->Clear();
	}
	else
	{
		CTCaption* pCaption = GetCaption();
		assert( pCaption ); if( pCaption == NULL ) return;
		SetHeight( pCaption->GetHeight() );

		CWinCtrl* pCtrl = Find( IID_ZLISTBOX_NPCSCRIPT );
		assert( pCtrl ); if( pCtrl == NULL ) return;

		int iListBoxHeight = 0;
		CZListBox* pListBox = (CZListBox*)pCtrl;
		pListBox->Clear();
		
		int iWidth		= 244;
		int iLineHeight = 21;
		int iImageID    = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "EVENT_DIALOG_UPSIDE_MIDDLE" );
		CDialogNpcScriptItem* pItem = new CDialogNpcScriptItem( 1, pszScript , iImageID, iLineHeight, iWidth , 30 );
		pListBox->Add( pItem ); iListBoxHeight += pItem->GetHeight();

		iImageID		= CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "EVENT_DIALOG_UPSIDE_BOTTOM" );
		iLineHeight     = 12;
		pItem = new CDialogNpcScriptItem( 0, NULL , iImageID, iLineHeight, iWidth , 0 );
		pListBox->Add( pItem ); iListBoxHeight += pItem->GetHeight();

		pListBox->SetHeight( iListBoxHeight );
		SetHeight( GetHeight() + iListBoxHeight );

		pListBox->SetValue( 0 );
		pListBox->SetExtent( 2 );

		///답변 예제를 담을 ListBox의 위치를 조정한다.
		pCtrl = Find( IID_ZLISTBOX_ANSWER_EXAMPLE );
		assert( pCtrl ); if( pCtrl == NULL ) return;

		pListBox = (CZListBox*)pCtrl;
		pListBox->SetOffset( 0, GetHeight() );
		MoveWindow( m_sPosition );

		pListBox->Clear();


		int iAnswerListBoxHeight = 0;
		iImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "EVENT_DIALOG_UNDERSIDE_TOP" );
		iLineHeight  = 10;
		iWidth   = 244;
		CDialogNpcScriptAnswerItem* pAnswerItem = new CDialogNpcScriptAnswerItem( 0, NULL ,0 , NULL , iImageID, iLineHeight, iWidth );
		pListBox->Add( pAnswerItem ); iAnswerListBoxHeight += pAnswerItem->GetHeight();

		iImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "EVENT_DIALOG_UNDERSIDE_BOTTOM" );
		iLineHeight  = 12;
		pAnswerItem = new CDialogNpcScriptAnswerItem( 0, NULL , 0 , NULL ,iImageID, iLineHeight , iWidth );
		pListBox->Add( pAnswerItem ); iAnswerListBoxHeight += pAnswerItem->GetHeight();


		iImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "EVENT_DIALOG_FOOT" );
		iLineHeight  = 3;
		pAnswerItem = new CDialogNpcScriptAnswerItem( 0, NULL , 0 , NULL ,iImageID, iLineHeight , iWidth );
		pListBox->Add( pAnswerItem ); iAnswerListBoxHeight += pAnswerItem->GetHeight();

		pListBox->SetExtent( 3 );
		pListBox->SetValue( 0 );
		pListBox->SetHeight( iAnswerListBoxHeight );
		SetHeight( GetHeight() + iAnswerListBoxHeight );
	}

}

void CEventDialog::AddAnswerExample( char* pszScript , int iEventID , void (*fpEventHandle)(int iEventID))
{
	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{
		CWinCtrl* pCtrl = Find( IID_ZLISTBOX_ANSWER_EXAMPLE );
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
		CWinCtrl* pCtrl = Find( IID_ZLISTBOX_ANSWER_EXAMPLE );
		assert( pCtrl ); if( pCtrl == NULL ) return;
		assert( pszScript ); if( pszScript == NULL ) return;
		CZListBox* pListBox = (CZListBox*)pCtrl;
		int iItemCount = pListBox->GetSize();

		if( iItemCount <= 0 ) return;

		int iImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "EVENT_DIALOG_UNDERSIDE_MIDDLE" );
		int iHeight  = 20;
		int iWidth	 = 244;
		CDialogNpcScriptAnswerItem* pAnswerItem = new CDialogNpcScriptAnswerItem( 
															iItemCount - 2, pszScript, iEventID, fpEventHandle, 
															iImageID, iHeight , iWidth );

		pListBox->InsertItem( iItemCount - 2 , pAnswerItem  );
		pListBox->SetExtent( pListBox->GetExtent() + 1 );

		pListBox->SetValue( 0 );
		pListBox->SetHeight( pListBox->GetHeight() + pAnswerItem->GetHeight() );

		SetHeight( GetHeight() + pAnswerItem->GetHeight() );
	}
}

unsigned CEventDialog::Process(unsigned uiMsg, WPARAM wParam, LPARAM lParam )
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


void CEventDialog::SetTargetNpcClientObjectIndex( short iIndex )
{


#pragma message("list_npc.stb의 데이타 카운트보다 큰지도 체크해야 한다")

	if( CCountry::GetSingleton().IsApplyNewVersion() && iIndex > 0  )
	{
		if( m_hNpcFace )
		{
			unloadTexture( m_hNpcFace );
			m_hNpcFace = NULL;
			m_widthNpcFace	= 0;
			m_heightNpcFace = 0;
		}

		m_strNpcName = NPC_NAME( iIndex );		
		m_hNpcFace = CImageResManager::GetSingleton().Load_NpcFace( NPC_FACE_ICON( iIndex ) );
		if( m_hNpcFace )
			::getTextureSize( m_hNpcFace, m_widthNpcFace, m_heightNpcFace );
	}

	
	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{
		m_Script.Clear();
		if( !m_strTempScript.empty() )
		{
			m_Script.SetSplitType( CJStringParser::SPLIT_WORD );
			m_Script.SetDefaultColor( g_dwWHITE );
			m_Script.SetDefaultFont( FONT_OUTLINE_11_BOLD );
			m_Script.SetString( m_strTempScript.c_str(), GetWidth() - ( m_widthNpcFace + 100 ) );
		}
	}
}

int	 CEventDialog::IsInValidShow()
{
	if( g_pAVATAR && g_pAVATAR->Get_HP() <= 0 )
		return 99;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_DEAL ) )
		return DLG_TYPE_DEAL;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_EXCHANGE ) )
		return DLG_TYPE_EXCHANGE;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_DIALOG ))
		return DLG_TYPE_DIALOG;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_MAKE ))
		return DLG_TYPE_MAKE;

	return 0;
}

void CEventDialog::Hide()
{
	CTDialog::Hide();
	CZListBox* pListBox = NULL;
	CWinCtrl* pCtrl = Find( IID_ZLISTBOX_NPCSCRIPT );
	assert( pCtrl );
	if( pCtrl )
	{
		pListBox = (CZListBox*)pCtrl;
		pListBox->Clear();
	}

	pCtrl = Find( IID_ZLISTBOX_ANSWER_EXAMPLE );
	assert( pCtrl );
	if( pCtrl )
	{
		pListBox = (CZListBox*)pCtrl;
		pListBox->Clear();
	}

	CTCaption* pCaption = GetCaption();
	if( pCaption )
		SetHeight( pCaption->GetHeight() );

	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{
		g_itMGR.ChangeState( IT_MGR::STATE_NORMAL );
	}


}

void CEventDialog::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{
		if( m_hNpcFace )
		{
			// Transform	
			D3DXMATRIX mat;	
			D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y + BG_IMAGE_HEIGHT - m_heightNpcFace, 0 );
			::setTransformSprite( mat );

			::drawSprite( m_hNpcFace,
						NULL,
						NULL,
						&D3DXVECTOR3( 0, 0 , 0 ),								
						D3DCOLOR_RGBA( 255, 255, 255, 255 ) );

			if( !m_strNpcName.empty() )
			{
				D3DXMATRIX mat;	
				D3DXMatrixTranslation( &mat, (float)m_sPosition.x , (float)m_sPosition.y, 0 );
				::setTransformSprite( mat );

				::drawFont( g_GameDATA.m_hFONT[ FONT_OUTLINE_16_BOLD ], true, m_widthNpcFace + 20, 5, g_dwWHITE, m_strNpcName.c_str());
			}

			int offset_y = 40;
			for( int i = 0 ; i < m_Script.GetStringCount(); ++i )
			{
				m_Script.GetString( i )->Draw( m_widthNpcFace + 20, offset_y, false );
				offset_y += m_Script.GetString( i )->GetStringHeight() + 5;
			}
		}
		else
		{
			D3DXMATRIX mat;	
			D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y, 0 );
			::setTransformSprite( mat );

			if( !m_strNpcName.empty() )
			{
				::drawFont( g_GameDATA.m_hFONT[ FONT_OUTLINE_16_BOLD ], true, 100, 5, g_dwWHITE, m_strNpcName.c_str());
			}

			int offset_y = 40;
			for( int i = 0 ; i < m_Script.GetStringCount(); ++i )
			{
				m_Script.GetString( i )->Draw( 100, offset_y, false );
				offset_y += m_Script.GetString( i )->GetStringHeight() + 5;
			}

		}
	}
	else 
	{
		if( !m_strNpcName.empty() )
		{
			D3DXMATRIX mat;	
			D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
			::setTransformSprite( mat );
			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 50, 5, g_dwWHITE, m_strNpcName.c_str());
		}
	}
}


void CEventDialog::Show()
{
	CTDialog::Show();

	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{
		POINT pt = { 0, g_pCApp->GetHEIGHT() * 2 / 5 - GetHeight()  };
		MoveWindow( pt );
		SetWidth( g_pCApp->GetWIDTH() );
		g_itMGR.ChangeState( IT_MGR::STATE_NPCDIALOG );
	}
}


bool CEventDialog::Create( const char* IDD )
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

			pCtrl = Find( IID_ZLISTBOX_ANSWER_EXAMPLE );
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