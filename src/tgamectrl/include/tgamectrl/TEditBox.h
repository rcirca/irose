#ifndef _TEDITBOX_
#define _TEDITBOX_

#include "winctrl.h"
class ITFont;
/**
* 에디트 박스 Class
* - 연관class		CTIme
*
* @Warning		다국어 지원 때문에 복잡도가 높아졌다. 수정시 모든 언어( 일어, 중국어에 대한 테스트가 필요하다 )
*
* @Author		최종진
*
* @Date			2005/8/30
*/
class TGAMECTRL_API CTEditBox :	public CWinCtrl
{
public:
	CTEditBox(void);
	virtual ~CTEditBox(void);
	bool    Create(int iScrX, int iScrY, int iWidth, int iHeight,int iCharWidth);

	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual void Draw();
	virtual void SetFocus(bool bFocus);
	virtual void Hide();
	virtual void Show();

	char*	get_text();
	void    SetText( const char* szTxt );
	/// IME에서 스트링을 가져와서 버퍼에 더하는 Method
	bool	add_text();											// 사용안함
	void	clear_text();
	void	delete_one_char(int iType);

	bool    chk_number( const char* szText );

	///입력된 문자의 처리
	bool	OnKeyDown(WPARAM wParam, LPARAM lParam );
	bool	OnChar( WPARAM wParam, LPARAM lParam );

	
	void	AppendText( const char* szText );					// 스트링을 버퍼에 더한다( 프롬프트는 맨뒤로 이동 )
	void    Insert( const char* text );							// 스트링을 현재 프롬프트위치에 Insert한다.
	static CTEditBox*	s_pFocusEdit;							// 현재 포커스를 가지고 있는 에디트 박스의 포인터


	enum{
		TES_NONE		= 0x0000,
		TES_NUMBER		= 0x0001,///숫자만 입력가능 
		TES_PASSWORD	= 0x0002,///Draw시에 *로 표시 
		TES_HIDECURSOR  = 0x0004,
	};

	enum{
		TES_ALIGN_LEFT,
		TES_ALIGN_CENTER,
		TES_ALIGN_RIGHT
	};

	void	SetType( DWORD dwType ){ m_dwType = dwType; }
	void	AddType( DWORD dwType ){ m_dwType |= dwType; }
	DWORD	GetType(){ return m_dwType; }

	bool	IsTypeNumber(){ return (m_dwType & TES_NUMBER)?true:false ;}
	bool	IsTypePassword(){ return ( m_dwType & TES_PASSWORD )?true:false; }
	bool	IsTypeHideCursor(){ return ( m_dwType & TES_HIDECURSOR)?true:false;}

	void	SetLimitText( int iLimit ){ m_iMaxCharCnt = iLimit; }
	void	SetTextAlign( int iAlign ){ m_iTextAlign = iAlign ; }

	void	SetMultiline( bool bMultiline );
	void	SetTextColor( int iColor );
	void	SetCharHeight( int iHeight );
protected:
	void	MoveEnd();


protected:
	int		m_iMaxCharCnt;			// 입력가능한 최대문자
	
	int		m_iCharWidth;			// 출력될 문자의 Width
	int		m_iCharHeight;			// 출력될 문자의 Height

	bool	m_bShowCursor;			// 프롬프트를 보여줄것인가?
	bool	m_bMultiline;			// 멀티라인 입력?( 실제적으로 Draw시에 줄넘김 기능만이 있어. 프롬프트의 위치가 이상하다 )

	int		m_iTextAlign;			// 출력시 정렬 : TES_ALIGN_LEFT, TES_ALIGN_CENTER, TES_ALIGN_RIGHT

	DWORD	m_dwTextColor;			// 폰트 색상
	DWORD   m_dwType;
	ITFont*	m_pFontMgr;				// client가 구현한 FontDraw Class의 포인터

	int		m_iPromptPos;			// 프롬프트 위치

	int		m_iViewPos;				// 입력된 문자열중 현재 화면에 보여질 최초의 문자위치
	char	m_pszBuffer[4096];		// 입력된 문자가 저장될 버퍼

	int		m_iFont;				// 폰트 인덱스 : 클라이언트에서 만들 폰트에 따른다.
	std::string m_strTemp;			
};
#endif ///_TEDITBOX_