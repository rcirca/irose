#ifndef _CTSCROLLBOXTYPE_
#define _CTSCROLLBOXTYPE_
#include <crtdbg.h>

/**
* CTScrollBar의 Type에 따른 Strategy Class( 수평/수직 )
*
* @Author	최종진
* @Date		2005/8/30
*/

class CTScrollBoxType
{
public:
	CTScrollBoxType(void);
	~CTScrollBoxType(void);
	virtual POINT GetMoveBoxPos( RECT MoveRange, int iValue, int iMax ,int iExtent ) = 0 { _ASSERT( 0 && "CTScrollBoxType::GetMoveBoxPos"); POINT pt; return pt; };
	virtual POINT GetMoveBoxPosByScreenPoint( SIZE sizeScrollBox, RECT MoveRange, POINT ptMove, int iMax ,int iExtent ,int& Value ) = 0 { _ASSERT( 0 && "CTScrollBoxType::GetMoveBoxPosByScreenPoint"); POINT pt; return pt;};

	void	SetOffset( POINT pt );
	void	SetTickMove( bool bTickMove );
protected:
	
	POINT	m_ptOffset;			///마우스로 LButtonDn시 마우스포인터와 스크롤박스의 Left-Top과의 거리 저장
	bool	m_bTickMove;		///스크롤박스 이동시 가장 가까운 Tick으로 이동할것인가?
};

class CTScrollBoxTypeVertical : public CTScrollBoxType
{
public:
	CTScrollBoxTypeVertical(void){}
	~CTScrollBoxTypeVertical(void){}
	virtual POINT GetMoveBoxPos( RECT MoveRange, int iValue, int iMax ,int iExtent );
	virtual POINT GetMoveBoxPosByScreenPoint( SIZE sizeScrollBox, RECT MoveRange, POINT ptMove,int iMax ,int iExtent ,int& Value );

};

class CTScrollBoxTypeHorizontal : public CTScrollBoxType
{
public:
	CTScrollBoxTypeHorizontal(void){}
	~CTScrollBoxTypeHorizontal(void){}
	virtual POINT GetMoveBoxPos( RECT MoveRange, int iValue, int iMax ,int iExtent );
	virtual POINT GetMoveBoxPosByScreenPoint( SIZE sizeScrollBox,RECT MoveRange, POINT ptMove,int iMax ,int iExtent ,int& Value );

};

#endif