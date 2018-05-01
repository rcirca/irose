#ifndef __BLANKINFO_H__
#define __BLANKINFO_H__

#include "TriggerVFS.h"

class CBlankInfo
{
public:
	static int iMaxCount;
	static int iDealedCount;
	static int iCurPercent;

	static VCALLBACK_CLEARBLANKALL procCallBack;
	static void DoStep ( void );
	static void SetStep ( int iPos );
};

#endif
