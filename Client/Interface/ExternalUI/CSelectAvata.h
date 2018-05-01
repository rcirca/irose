#ifndef _SELECTAVATA_
#define _SELECTAVATA_

#include "CExternalUI.h"
#include <vector>
#include <string>
#include "TDialog.h"

struct t_PACKET;

/**
* 캐릭터 선택창에서 사용되는 인터페이스
* - CGameDataCreateAvatar와 같은 부분을 처리하고 있다.
*
* @Warning		과금처리에 따라 4,5번째 슬롯의 캐릭터의 선택이 가능/불가능해진다.
* @Todo			CGameDataCreateAvatar와 같이 정리해서 인터페이스와 데이타를 확연히 구분하던가 한군데( CSelectAvata )로 합치는건?
* @Author		최종진
* @Date			2005/9/6
*/
class CSelectAvata :  public CTDialog
{
public:
	CExternalUIManager*			m_pEUIManager;


public:
	CSelectAvata();
	~CSelectAvata();

	void	SetEUIObserver( CExternalUIManager* pObserver );

	virtual UINT	Process( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void	Show();

	
	void	SendSelectAvataReq();
	void	RecvAvataList( t_PACKET* recvPacket );

	enum{
		IID_BTN_CREATE	= 10,
		IID_BTN_DELETE	= 11,
		IID_BTN_OK		= 12,
		IID_BTN_CANCEL	= 13
	};

	enum{
		MSG_TYPE_NORMAL,
		MSG_TYPE_DELETECHAR
	};

protected:
	void	OnLButtonUp( unsigned iProcID );
	bool	m_bWaitServerReply;

};

#endif //_SELECTAVATA_