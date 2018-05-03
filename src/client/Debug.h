#ifndef _DEBUG_HEADER_
#define _DEBUG_HEADER_
//-------------------------------------------------------------------------------------------------

#ifdef _USE_BG
void SetExceptionReport (void);

void ReportZnzinLog ( const char * pStrMsg, int iNumLogLines );

extern std::string g_TempDebugMsg;
extern std::string g_CrashMsg;

void SendServerDisconnectMsg( const char* strTitle );
void SendDebugMsgToWeb( const char* pStrMsg );

#endif


#endif //_DEBUG_HEADER_
//-------------------------------------------------------------------------------------------------
