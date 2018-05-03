#ifndef _ISCROLLMODEL_
#define _ISCROLLMODEL_

#include <crtdbg.h>

//-------------------------------------------------------------------------------------------
/// Scroll을 붙이기 원하는 클래스가 Implement해야하는 Inteface
///		- Extent : 한 화면에서 보여질 Item의 개수
///
///
/// @Author				최종진
/// @Date				2003/11/26 
//-------------------------------------------------------------------------------------------

class TGAMECTRL_API IScrollModel
{
public:

	/// 현재 화면에서 제일 위에 그려질 Item의 인덱스 리턴
	virtual int GetValue() = 0; /*{ _ASSERT(0 && "IScrollModel::GetValue")		; return 0; }*/;
	
	/// 화면에 한번에 보여질 Item의 개수 리턴
	virtual int GetExtent() = 0; /*{ _ASSERT(0 && "IScrollModel::GetExtent")	; return 0; }*/;

	/// 의미상으로는 최대Item의 개수이지만 실제적으로는 현재 Item의 수를 리턴하고 있다.
	virtual int GetMaximum() = 0; /*{ _ASSERT(0 && "IScrollModel::GetMaximum")	; return 0; };*/

	/// CTree이외에는 모두 return 0;
	virtual int GetMinimum() = 0; /*{ _ASSERT(0 && "IScrollModel::GetMinimum")	; return 0; };*/
	




	/// 현재 화면에서 제일 위에 그려질 Item 변경
	virtual void SetValue( int ) = 0; /*{ _ASSERT(0 && "IScrollModel::SetValue");	};*/

	/// Set Extent
	virtual void SetExtent( int ) = 0; /*{ _ASSERT(0 && "IScrollModel::SetExtent");	};*/

	/// 거의 구현하지 않고 있다.
	virtual void SetMaximum( int ) = 0; /*{ _ASSERT(0 && "IScrollModel::SetMaximum");};*/

	/// 유명무실
	virtual void SetMinimum( int ) = 0;/* { _ASSERT(0 && "IScrollModel::SetMinimum");};*/

	/// ScrollBox에서 휠및 사용자 처리를 하기 위하여 Model의 화면상 영역이 필요하다
	virtual RECT GetWindowRect() = 0; /*{ _ASSERT( 0 && "IScrollModel::GetWindowRect"); RECT rc = {0,0,0,0}; return rc; };*/
	

	/// 마지막 아이템이 그려진 적이 있는가?
	virtual bool IsLastItemDrawn() { return true; }
};
#endif