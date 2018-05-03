#include "stdafx.h"
#include "BlankInfo.h"

int CBlankInfo::iMaxCount = 0;
int CBlankInfo::iDealedCount = 0;
int CBlankInfo::iCurPercent = 0;
VCALLBACK_CLEARBLANKALL CBlankInfo::procCallBack = NULL;

/****************************************************************
 *
 * ClearBlankAll 진행 될때마다 한번씩 Call 해서 상태를 저장하고 
 * Callbakc 함수가 있으면 call 해서 user에게 보여 줄수 있도록 한다
 *
 */
void CBlankInfo::DoStep ( void )
{
	iDealedCount++;

	if( procCallBack )
	{
		int iPerCnt = 100;
		if(iMaxCount > 0)
		{
			iPerCnt = ( (float)iDealedCount / iMaxCount ) * 1000;
		}

		if( iPerCnt > iCurPercent )
		{
			iCurPercent = iPerCnt;
			procCallBack (iPerCnt);
		}
	}
}


void CBlankInfo::SetStep ( int iPos )
{
	iDealedCount = iPos;

	if( procCallBack )
	{
		int iPerCnt = 100;
		if(iMaxCount > 0)
		{
			iPerCnt = ( (float)iDealedCount / iMaxCount ) * 1000;
		}

		if( iPerCnt > iCurPercent )
		{
			iCurPercent = iPerCnt;
			procCallBack (iPerCnt);
		}
	}
}