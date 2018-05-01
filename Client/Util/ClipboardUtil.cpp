#include "stdafx.h"
#include <string>
#include ".\clipboardutil.h"

CClipboardUtil::CClipboardUtil(void)
{
}

CClipboardUtil::~CClipboardUtil(void)
{
}

void CClipboardUtil::CopyStringtoClibboard(std::string istr)
{
	char * ptr;
	HGLOBAL hmem;
	hmem = GlobalAlloc(GHND, istr.size()+1);
	ptr = (char *)GlobalLock(hmem);
	strcpy(ptr, istr.c_str());
	GlobalUnlock(hmem);
	if(OpenClipboard(NULL))
	{
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hmem);
		CloseClipboard();
	}
}
std::string CClipboardUtil::GetStringFromClibboard()
{
	HGLOBAL hmem;
	char * ptr;
	std::string str;

	if(IsClipboardFormatAvailable(CF_TEXT))
	{
		OpenClipboard(NULL);
		hmem=GetClipboardData(CF_TEXT);
		ptr=(char *)GlobalLock(hmem);

		str = std::string( ptr );
		GlobalUnlock(hmem);
		CloseClipboard();
	}
	return str;
}