#pragma once

#include <wininet.h>


class CHttp
{
public:
	CHttp(void);
	~CHttp(void);

	static bool SendPostData (const char * szServerIP, const char * szPage, const char * pData);

	static bool SendQueryData (const char * szURL);
	
};
