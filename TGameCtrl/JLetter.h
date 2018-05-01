#ifndef _CJLetter_
#define _CJLetter_

#include <string>
#include "TGameCtrl.h"

//------------------------------------------------------------------------------------------------------------------------
///	문자를 저장할 클래스
/// - 연관class			CJString
///
/// @Author				최종진
///
/// @Date				2005/08/30
//------------------------------------------------------------------------------------------------------------------------

class TGAMECTRL_API CJLetter
{
public:
	CJLetter();
	~CJLetter(){}

	void	SetLetter( const char* pszLetter );
	void	SetSize( SIZE& size );
	void	SetFont( int iFont );
	void	SetColor( DWORD dwColor );

	const SIZE& GetSize();
	int			GetFont();
	DWORD		GetColor();
	const char* GetLetter();


private:
	int				m_iFont;						/// Client에 정의된 Font Index : 잘못된 Index일경우 안나오거나 잘못 나올수 있다(Client의 구현에 따라 틀림 )
	DWORD			m_dwColor;						/// 색상				
	SIZE            m_Size;							/// Draw시 Width,Height
	std::string		m_Letter;						

};
#endif