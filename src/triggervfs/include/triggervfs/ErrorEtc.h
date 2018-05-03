#ifndef __ERRORETC_H__
#define __ERRORETC_H__

void ODS (char *fmt, ...);

#define VERROR_NONE				0
#define VERROR_TOOMANYENTRY		1
#define VERROR_2
#define VERROR_3
#define VERROR_4
#define VERROR_5
#define VERROR_6
#define VERROR_7
#define VERROR_8
#define VERROR_9
#define VERROR_10
#define VERROR_11
#define VERROR_12

class CErrorInfo
{
private:
	static DWORD m_dwLastErrorCode;

public:
	static void Refresh ( void );
	static DWORD GetError ( void );
};

#endif
