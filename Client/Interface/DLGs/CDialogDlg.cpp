#include "stdafx.h"
#include ".\cdialogdlg.h"
#include "../../Object.h"
#include "../it_MGR.h"
#include "subclass/CDialogNpcScriptItem.h"
#include "subclass/CDialogNpcScriptAnswerItem.h"
#include "subclass/CDialogNpcScriptAnswerItemNew.h"
#include "../IO_ImageRes.h"
#include "../../Country.h"
#include "CMinimapDlg.h"
#include "ResourceMgr.h"
#include "TCaption.h"
#include "ZListBox.h"
#include "TImage.h"
#include "TButton.h"
#include "zz_interface.h"
#include "../../System/CGame.h"

const int BG_IMAGE_HEIGHT = 150;
CDialogDlg::CDialogDlg(void)
{
	m_hNpcFace = NULL;
	m_heightNpcFace = 0;
	m_widthNpcFace  = 0;
}

CDialogDlg::~CDialogDlg(void)
{

}

void CDialogDlg::SetScript( char* pszScript )
{
	assert( pszScript ); if( pszScript == NULL ) return;


	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{
		SetHeight( BG_IMAGE_HEIGHT );	
		m_strTempScript.clear();	
		m_strTempScript = pszScript;


		CWinCtrl* pCtrl = Find( IID_ZLISTBOX_ANSWER_EXAMPLE );
		if( pCtrl == NULL ) 
			return;

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

		int iWidth		= 350;
		int iImageID    = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI13_NPC_SCRIPT_IMAGE_TOP" );
		int iLineHeight = 16;
		CDialogNpcScriptItem* pItem = new CDialogNpcScriptItem( 0, NULL , iImageID, iLineHeight, iWidth , 0 );
		pListBox->Add( pItem ); iListBoxHeight += pItem->GetHeight();


		iImageID    = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI13_NPC_SCRIPT_IMAGE_MIDDLE" );
		iLineHeight = 20;
		pItem = new CDialogNpcScriptItem( 1, pszScript , iImageID, iLineHeight, iWidth , 25 );
		pListBox->Add( pItem ); iListBoxHeight += pItem->GetHeight();

		iImageID    = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI13_NPC_SCRIPT_IMAGE_BOTTOM" );
		iLineHeight = 16;
		pItem = new CDialogNpcScriptItem( -1, NULL , iImageID, iLineHeight, iWidth , 0);
		pListBox->Add( pItem ); iListBoxHeight += pItem->GetHeight();

		pListBox->SetHeight( iListBoxHeight );
		SetHeight( GetHeight() + iListBoxHeight );

		pListBox->SetValue( 0 );
		pListBox->SetExtent( 3 );


		///답변 예제를 담을 ListBox의 위치를 조정한다.
		pCtrl = Find( IID_ZLISTBOX_ANSWER_EXAMPLE );
		assert( pCtrl ); if( pCtrl == NULL ) return;

		pListBox = (CZListBox*)pCtrl;
		pListBox->SetOffset( 0, GetHeight() );
		MoveWindow( m_sPosition );

		pListBox->Clear();


		int iAnswerListBoxHeight = 0;
		iImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI13_NPC_SCRIPT_ANSWER_TOP" );
		iLineHeight  = 10;
		iWidth   = 350;
		CDialogNpcScriptAnswerItem* pAnswerItem = new CDialogNpcScriptAnswerItem( 0, NULL ,0 , NULL , iImageID, iLineHeight, iWidth );
		pListBox->Add( pAnswerItem ); iAnswerListBoxHeight += pAnswerItem->GetHeight();


		iImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI13_NPC_SCRIPT_ANSWER_BOTTOM" );
		iLineHeight  = 17;
		pAnswerItem = new CDialogNpcScriptAnswerItem( -1, NULL , 0 , NULL ,iImageID, iLineHeight , iWidth );
		pListBox->Add( pAnswerItem ); iAnswerListBoxHeight += pAnswerItem->GetHeight();

		pListBox->SetExtent( 2 );
		pListBox->SetValue( 0 );
		pListBox->SetHeight( iAnswerListBoxHeight );
		SetHeight( GetHeight() + iAnswerListBoxHeight );
	}
}

void CDialogDlg::AddAnswerExample( char* pszScript , int iEventID , void (*fpEventHandle)(int iEventID))
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

		int iImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI13_NPC_SCRIPT_ANSWER_MIDDLE" );
		int iHeight  = 25;
		int iWidth	 = 350;
		CDialogNpcScriptAnswerItem* pAnswerItem = new CDialogNpcScriptAnswerItem( 
															iItemCount -1, pszScript, iEventID, fpEventHandle, 
															iImageID, iHeight , iWidth );



		pListBox->InsertItem( iItemCount - 1 , pAnswerItem  );
		pListBox->SetExtent( pListBox->GetExtent() + 1 );

		pListBox->SetValue( 0 );
		pListBox->SetHeight( pListBox->GetHeight() + pAnswerItem->GetHeight() );

		SetHeight( GetHeight() + pAnswerItem->GetHeight() );
	}
}

unsigned CDialogDlg::Process(unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		if( uiProcID == IID_BTN_CLOSE && uiMsg == WM_LBUTTONUP )
			Hide();

		//if( CCountry::GetSingleton().IsApplyNewVersion() )
		//{
		//	if( uiProcID == WM_KEYUP && wParam == VK_ESCAPE )
		//		Hide();
		//}

		return uiMsg;
	}

	if( CCountry::GetSingleton().IsApplyNewVersion() )
		return uiMsg;

	return 0;
}

void CDialogDlg::Update( POINT ptMouse )
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

void CDialogDlg::SetTargetNpcClientObjectIndex( short iIndex )
{
	m_nTargetClientIdx = iIndex;
	CGameOBJ* pObj = g_pObjMGR->Get_OBJECT( iIndex );
	if( pObj && pObj->IsA( OBJ_NPC) )
	{
		m_strNpcName = pObj->Get_NAME();
		CObjNPC* p = (CObjNPC*)pObj;
		
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_MINIMAP ) )
		{
			CMinimapDLG* pMinimap = (CMinimapDLG*)pDlg;
			if( pMinimap->GetIndicatorNpc( p->Get_CharNO() ) )
				pMinimap->RemoveAutodeleteIndicatorNpc( p->Get_CharNO() );
		}


		if( CCountry::GetSingleton().IsApplyNewVersion() )
		{
			if( m_hNpcFace )
			{
				unloadTexture( m_hNpcFace );
				m_hNpcFace = NULL;
				m_widthNpcFace	= 0;
				m_heightNpcFace = 0;
			}

			m_hNpcFace = CImageResManager::GetSingleton().Load_NpcFace( NPC_FACE_ICON( p->Get_CharNO() ) );
			if( m_hNpcFace )
				::getTextureSize( m_hNpcFace, m_widthNpcFace, m_heightNpcFace );
		}

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

int	 CDialogDlg::IsInValidShow()
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
void CDialogDlg::Hide()
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
		g_itMGR.ChangeState( IT_MGR::STATE_NORMAL );
}

void CDialogDlg::Draw()
{
	if( !IsVision() ) return;

	// 홍근 : 히어로 퀘스트 추가.
	if( m_fNpctalkinterfaceHideTime > 0 )
	{
		m_fNpctalkinterfaceHideTime -= g_GameDATA.GetElapsedFrameTime();
		return;
	}

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
			if( !m_strNpcName.empty() )
			{
				D3DXMATRIX mat;	
				D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y, 0 );
				::setTransformSprite( mat );

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

short CDialogDlg::GetTargetNpcClientObjectIndex()
{
	return m_nTargetClientIdx;
}

void CDialogDlg::Show()
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
bool CDialogDlg::Create( const char* IDD )
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

void CDialogDlg::SetNpcFace( HNODE hNode_ )
{
	m_hNpcFace = hNode_;
}

void CDialogDlg::SetNpcName( std::string strName )
{
	m_strNpcName = strName;
}

//홍근 : 히어로 퀘스트
void CDialogDlg::SetNpctalkinterfaceHide( float fTime )
{
	m_fNpctalkinterfaceHideTime = fTime;
}

//홍근 : 히어로 퀘스트
float CDialogDlg::GetNpctalkinterfaceHide()
{
	return m_fNpctalkinterfaceHideTime;
}

