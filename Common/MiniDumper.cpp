#include "stdafx.h"
#include "MiniDumper.h"
#include "CAssertNew.h"
#include <dbghelp.h>
#include <signal.h>
#include <tchar.h>

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
	HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
	);

MiniDumper::DumpLevel MiniDumper::s_DumpLevel                = MiniDumper::DUMP_LEVEL_0;
bool                  MiniDumper::s_bAddTimeStamp            = true;
TCHAR                 MiniDumper::s_szAppName[_MAX_PATH]     = {0,};
TCHAR                 MiniDumper::s_szFaultReason[2048]      = {0,};

//-------------------------------------------------------------------------------------------------
// Crash Handlers
//-------------------------------------------------------------------------------------------------
inline void terminator() 
{
	int*z = 0; *z=13; 
}

inline void signal_handler(int)
{
	terminator();
}

inline void __cdecl invalid_parameter_handler(const wchar_t *, const wchar_t *, const wchar_t *, unsigned int, uintptr_t)
{
	terminator();
}

MiniDumper::MiniDumper()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(-1L);

	s_DumpLevel     = DUMP_LEVEL_0;
	s_bAddTimeStamp = true;

	TCHAR szFilename[_MAX_PATH];
	GetModuleFileName(NULL, szFilename, _MAX_PATH);

	TCHAR* dot = _tcsrchr(szFilename, '.');
	lstrcpyn(s_szAppName, szFilename, (int)(dot - szFilename + 1));

	signal(SIGABRT, signal_handler);
	_set_abort_behavior(0, _WRITE_ABORT_MSG|_CALL_REPORTFAULT);

	set_terminate( &terminator );
	set_unexpected( &terminator );
	_set_purecall_handler( &terminator );
	_set_invalid_parameter_handler( &invalid_parameter_handler );

	SetUnhandledExceptionFilter(TopLevelFilter);
}

MiniDumper::MiniDumper(DumpLevel DL, bool bAddTimeStamp)
{
	Assert(s_szAppName[0] == 0, "");
	Assert(DL >= 0,"");
	Assert(DL <= DUMP_LEVEL_2,"");

	s_DumpLevel     = DL;
	s_bAddTimeStamp = bAddTimeStamp;

	TCHAR szFilename[_MAX_PATH];
	GetModuleFileName(NULL, szFilename, _MAX_PATH);

	TCHAR* dot = _tcsrchr(szFilename, '.');
	lstrcpyn(s_szAppName, szFilename, (int)(dot - szFilename + 1));

	signal(SIGABRT, signal_handler);
	_set_abort_behavior(0, _WRITE_ABORT_MSG|_CALL_REPORTFAULT);

	set_terminate( &terminator );
	set_unexpected( &terminator );
	_set_purecall_handler( &terminator );
	_set_invalid_parameter_handler( &invalid_parameter_handler );

	SetUnhandledExceptionFilter(TopLevelFilter);
}

MiniDumper::~MiniDumper()
{
}

LONG WINAPI MiniDumper::TopLevelFilter(struct _EXCEPTION_POINTERS* pExPtr)
{
	LONG    retval                   = EXCEPTION_CONTINUE_SEARCH;
	HMODULE hDLL                     = NULL;
	TCHAR   szDbgHelpPath[_MAX_PATH] = {0, };
	TCHAR   szDumpPath[MAX_PATH * 2] = {0,};
	
	if(GetModuleFileName(NULL, szDbgHelpPath, _MAX_PATH))
	{
		LPTSTR pSlash = _tcsrchr(szDbgHelpPath, '\\');
		if (pSlash)
		{
			lstrcpy(pSlash + 1, _T("DBGHELP.DLL"));
			hDLL = ::LoadLibrary(szDbgHelpPath);
		}
	}
	if (hDLL == NULL) hDLL = ::LoadLibrary(_T("DBGHELP.DLL"));
	if (hDLL == NULL)
	{
		return retval;
	}

	MINIDUMPWRITEDUMP pfnMiniDumpWriteDump = 
		(MINIDUMPWRITEDUMP)::GetProcAddress(hDLL, "MiniDumpWriteDump");

	if (pfnMiniDumpWriteDump == NULL)
	{
		return retval;
	}

	if (s_bAddTimeStamp)
	{
		SYSTEMTIME t;
		GetLocalTime(&t);

		TCHAR szTail[_MAX_PATH];
		_stprintf_s(szTail, _MAX_PATH,
			_T(" %04d-%02d-%02d %02d-%02d-%02d"),
			t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

		lstrcat(szDumpPath, s_szAppName);
		lstrcat(szDumpPath, szTail);
	}

	lstrcat(szDumpPath, _T(".dmp"));

	HANDLE hFile = ::CreateFile(
		szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		//printf("Failed to create dump file '%s' (error %s)", 
		//	szDumpPath, GetLastErrorString().c_str());
		return retval;
	}

	MINIDUMP_EXCEPTION_INFORMATION ExceptionParam;

	ExceptionParam.ThreadId = ::GetCurrentThreadId();
	ExceptionParam.ExceptionPointers = pExPtr;
	ExceptionParam.ClientPointers = FALSE;

	BOOL bResult = FALSE;
	switch (s_DumpLevel)
	{
	case DUMP_LEVEL_0: // MiniDumpNormal
		bResult = pfnMiniDumpWriteDump(
			GetCurrentProcess(), ::GetCurrentProcessId(), 
			hFile, MiniDumpNormal, &ExceptionParam, NULL, NULL);
		break;
	case DUMP_LEVEL_1: // MiniDumpWithDataSegs 
		bResult = pfnMiniDumpWriteDump(
			GetCurrentProcess(), ::GetCurrentProcessId(), 
			hFile, MiniDumpWithDataSegs, &ExceptionParam, NULL, NULL);
		break;
	case DUMP_LEVEL_2: // MiniDumpWithFullMemory 
		bResult = pfnMiniDumpWriteDump(
			GetCurrentProcess(), ::GetCurrentProcessId(), 
			hFile, MiniDumpWithFullMemory, &ExceptionParam, NULL, NULL);
		break;
	default:
		bResult = pfnMiniDumpWriteDump(
			GetCurrentProcess(), ::GetCurrentProcessId(), 
			hFile, MiniDumpNormal, &ExceptionParam, NULL, NULL);
		break;
	}

	if (bResult)
	{
		TCHAR szMessage[8192] = {0,};
		lstrcat(szMessage, _T("Saved dump file to '"));
		lstrcat(szMessage, szDumpPath);
		lstrcat(szMessage, _T("'.\nFault Reason : "));
		lstrcat(szMessage, GetFaultReason(pExPtr));

		//printf(szMessage);
		retval = EXCEPTION_EXECUTE_HANDLER;
	}
	else
	{
		//printf("Failed to save dump file to '%s' (error %d,%s)", 
		//	szDumpPath, ::GetLastError(), GetLastErrorString().c_str());

		//Assert(false);
		assert(false);
	}

	CloseHandle(hFile);

	return retval;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief 
/// 
/// \param pExPtrs 
/// \return LPCTSTR
//////////////////////////////////////////////////////////////////////////////
LPCTSTR MiniDumper::GetFaultReason(struct _EXCEPTION_POINTERS* pExPtrs)
{
	if (IsBadReadPtr(pExPtrs, sizeof(EXCEPTION_POINTERS))) 
		return _T("BAD EXCEPTION POINTERS");

	switch (pExPtrs->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:         return _T("EXCEPTION_ACCESS_VIOLATION");
	case EXCEPTION_DATATYPE_MISALIGNMENT:    return _T("EXCEPTION_DATATYPE_MISALIGNMENT");
	case EXCEPTION_BREAKPOINT:               return _T("EXCEPTION_BREAKPOINT");
	case EXCEPTION_SINGLE_STEP:              return _T("EXCEPTION_SINGLE_STEP");
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return _T("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
	case EXCEPTION_FLT_DENORMAL_OPERAND:     return _T("EXCEPTION_FLT_DENORMAL_OPERAND");
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return _T("EXCEPTION_FLT_DIVIDE_BY_ZERO");
	case EXCEPTION_FLT_INEXACT_RESULT:       return _T("EXCEPTION_FLT_INEXACT_RESULT");
	case EXCEPTION_FLT_INVALID_OPERATION:    return _T("EXCEPTION_FLT_INVALID_OPERATION");
	case EXCEPTION_FLT_OVERFLOW:             return _T("EXCEPTION_FLT_OVERFLOW");
	case EXCEPTION_FLT_STACK_CHECK:          return _T("EXCEPTION_FLT_STACK_CHECK");
	case EXCEPTION_FLT_UNDERFLOW:            return _T("EXCEPTION_FLT_UNDERFLOW");
	case EXCEPTION_INT_DIVIDE_BY_ZERO:       return _T("EXCEPTION_INT_DIVIDE_BY_ZERO");
	case EXCEPTION_INT_OVERFLOW:             return _T("EXCEPTION_INT_OVERFLOW");
	case EXCEPTION_PRIV_INSTRUCTION:         return _T("EXCEPTION_PRIV_INSTRUCTION");
	case EXCEPTION_IN_PAGE_ERROR:            return _T("EXCEPTION_IN_PAGE_ERROR");
	case EXCEPTION_ILLEGAL_INSTRUCTION:      return _T("EXCEPTION_ILLEGAL_INSTRUCTION");
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return _T("EXCEPTION_NONCONTINUABLE_EXCEPTION");
	case EXCEPTION_STACK_OVERFLOW:           return _T("EXCEPTION_STACK_OVERFLOW");
	case EXCEPTION_INVALID_DISPOSITION:      return _T("EXCEPTION_INVALID_DISPOSITION");
	case EXCEPTION_GUARD_PAGE:               return _T("EXCEPTION_GUARD_PAGE");
	case EXCEPTION_INVALID_HANDLE:           return _T("EXCEPTION_INVALID_HANDLE");
	case 0xE06D7363:                         return _T("Microsoft C++ Exception");
	default:
		break;
	}

	lstrcpy(s_szFaultReason, _T("Unknown")); 
	FormatMessage(
		FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
		GetModuleHandle(_T("NTDLL.DLL")),
		pExPtrs->ExceptionRecord->ExceptionCode, 
		0,
		s_szFaultReason,
		0,
		NULL);

	return s_szFaultReason;
}

void MiniDumper::BreakMe()
{
	terminator();
}
