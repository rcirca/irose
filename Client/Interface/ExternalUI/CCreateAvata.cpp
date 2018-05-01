#include "stdafx.h"
#include "CCreateAvata.h"

#include "..\\..\\Network\\CNetwork.h"

#include "../../../TGameCtrl/TEditBox.h"
#include "../../../TGameCtrl/JComboBox.h"
#include "../../../TGameCtrl/TContainer.h"
#include "../../../TGameCtrl/TListCellRenderer.h"
#include "../../../TGameCtrl/JListBox.h"
#include "../../../TGameCtrl/TContainerItem.h"
#include "../../../TGameCtrl/TMsgBox.h"
#include "ExternalUILobby.h"
#include "../../Game.h"
#include "../../System/CGame.h"
#include "../../System/SystemProcScript.h"
#include "../../GameData/CGameDataCreateAvatar.h"
#include "../../GameCommon/CFilterWord.h"


#include "../it_MGR.h"

CCreateAvata::CCreateAvata()
{
	m_iSelectedSex = 0;
	m_iSelectedFace= 0;
	m_iSelectedHair= 0;
	m_iSelectedStartPos= 0;
	m_iSelectedBirthStone=0;
}

CCreateAvata::~CCreateAvata()
{
}

///
/// set observer class
///
void CCreateAvata::SetEUIObserver( CExternalUIManager* pObserver )
{
}

UINT CCreateAvata::Process( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;

	unsigned iProcID;
	if( iProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			{
				OnLButtonUp( iProcID );
				break;
			}
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CCreateAvata::SetAvataInfo( char* strName, cli_CREATE_CHAR& info )
{
	if( strName != NULL )
		m_strAvataName = std::string( strName );
	else
		m_strAvataName = std::string( "" );

	memcpy( &m_AvataInfo, &info, sizeof( m_AvataInfo ) );
}

///
/// Create Avata
///
void CCreateAvata::SendCreateAvata( )
{
	
	strcpy( g_MsgBuf, m_strAvataName.c_str() );
	g_pNet->Send_cli_CREATE_CHAR( g_MsgBuf, 
									m_AvataInfo.m_btCharRACE,
									m_AvataInfo.m_cBoneSTONE,
									m_AvataInfo.m_cHairIDX,
									m_AvataInfo.m_cFaceIDX,
									0,
									m_AvataInfo.m_nZoneNO );
	g_EUILobby.ShowWaitMsgBox();
}


bool CCreateAvata::RecvCreateAvata( t_PACKET* recvPacket )
{
	switch ( recvPacket->m_wsv_CREATE_CHAR.m_btResult ) 
	{
	case RESULT_CREATE_CHAR_OK				:
		CGame::GetInstance().ChangeState( CGame::GS_SELECTAVATAR );
		return true;
	case RESULT_CREATE_CHAR_FAILED			:
		g_EUILobby.ShowMsgBox( CStr::Printf( "%s(%d)",STR_FAIL_MAKE_NEW_CHARACTER, 1), CMsgBox::BT_OK , true, 0 );
		break;
	case RESULT_CREATE_CHAR_DUP_NAME		:
		g_EUILobby.ShowMsgBox( STR_RESULT_CREATE_CHAR_DUP_NAME, CMsgBox::BT_OK , true, 0 );
		break;
	case RESULT_CREATE_CHAR_INVALID_VALUE	:
		g_EUILobby.ShowMsgBox( CStr::Printf( "%s(%d)",STR_FAIL_MAKE_NEW_CHARACTER, 2), CMsgBox::BT_OK , true, 0 );
		break;
	case RESULT_CREATE_CHAR_NO_MORE_SLOT:
		g_EUILobby.ShowMsgBox( STR_CANT_CREATE_MORE_CHARACTER, CMsgBox::BT_OK , true, 0 );
		break;
	case RESULT_CREATE_CHAR_BLOCKED:
		g_EUILobby.ShowMsgBox( "현재 서버에서는 캐릭터 생성이 불가능합니다. 자세한 내용은 홈페이지를 참고해주세요.", CMsgBox::BT_OK , true, 0 );
		break;
	default:
		g_EUILobby.ShowMsgBox( STR_FAIL_MAKE_NEW_CHARACTER, CMsgBox::BT_OK , true, 0 );
		break;
	}
	return false;
}

void CCreateAvata::OnLButtonUp( unsigned iProcID )
{
	switch( iProcID )
	{
	case IID_BTN_OK:
		{
			int iSex		= atoi( m_mapSex[m_iSelectedSex].m_strValue.c_str() );
			int iBoneStone	= atoi( m_mapBirthStone[m_iSelectedBirthStone].m_strValue.c_str() );
			int iHair		= atoi( m_mapHair[m_iSelectedHair].m_strValue.c_str() );
			int iFace		= atoi( m_mapFace[m_iSelectedFace].m_strValue.c_str() );
			int iStartPos   = atoi( m_mapStartPos[m_iSelectedStartPos].m_strValue.c_str() );

			
			char szName[255] ={0};

			CWinCtrl* pCtrl = Find( IID_EDITBOX );
			if( pCtrl == NULL || pCtrl->GetControlType() != CTRL_EDITBOX )
			{
				LogString (LOG_NORMAL, "Not Found EditBox(ID-%d) in CCreateAvata::OnLButtonUp\n",IID_EDITBOX );
				return ;
			}
			CTEditBox* pEditBox = ( CTEditBox* ) pCtrl;

			strcpy( szName, pEditBox->get_text() );

			if( CFilterWord::GetInstance().IsValidName( szName) )
			{
				CreateAvata( szName, 0, iSex, iBoneStone, iHair, iFace, iStartPos );
			}
			else
			{
				g_EUILobby.ShowMsgBox(STR_INVALID_CHARACTER_NAME,CTMsgBox::BT_OK ,true ,GetDialogType() );
				pEditBox->clear_text();
			}
				
		}
		break;
	case IID_BTN_CANCEL:
		CGame::GetInstance().ChangeState( CGame::GS_SELECTAVATAR );

//		g_EUILobby.CreateServerListDlg();
		break;
	case IID_BTN_LEFT_SEX:
		--m_iSelectedSex;
		if( m_iSelectedSex < 0 )
			m_iSelectedSex = m_mapSex.size() - 1;
		CSystemProcScript::GetSingleton().CallLuaFunction( "OnChangeSex", ZZ_PARAM_INT, 
			atoi(m_mapSex[m_iSelectedSex].m_strValue.c_str()), ZZ_PARAM_END );
		break;
	case IID_BTN_LEFT_FACE:
		--m_iSelectedFace;
		if( m_iSelectedFace < 0 )
			m_iSelectedFace = m_mapFace.size() - 1;

		CSystemProcScript::GetSingleton().CallLuaFunction( "OnChangeFace", ZZ_PARAM_INT, 
			atoi( m_mapFace[m_iSelectedFace].m_strValue.c_str()), ZZ_PARAM_END );
		break;
	case IID_BTN_LEFT_HAIR:
		--m_iSelectedHair;
		if( m_iSelectedHair < 0 )
			m_iSelectedHair = m_mapHair.size() - 1 ;

		CSystemProcScript::GetSingleton().CallLuaFunction( "OnChangeHair", ZZ_PARAM_INT, 
			atoi( m_mapHair[m_iSelectedHair].m_strValue.c_str()), ZZ_PARAM_END );
		break;
	case IID_BTN_LEFT_STARTPOS:
		--m_iSelectedStartPos;
		if( m_iSelectedStartPos < 0 )
			m_iSelectedStartPos = m_mapStartPos.size() - 1;
		break;
	case IID_BTN_LEFT_BIRTHSTONE:
		--m_iSelectedBirthStone;
		if( m_iSelectedBirthStone < 0 )
			m_iSelectedBirthStone = m_mapBirthStone.size() - 1;
		break;
	case IID_BTN_RIGHT_SEX:
		++m_iSelectedSex;
		if( m_iSelectedSex >= m_mapSex.size() )
			m_iSelectedSex = 0;

		CSystemProcScript::GetSingleton().CallLuaFunction( "OnChangeSex", ZZ_PARAM_INT, 
			atoi(m_mapSex[m_iSelectedSex].m_strValue.c_str()), ZZ_PARAM_END );

		break;
	case IID_BTN_RIGHT_FACE:
		++m_iSelectedFace;
		if( m_iSelectedFace >= m_mapFace.size() )
			m_iSelectedFace = 0;

		CSystemProcScript::GetSingleton().CallLuaFunction( "OnChangeFace", ZZ_PARAM_INT, 
			atoi( m_mapFace[m_iSelectedFace].m_strValue.c_str()), ZZ_PARAM_END );

		break;
	case IID_BTN_RIGHT_HAIR:
		++m_iSelectedHair;
		if( m_iSelectedHair >= m_mapHair.size() )
			m_iSelectedHair = 0;

		CSystemProcScript::GetSingleton().CallLuaFunction( "OnChangeHair", ZZ_PARAM_INT, 
			atoi( m_mapHair[m_iSelectedHair].m_strValue.c_str()), ZZ_PARAM_END );

		break;
	case IID_BTN_RIGHT_STARTPOS:
		++m_iSelectedStartPos;
		if( m_iSelectedStartPos >= m_mapStartPos.size() )
			m_iSelectedStartPos = 0;
		break;
	case IID_BTN_RIGHT_BIRTHSTONE:
		++m_iSelectedBirthStone;
		if( m_iSelectedBirthStone >= m_mapBirthStone.size() )
			m_iSelectedBirthStone = 0;
		break;
	default:
		break;
	}
}




void CCreateAvata::CreateAvata( char* strName, int iRace, int iSex, int iBoneStone, int iHair, int iFace, int iStartPos )
{
	if( strName == NULL || strcmp( strName, "" ) == 0 )
		return;

	cli_CREATE_CHAR info;

	info.m_btCharRACE = iSex;
	info.m_cBoneSTONE = iBoneStone;
	info.m_cHairIDX = iHair;
	info.m_cFaceIDX = iFace;
	
	info.m_nZoneNO = iStartPos;

	SetAvataInfo( strName, info );
	SendCreateAvata( );
}

void CCreateAvata::FailCreateAvata()
{
	g_EUILobby.ShowMsgBox(STR_FAIL_MAKE_NEW_CHARACTER,CTMsgBox::BT_OK ,true ,GetDialogType());
}

void CCreateAvata::MsgBoxProc(  unsigned int iMsgType, short int iButton )
{
	g_EUILobby.HideMsgBox();
}

void CCreateAvata::ClearSelectItem( int iType )
{
	switch( iType )
	{
	case 0:
		m_mapSex.clear();
		break;
	case 1:
		m_mapFace.clear();
		break;
	case 2:
		m_mapHair.clear();
		break;
	case 3:
		m_mapStartPos.clear();
		break;
	case 4:
		m_mapBirthStone.clear();
		break;
	default:
		break;
	}
}

void CCreateAvata::AddSelectItem( int iType, const char* pszIdentify, const char* pszValue )
{
	assert( pszIdentify && pszValue );
	if( pszIdentify == NULL || pszValue == NULL ) return;

	AvatarSelectItem Item;
	Item.m_strValue    = pszValue;
	Item.m_strIdentify = pszIdentify;
	
	switch( iType )
	{
	case 0:
		m_mapSex.push_back( Item );
		break;
	case 1:
		m_mapFace.push_back( Item );
		break;
	case 2:
		m_mapHair.push_back( Item );
		break;
	case 3:
		m_mapStartPos.push_back( Item );
		break;
	case 4:
		m_mapBirthStone.push_back( Item );
		break;
	default:
		break;
	}
}

void CCreateAvata::SelectItem( int iType, unsigned iSubscript )
{
	switch( iType )
	{
	case 0:
		assert( iSubscript >= 0 && iSubscript < m_mapSex.size() );
		if(  iSubscript >= 0 && iSubscript < m_mapSex.size() )
			m_iSelectedSex = iSubscript;
		break;
	case 1:
		assert( iSubscript >= 0 && iSubscript < m_mapFace.size() );
		if(  iSubscript >= 0 && iSubscript < m_mapFace.size() )
			m_iSelectedFace = iSubscript;
		break;
	case 2:
		assert( iSubscript >= 0 && iSubscript < m_mapHair.size() );
		if( iSubscript >= 0 && iSubscript < m_mapHair.size() )
			m_iSelectedHair= iSubscript;
		break;
	case 3:
		assert( iSubscript >= 0 && iSubscript < m_mapStartPos.size() );
		if( iSubscript >= 0 && iSubscript < m_mapStartPos.size()  )
			m_iSelectedStartPos= iSubscript;
		break;
	case 4:
		assert( iSubscript >= 0 && iSubscript < m_mapBirthStone.size() );
		if( iSubscript >= 0 && iSubscript < m_mapBirthStone.size() )
			m_iSelectedBirthStone=iSubscript;
		break;
	default:
		break;
	}
}

void CCreateAvata::Show()
{
	POINT pt = {  0, 100 };

	MoveWindow( pt );

	CTDialog::Show();
	CWinCtrl* pCtrl = Find( IID_EDITBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
	{
		CTEditBox* pEditBox = (CTEditBox*)pCtrl;
		pEditBox->SetFocus( true );
	}
}

void CCreateAvata::Hide()
{
	CTDialog::Hide();


	for( int i = 0; i < 5; ++i )
		ClearSelectItem( i );
}

void CCreateAvata::Draw()
{
	if( !IsVision() ) return;

	CTDialog::Draw();
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	RECT rcDraw;
	SetRect( &rcDraw, 172, 155, 235, 174 );
	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE, DT_CENTER, m_mapSex[m_iSelectedSex].m_strIdentify.c_str() );
	SetRect( &rcDraw, 172, 185, 235, 205 );
	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE, DT_CENTER, m_mapFace[m_iSelectedFace].m_strIdentify.c_str() );
	SetRect( &rcDraw, 172, 215, 235, 235 );
	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE, DT_CENTER, m_mapHair[m_iSelectedHair].m_strIdentify.c_str() );
	SetRect( &rcDraw, 172, 245, 235, 268 );
	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE, DT_CENTER, m_mapStartPos[m_iSelectedStartPos].m_strIdentify.c_str() );
	SetRect( &rcDraw, 172, 275, 235, 295 );
	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE, DT_CENTER, m_mapBirthStone[m_iSelectedBirthStone].m_strIdentify.c_str() );
}