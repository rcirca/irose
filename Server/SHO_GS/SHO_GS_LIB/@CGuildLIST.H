#ifndef	__GS_GUILD_H
#define	__GS_GUILD_H
#include "CDLList.h"
#include "CStrMAP.h"
#include "classSYNCOBJ.h"
#include "classSTR.h"

#if !defined( __SHO_WS ) && !defined( __SHO_GS )
	#error >>> ERROR :: must defined __SHO_WS or __SHO_GS
#endif

#ifdef	__SHO_WS
	#include "CWS_Client.h"
	typedef	CWS_Client	classUSER;
#endif

#ifdef	__SHO_GS
	#include "GS_ListUSER.h"
#endif
//-------------------------------------------------------------------------------------------------

class CGuild : public CCriticalSection
{
private:
	int			m_iGuildID;
	int			m_iMemberCNT;

	CStrVAR		m_Name;
	CStrVAR		m_Desc;

	CDLList< classUSER* >	m_ListUSER;

public :
	CGuild ();
	~CGuild ();

	bool Add_GuildUSER( classUSER *pUSER );
	bool Sub_GuildUSER( classUSER *pUSER );

	void SendPacketToGUILD (classPACKET *pCPacket);
} ;


class CGuildLIST : public CCriticalSection
{
private :
//	CDLList< CGuild* >	m_ListGUILD;
	CStrMAP< CGuild* >	m_GUILDs;

public  :
	CGuildLIST ();
	~CGuildLIST ();

	CGuild *Find_GUILD( t_HASHKEY HashGUILD );

	bool Create( classUSER *pUSER, char *szGuildName);

	bool Recv_cli_GUILD_COMMAND ( classUSER *pUSER, t_PACKET *pPacket );
} ;
extern CGuildLIST *g_pGuildLIST;

//-------------------------------------------------------------------------------------------------
#endif
