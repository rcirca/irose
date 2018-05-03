#pragma once

class CSystemUtil
{
public:
	CSystemUtil(void);
	~CSystemUtil(void);

	DWORD		FindProcess( char* szExeName );
	BOOL		KillProcess ( DWORD dwPID );
};
