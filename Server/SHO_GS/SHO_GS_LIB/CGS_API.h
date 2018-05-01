#ifndef __CGS_API_H
#define __CGS_API_H
//---------------------------------------------------------------------------
/**
 * ingroup SHO_GS_LIB
 * file	CGS_API.h
 * class	EXE_GS_API
 * author	wookSang.Jo
 * brief	sho_gs_exe와 통신하는 클래스
 */
class EXE_GS_API
{
public :
	virtual	 void __stdcall SetUserCNT( int iUserCNT ) = 0;

	virtual void  __stdcall WriteLOG(char *szString) = 0;
    virtual void  __stdcall SetListItemINT(void *pListItem, int iSubStrIDX, int iValue) = 0;
	virtual void  __stdcall SetListItemSTR(void *pListItem, int iSubStrIDX, char *szStr) = 0;

	virtual void* __stdcall	AddZoneITEM(void *pOwner, short nZoneNO, char *szZoneName) = 0;
	virtual void  __stdcall DelZoneITEM(void *pListItem) = 0;
} ;

#if	defined(__BORLANDC__) || defined(_MSC_VER)
    class CEXE_API : public EXE_GS_API
    {
    public :
		void  __stdcall SetUserCNT( int iUserCNT );

		void  __stdcall WriteLOG(char *szString);
        void  __stdcall SetListItemINT(void *pListItem, int iSubStrIDX, int iValue);
		void  __stdcall SetListItemSTR(void *pListItem, int iSubStrIDX, char *szStr);

		void* __stdcall	AddZoneITEM(void *pOwner, short nZoneNO, char *szZoneName);
		void  __stdcall DelZoneITEM(void *pListItem);
    } ;
#endif

//---------------------------------------------------------------------------
#endif
