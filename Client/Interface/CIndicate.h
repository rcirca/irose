#ifndef _CIndicate_
#define _CIndicate_
#include "tgamectrl/winctrl.h"

//*----------------------------------------------------------------------------------/
/// 존 진입시 혹은 퀘스트 시작, 종료시 화면에 표시하는 Class
//*----------------------------------------------------------------------------------/
class CIndicate : public CWinCtrl
{
public:
	CIndicate(void);
	virtual ~CIndicate(void);

	virtual void Draw();
	virtual void Update( POINT ptMouse );
	virtual void	Show();
	virtual void	Hide();
	void	SetTitle( const char* pszTitle , const char* pszSubTitle );
	void	AddDescription( const char* pszDescription );
	void	ResetDescription();
	
protected:
	std::string					m_strTitle;
	std::string					m_strSubTitle;
	std::list< string >			m_Descriptions;


	DWORD				m_dwShowTime;		///보이기 시작한 시간
	DWORD				m_dwDisplayTime;	///보여줄 시간

	int					m_iLineSpace;		///설명에서 라인간격
	int					m_iLineHeight;		///설명에서 한라인의 높이
	int					m_iTitleHeight;
};
#endif