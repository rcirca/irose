#ifndef _CTSCROLLBARTYPE_
#define _CTSCROLLBARTYPE_
#include <crtdbg.h>
/**
* CTScrollBar의 Type에 따른 Strategy Class( 수평/수직 )
*		- 현재 Strategy Pattern을 쓰지않아도 무방하지만 디버깅의 용이함과 앞으로 실시간으로 스크롤바가
*		- 타입이 바뀔경우도 생각해서 만든다.
*
* @Author		최종진
*
* @Date			2005/8/30
*/
class CTScrollBarType
{
public:
	CTScrollBarType(void);
	virtual ~CTScrollBarType(void);
	///스크롤바 타입(TScrollBar Type )

	virtual POINT GetPrevButtonOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE PrevButtonSize ) = 0 { _ASSERT( 0 && "CTScrollBarType::GetPrevButtonOffset"); POINT pt; return pt;};
	virtual POINT GetNextButtonOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE NextButtonSize ) = 0 { _ASSERT( 0 && "CTScrollBarType::GetNextButtonOffset"); POINT pt; return pt;};
	virtual POINT GetScrollBoxOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE PrevButtonSize ,SIZE ScrollBoxSize ) = 0 { _ASSERT( 0 && "CTScrollBarType::GetScrollBoxOffset"); POINT pt; return pt;};
	virtual RECT  GetThumbMoveRange( POINT ptScrollBarPos,POINT ptNextButtonPos,SIZE ScrollBarSize, SIZE PrevButtonSize, SIZE ScrollBoxSize ) = 0 { _ASSERT( 0 && "CTScrollBarType::GetThumbMoveRange"); RECT rc; return rc; };

	enum{
		TSBT_VERTICAL,
		TSBT_HORIZONTAL
	};

};

class CTScrollBarTypeVertical : public CTScrollBarType
{
public:
	CTScrollBarTypeVertical(void){};
	virtual ~CTScrollBarTypeVertical(void){};

	virtual POINT GetPrevButtonOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE PrevButtonSize );
	virtual POINT GetNextButtonOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE NextButtonSize );
	virtual POINT GetScrollBoxOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE PrevButtonSize ,SIZE ScrollBoxSize);
	virtual RECT  GetThumbMoveRange( POINT ptScrollBarPos,POINT ptNextButtonPos,SIZE ScrollBarSize, SIZE PrevButtonSize, SIZE ScrollBoxSize );

};

class CTScrollBarTypeHorizontal : public CTScrollBarType
{
public:
	CTScrollBarTypeHorizontal(void){};
	virtual ~CTScrollBarTypeHorizontal(void){};

	virtual POINT GetPrevButtonOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE PrevButtonSize );
	virtual POINT GetNextButtonOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE NextButtonSize );
	virtual POINT GetScrollBoxOffset( POINT ptScrollBarOffset, SIZE ScrollBarSize, SIZE PrevButtonSize ,SIZE ScrollBoxSize);
	virtual RECT  GetThumbMoveRange( POINT ptScrollBarPos,POINT ptNextButtonPos,SIZE ScrollBarSize, SIZE PrevButtonSize, SIZE ScrollBoxSize );

};

#endif