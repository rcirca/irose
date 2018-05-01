#ifndef _SELECTSERVER_
#define _SELECTSERVER_

#include "CExternalUI.h"
#include "../../GameCommon/IObserver.h"
#include <string>
#include <vector>
#include <deque>


#include "TDialog.h"

struct ServerInfo
{
	std::string			strServerName;
	unsigned int		iServerID;
};

struct ChannelInfo
{
	int	m_btChannelNO;		// 보여줄 채널 슬롯 번호
	int	m_btLowAGE;			// 접속 가능한 최저 연령 0 이면 최저 연령 제한 없음
	int	m_btHighAGE;		// 접속 가능한 최고 연령 0 이면 최고 연령 제한 없음
};

struct t_PACKET;
struct tagCHANNEL_SRV;


/**
* 월드서버&채널 서버 선택시 사용되는 인터페이스
* - Observable : GameData/CServerList
*
* @Warning		현재(2005/9/6) 한국의 경우 채널서버가 1개이고 채널 리스트를 보여주지 않는다.그 외 국가는 모두 채널서버가 2개 이상으로 선택을 할 수 있다.
* @Author		최종진
* @Date			2005/9/6
*/
class CSelectServer : public CTDialog, public IObserver
{
private:	
	///
	///	Observer class
	///
	CExternalUIManager*				m_pEUIManager;

	std::vector< ServerInfo >		m_ServerInfoList;
	std::deque< ChannelInfo >		m_Channels;

	bool							m_bWaitServerReply;
public:
	CSelectServer();
	~CSelectServer();

	///
	/// set observer class
	///
	void SetEUIObserver( CExternalUIManager* pObserver );
	///
	/// Message procedure
	///
	virtual UINT Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	///
	/// Drawing method
	///
	virtual void Update( CObservable* pObservable, CTObject* pObj );

	virtual void    Hide();
	virtual void	Show();
	virtual void    MsgBoxProc(  unsigned int iMsgType, short int iButton );
	///
	///	All server list count
	///
	///
	///	Add server to server list
	///
	ServerInfo* GetServerInfo( int iIndex );

	///
	///	Send select server request
	///

	///
	///	Recv proc..
	///
	void	RecvSelectServer( t_PACKET* recvPacket );
	void	SendReqFirestServerChannelList();

	enum{
		IID_LISTBOX_SERVER	= 2,
		IID_LISTBOX_CHANNEL = 3,
		IID_BTN_OK		= 10,
		IID_BTN_CANCEL	= 11
	};
	enum{
		MSG_TYPE_EXIT,
		MSG_TYPE_NORMAL
	};
protected:

	void	OnLButtonUp( unsigned iProcID );
	void	OnLButtonDblClk( unsigned iProcID );
	void	OnLButtonDown( unsigned iProcID );
	
	unsigned int	FindSvrId( short ItemID );
	int				FindChannelNo(  int iIndex );

	void	SendSelectChannel();
	void	SendSelectServerReq();
};

#endif //_SELECTSERVER_