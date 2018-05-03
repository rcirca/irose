#ifndef __CLS_API_H
#define __CLS_API_H
//---------------------------------------------------------------------------

class EXE_LS_API
{
public :
	virtual void  __stdcall WriteLOG(char *szString) = 0;
    virtual void  __stdcall SetListItemINT(void *pListItem, int iSubStrIDX, int iValue) = 0;
	virtual void  __stdcall SetListItemSTR(void *pListItem, int iSubStrIDX, char *szStr) = 0;

	virtual void* __stdcall AddConnectorITEM (void *pOwner, char *szIP) = 0;
	virtual void  __stdcall DelConnectorITEM (void *pListItem) = 0;

	virtual void* __stdcall AddWorldITEM (void *pOwner, char *szWorld, char *szIP, int iPort, unsigned int dwRight) = 0;
	virtual void  __stdcall DelWorldITEM (void *pListItem) = 0;

	virtual void* __stdcall AddBlockITEM (void *pOwner, char *szIP, unsigned int dwEndTime) = 0;
	virtual void  __stdcall DelBlockITEM (void *pListItem) = 0;

	virtual void  __stdcall SetStatusBarTEXT (unsigned int iItemIDX, char *szText) = 0;
} ;

#if defined(__BORLANDC__) || defined(_MSC_VER)
    class CEXE_API : public EXE_LS_API
    {
    public :
		void  __stdcall WriteLOG(char *szString);
        void  __stdcall SetListItemINT(void *pListItem, int iSubStrIDX, int iValue);
		void  __stdcall SetListItemSTR(void *pListItem, int iSubStrIDX, char *szStr);

		void* __stdcall AddConnectorITEM (void *pOwner, char *szIP);
		void  __stdcall DelConnectorITEM (void *pListItem);

		void* __stdcall AddWorldITEM (void *pOwner, char *szWorld, char *szIP, int iPort, unsigned int dwRight);
		void  __stdcall DelWorldITEM (void *pListItem);

		void* __stdcall AddBlockITEM (void *pOwner, char *szIP, unsigned int dwEndTime);
		void  __stdcall DelBlockITEM (void *pListItem);

		void  __stdcall SetStatusBarTEXT (unsigned int iItemIDX, char *szText);
    } ;
#endif

//---------------------------------------------------------------------------
#endif

 
