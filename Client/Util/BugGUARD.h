#ifndef	__SN_DEBUG_H
#define	__SN_DEBUG_H
//-------------------------------------------------------------------------------------------------

#ifdef	DLL_BUGGUARD_EXPORTS
	#define DLL_BUGGUARD	__declspec(dllexport)
#else
	#define DLL_BUGGUARD	__declspec(dllimport)
#endif


/*
	1.	LONG __stdcall UnhandledExceptionFilter ( EXCEPTION_POINTERS* pExPtrs);
		함수를 작성. 리턴값은 MSDN 참조 !!!
		함수 내부에서 아래의 함수를 호출하여 결과를 얻는다.

		LPCSTR	GetExceptionReason	( EXCEPTION_POINTERS* pExPtrs );
		LPCSTR	GetRegisters		( EXCEPTION_POINTERS* pExPtrs );
		LPCSTR	GetFirstCallStack	( EXCEPTION_POINTERS* pExPtrs );
		LPCSTR	GetNextCallStack	( EXCEPTION_POINTERS* pExPtrs );

	2.	SetUnhandledExceptionFilter (UnhandledExceptionFilter); 호출.

	3.	끝.


	Ex)
	LONG __stdcall UnhandledExceptionFilter( EXCEPTION_POINTERS* pExPtrs)
	{
		// Program Version:		각자 알아서...
		// Computer Name:		"
		// Time:				"

		OutputDebugString ( GetExceptionReason( pExPtrs ) );
		OutputDebugString ( GetRegisters	  ( pExPtrs ) );

		LPCSTR lpCStr;

		lpCStr = GetFirstCallStack ( pExPtrs );
		if ( lpCStr ) {
			OutputDebugString ( lpCStr );

			while ( lpCStr = GetNextCallStack (pExPtrs ) )
				OutputDebugString ( lpCStr );
		}

		return EXCEPTION_EXECUTE_HANDLER;
	}
*/
///*
LPCSTR DLL_BUGGUARD __stdcall GetExceptionReason( EXCEPTION_POINTERS* pExPtrs );
LPCSTR DLL_BUGGUARD __stdcall GetRegisters		( EXCEPTION_POINTERS* pExPtrs );
LPCSTR DLL_BUGGUARD __stdcall GetFirstCallStack ( EXCEPTION_POINTERS* pExPtrs );
LPCSTR DLL_BUGGUARD __stdcall GetNextCallStack	( EXCEPTION_POINTERS* pExPtrs );
//*/
/*
LPCSTR DLL_BUGGUARD __stdcall _		( EXCEPTION_POINTERS* pExPtrs );	// GetExceptionReason
LPCSTR DLL_BUGGUARD __stdcall __	( EXCEPTION_POINTERS* pExPtrs );	// GetRegisters
LPCSTR DLL_BUGGUARD __stdcall ___	( EXCEPTION_POINTERS* pExPtrs );	// GetFirstCallStack
LPCSTR DLL_BUGGUARD __stdcall ____	( EXCEPTION_POINTERS* pExPtrs );	// GetNextCallStack


#define	GetExceptionReason		_
#define GetRegisters			__
#define GetFirstCallStack		___
#define GetNextCallStack		____
*/

//-------------------------------------------------------------------------------------------------
#endif

