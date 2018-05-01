#include "stdafx.h"
#include "CAssertNew.h"

// @Return: Returns false if the expression is true
// @Return: Returns true and breaks debugger if Expression is false
bool CustomAssertFunction( bool exp, char* description, int line, char* file, bool& ignore )
{
	if(!exp)
	{
		char* msg[250];
		sprintf_s( (char*)msg, 250, "%s-%i: %s", file, line, description );
		int rtn = MessageBoxA( NULL, (char*)msg, "Assert", MB_ABORTRETRYIGNORE );
		switch( rtn )
		{
		case IDOK:
			return true;
			break;
		case IDCANCEL:
			break;
		case IDABORT: // Abort
			break;
		case IDRETRY: // Retry
			return true;
			break;
		case IDIGNORE: // Ignore
			ignore = true;
			break;
		case IDYES:
		case IDNO:
		default:
			break;
		}
	}
	return false;
}