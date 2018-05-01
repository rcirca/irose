
#include "StdAfx.h"
#include "Debug.h"	
#include <string>
#include "Game.h"

#ifdef _USE_BG

#include "Util\\BugGUARD.h"
#include "Util/classHTTP.h"
#include "Object.h"
#include "IO_Terrain.h"
#include "TriggerInfo.h"
#include "System/CGame.h"
#include "Interface/it_MGR.h"

#include "Util/FileVersionInfo.h"

const char* DEBUG_REPORT_IP = /*"211.54.74.142"*//*"support.trigger.co.kr"*/"aeongames.net";

std::string g_TempDebugMsg;
std::string g_CrashMsg = "Program is ending because of unknown error. Would you like to send this error?";

void GetHeaderString ( std::string& strURLMSG, std::string& strText )
{
	strText = "\n\n===================================================================\n";
	strText += "                           NEW_INFO                                \n";
	strText += "===================================================================\n\n";

	
	sprintf( g_MsgBuf, "%d_%d_%d_%d_%d", 
					g_GameDATA.m_SystemTime.wYear,
					g_GameDATA.m_SystemTime.wMonth,
					g_GameDATA.m_SystemTime.wDay,
					g_GameDATA.m_SystemTime.wHour,
					g_GameDATA.m_SystemTime.wMinute );

	strText += "[" + std::string( g_MsgBuf ) + "]\n";

	CFileVersionInfo versionInfo;
	if( versionInfo.GetVersionInfo( "TRose.exe" ) )
		versionInfo.GetVersionInfo( "D_TRose.exe" );

	if (g_pAVATAR) {
		strURLMSG += "[" + std::string( g_pAVATAR->Get_NAME() ) + "]<br \>";	
		sprintf( g_MsgBuf, "[Version:[%d.%d.%d.%d]-[%s],GameState:%d]<br \>[Lev:%d]<br \>[Server:%s %s]<br \>[ZoneNO:%d]<br \>[Pos:%f,%f]<br \>[PlayTime:%d]<br \>", 
				versionInfo.m_iMajorVersion,
				versionInfo.m_MinorVersion,
				versionInfo.m_BuildNumber,
				versionInfo.m_RevisionNumber,
				::getEngineVersion(),
				CGame::GetInstance().GetCurrStateID(), 
				g_pAVATAR->Get_LEVEL(), 
				g_GameDATA.m_strJoinServerName.c_str(),
				g_GameDATA.m_strJoinChannelName.c_str(),
				g_pTerrain->GetZoneNO(), g_pAVATAR->Get_CurPOS().x, g_pAVATAR->Get_CurPOS().y, g_GameDATA.m_dwElapsedGameTime );
	}
	else 
	{
		strURLMSG += "[이름없음]<br \>";	
		sprintf( g_MsgBuf, "[ Version:[%d.%d.%d.%d]-[%s],GameState:%d]<br \>", 
			versionInfo.m_iMajorVersion,
			versionInfo.m_MinorVersion,
			versionInfo.m_BuildNumber,
			versionInfo.m_RevisionNumber,
			::getEngineVersion(),
			CGame::GetInstance().GetCurrStateID());
	}

	strURLMSG += std::string( g_MsgBuf ); 
}


void ReportZnzinLog ( const char * pStrMsg, int iNumLogLines )
{
	std::string strText;
	std::string strURLMSG;
	
	GetHeaderString( strURLMSG, strText );

	strURLMSG += std::string( readLogString( iNumLogLines, "<BR \>" ) );

	if( ::MessageBox( NULL, pStrMsg, "ERROR", MB_YESNO | MB_TOPMOST ) == IDYES )
	{
		std::string strInfo = "bugstring=" + strURLMSG;

		TI_SendHttpPostData (DEBUG_REPORT_IP, "/~support/get_clientinfo.php", strInfo.c_str ());
	}	
}


LONG __stdcall WGUnhandledExceptionFilter ( EXCEPTION_POINTERS* pExPtrs)
{
	std::string strText;
	std::string strURLMSG;

	char *pText =	"알수없는 오류로 인해 종료합니다.\n";
	
	GetHeaderString( strURLMSG, strText );
		
	char* pStr = (char*)GetExceptionReason( pExPtrs );
	strText += std::string( pStr ) + "\n";
	///strURLMSG += std::string( pStr ) + "\n";

	pStr = (char*)GetRegisters ( pExPtrs );
	strText += std::string( pStr ) + "\n";
	///strURLMSG += std::string( pStr ) + "\n";

	// Call stack
	strText += std::string( "\n[[Call Stack]]" ) + "\n";
	strURLMSG += std::string( pStr ) + "\n";

	pStr = (char*)GetFirstCallStack ( pExPtrs );
	if ( pStr ) 
	{
		strText += std::string( pStr ) + "\n";
		strURLMSG += std::string( pStr ) + "<br \>";

		while ( pStr = (char*)GetNextCallStack (pExPtrs ) )
		{
			strText += std::string( pStr ) + "\n";
			strURLMSG += std::string( pStr ) + "<br \>";
		}
	}	

	// engine log message
	strURLMSG += "<br \>";
	strURLMSG += std::string( readLogString( 20, "<BR \>" ) );

	std::string strBugInfo;
	if( ::MessageBox( NULL, g_CrashMsg.c_str(), "ERROR", MB_YESNO | MB_TOPMOST ) == IDYES )
	{
		strBugInfo = "bugstring=" + strURLMSG;
		strBugInfo += TI_GetSysInfo();
		strBugInfo += g_TempDebugMsg;
		
		TI_SendHttpPostData (DEBUG_REPORT_IP, "/~support/get_clientinfo.php", strBugInfo.c_str ());

		//g_pCApp->ErrorBOX( pText, "감사합니다.");
	}

	FILE* fp;
	fp = fopen( "Error.txt", "at" );
	if( fp != NULL )
	{
		fwrite( strBugInfo.c_str(), strlen( strBugInfo.c_str() ) + 1, 1, fp );

		fclose( fp );
	}

	return EXCEPTION_EXECUTE_HANDLER;
}
void SetExceptionReport (void)
{
	SetUnhandledExceptionFilter (WGUnhandledExceptionFilter);
}

void SendServerDisconnectMsg( const char* strTitle )
{
	std::string strBugInfo;

	std::string strURLMSG;
	std::string strText;
	GetHeaderString( strURLMSG, strText );

	strBugInfo = std::string( strTitle ) + "FORCE SERVER DISCONNECT<br \><br \>";
	strBugInfo += strURLMSG;

	SendDebugMsgToWeb( strBugInfo.c_str() );
}

void SendDebugMsgToWeb( const char* pStrMsg )
{	
	TI_SendHttpPostData (DEBUG_REPORT_IP, "/~support/get_clientinfo.php", pStrMsg);
}
//-------------------------------------------------------------------------------------------------

#endif