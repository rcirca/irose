#ifndef _CMemoViewDlg_
#define _CMemoViewDlg_
#include "TDialog.h"

/**
* 받은 쪽지 보기용 다이얼로그
*
* @Author		최종진
* @Date			2005/9/12
*/
class CMemoViewDlg : public CTDialog
{
public:
	CMemoViewDlg(void);
	virtual ~CMemoViewDlg(void);
	virtual bool Create( const char* IDD );
	virtual void Draw();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );


	void	SetTargetName( const char* pszName );
	void	SetContent( const char* pszContent );
	void	SetID( int iID );

private:
	enum{
		IID_BTN_CLOSE	= 10,
		IID_BTN_OK		= 11,
		IID_BTN_DELETE  = 12,
	};

	int			m_iID;							/// 여러개의 쪽지중 구분 인덱스
	std::string	m_strName;						/// 보낸 사람 이름
	std::string m_strContent;					/// 쪽지 내용

};
#endif