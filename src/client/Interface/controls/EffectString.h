#ifndef _CEffectString_
#define _CEffectString_

#include "tgamectrl/TSplitString.h"
/**
* 시간에 따라 폰트 색깔및 알파값이 변경되는 String출력용 Class
*	- Outline Font의 경우 Alpha값이 변경될경우 Outline이 같이 변경되지 않는 상황으로 사용안되고 있다.
*	- Update()를 수정하면  Alpha값및 색상이 변경되도록 사용가능하다
*
* @Author		최종진
* @Date			2005/9/6
*/
class CEffectString
{
	CTSplitString	m_parser;			/// 텍스트 저장 
	POINT			m_draw_position;	/// 화면 좌표
	DWORD			m_time_display;		/// 화면에 표시할 총시간
	DWORD			m_time_set;			/// 이전 업데이트 시간
	BYTE			m_color_red;
	BYTE			m_color_green;
	BYTE			m_color_blue;
	BYTE			m_color_alpha;
	int				m_font;

	int				m_type;				/// 객체를 구분하기 위한 타입
public:
	CEffectString(void);
	virtual ~CEffectString(void);
	void	SetColor( BYTE red, BYTE green, BYTE blue );
	bool	SetString( int font, char* message, POINT draw_position, int display_width, DWORD display_time );

	int		Update();
	void	Draw();

	void	SetType( int type );
	int		GetType();
};
#endif