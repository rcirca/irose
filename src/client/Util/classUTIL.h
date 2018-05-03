#ifndef	__CLASSUTIL_H
#define	__CLASSUTIL_H
//-------------------------------------------------------------------------------------------------


#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif


#ifndef	GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#endif

#define	CUTIL_MAX_STRLEN	512
class CUtil {
private :
	static char  m_szStr[ CUTIL_MAX_STRLEN+1 ];
public  :
	static bool	 Check_PersonalNumber  (char *szNumber);
	static bool  Check_BussinessNumber (char *szNumber);
	static bool  Is_FileExist (char *szFileName);
	static DWORD Is_InternetConnected (void);

	static bool  Get_HostName (char *szRecvBuff, int iBuffLen);
	static bool  Get_IPAddressFromHostName (const char *szHostName, char *szBuffer, short nBufferSize);

	static char *GetCurrentDir (void);
	static char *GetTokenFirst (char *pStr, char *pDelimiters);
	static char *GetTokenNext  (char *pDelimiters);
	static int   ExtractFilePath (char *pFullFilePath, char *pOutResult, WORD wOutBuffLEN );
	static int	 ExtractFileName (char *pResult, char *pFullPath);
	static char *GetLastErrorMsg (DWORD dwLastError);
} ;

extern "C" long My_ftol(float arg) ;

//-------------------------------------------------------------------------------------------------
#endif
