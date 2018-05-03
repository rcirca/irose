/**
 * \ingroup SHO_GS
 * \file	CThreadGUILD.h
 * \brief	월드의 클랜 관리및 처리, 클랜 디비와 연동
 */
#ifndef	__GS_GUILD_H
#define	__GS_GUILD_H
#include "CDLList.h"
#include "classHASH.h"
#include "classSTR.h"

#if !defined( __SHO_WS ) && !defined( __SHO_GS )
	#error >>> ERROR :: must defined __SHO_WS or __SHO_GS
#endif

#ifdef __SHO_WS
	class CWS_Client;
#else
	class classUSER;
	typedef	classUSER	CWS_Client;
#endif


#include "CSqlTHREAD.h"

//-------------------------------------------------------------------------------------------------
/**
 * \ingroup SHO_GS_LIB
 * \class	CClanUSER
 * \author	wookSang.Jo
 * \brief	클랜에 가입한 사용자 정보
 */
class CClanUSER 
{
public :
	CStrVAR		m_Name;				// 케릭이름
	t_HASHKEY	m_HashName;
	int			m_iContribute;		// 기여도
	int			m_iPosition;		// 직위
	int			m_iConnSockIDX;		// 접속됐을경우 소켓 IDX
	BYTE		m_btChannelNo;		// 접속됐을경우 채널 번호..
	short		m_nLevel;
	short		m_nJob;
	CClanUSER () 
	{
		m_iContribute = 0;
		m_iConnSockIDX = 0;
		m_nLevel = 0;
		m_nJob = 0;
	}
} ;


//#ifdef	MAX_CLAN_SKILL_SLOT
//	#pragma pack (push, 1)
//	struct tagClanSKILL {
//		short	m_nSkillIDX;
//		DWORD	m_dwExpiredTIME;	// 사용 만료 시간
//	//	short	m_nCount;			// 사용 가능 횟수 -> 누가 사용가능한 횟수?? 마스터만??
//	} ;
//	struct tagClanBIN {
//		tagClanSKILL	m_SKILL[ MAX_CLAN_SKILL_SLOT ];
//
//		void Init ()
//		{
//			::ZeroMemory( m_SKILL, sizeof(m_SKILL) );
//		}
//		void Set( BYTE *pBin )
//		{
//			if ( pBin )
//				::CopyMemory( m_SKILL, pBin, sizeof(m_SKILL) );
//			else
//				this->Init();
//		}
//	} ;
//	#pragma pack (pop)
//#endif

/**
 * \ingroup SHO_GS_LIB
 * \class	CClan
 * \author	wookSang.Jo
 * \brief	클랜 데이타및 클랜멤버 관치및 처리
 */
class CClan : public CCriticalSection
{
public :
	static const int s_iUserLimit[MAX_CLAN_LEVEL+1];
//	static const int s_iPosLimit [MAX_CLAN_LEVEL+1];

	DWORD		m_dwClanID;

	short		m_nClanLEVEL;
	int			m_iClanSCORE;
	union {
		DWORD	m_dwClanMARK;
		WORD	m_wClanMARK[2];
	} ;
	DWORD		m_dwAlliedGroupID;				// 클랜의 동맹그룹
	int			m_iClanRATE;
	__int64		m_biClanMONEY;

	CStrVAR		m_Name;							// 길드 이름
	CStrVAR		m_Desc;							// 길드 설명
	CStrVAR		m_Motd;							// 길드 공지

	short		m_nClanMarkLEN;
	BYTE		m_pClanMARK[ 1024 ];

	wsv_CLANMARK_REG_TIME	m_RegTIME;

#ifdef	MAX_CLAN_SKILL_SLOT
	tagClanBIN	m_ClanBIN;
#endif

	short		m_nPosCNT[ GPOS_MASTER+1 ];		// 등급별 멤버수

	CDLList< CClanUSER >	m_ListUSER;	

public :
	CClan ();
	~CClan ();

	void Init (char *szName, char *szDesc, DWORD dwClanID, WORD wMark1, WORD wMark2, BYTE *pClanBIN, WORD wMarkCRC, short nMarkDataLen, BYTE *pClanMARK )
	{
		m_nClanLEVEL = 1;
		m_iClanSCORE = 0;
		m_dwAlliedGroupID = 0;
		m_iClanRATE = 100;
		m_biClanMONEY = 0;

		m_Name.Set( szName );
		m_Desc.Set( szDesc );
		m_dwClanID = dwClanID;

		::ZeroMemory( m_nPosCNT, sizeof(m_nPosCNT) );
#ifdef	MAX_CLAN_SKILL_SLOT
		if ( pClanBIN )
			::CopyMemory( m_ClanBIN.m_pDATA, pClanBIN, sizeof( tagClanBIN ) );
		else
			::ZeroMemory( m_ClanBIN.m_pDATA, sizeof( tagClanBIN ) );
#endif
		m_nClanMarkLEN = nMarkDataLen;
		if ( pClanMARK ) {
			m_wClanMARK[0] = 0;
			m_wClanMARK[1] = wMarkCRC;
			::CopyMemory( m_pClanMARK, pClanMARK, nMarkDataLen );
		} else {
			m_wClanMARK[0] = wMark1;
			m_wClanMARK[1] = wMark2;
			::ZeroMemory( m_pClanMARK, sizeof(m_pClanMARK) );
		}
	}
	void Free ()
	{
		CDLList< CClanUSER >::tagNODE *pNode;
		pNode = m_ListUSER.GetHeadNode();
		while( pNode ) {
			m_ListUSER.DeleteNFree( pNode );
			pNode = m_ListUSER.GetHeadNode();
		}
	}

	int  GetUserLimitCNT ()
	{
		return s_iUserLimit[ m_nClanLEVEL ];
	}
	int  GetPositionLimitCNT (int iPos)
	{
		int iLimitCnt=GetUserLimitCNT ();
		switch( iPos ) {
			case GPOS_RENDER :		iLimitCnt = (int)(m_nClanLEVEL*1.8f) + 2;	break;
			case GPOS_COMMANDER :	iLimitCnt = m_nClanLEVEL + 2;				break;
			case GPOS_SUB_MASTER :	iLimitCnt = (int)(m_nClanLEVEL*0.3f) + 1;	break;
			case GPOS_MASTER :		iLimitCnt = 1;	break;
		}
		return iLimitCnt;
	}

	void Disband ();

	bool LogIn_ClanUSER( char *szCharName, int iSenderSockIDX, int iContribute );
	bool LogOut_ClanUSER( char *szCharName );

	bool Send_SetCLAN (BYTE btCMD, CWS_Client *pMember );

	bool Send_MemberSTATUS( CClanUSER *pClanUser, BYTE btResult );

	bool Insert_MEMBER( BYTE btResult, CWS_Client *pMember, int iClanPos=GPOS_JUNIOR, char *szMaster=NULL );
	bool Delete_MEMBER( t_HASHKEY HashCommander, char *szCharName, BYTE btCMD, char *szKicker=NULL );
	bool Update_MEMBER();
	bool Adjust_MEMBER( t_HASHKEY HashCommander, char *szCharName, int iAdjPoint, int iAdjPos, char *szMaster, int iMaxPos, int iMinPos=0, bool bCheckPosLimit=false );

	DWORD GetCurAbsSEC ();
	BYTE FindClanSKILL (short nSkillNo1, short nSkillNo2);
	bool AddClanSKILL (short nSkillNo);
	bool DelClanSKILL (short nSkillNo);

	bool SetClanMARK( BYTE *pMarkData, short nDataLen, WORD wMarkCRC16, CWS_Client *pReqUSER );
	void SetJOBnLEV( t_HASHKEY HashCHAR, char *szCharName, short nJob, short nLev );
	void SendRoster( int iSenderSockIDX );
	void Send_UpdateInfo ();

	void SendPacketToMEMBER (classPACKET *pCPacket);
	bool Send_wsv_CLAN_COMMAND (BYTE btCMD, ... );
	bool Send_wsv_CLANMARK_REG_TIME( CWS_Client *pMember );
} ;


struct tagCLAN_CMD {
	BYTE		 m_btCMD;
	int			 m_iSocketIDX;
//	DWORD		 m_dwDBID;
	t_PACKET	*m_pPacket;
	CStrVAR		 m_CharNAME;
} ;

/**
 * \ingroup SHO_GS_LIB
 * \class	CThreadGUILD
 * \author	wookSang.Jo
 * \brief	클랜 데이타를 디비와 연동하는 클래스
 */
class CThreadGUILD : public CSqlTHREAD
{
private :
	CDataPOOL< CClan >  m_Pools;
	classHASH< CClan*>	*m_pHashCLAN;

	CDLList< tagCLAN_CMD >	m_ProcCMD;
	CDLList< tagCLAN_CMD >	m_WaitCMD;

	// inherit virtual function from CSqlTHREAD...
	bool Run_SqlPACKET( tagQueryDATA *pSqlPACKET )	{	return true;	}

	void Test_add (char *szName, char *szDesc);
	void Test_del (char *pGuildName);

	void Execute ();

	bool Run_GuildPACKET ( tagCLAN_CMD *pGuildCMD );
	CClan *Query_CreateCLAN( int iSocketIDX, t_PACKET *pPacket );
	bool Query_DeleteCLAN( char *szClanName );

public  :
	CThreadGUILD ( UINT uiInitDataCNT, UINT uiIncDataCNT );
	~CThreadGUILD ();

	void Set_EVENT ()		{	m_pEVENT->SetEvent ();	}
	bool Add_ClanCMD (BYTE btCMD, int iSocketIDX, t_PACKET *pPacket, char *szCharName=NULL);

	bool Query_LoginClanMember ( char *szCharName, int iSenderSockIDX );
	bool Query_InsertClanMember( char *szCharName, DWORD dwClanID, int iClanPos );
	bool Query_DeleteClanMember( char *szClanName );
	bool Query_AdjustClanMember( char *szCharName, int iAdjPoint, int iAdjPos );
	bool Query_UpdateClanMOTD  ( DWORD dwClanID, char *szMessage );
	bool Query_UpdateClanSLOGAN( DWORD dwClanID, char *szMessage );
	bool Query_UpdateClanDATA  ( char *szField, t_PACKET *pPacket );//BYTE btAdjType, int iAdjValue);
	bool Query_UpdateClanBINARY( DWORD dwClanID, BYTE *pDATA, unsigned int uiSize );
	WORD Query_UpdateClanMARK  ( CClan *pClan, WORD wMarkCRC, BYTE *pDATA, unsigned int uiSize );

	void Login_GuildUSER( char *szCharName, DWORD dwGuildID, int iSocketIDX );

	CClan *Find_CLAN( DWORD dwClanID );
	CClan *Load_CLAN( DWORD dwClanID );

	bool Create( CWS_Client *pUSER, char *szGuildName);
} ;
extern CThreadGUILD *g_pThreadGUILD;

//-------------------------------------------------------------------------------------------------
#endif
