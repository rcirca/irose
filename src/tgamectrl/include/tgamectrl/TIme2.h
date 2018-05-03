#ifndef _CTIme_
#define _CTIme_
#include <string>
#include <vector>
#include "TGameCtrl.h"
class CTCandidateWindow;


/**
* IME Class
* - ??Class	CTCandidateWindow, CTEdit
*
* @Author	???
*
* @Date		2005/8/30
*/
class TGAMECTRL_API CTIme
{
	CTIme(void);
	~CTIme(void);
public:
	static	CTIme& GetInstance();
	int		Initialize( SIZE sizeScreen );
	int		Finalize();
	int		Process( HWND hWnd, unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	void	Draw();///CandidateWindow&CompositionWindow

	std::string&	GetCompsitionString();
	void			SetCandidateWindowPosition( POINT pt );
	void			ChangeScreenSize( SIZE sizeScreen );
	short			GetCursorPos();
	short			GetDeltaStart();
	std::vector<BYTE>& GetCompReadAttr();
	std::vector<BYTE>& GetCompAttr();
	std::vector<DWORD>& GetCompClauseInfo();
	std::vector<DWORD>& GetCompReadClauseInfo();

	std::string&	GetStr_ATTR_TARGET_CONVERTED();
	std::string&	GetStr_ATTR_CONVERTED();
	std::string&	GetStr_ATTR_INPUT();
	std::string&	GetStrPrompt();


	bool			IsVisibleCandidateWindow();
	static			HIMC	GetOldHIMC();
	static			void	SetOldHIMC( HIMC hImc );

private:
	std::string					m_strPrompt;
	std::string					m_strComposition;
	std::string					m_strCompositionTemp;

	std::string					m_strMark_ATTR_TARGET_CONVERTED;
	std::string					m_strATTR_CONVERTED;
	std::string					m_strATTR_INPUT;

	
	std::vector<DWORD>			m_CompReadClauseInfo;
	std::vector<DWORD>			m_CompClauseInfo;
	std::vector<BYTE>			m_CompReadAttr;
	std::vector<BYTE>			m_CompAttr;


	POINT						m_ptCandidateWindow;
	SIZE						m_sizeScreen;
	CTCandidateWindow*			m_pCandidateWindow;
	short						m_nCursorPos;
	short						m_nDeltaStart;

	///?? ? ???? ???? ?? ??? ??? ???? IME? Disable??? ??? HIMC ? ????? ?? ??
	static HIMC					s_old_HIMC;
};
#endif