
#include "stdAFX.h"
#include "CGuildLIST.h"

//-------------------------------------------------------------------------------------------------
CGuild::CGuild () : CCriticalSection( 4000 )
{
}
CGuild::~CGuild ()
{
}

//-------------------------------------------------------------------------------------------------
bool CGuild::Add_GuildUSER( classUSER *pUSER )
{
	this->Lock ();
	{
		// m_ListUSER.AppendNode( xxx );
	}
	this->Unlock ();

	return false;
}

bool CGuild::Sub_GuildUSER( classUSER *pUSER )
{
	this->Lock ();
	{
		// m_ListUSER.DeleteNode( pUSER->
	}
	this->Unlock ();

	return false;
}

void CGuild::SendPacketToGUILD (classPACKET *pCPacket)
{
	CDLList< classUSER* >::tagNODE *pNODE;

	this->Lock ();
	{
		pNODE = m_ListUSER.GetHeadNode();
		while( pNODE ) {
			pNODE->m_VALUE->Send_Start( pCPacket );
			pNODE = pNODE->GetNext ();
		}
	}
	this->Unlock ();
}


//-------------------------------------------------------------------------------------------------
CGuildLIST::CGuildLIST () : CCriticalSection( 4000 )
{
}
CGuildLIST::~CGuildLIST ()
{
}

//-------------------------------------------------------------------------------------------------
CGuild *CGuildLIST::Find_GUILD( t_HASHKEY HashKEY )
{
//	return m_GUILDs.Search( HashKEY );
	return NULL;
}

//-------------------------------------------------------------------------------------------------
bool CGuildLIST::Create( classUSER *pUSER, char *szGuildName)
{
	t_HASHKEY HashNAME = m_GUILDs.MakeKey( szGuildName );

	CGuild *pGuild = this->Find_GUILD( HashNAME );
	if ( pGuild ) {
		// 중복된 길드 이름이 있다.
		return false;
	}

	return true;
}

bool CGuildLIST::Recv_cli_GUILD_COMMAND ( classUSER *pUSER, t_PACKET *pPacket )
{
	short nOffset = sizeof( cli_GUILD_COMMAND );

	switch( pPacket->m_cli_GUILD_COMMAND.m_btCMD ) {
		case GCMD_CREATE	:		//길드생성
		{
			char *pGuildName = pPacket->m_cli_GUILD_COMMAND.m_szStr;	//  Packet_GetStringPtr( pPacket, nOffset );
			if ( !pGuildName || !pGuildName[0] )
				return false;

			return this->Create( pUSER, pGuildName );
		}
		case GCMD_INFO		:		//길드정보
		{
			return true;
		}
		case GCMD_INVITE	:		//길드초대
		{
			return true;
		}
		case GCMD_REMOVE	:		//길드추방
		{
			char *pMember = pPacket->m_cli_GUILD_COMMAND.m_szStr;	//  Packet_GetStringPtr( pPacket, nOffset );
			if ( !pMember || !pMember[0] )
				return false;

			return true;
		}
		case GCMD_PROMOTE	:		//길드승급
		{
			char *pMember = pPacket->m_cli_GUILD_COMMAND.m_szStr;	//  Packet_GetStringPtr( pPacket, nOffset );
			if ( !pMember || !pMember[0] )
				return false;

			return true;
		}
		case GCMD_DEMOTE	:		//길드강등
		{
			char *pMember = pPacket->m_cli_GUILD_COMMAND.m_szStr;	//  Packet_GetStringPtr( pPacket, nOffset );
			if ( !pMember || !pMember[0] )
				return false;

			return true;
		}
		case GCMD_MOTD		:		//길드공지
		{
			char *pMessage = pPacket->m_cli_GUILD_COMMAND.m_szStr;	//  Packet_GetStringPtr( pPacket, nOffset );
			if ( !pMessage || !pMessage[0] )
				return false;

			return true;
		}
		case GCMD_QUIT		:		//길드탈퇴
		{
			return true;
		}
		case GCMD_ROSTER	:		//길드목록
		{
			return true;
		}
		case GCMD_LEADER	:		//길드위임
		{
			char *pMember = pPacket->m_cli_GUILD_COMMAND.m_szStr;	//  Packet_GetStringPtr( pPacket, nOffset );
			if ( !pMember || !pMember[0] )
				return false;

			return true;
		}
		case GCMD_DISBAND	:		//길드해체
		{
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
