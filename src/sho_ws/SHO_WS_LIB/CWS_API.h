#ifndef __CWS_API_H
#define __CWS_API_H
//---------------------------------------------------------------------------

class EXE_WS_API
{
public :
	virtual	void  __stdcall SetUserCNT( int iUserCNT ) = 0;

	virtual void  __stdcall WriteLOG(char *szString) = 0;
    virtual void  __stdcall SetListItemINT(void *pListItem, int iSubStrIDX, int iValue) = 0;
	virtual void  __stdcall SetListItemSTR(void *pListItem, int iSubStrIDX, char *szStr) = 0;

	virtual void* __stdcall AddChannelITEM(void *pOwner, short nChannelNO, char *szChannelName, char *szServerIP, int iPortNO ) = 0;
	virtual void  __stdcall DelChannelITEM(void *pListItem) = 0;

	virtual void* __stdcall AddUserITEM(void *pUser, char *szAccount, char *szCharName, char *szIP) = 0;
	virtual void  __stdcall DelUserITEM(void *pListItem) = 0;
} ;

#if	defined(__BORLANDC__) || defined(_MSC_VER)
    class CEXE_API : public EXE_WS_API
    {
    public :
		void  __stdcall SetUserCNT( int iUserCNT );

		void  __stdcall WriteLOG(char *szString);
        void  __stdcall SetListItemINT(void *pListItem, int iSubStrIDX, int iValue);
		void  __stdcall SetListItemSTR(void *pListItem, int iSubStrIDX, char *szStr);

        void* __stdcall AddChannelITEM(void *pOwner, short nChannelNO, char *szChannelName, char *szServerIP, int iPortNO );
        void  __stdcall DelChannelITEM(void *pListItem);

		void* __stdcall AddUserITEM(void *pUser, char *szAccount, char *szCharName, char *szIP);
		void  __stdcall DelUserITEM(void *pListItem);
    } ;
#endif

//---------------------------------------------------------------------------
#endif

 
