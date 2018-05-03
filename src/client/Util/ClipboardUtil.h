#pragma once
#include <string>

class CClipboardUtil
{

public:
	CClipboardUtil(void);
	~CClipboardUtil(void);

	static void			CopyStringtoClibboard(std::string istr);
	static std::string  GetStringFromClibboard();
};


