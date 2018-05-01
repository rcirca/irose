
#include <windows.h>
#include <wininet.h>
#include <IO.h>
#include <stdio.h>
#include "classSTR.h"
#include "classUTIL.h"

#pragma comment (lib, "Wininet.lib")
//#pragma comment (lib, "WSOCK32.LIB")
#pragma comment (lib, "ws2_32.lib")


//-------------------------------------------------------------------------------------------------

#define	STATIC_STR_LEN		512
char  CUtil::m_szStr[ STATIC_STR_LEN+1 ];

//-------------------------------------------------------------------------------------------------
char *CUtil::GetTokenFirst(char *pStr, char *pDelimiters)
{
	char *pToken;
	pToken = strtok( pStr, pDelimiters );
	
	if ( pToken ) {
		switch( pToken[ 0 ] ) {
			case '\'' :
			{
				strcpy(m_szStr, &pToken[ 1 ]);
				pToken = strtok( NULL, "\'");
				strcat(m_szStr, pToken);
				return m_szStr;
			}
			case '\"' :
			{
				strcpy(m_szStr, &pToken[ 1 ]);
				pToken = strtok( NULL, "\"");
				strcat(m_szStr, pToken);
				return m_szStr;
			}
		}
	}

	return pToken;
}

char *CUtil::GetTokenNext (char *pDelimiters)
{	
	char *pToken;
	pToken = strtok( NULL, pDelimiters );

	if ( pToken ) {
		switch( pToken[ 0 ] ) {
			case '\'' :
			{
				strcpy(m_szStr, &pToken[ 1 ]);
				pToken = strtok( NULL, "\'");
				strcat(m_szStr, pToken);
				OutputDebugString ( "   TokenNEXT[");
				OutputDebugString ( m_szStr );
				OutputDebugString ( "]\n");
				return m_szStr;
			}
			case '\"' :
			{
				strcpy(m_szStr, &pToken[ 1 ]);
				pToken = strtok( NULL, "\"");
				strcat(m_szStr, pToken);
				OutputDebugString ( "   TokenNEXT[");
				OutputDebugString ( m_szStr );
				OutputDebugString ( "]\n");
				return m_szStr;
			}
		}
	}

	return pToken;
}

//-------------------------------------------------------------------------------------------------
char *CUtil::GetCurrentDir (void)
{
	DWORD dwRet;
	dwRet = ::GetCurrentDirectory(STATIC_STR_LEN, m_szStr);
	if ( dwRet > 0 )
		return m_szStr;

	return NULL;
}

//-------------------------------------------------------------------------------------------------
bool CUtil::Is_FileExist (char *szFileName)
{
#ifdef __BORLANDC__
    if ( access (szFileName, 0) != -1 )
        return true;
#else
    if ( _access (szFileName, 0) != -1 )
        return true;
#endif

    return false;
}

//-------------------------------------------------------------------------------------------------
//  주민등록 번호 검증.
bool CUtil::Check_PersonalNumber (char *szNumber)
{
    const int nWeight[] = { 2, 3, 4, 5, 6, 7, 8, 9, 2, 3, 4, 5 };
    int nLoop, nSum, nRest;

    if ( strlen(szNumber) != 13 )
        return false;

    nSum = 0;
    for (nLoop=0; nLoop<12; nLoop++)
        nSum += ( szNumber[ nLoop ] -'0' ) * nWeight[ nLoop ];

    nRest = 11 - ( nSum % 11 );

    return ( (nRest%10) == ( szNumber[ 12 ] - '0' ) );
}


//-------------------------------------------------------------------------------------------------
//  사업자 등록번호 검증.
bool CUtil::Check_BussinessNumber (char *szNumber)
{
    const int nWeight[] = { 1, 3, 7, 1, 3, 7, 1, 3 };
    int nLoop, nSum;

    nSum = 0;
    for (nLoop=0; nLoop<8; nLoop++)
        nSum += ( szNumber[nLoop]-'0' ) * nWeight[ nLoop ];

    nLoop = ( szNumber[ 8 ]-'0' ) * 5;
    nSum  = nSum + (nLoop / 10) + (nLoop % 10);
    nSum %= 10;

    if ( !nSum )
        nSum = (10 - nSum);

    if ( nSum != (szNumber[ 9 ]-'0') )
        return false;

    return true;
}

//-------------------------------------------------------------------------------------------------
DWORD CUtil::Is_InternetConnected (void)
{
	struct {
		DWORD m_dwFlag;
		char *m_szDesc;
	} netSTATUS [] = {
//		{	INTERNET_CONNECTION_CONFIGURED,	"Local system has a valid connection to the Internet, but it may or may not be currently connected.    "	},
		{	INTERNET_CONNECTION_LAN,		"Local system uses a local area network to connect to the Internet.    "	},
		{	INTERNET_CONNECTION_MODEM,		"Local system uses a modem to connect to the Internet.    "	},
		{	INTERNET_CONNECTION_MODEM_BUSY,	"No longer used.    "	},
//		{	INTERNET_CONNECTION_OFFLINE,	"Local system is in offline mode.    "	},
		{	INTERNET_CONNECTION_PROXY,		"Local system uses a proxy server to connect to the Internet.    "	},
///		{	INTERNET_RAS_INSTALLED,			"Local system has RAS installed.  "	},
		{	0,	NULL	}
	};

	DWORD dwReturn;

	// dwFlags = INTERNET_CONNECTION_LAN | INTERNET_CONNECTION_MODEM | INTERNET_CONNECTION_PROXY;
	dwReturn = 0;

	if ( ::InternetGetConnectedState( &dwReturn, 0) ) {
		for (short nI=0; NULL != netSTATUS[ nI ].m_szDesc; nI++) {
			if ( dwReturn & netSTATUS[ nI ].m_dwFlag ) {
				// g_CConsole.Printf ("[[ %s ]]\n", netSTATUS[ nI ].m_szDesc);
			}
		}
	} else
		dwReturn = 0;

	return dwReturn;
}


//-------------------------------------------------------------------------------------------------
bool CUtil::Get_HostName (char *szRecvBuff, int iBuffLen)
{
	return ( SOCKET_ERROR != ::gethostname(szRecvBuff, iBuffLen) );
}

bool CUtil::Get_IPAddressFromHostName (const char *szHostName, char *szBuffer, short nBufferSize)
{
	if ( !szBuffer ) 
		return false;

    int err;

    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD( 1, 1 );

    err = ::WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        return false;
    }

    if ( LOBYTE( wsaData.wVersion ) != LOBYTE( wVersionRequested ) ||
         HIBYTE( wsaData.wVersion ) != HIBYTE( wVersionRequested ) ) {
        ::WSACleanup( );
        return false;
    }

	bool bReturn = false;
	HOSTENT *pHE;

	pHE = ::gethostbyname ( szHostName );
	if ( pHE ) {
		char *pAddr;

		pAddr = inet_ntoa(*(struct in_addr*)*pHE->h_addr_list);
		if ( strlen( pAddr ) > (WORD)nBufferSize )
			strcpy (szBuffer, pAddr);
		else
			strncpy (szBuffer, pAddr, nBufferSize-1);

/*
		if ( pHE->h_addrtype == AF_INET )  { 
			struct in_addr *ptr;
			char **listptr=pHE->h_addr_list;

			while ( (ptr=(struct in_addr *) *listptr++) != NULL ) {
				char *pAddr;

				pAddr = inet_ntoa(*(ptr));
				if ( strlen( pAddr ) > (WORD)nBufferSize )
					strcpy (szBuffer, pAddr);
				else
					strncpy (szBuffer, pAddr, nBufferSize-1);

				iCARUS_LogString(LOG_DEBUG,"%s\n",szBuffer);
			}
		}
*/
		bReturn = true;
	}

	::WSACleanup ();

	return bReturn;
}

//-------------------------------------------------------------------------------------------------
int CUtil::ExtractFilePath (char *pFullFilePath, char *pOutResult, WORD wOutBuffLEN )
{
	short nI, nLen;

	nLen = strlen( pFullFilePath )-1;
	for (nI=nLen; nI>=0 && pFullFilePath[ nI ] != '\\'; nI--) ;

	if ( nI >= 0 && nI < nLen ) {
		if ( pFullFilePath[ nI ] == '\\' ) {
			nI ++;
			// strcpy (pResult, &pFullPath[ nI+1 ] );
		}
		if ( pOutResult ) {
			strncpy (pOutResult, pFullFilePath, nI >= wOutBuffLEN ? wOutBuffLEN-1 : nI );
		}

		return nI;
	}

	return -1;
}
int CUtil::ExtractFileName (char *pResult, char *pFullPath)
{
	short nI, nLen;

	nLen = strlen( pFullPath )-1;
	for (nI=nLen; nI>=0 && pFullPath[ nI ] != '\\'; nI--) ;

	if ( nI >= 0 && nI < nLen ) {
		if ( pFullPath[ nI ] == '\\' ) {
			nI ++;
			// strcpy (pResult, &pFullPath[ nI+1 ] );
		}
		if ( pResult )
			strcpy (pResult, &pFullPath[ nI ]);

		return nI;
	}

	return -1;
}


//-------------------------------------------------------------------------------------------------
char *CUtil::GetLastErrorMsg( DWORD dwLastError )
{
	DWORD  dwReturn;
	LPVOID lpMsgBuf;

	dwReturn = FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwLastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	if ( dwReturn < CUTIL_MAX_STRLEN )
		::CopyMemory( m_szStr, lpMsgBuf, dwReturn );
	else {
		::CopyMemory( m_szStr, lpMsgBuf, CUTIL_MAX_STRLEN );
		m_szStr[ CUTIL_MAX_STRLEN ] = 0;
	}

	// Free the buffer.
	LocalFree( lpMsgBuf );

	return m_szStr;
}

//-------------------------------------------------------------------------------------------------
/*
extern "C" __declspec(naked) long _myftol(float arg) 
{
	__asm {
	    // store as a quadword int and reload
	    fld     st(0)                  // X X
	    fistp   QWORD PTR [esp-8]      // X
	    fild    QWORD PTR [esp-8]      // X [X]

		mov     edx,DWORD PTR [esp-4]
		mov     eax,DWORD PTR [esp-8]
		test    eax,eax
		je      maybe_zero

		// number isn't zero, so get X - [X]
	not_zero:
		fsubp   st(1),st               // X - [X]
		test    edx,edx
		jns     positive

		// number < 0 - inc eax if X - [X] is >0
		fstp    DWORD PTR [esp-8]
		mov     ecx,DWORD PTR [esp-8]  // get IEEE rep
		xor     ecx,80000000h          // now <0 if diff >0
		add     ecx,7FFFFFFFh          // carry if it was 00000001 to 7FFFFFFF
		adc     eax,0                  // add carry in
		ret

	positive:
		// number > 0 - dec eax if X - [X] is <0
		fstp    DWORD PTR [esp-8]
		mov     ecx,DWORD PTR [esp-8]  // get IEEE rep
		add     ecx,7FFFFFFFh          // carry if it was 80000001 to FFFFFFFF
		sbb     eax,0                  // sub carry
		ret

	maybe_zero:
		test    edx,7FFFFFFFh
		jnz     not_zero

		// number is zero - clear the stack
		fstp    st(0)
		fstp    st(0)
		ret
	}
}
*/