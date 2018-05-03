#include "StdAfx.h"
#include ".\tscrollboxtype.h"

CTScrollBoxType::CTScrollBoxType(void)
{
	m_bTickMove = false;
}

CTScrollBoxType::~CTScrollBoxType(void)
{
}
void CTScrollBoxType::SetOffset( POINT pt )
{ 
	m_ptOffset = pt; 
}
void CTScrollBoxType::SetTickMove( bool bTickMove )
{
	m_bTickMove = bTickMove;
}
///*----------------------------------------------------------------------------------------------------------------------------*/
///CTScrollBoxTypeVertical
///*----------------------------------------------------------------------------------------------------------------------------*/
POINT CTScrollBoxTypeVertical::GetMoveBoxPos( RECT MoveRange, int iValue, int iMax ,int iExtent )
{
	POINT ptRet = {0,0};

	ptRet.x = MoveRange.left;

	int iRangeHeight = MoveRange.bottom - MoveRange.top;
	if( iValue <= 0 )
		ptRet.y = MoveRange.top;
	else if( iValue > iMax - iExtent )
		ptRet.y = MoveRange.bottom;
	else
		ptRet.y = MoveRange.top + ( iValue * (iRangeHeight )) / ( iMax - iExtent ) ;


	return ptRet;
}

POINT CTScrollBoxTypeVertical::GetMoveBoxPosByScreenPoint(SIZE sizeScrollBox, RECT MoveRange, POINT ptMove, int iMax ,int iExtent ,int& Value )
{
	POINT ptRet = ptMove;

	ptRet.y -= m_ptOffset.y;

	ptRet.x = MoveRange.left;
	
	//홍근 : 스크롤바 영역 밖으로 나가지 않게 하는 부분.
	//if( ptRet.y <= MoveRange.top + sizeScrollBox.cy )
	if( ptRet.y <= MoveRange.top )
	{
		ptRet.y = MoveRange.top;
		Value = 0;
	}
	//else if( ptRet.y >= MoveRange.bottom - sizeScrollBox.cy )
	else if( ptRet.y >= MoveRange.bottom )
	{
		ptRet.y = MoveRange.bottom;
		Value = iMax - iExtent ;
	}
	else
	{
		int iRangeHeight = MoveRange.bottom - MoveRange.top;
		if( iRangeHeight > 0)
			Value = ( ptRet.y - MoveRange.top ) * ( iMax - iExtent + 1 ) / iRangeHeight ;
		
		if( m_bTickMove )
		{
			if( Value == 0 )
			{
				ptRet.y = MoveRange.top;
			}
			else if( Value >= iMax - iExtent )
			{
				ptRet.y = MoveRange.bottom;
			}
			else
			{
				ptRet.y = MoveRange.top + ( Value * (iRangeHeight )) / ( iMax - iExtent ) ;
			}
		}
	}
	
	return ptRet;
}
///*----------------------------------------------------------------------------------------------------------------------------*/
///CTScrollBoxTypeHorizontal
///*----------------------------------------------------------------------------------------------------------------------------*/
POINT CTScrollBoxTypeHorizontal::GetMoveBoxPos( RECT MoveRange, int iValue, int iMax ,int iExtent )
{
	POINT ptRet = {0,0};

	ptRet.y = MoveRange.top;

	int iRangeWidth = MoveRange.right - MoveRange.left;
	if( iValue <= 0 )
		ptRet.x = MoveRange.left;
	else if( iValue > iMax - iExtent )
		ptRet.x = MoveRange.right;
	else
		ptRet.x = MoveRange.left + ( iValue * (iRangeWidth )) / ( iMax - iExtent ) ;


	return ptRet;
}

POINT CTScrollBoxTypeHorizontal::GetMoveBoxPosByScreenPoint( SIZE sizeScrollBox, RECT MoveRange, POINT ptMove, int iMax ,int iExtent ,int& Value )
{
	POINT ptRet = ptMove;
	
	ptRet.x -= m_ptOffset.x;

	ptRet.y = MoveRange.top;
	
	if( ptRet.x <= MoveRange.left + sizeScrollBox.cx )
	{
		ptRet.x = MoveRange.left;
		Value = 0;
	}
	else if( ptRet.x >= MoveRange.right - sizeScrollBox.cx )
	{
		ptRet.x = MoveRange.right;
		Value = iMax - iExtent ;
	}
	else
	{
		int iRangeWidth = MoveRange.right - MoveRange.left;
		if( iRangeWidth > 0)
		{
			Value = ( ptRet.x - MoveRange.left ) * ( iMax - iExtent + 1 ) / iRangeWidth ;
			
		}

		if( m_bTickMove )
		{
			if( Value <= 0 )
			{
				ptRet.x = MoveRange.left;
			}
			else if( Value >= iMax - iExtent )
			{
				ptRet.x = MoveRange.right;
			} 
			else
			{
				ptRet.x = MoveRange.left + ( Value * (iRangeWidth )) / ( iMax - iExtent ) ;
			}
		}
	}
	return ptRet;
}
