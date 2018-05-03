/**
 * \ingroup SHO_GS
 * \file	GS_Party.h
 * \brief	게임내의 파티관리및 처리
 */
#ifndef	__GS_PARTY_H
#define	__GS_PARTY_H
#include "CDLList.h"
#include "classSYNCOBJ.h"
#include "CDataPOOL.h"

 #define	__USE_ARRAY_PARTY_USER

//#define	MAX_PARTY_MEMBERS		7
//#define	MAX_PARTY_LEVEL			99


class classUSER;

//-------------------------------------------------------------------------------------------------
struct tagPartyUSER {
	DWORD		m_dwDisconnectTIME;
	classUSER  *m_pUSER;
	DWORD		m_dwDBID;
	short		m_nLevel;
	CStrVAR		m_NameCHAR;

	tagPartyUSER ()
	{
		m_dwDisconnectTIME = 0;
	}
} ;

//-------------------------------------------------------------------------------------------------

/**
 * \ingroup SHO_GS_LIB
 * \class	CParty
 * \author	wookSang.Jo
 * \brief	파티및 파티원 관리 클래스
 */
class CParty : public CCriticalSection
{
private:
	short	m_nMemberCNT;
	int		m_iPartyEXP;		// 파티 경험치
	BYTE	m_btPartyLEV;		// 현재 파티레벨

//	void	Check_UserLEV ();
	void	LevelUP ();
	bool	Send_PartyUserLIST( classUSER *pUser, DWORD dwUserTAG  );
	bool	Send_gsv_PARTY_ITEM (classUSER *pPicker, CObjITEM *pItemOBJ);

public :
	WORD	m_wPartyWSID;
	BYTE	m_btPartyRULE;		// 파티 아이템/경험치 분배 방식
	DWORD	m_dwLastKillTIME;
	int		m_iAverageLEV;		// 평균 파티원 레벨
	int		m_iTotalLEV;		// 모든 파티원 레벨 합

	//short	m_nMinUserLEV;
	//short	m_nMaxUserLEV;

#ifdef	__USE_ARRAY_PARTY_USER
	BYTE	m_btItemORDERs[ MAX_INV_TYPE+1 ];	// MAX_INV_TYPE == 돈 !!!
	tagPartyUSER *m_pPartyUSERs[ MAX_PARTY_MEMBERS ];
#else
	CDLList< tagPartyUSER >		m_PartyUSER;	// 파티원 리스트
#endif

	CParty () : CCriticalSection( 4000 )
	{	
		;
	}
	void InitPARTY ()
	{
		m_btPartyRULE = 0;
		m_nMemberCNT = 1;
		m_iPartyEXP = 0;
		m_btPartyLEV = 1;

		m_iTotalLEV = 0;

#ifdef	__USE_ARRAY_PARTY_USER
		memset( m_btItemORDERs, MAX_PARTY_MEMBERS, MAX_INV_TYPE );		// 첫순서로 0이 되도록...
		::ZeroMemory( m_pPartyUSERs, sizeof(tagPartyUSER*) * MAX_PARTY_MEMBERS );
#endif
	}

	classUSER *GetPartyOWNER ()			// 파티짱을 얻음
	{
#ifdef	__USE_ARRAY_PARTY_USER
		classUSER *pOwner;
		this->Lock ();
		pOwner = m_pPartyUSERs[ 0 ] ? m_pPartyUSERs[ 0 ]->m_pUSER : NULL;
		this->Unlock ();
		return pOwner;
#else
		CDLList< tagPartyUSER >::tagNODE *pHeadNODE;
		this->Lock ();
			pHeadNODE = m_PartyUSER.GetHeadNode();
		this->Unlock ();
		return pHeadNODE ? pHeadNODE->m_VALUE.m_pUSER : NULL;
#endif
	}

	bool AddEXP( CObjMOB *pMobCHAR, int iTotalEXP, int iPartyExpA, int iPartyExpB );

	bool Change_PartyOWNER (classUSER *pNewOWNER, BYTE btReply=PARTY_REPLY_CHANGE_OWNER);
	bool Change_ObjectIDX (classUSER *pUSER);

	BYTE Add_PartyUSER (classUSER *pUSER);

#ifdef	__USE_ARRAY_PARTY_USER
	bool Sub_PartyUSER( short nPartyPOS );
#else
x	//bool Sub_PartyUSER (CDLList< tagPartyUSER >::tagNODE *pPartyNODE );
#endif

	bool Kick_MEMBER( DWORD dwDBID );
	void Member_LevelUP (short nPartyPOS, short nLevelDIFF);

#ifdef	__USE_ARRAY_PARTY_USER
	bool Give_Item2ORDER(classUSER *pTryOBJ, CObjITEM *pItemOBJ);
	bool Is_ShareMODE()		{	return 0 == (m_btPartyRULE & BIT_PARTY_RULE_ITEM_TO_ORDER);	}
#endif
	void Share_MONEY( int iMoney );

	int  GetMemberCNT()		{	return	this->m_nMemberCNT;		}
	BYTE GetPartyLEV()		{	return	this->m_btPartyLEV;		}
	int	 GetAverageLEV()	{	return	this->m_iAverageLEV;	}
	int  GetPartyEXP()		{	return	this->m_iPartyEXP;		}

	bool SendWhisperToPartyMembers( char *szMessage );
	bool Send_gsv_PARTY_REPLY2Members (	BYTE btReply, DWORD dwFromIDXorTAG );
	bool SendToPartyMembers( classPACKET *pCPacket );
//	bool SendToPartyMembersExecptNEAR( classUSER *pSender, classPACKET *pCPacket );
	bool SendToPartyMembersExceptME( classUSER *pSender, classPACKET *pCPacket );
	bool SendPartyLEVnEXP (classUSER *pUSER, BYTE bitLevelUP=0);
	bool Send_gsv_CHECK_NPC_EVENT(  classUSER *pUSER, short nNpcIDX );

	void Set_PartyRULE( BYTE btRULE );
	void Warp_USERS(int iZoneNO, tPOINTF &PosGOTO);

	void OnConnect (classUSER *pUSER);
	void OnDisconnect (classUSER *pUSER);
} ;

//-------------------------------------------------------------------------------------------------
/**
 * \ingroup SHO_GS_LIB
 * \class	CPartyBUFF
 * \author	wookSang.Jo
 * \brief	전체 파티 관리 클래스
 */
// class CPartyBUFF : public CDataPOOL< CParty >
class CPartyBUFF : public CIndexARRAY< CParty* >
{
private :
	CCriticalSection	 m_csPartyBUFF;
	CParty				*m_pPartyBUFF;

	CParty *Pool_Alloc ()
	{
		CParty *pCParty;
		
		this->m_csPartyBUFF.Lock ();
		{
			int iSlot = this->GetEmptySlot ();
			if ( iSlot > 0 ) {
				pCParty = &m_pPartyBUFF[ iSlot ];
				this->m_pINDEX[ iSlot ] = pCParty;
				this->m_iUsedSlot ++;
			} else pCParty = NULL;
		}
		this->m_csPartyBUFF.Unlock ();

		return pCParty;
	}
	void Pool_Free ( CParty *pCParty )
	{	// Lock 걸려서 호출된다..
		// this->m_csPartyBUFF.Lock ();
		{
			this->m_pINDEX[ pCParty->m_wPartyWSID ] = NULL;
			this->m_iUsedSlot --; 
		}
		// this->m_csPartyBUFF.Unlock ();
	}

public  :
//	CPartyBUFF(UINT uiInitDataCNT, UINT uiIncDataCNT);
	CPartyBUFF( WORD wMaxPartyCNT=MAX_PARTY_BUFF );
	~CPartyBUFF ();

	bool	CreatePARTY ( classUSER *pOwner );
	void	DeletePARTY ( CParty *pCParty );

	void	OnConnect (unsigned int uiPartyIDX, classUSER *pUSER)
	{
		CParty *pCParty = this->GetData( uiPartyIDX );
		if ( pCParty ) {
			pCParty->OnConnect( pUSER );
		}
	}
} ;

extern CPartyBUFF *g_pPartyBUFF;


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#endif
