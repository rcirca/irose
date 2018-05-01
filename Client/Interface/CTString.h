#ifndef _CTString_
#define _CTString_

/**
* 초기에 작성된 string class : CInfo에서 사용중이다
*
* @Author		최종진
* @Date			2005/9/5
*/
class CTString
{
public:
	CTString(void);
	~CTString(void);

	void SetString( const char* pszTxt );
	void SetColor( DWORD dwColor );
	void SetFormat( UINT uFormat );
	void SetFont( HNODE hFont );
	void Draw( RECT& rcDraw );

	std::string	m_strText;
	DWORD		m_dwColor;
	UINT		m_uFormat;
	HNODE		m_hFont;

};
#endif