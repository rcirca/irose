#include "stdafx.h"
#include "libString.h"
#include <windows.h>

bool CLibString::CompareNI (const char * s1, const char * s2)
{
	if(s1 == s2)
		return true;

	if(s1 == NULL || s2 == NULL)
		return false;

	return ( stricmp ( s1 , s2 ) == 0 );
}