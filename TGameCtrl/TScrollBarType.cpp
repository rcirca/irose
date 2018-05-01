#include "StdAfx.h"
#include ".\tscrollbartype.h"

CTScrollBarType::CTScrollBarType(void)
{
}

CTScrollBarType::~CTScrollBarType(void)
{
}
///*----------------------------------------------------------------------------------------------------------------------------*/
///CTScrollBarTypeVertical
///*----------------------------------------------------------------------------------------------------------------------------*/
POINT CTScrollBarTypeVertical::GetPrevButtonOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE PrevButtonSize )
{
	POINT ptRet = ptScrollBarOffset;
	ptRet.x     += ScrollBarSize.cx / 2 - PrevButtonSize.cx / 2;
	return ptRet;
}
POINT CTScrollBarTypeVertical::GetNextButtonOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE NextButtonSize )
{
	POINT ptRet = {0,0};

	ptRet   = ptScrollBarOffset;
	ptRet.y = ptRet.y + ScrollBarSize.cy - NextButtonSize.cy;

	return ptRet;
}

POINT CTScrollBarTypeVertical::GetScrollBoxOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE PrevButtonSize ,SIZE ScrollBoxSize)
{
	POINT ptRet = {0,0};

	ptRet   = ptScrollBarOffset;
	ptRet.x = ptRet.x + ( (ScrollBarSize.cx - ScrollBoxSize.cx) / 2 );
	ptRet.y = ptRet.y + PrevButtonSize.cy;
	return ptRet;
}

RECT  CTScrollBarTypeVertical::GetThumbMoveRange( POINT ptScrollBarPos,POINT ptNextButtonPos,SIZE ScrollBarSize, SIZE PrevButtonSize, SIZE ScrollBoxSize )
{
	RECT rcRet = { 0,0,0,0 };

	rcRet.left   =  ptScrollBarPos.x + ( ScrollBarSize.cx - ScrollBoxSize.cx ) / 2;
	rcRet.top    =  ptScrollBarPos.y + PrevButtonSize.cy;
	rcRet.right  =  rcRet.left;
	rcRet.bottom =  ptNextButtonPos.y - ScrollBoxSize.cy;

	return rcRet;
}

///*----------------------------------------------------------------------------------------------------------------------------*/
///CTScrollBarTypeHorizontal
///*----------------------------------------------------------------------------------------------------------------------------*/
POINT CTScrollBarTypeHorizontal::GetPrevButtonOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE PrevButtonSize )
{
	POINT ptRet = ptScrollBarOffset;
	ptRet.y     += ScrollBarSize.cy / 2 - PrevButtonSize.cy / 2;
	return ptRet;
}
POINT CTScrollBarTypeHorizontal::GetNextButtonOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE NextButtonSize )
{
	POINT ptRet = {0,0};

	ptRet   = ptScrollBarOffset;
	ptRet.x = ptRet.x + ScrollBarSize.cx - NextButtonSize.cx;

	return ptRet;
}
POINT CTScrollBarTypeHorizontal::GetScrollBoxOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE PrevButtonSize ,SIZE ScrollBoxSize)
{
	POINT ptRet = {0,0 };

	ptRet   = ptScrollBarOffset;
	ptRet.x = ptRet.x + PrevButtonSize.cx;
	ptRet.y = ptRet.y + ( (ScrollBarSize.cy - ScrollBoxSize.cy) / 2 );
	return ptRet;
}

RECT  CTScrollBarTypeHorizontal::GetThumbMoveRange( POINT ptScrollBarPos,POINT ptNextButtonPos,SIZE ScrollBarSize, SIZE PrevButtonSize, SIZE ScrollBoxSize )
{
	RECT rcRet = { 0,0,0,0 };

	rcRet.left   = ptScrollBarPos.x  + PrevButtonSize.cx;
	rcRet.right  = ptNextButtonPos.x - ScrollBoxSize.cx;
	rcRet.top    = ptScrollBarPos.y + ( ScrollBarSize.cy - ScrollBoxSize.cy ) / 2;
	rcRet.bottom = rcRet.top;
	return rcRet;
}

