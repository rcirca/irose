#include "stdafx.h"
#include "exuicommand.h"
#include "../../../TGameCtrl/TDialog.h"
#include "../../Network/CNetwork.h"

CTCmdDeleteCharacter::CTCmdDeleteCharacter(int iSelectListID, const char* pszName )
{
	m_iSelectListID = iSelectListID;
	m_strName = pszName;
}

bool CTCmdDeleteCharacter::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_DELETE_CHAR( m_iSelectListID, true,(char*)m_strName.c_str() );
//	g_pNet->Send_cli_CHAR_LIST();
	return true;
}

CTCmdReviveCharacter::CTCmdReviveCharacter(int iSelectListID, const char* pszName )
{
	m_iSelectListID = iSelectListID;
	m_strName = pszName;
}

bool CTCmdReviveCharacter::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_DELETE_CHAR( m_iSelectListID, false, (char*)m_strName.c_str() );
//	g_pNet->Send_cli_CHAR_LIST();
	return true;
}
