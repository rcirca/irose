#ifndef _CSYSTEMMSGDLG_
#define _CSYSTEMMSGDLG_

#include "tgamectrl/tdialog.h"
#include "tgamectrl/TSplitString.h"

/**
* 게임화면에서 상단 중간에 나오는 팁및 공지사항을 보여주기 위한 다이얼로그
* 
* @Author		최종진
* @Date			2005/9/14
*/ 
class CSystemMsgDlg : public CTDialog
{
public:
	CSystemMsgDlg(void);
	virtual ~CSystemMsgDlg(void);

	virtual void Draw();
	virtual void Update( POINT ptMouse );
	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );


	bool SetMessage( const char* szTitle,const char* szMsg, short iType, D3DCOLOR color = NULL, DWORD dwShowTime = 10000 ,HNODE hFont = NULL);

	enum{
		MSG_TYPE_NOTICE,	/// 공지사항
		MSG_TYPE_HELP		/// 팁
	};

protected: 
	HNODE			m_hFont;			/// 미리 만들어져 있는 폰트에 대한 노드만 가지고 있는다.
	CTSplitString	m_Notice;			/// 출력될 내용( 제목 + 내용 )
	D3DCOLOR		m_Color;			/// 출력될 색상
	short			m_iMsgType;			/// 공지 or 팁
	DWORD			m_dwShowTime;		/// 화면에 보여질 시간
	DWORD			m_dwMsgSetTime;		/// 화면에 보여지기 시작한 시간

	std::string		m_stTitle;			/// 제목 스트링
	std::string		m_stMsg;			/// 내용 스트링
	int				m_iImageIndex;		/// 배경에 그려질 이미지 그래픽 ID
};
#endif