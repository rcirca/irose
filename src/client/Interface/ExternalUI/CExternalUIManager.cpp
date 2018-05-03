#include "stdafx.h"
#include "CExternalUIManager.h"
#include "../../Game.h"

#include "tgamectrl/tdialog.h"
#include "tgamectrl/tcombobox.h"
CExternalUIManager::CExternalUIManager()
{
	for( int i = 0; i < EUI_MAX; ++i )
		m_ExternalUI[ EUI_MAX ] = NULL;	
}

CExternalUIManager::~CExternalUIManager()
{
	for( int i = 0; i < EUI_MAX; ++i )
	{
		if( m_ExternalUI[ EUI_MAX ] )
		{
			delete m_ExternalUI[ i ];
			m_ExternalUI[ i ] = NULL;
		}
	}
}

///
/// add External UI in EUI manager
///
bool CExternalUIManager::AddEUI( unsigned int iType, CTDialog* pEUI )
{
	if( pEUI == NULL)
	{
		assert( pEUI == NULL && " pEUI is NULL" );
		return false;
	}

	if( iType > EUI_MAX )
	{
		assert( iType > EUI_MAX && " AddEUI(): iType > EUI_MAX" );
		return false;
	}

	if( m_ExternalUI[ iType ] )
	{
		delete m_ExternalUI[ iType ];
		m_ExternalUI[ iType ] = NULL;
	}

	m_ExternalUI[ iType ] = pEUI;

	
	return true;
}

///
/// get External UI from EUI manager
///
CTDialog* CExternalUIManager::GetEUI( int iType )
{
	if( iType > EUI_MAX )
	{
		assert( iType > EUI_MAX && " GetEUI() : iType > EUI_MAX" );
		return false;
	}	

	return m_ExternalUI[ iType ];
}

///
/// Init all EUIS
///
bool CExternalUIManager::InitExternalUIs()
{
	POINT ptNew = {0,0};

	//EUI_LOGIN,
	CLogin* pLoginDlg = new CLogin;
	AddEUI( EUI_LOGIN, pLoginDlg );
	pLoginDlg->SetEUIObserver( this );

	pLoginDlg->Create("DlgLogin");
	ptNew.x = g_pCApp->GetWIDTH()/ 2 - pLoginDlg->GetWidth()/2;
	ptNew.y = g_pCApp->GetHEIGHT() / 3;
	pLoginDlg->MoveWindow( ptNew );

#ifdef _WARNING
	// EUI_WARNING
	CWarningDlg * pWarningDlg = new CWarningDlg;
	AddEUI( EUI_WARNING, pWarningDlg );
	pWarningDlg->SetEUIObserver( this );

	pWarningDlg->Create("Warning");	
	ptNew.x = g_pCApp->GetWIDTH()/2 - pWarningDlg->GetWidth()/2;
	ptNew.y = g_pCApp->GetHEIGHT()/2 - pWarningDlg->GetHeight()/2;
	pWarningDlg->MoveWindow( ptNew );
#endif

	//EUI_SELECT_AVATA,
	CSelectAvata* pSelectAvata = new CSelectAvata;
	AddEUI( EUI_SELECT_AVATA, pSelectAvata );	
	pSelectAvata->SetEUIObserver( this );

	pSelectAvata->Create("DlgSelAvatar");

/*	ptNew.x = g_GameDATA.m_nScrWidth/ 2 - pSelectAvata->GetWidth()/2;
	ptNew.y = g_GameDATA.m_nScrHeight/ 3;
	pSelectAvata->MoveWindow( ptNew );
*/
	//EUI_CREATE_AVATA,
	CCreateAvata* pCreateAvata = new CCreateAvata;
	AddEUI( EUI_CREATE_AVATA, pCreateAvata );

	pCreateAvata->SetEUIObserver( this );

	pCreateAvata->Create("DlgCreateAvatar");
	ptNew.x = 100;//g_pCApp->GetWIDTH() / 2 + pCreateAvata->GetWidth();
	ptNew.y = 200;//g_pCApp->GetHEIGHT() / 3;

	pCreateAvata->MoveWindow( ptNew );

	//CTComboBox* pComboBox = NULL;
	// 
	//if( pComboBox = (CTComboBox*)pCreateAvata->Find( CCreateAvata::IID_COMBOBOX_RACE ) )
	//	pComboBox->AddItem("인간");


	//if( pComboBox = (CTComboBox*)pCreateAvata->Find( CCreateAvata::IID_COMBOBOX_SEX ) )
	//{
	//	pComboBox->AddItem("남자");
	//	pComboBox->AddItem("여자");
	//}



	//if( pComboBox = (CTComboBox*)pCreateAvata->Find( CCreateAvata::IID_COMBOBOX_BONESTONE ) )
	//{
	//	pComboBox->AddItem("0");
	//	pComboBox->AddItem("1");
	//	pComboBox->AddItem("2");
	//	pComboBox->AddItem("3");
	//}

	//if( pComboBox = (CTComboBox*)pCreateAvata->Find( CCreateAvata::IID_COMBOBOX_HAIR) )
	//{
	//	pComboBox->AddItem("1");
	//	pComboBox->AddItem("2");
	//}

	//if( pComboBox = (CTComboBox*)pCreateAvata->Find( CCreateAvata::IID_COMBOBOX_FACE) )
	//{
	//	pComboBox->AddItem("1");
	//	pComboBox->AddItem("2");
	//	pComboBox->AddItem("3");
	//}

	//if( pComboBox = (CTComboBox*)pCreateAvata->Find( CCreateAvata::IID_COMBOBOX_STARTPOS) )
	//{
	//	pComboBox->AddItem("모험자마을");
	//	pComboBox->AddItem("이상한필드");
	//}
	//EUI_SETTINT,
	return true;
}