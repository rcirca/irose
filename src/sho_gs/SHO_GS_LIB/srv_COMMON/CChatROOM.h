#if	defined( __SHO_WS ) || defined( __INC_WORLD )
#ifndef	__CCHAT_ROOM_H
#define	__CCHAT_ROOM_H
#include "CDLList.h"
#include "classSTR.h"
#include "classSyncOBJ.h"
#include "classINDEX.h"

#if !defined( __SHO_WS ) && !defined( __SHO_GS )
	#error >>> ERROR :: must defined __SHO_WS or __SHO_GS
#endif
//-------------------------------------------------------------------------------------------------

#ifdef	__SHO_WS
	#include "CWS_Client.h"
	typedef	CWS_Client	classUSER;
#endif

#ifdef	__SHO_GS
	#include "GS_USER.h"
#endif

/**
 * \ingroup SHO_GS_LIB
 * \class	CChatROOM
 * \author	wookSang.Jo
 * \brief	대화방 관리 클래스
 */
class CChatROOM {
private:
	CCriticalSection		m_ListCS;
	CDLList< classUSER* >	m_ListUSER;

public :
	BYTE	m_btMaxUSER;
//	char	m_cUserCNT;
	CStrVAR	m_szTITLE;
	CStrVAR	m_szPASSWD;

	BYTE	m_btRoomTYPE;

	CChatROOM () : m_ListCS( 4000 )
	{
//		m_cUserCNT = 0;
	}

	int  GetRoomUSERS ()	{	return m_ListUSER.GetNodeCount();	}

	bool ReqMake( classUSER *pUSER, cli_CHAT_ROOM_MAKE *pMake, char *szTitle, char *szPasswd);
	bool ReqJoin( classUSER *pUSER );
	int  ReqLeft( classUSER *pUSER );
	bool ReqKick( classUSER *pUSER, t_HASHKEY HashUSER );

	bool Send_wsv_CHATROOM (BYTE btCMD, WORD wUserID, char *szSTR);
	bool Send_wsv_CHATROOM_MSG( WORD wObjectID, char *szMSG );
} ;


//-------------------------------------------------------------------------------------------------
/**
 * \ingroup SHO_GS_LIB
 * \class	CChatRoomLIST
 * \author	wookSang.Jo
 * \brief	전체 대화방 관리 클래스
 */
class CChatRoomLIST : public CIndexARRAY< CChatROOM* > {
private:
	short		m_nRoomCNT;

	CCriticalSection	 m_RoomsCS;
	CChatROOM			*m_pROOMs;

public :
	CChatRoomLIST( WORD wMaxRoomCNT );
	~CChatRoomLIST();

	bool LeftUSER( classUSER *pUSER );

	bool Recv_cli_CHATROOM_MSG( classUSER *pUSER, t_PACKET *pPacket );
	bool Recv_cli_CHATROOM( classUSER *pUSER, t_PACKET *pPacket );
} ;
extern CChatRoomLIST *g_pChatROOMs;

//-------------------------------------------------------------------------------------------------
#endif
#endif