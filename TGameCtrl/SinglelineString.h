#ifndef _CSinglelineString_
#define _CSinglelineString_

#include "TGameCtrl.h"
#include <string>

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/// 표시할 String이 해당 영역을 벗어날 경우 뒤에 ".."을 붙혀서 영역안에 다 표시되도록 하는 Class
///		- Client에서 get_rect()와 is_ellipsis()로 마우스의 위치와 짤림을 체크하여 툴팁으로 전체 스트링을 보여주는 데 사용하고 있다.
///
/// @Author					최종진
/// @Date					2005/8/30
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
class TGAMECTRL_API CSinglelineString
{
public:
	CSinglelineString(void);
	~CSinglelineString(void);
	void	set_string( const char* msg, const RECT& rect , int ifont );
	void	set_rect( const RECT& rect );
	void	set_color( DWORD color );
	void	set_format( DWORD format );

	const char* get_string();
	const RECT&	get_rect();

	void	draw();
	bool	is_ellipsis();
	bool	empty();
	void	clear();


protected:
	std::string		m_msg;			/// 원본 스트링
	std::string		m_ellipsis_msg; /// 영역안에 다 표시가 안될경우 영역안에 표시할 string을 만들어서 저장해두는 string
	DWORD			m_format;		/// 
	RECT			m_rect;			/// 스트링을 표시할 영역
	bool			m_end_ellipsis;	/// 스트링이 해당 영역안에 다 표시가 안되는가?
	int				m_font;			/// 폰트
	DWORD			m_color;		/// 폰트 색상
};
#endif