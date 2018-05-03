
#include "stdAFX.h"
#if defined(__SHO_WS) || defined(__INC_WORLD)


#ifdef	__INC_WORLD
#include "LIB_gsMAIN.h"
#include "GS_ListUSER.h"
#endif

#ifdef	__SHO_WS
#include "CWS_Client.h"
#endif

#include "CThreadMSGR.h"
#include "classLOG.h"
#include "IO_STB.h"

//-------------------------------------------------------------------------------------------------
CThreadMSGR::CThreadMSGR( UINT uiInitDataCNT, UINT uiIncDataCNT ) 
							: CSqlTHREAD( true ), /* m_csListCMD( 4000 ) */
							  m_Pools("CMessengerPOOL", uiInitDataCNT, uiIncDataCNT ),
							  m_HashMSGR( 1024 * 3 )
{
	m_pListBUFF = new BYTE[ 2048 ];
}
CThreadMSGR::~CThreadMSGR ()
{
	SAFE_DELETE_ARRAY( m_pListBUFF );
}

void CThreadMSGR::Check_FRIENDS ()
{
/*
	int iCharID=0, iFriendCNT, iTotCNT=0;
	BYTE *pLIST;
	while( true ) 
	{
		if ( !this->m_pSQL->QuerySQL( "SELECT TOP 1 * FROM tblWS_FRIEND WHERE intCharID > %d ORDER BY intCharID ASC", iCharID ) ) {
			g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
			return;
		}

		if ( !this->m_pSQL->GetNextRECORD() ) 
			break;

		{
			iCharID = this->m_pSQL->GetInteger( 0 );
			iFriendCNT = this->m_pSQL->GetInteger( 1 );
			pLIST = this->m_pSQL->GetDataPTR( 2 );

			char *pName;
			tagFriend_H *pFrH;
			tagMyFriend  FR;
			for (int iC=0; iC<iFriendCNT; iC++) {
				pFrH = (tagFriend_H*)pLIST;
				pName = (char*)( pLIST+sizeof(tagFriend_H) );

				_ASSERT( iCharID != pFrH->m_dwDBID );

				FR.m_dwDBID = pFrH->m_dwDBID;
				FR.m_btSTATUS = pFrH->m_btSTATUS;
				FR.m_Name.Set( pName );
				
				// check name & dbid...
				if ( !this->m_pSQL->QuerySQL( "SELECT txtNAME FROM tblGS_AVATAR WHERE intCharID=%d", FR.m_dwDBID ) ) {
					g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
				} else
				if ( this->m_pSQL->GetNextRECORD() ) {
					pName = this->m_pSQL->GetStrPTR( 0 ); 
					if ( 0 == pFrH->m_dwDBID || pFrH->m_dwDBID > 31793 ) {
						LogString (0xffff, "Invalid DBID: %d, %s \n", FR.m_dwDBID, FR.m_Name.Get() );
					} else
					if ( strcmpi(pName, FR.m_Name.Get() ) ) {
						LogString (0xffff, "Name mismatch %d, %s/%s \n", pFrH->m_dwDBID, pName, FR.m_Name.Get() );
					}
				}

				pLIST += ( sizeof(tagFriend_H)+strlen(pName)+1 );
			}

			iTotCNT ++;
		}
	}
	LogString (0xffff, ">>>>>>>>>>>>> Totcal friend record %d \n", iTotCNT );
*/
}

//-------------------------------------------------------------------------------------------------
	struct tagRU
	{
		CStrVAR		m_Char;
		int			m_iCnt;
//		CInventory *m_pINV;
	} ;
void CThreadMSGR::Check_ItemCNT (int iItemType, int iItemNo)
{
/*
	int iCharID=0, iMaxCNT=-1, iCurCNT, iTotCNT=0;
	char *szName, *szAccount;
	CInventory *pINV;
	tagITEM *pITEM;



	CDLList< tagRU >		   RoseUSER;
	CDLList< tagRU >::tagNODE *pUserNODE;


#define	TOP_CNT	40
	struct {
		CStrVAR m_Acc;
		CStrVAR m_Name;
		short	m_nCnt;
	} ll[ TOP_CNT ];
	short nI;
	for (nI=0; nI<TOP_CNT; nI++) {
		ll[nI].m_nCnt = 0;
	}
	short nMinIdx=0;

	g_LOG.CS_ODS( 0xffff, "!!!!!!!!! start Check_ItemCNT( %d, %d ) \n", iItemType, iItemNo );

	// this->SetPriority( THREAD_PRIORITY_HIGHEST );	// Priority 2 point above the priority class
	this->SetPriority( THREAD_PRIORITY_ABOVE_NORMAL );	// Priority 2 point above the priority class

	FILE *fp = fopen( "item_cnt.txt", "w" );
	while( true ) 
	{
		if ( !this->m_pSQL->QuerySQL( "SELECT TOP 500 intCharID, txtACCOUNT, txtNAME, blobINV FROM tblGS_AVATAR WHERE intCharID > %d ORDER BY intCharID ASC", iCharID ) ) {
			g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
			break;
		}

		if ( !this->m_pSQL->GetNextRECORD() ) 
			break;

		do {
			iCharID	  = this->m_pSQL->GetInteger( 0 );
			szAccount = this->m_pSQL->GetStrPTR( 1 );
			szName	  = this->m_pSQL->GetStrPTR( 2 );
			pINV = (CInventory*)this->m_pSQL->GetDataPTR( 3 );
			
			iCurCNT = 0;
			for (int iC=0; iC<INVENTORY_PAGE_SIZE; iC++) {
				//pITEM = &pINV->m_ItemPAGE[ INV_USE ][ iC ];
				pITEM = &pINV->m_ItemPAGE[ INV_ETC ][ iC ];
				if ( 0 == pITEM->GetHEADER() )
					continue;
				if ( iItemType == pITEM->GetTYPE() && iItemNo == pITEM->GetItemNO () ) {
					iCurCNT += pITEM->GetQuantity ();
					pITEM->Clear ();
				}
			}

			if ( iCurCNT > 0 ) {
				pUserNODE = new CDLList< tagRU >::tagNODE;
				pUserNODE->m_VALUE.m_Char.Set( szName );
				pUserNODE->m_VALUE.m_iCnt = iCurCNT;
				//pUserNODE->m_VALUE.m_pINV = new CInventory;
				//CopyMemory( pUserNODE->m_VALUE.m_pINV, pINV, sizeof( CInventory ) );

				RoseUSER.AppendNode( pUserNODE );

				// LogString (0xffff, "%s:%s : %d\n", szAccount, szName, iCurCNT );
				// LogString (0xffff, "( \'%s\', \'%s\', \'%d\' ), \n", szAccount, szName, iCurCNT );

				if ( iCurCNT > iMaxCNT ) {
					iMaxCNT = iCurCNT;
				}
				if ( iCurCNT > ll[ nMinIdx ].m_nCnt ) {
					short nMin = 9999;
					ll[ nMinIdx ].m_Acc.Set( szAccount );
					ll[ nMinIdx ].m_Name.Set( szName );
					ll[ nMinIdx ].m_nCnt = iCurCNT;
					for (nI=0; nI<TOP_CNT; nI++) {
						if ( ll[nI].m_nCnt < nMin ) {
							nMinIdx = nI;
							nMin = ll[nI].m_nCnt; 
						}
					}
				}
			}
			iTotCNT ++;
		} while( this->m_pSQL->GetNextRECORD() );
	}
	for (nI=0; nI<TOP_CNT; nI++) {
		fprintf (fp, "  %d %s %s %d \n", nI, ll[nI].m_Acc.Get(), ll[nI].m_Name.Get(), ll[nI].m_nCnt );
	}
	fprintf( fp, ">>>>>>>>>>>>> Totcal record %d \n", iTotCNT );
	fclose( fp );
	g_LOG.CS_ODS( 0xffff, "!!!!!!!!! end Check_ItemCNT( %d, %d :: %d ) //  \n", iItemType, iItemNo, RoseUSER.GetNodeCount() );
*/

/*
	// ******** 테이블 뻑남...
	int ii = 0;
	pUserNODE = RoseUSER.GetHeadNode();
	while( pUserNODE ) {
		// 포인터인 m_pINV앞에 & 붙여서 뻑???
		//this->m_pSQL->BindPARAM( 1, (BYTE*)&pUserNODE->m_VALUE.m_pINV,			sizeof( CInventory )		);
		this->m_pSQL->BindPARAM( 1, (BYTE*)pUserNODE->m_VALUE.m_pINV,			sizeof( CInventory )		);
		this->m_pSQL->MakeQuery( "UPDATE tblGS_AVATAR SET blobINV=",
													MQ_PARAM_BINDIDX,	1,
					MQ_PARAM_ADDSTR, "WHERE txtNAME=",	MQ_PARAM_STR,		pUserNODE->m_VALUE.m_Char.Get(),
													MQ_PARAM_END );
		if ( this->m_pSQL->ExecSQLBuffer() < 0 ) {
			// 고치기 실패 !!!
			// log ...
			g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: UPDATE Char:%s %s \n", pUserNODE->m_VALUE.m_Char.Get(), m_pSQL->GetERROR() );
		} else
			LogString( 0xffff, "%d update inv: %s \n", ++ii, pUserNODE->m_VALUE.m_Char.Get() );
//		g_LOG.CS_ODS( 0xffff, "roseUSER: %s : %d \n", pUserNODE->m_VALUE.m_Char.Get(), pUserNODE->m_VALUE.m_iCnt );

		pUserNODE = RoseUSER.GetNextNode( pUserNODE );
	} ;
*/
}

//-------------------------------------------------------------------------------------------------
void CThreadMSGR::Execute ()
{
	CDLList< tagMSGR_CMD >::tagNODE *pCmdNODE;

	g_LOG.CS_ODS( 0xffff, ">  > >> CThreadMSGR::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );

	
//	this->Check_FRIENDS ();
//	this->Check_ItemCNT (ITEM_TYPE_NATURAL, 401);

    while( TRUE ) {
		if ( !this->Terminated ) {
			m_pEVENT->WaitFor( INFINITE );
		} else {
			int iReaminCNT;
			this->m_CS.Lock ();
				iReaminCNT = m_WaitCMD.GetNodeCount();
			this->m_CS.Unlock ();

			if ( iReaminCNT <= 0 )
				break;
		}

		this->m_CS.Lock ();
		m_ProcCMD.AppendNodeList( &m_WaitCMD );
		m_WaitCMD.Init ();
		m_pEVENT->ResetEvent ();
		this->m_CS.Unlock ();

		for( pCmdNODE = m_ProcCMD.GetHeadNode(); pCmdNODE; ) {
			this->Run_MessengerPACKET( &pCmdNODE->m_VALUE );

			SAFE_DELETE_ARRAY( pCmdNODE->m_VALUE.m_pPacket );
			m_ProcCMD.DeleteNFree( pCmdNODE );
			pCmdNODE = m_ProcCMD.GetHeadNode();
		}
	}

	int iCnt = m_AddPACKET.GetNodeCount();
	_ASSERT( iCnt == 0 );

	g_LOG.CS_ODS( 0xffff, "<<<< CThreadMSGR::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );
}


//-------------------------------------------------------------------------------------------------
void CThreadMSGR::Add_MessengerCMD (char *szCharName, BYTE btCMD, t_PACKET *pPacket, int iSocketIDX, DWORD dwDBID)
{
	CDLList< tagMSGR_CMD >::tagNODE *pNewNODE;

	pNewNODE = new CDLList< tagMSGR_CMD >::tagNODE;

	pNewNODE->m_VALUE.m_iSocketIDX	= iSocketIDX;
	pNewNODE->m_VALUE.m_dwDBID		= dwDBID;
	pNewNODE->m_VALUE.m_btCMD		= btCMD;
	pNewNODE->m_VALUE.m_Name.Set( szCharName );
	if ( pPacket ) {
		pNewNODE->m_VALUE.m_pPacket	= (t_PACKET*) new BYTE[ pPacket->m_HEADER.m_nSize ];
		::CopyMemory( pNewNODE->m_VALUE.m_pPacket, pPacket, pPacket->m_HEADER.m_nSize );
	} else
		pNewNODE->m_VALUE.m_pPacket	= NULL;

	this->m_CS.Lock ();
	m_WaitCMD.AppendNode( pNewNODE );
	this->m_CS.Unlock ();

	this->Set_EVENT ();
}


//-------------------------------------------------------------------------------------------------
CMessenger *CThreadMSGR::SearchMSGR (char *szCharName)
{
	t_HASHKEY HashKEY = CStr::GetHASH( szCharName );

	tagHASH< CMessenger* > *pHashNode = m_HashMSGR.Search( HashKEY );
	while( pHashNode ) {
		if ( !strcmpi( szCharName, pHashNode->m_DATA->m_Name.Get() ) ) {
			return pHashNode->m_DATA;
		}
		pHashNode = m_HashMSGR.SearchContinue( pHashNode, HashKEY );
	}
	return NULL;
}

bool CThreadMSGR::Run_MessengerPACKET( tagMSGR_CMD *pMsgCMD )
{
	switch( pMsgCMD->m_btCMD ) {
		case MSGR_CMD_LOGIN			:
			this->LogIN( pMsgCMD );
			return true;
		case MSGR_CMD_LOGOUT		:
		{
			CMessenger *pMSGR = this->SearchMSGR( pMsgCMD->m_Name.Get() );
			if ( pMSGR )
				this->LogOUT( pMSGR );
			return true;
		}

		case MSGR_CMD_APPEND_ACCEPT	:
		{
			CWS_Client *pRequestUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pMsgCMD->m_pPacket->m_cli_MCMD_APPEND_REPLY.m_wUserIDX );
			short nOffset=sizeof( cli_MCMD_APPEND_REPLY );
			char *szName = Packet_GetStringPtr( pMsgCMD->m_pPacket, nOffset );
			if ( pRequestUSER && szName && !strcmpi( szName, pRequestUSER->Get_NAME() ) ) {
				CMessenger *pMSGR1 = this->SearchMSGR( pMsgCMD->m_Name.Get() );
				if ( pMSGR1 ) {
					if ( pMSGR1->Get_FriendCNT() < MAX_FRIEND_COUNT ) {
						CMessenger *pMSGR2 = this->SearchMSGR( pRequestUSER->Get_NAME() );
						if ( pMSGR2 && pMSGR1->m_dwDBID != pMSGR2->m_dwDBID ) {
							if ( pMSGR2->Get_FriendCNT() < MAX_FRIEND_COUNT ) {
								pMSGR1->MSGR_Add( pMSGR2 );
								pMSGR2->MSGR_Add( pMSGR1 );
								return true;
							}
							// pMSGR2에게 친구 추가 실패 전송
						}
					}
					// pMSGR1에게 친구 추가 실패 전송
				}
			}
			return true;
		}

		case MSGR_CMD_DELETE		:	
		{
			CMessenger *pMSGR = this->SearchMSGR( pMsgCMD->m_Name.Get() );
			if ( pMSGR )
				pMSGR->MSGR_Del( pMsgCMD->m_pPacket->m_cli_MCMD_TAG.m_dwUserTAG );
			return true;
		}

		case MSGR_CMD_REFUSE		:
		{
			CMessenger *pMSGR = this->SearchMSGR( pMsgCMD->m_Name.Get() );
			if ( pMSGR )
				pMSGR->MSGR_Ban( pMsgCMD->m_pPacket->m_cli_MCMD_TAG.m_dwUserTAG );
			return true;
		}

		case MSGR_CMD_CHANGE_STATUS	:
		{
			// 내상태 변경... 등록된 모든 친구에게...
			CMessenger *pMSGR = this->SearchMSGR( pMsgCMD->m_Name.Get() );
			if ( pMSGR )
				pMSGR->MSGR_Status2ALL( pMsgCMD->m_pPacket->m_cli_MCMD_STATUS_REQ.m_btStatus );
			break;
		}

		case 0x0ff :
		{
			CMessenger *pMSGR = this->SearchMSGR( pMsgCMD->m_Name.Get() );
			if ( pMSGR && FRIEND_STATUS_REFUSE_MSG != pMSGR->m_btMsgrSTATUS )
				pMSGR->MSGR_Msg( pMsgCMD->m_pPacket );
			return true;
		}
	}

	return false;
}


//-------------------------------------------------------------------------------------------------
bool CThreadMSGR::LogIN (tagMSGR_CMD *pCMD)
{
	if ( !this->m_pSQL->QuerySQL( "{call ws_GetFRIEND(%d)}", pCMD->m_dwDBID ) ) {
//	if ( !this->m_pSQL->QuerySQL( "SELECT intFriendCNT, blobFRIENDS FROM tblWS_FRIEND WHERE intCharID=%d", pCMD->m_dwDBID ) ) {
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		return false;
	}
	
	CMessenger *pMSGR;
	pMSGR = this->SearchMSGR( pCMD->m_Name.Get() );
	if ( pMSGR )
		return true;

	pMSGR = this->AllocMEM();
	if ( NULL == pMSGR ) 
		return false;

	pMSGR->Init( pCMD->m_Name.Get(), pCMD->m_dwDBID, pCMD->m_iSocketIDX );
	if ( !this->m_pSQL->GetNextRECORD() ) {
		// insert !!!
		if ( this->m_pSQL->ExecSQL("INSERT tblWS_FRIEND ( intCharID ) VALUES(%d);", pCMD->m_dwDBID ) < 1 ) {
			g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: INSERT %s friend : %s \n", pCMD->m_Name.Get(), m_pSQL->GetERROR() );
			this->FreeMEM( pMSGR );
			return true;
		}
	} else {
		int iFriendCNT = this->m_pSQL->GetInteger( 0 );
		if ( iFriendCNT > 0 ) {
			BYTE *pDATA = this->m_pSQL->GetDataPTR( 1 );
			pMSGR->MSGR_LogIN( iFriendCNT, pDATA );
		} 
	}
	pMSGR->m_btMsgrSTATUS  = FRIEND_STATUS_ONLINE;

	t_HASHKEY HashKEY = CStr::GetHASH( pCMD->m_Name.Get() );
	this->m_HashMSGR.Insert( HashKEY, pMSGR );

	return true;
}

//-------------------------------------------------------------------------------------------------
void CThreadMSGR::LogOUT (CMessenger *pMSGR)
{
	// update db...
	int iFriendCNT = pMSGR->Get_FriendCNT();
	int iBuffLEN   = pMSGR->MSGR_LogOUT( this->m_pListBUFF );

	if ( pMSGR->MSGR_IsUPDATE() ) {
		if ( iFriendCNT > 0 ) {
			this->m_pSQL->BindPARAM( 1, this->m_pListBUFF, iBuffLEN );
			this->m_pSQL->MakeQuery( "UPDATE tblWS_FRIEND SET blobFRIENDS=",
															MQ_PARAM_BINDIDX,	1,
					MQ_PARAM_ADDSTR, ",intFriendCNT=",		MQ_PARAM_INT,	iFriendCNT,
					MQ_PARAM_ADDSTR, "WHERE intCharID=",	MQ_PARAM_INT,	pMSGR->Get_DBID(),
															MQ_PARAM_END );
		} else {
			this->m_pSQL->MakeQuery( "UPDATE tblWS_FRIEND SET blobFRIENDS=NULL",
					MQ_PARAM_ADDSTR, ",intFriendCNT=",		MQ_PARAM_INT,	iFriendCNT,
					MQ_PARAM_ADDSTR, "WHERE intCharID=",	MQ_PARAM_INT,	pMSGR->Get_DBID(),
															MQ_PARAM_END );
		}
		if ( this->m_pSQL->ExecSQLBuffer() < 0 ) {
			// 고치기 실패 !!!
			g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: UPDATE messenger:%d %s \n", pMSGR->Get_DBID(), m_pSQL->GetERROR() );
		}
	}

#ifdef	_DEBUG
	if ( pMSGR->Get_FriendCNT() ) {
		g_LOG.CS_ODS( 0xffff, "ERROR:: pMSGR->m_ListFRIEND.GetNodeCount() must 0 / %d ", pMSGR->Get_FriendCNT(), pMSGR->m_dwDBID );
	}
#endif

	t_HASHKEY HashKEY = CStr::GetHASH( pMSGR->m_Name.Get() );
	this->m_HashMSGR.Delete( HashKEY, pMSGR );

	this->FreeMEM( pMSGR );
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool CMessenger::SendPacket (int iClientSocketIDX, DWORD dwDBID, classPACKET *pCPacket)
{
	CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( iClientSocketIDX );
	if ( pFindUSER && pFindUSER->m_dwDBID == dwDBID ) {
		if ( !pFindUSER->SendPacket( pCPacket ) ) {
			// 보내기 실패...
		}
		return true;
	} 

	return false;
}

//-------------------------------------------------------------------------------------------------
void CMessenger::MSGR_LogIN (int iCount, BYTE *pLIST)
{
	classPACKET *pToFR = Packet_AllocNLock ();
	classPACKET *pToME = Packet_AllocNLock ();

	pToFR->m_HEADER.m_wType = WSV_MESSENGER;
	pToFR->m_HEADER.m_nSize = sizeof( wsv_MCMD_STATUS_REPLY );

	pToFR->m_wsv_MCMD_STATUS_REPLY.m_btCMD = MSGR_CMD_CHANGE_STATUS;
	pToFR->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG = this->Get_DBID ();
	pToFR->m_wsv_MCMD_STATUS_REPLY.m_btStatus  = FRIEND_STATUS_ONLINE;

	
	pToME->m_HEADER.m_wType = WSV_MESSENGER;
	pToME->m_HEADER.m_nSize = sizeof( wsv_MCMD_LIST );

	pToME->m_wsv_MCMD_LIST.m_btCMD = MSGR_CMD_LIST;
	pToME->m_wsv_MCMD_LIST.m_btFriendCNT = iCount;
	
	tagFriend_H sFrStatus;

	char *pName;
	tagFriend_H *pFrH;

	CMessenger *pFindMSGR;
	CDLList< tagMyFriend >::tagNODE *pNODE;
	for (int iC=0; iC<iCount; iC++) {
		pFrH = (tagFriend_H*)pLIST;
		pName = (char*)( pLIST+sizeof(tagFriend_H) );

		if ( this->m_dwDBID == pFrH->m_dwDBID ) {
			// 자기 자신이 등록된 오류가 있는가 ?
			CStrVAR tmpName;
			tmpName.Set( pName );
			pLIST += ( sizeof(tagFriend_H)+tmpName.BuffLength()+1 );
			this->m_bFriendUPDATE = true;
			continue;
		}

		pNODE = m_ListFRIEND.AllocNAppend();

		pNODE->m_VALUE.m_dwDBID   = pFrH->m_dwDBID;
		pNODE->m_VALUE.m_btSTATUS = pFrH->m_btSTATUS;
		pNODE->m_VALUE.m_Name.Set( pName );
		pNODE->m_VALUE.m_pMSGR    = NULL;

		pLIST += ( sizeof(tagFriend_H)+pNODE->m_VALUE.m_Name.BuffLength()+1 );

		if ( !(FRIEND_STATUS_REFUSED & pNODE->m_VALUE.m_btSTATUS) ) {
			pFindMSGR = g_pThreadMSGR->SearchMSGR( pName );
			if ( pFindMSGR ) {
				pNODE->m_VALUE.m_btSTATUS = pFindMSGR->MSGR_OnOffLine( pToFR, this, this->Get_DBID(), FRIEND_STATUS_ONLINE );
				if ( FRIEND_STATUS_DELETED != pNODE->m_VALUE.m_btSTATUS ) {
					// pFindMSGR목록에 내가 있다...
					pNODE->m_VALUE.m_pMSGR = pFindMSGR;
				}
			} else {
				pNODE->m_VALUE.m_btSTATUS = FRIEND_STATUS_OFFLINE;
			}
		} /* else {
			// 나를 삭제하거나 내가 차단한넘이기 땜에 내 상태 통보할 필요없다.
			// 나도 이넘을 로그아웃 상태로 보이게함...
		} */

		sFrStatus.m_dwDBID   = pNODE->m_VALUE.m_dwDBID;
		sFrStatus.m_btSTATUS = pNODE->m_VALUE.m_btSTATUS;

		pToME->AppendData( &sFrStatus, sizeof( tagFriend_H ) );
		pToME->AppendString( pName );
		if ( pToME->m_HEADER.m_nSize > MAX_PACKET_SIZE-30 )  {
			this->m_bFriendUPDATE = true;
			break;
		}
	}

	this->SendPacket( this->m_iOwnerSocetIDX, this->m_dwDBID, pToME );

	Packet_ReleaseNUnlock( pToME );
	Packet_ReleaseNUnlock( pToFR );
}


//-------------------------------------------------------------------------------------------------
/*
2. 로그아웃시
	. 나가 등록한 친구리스트 조회..
		NODE->m_VALUE.m_pUSER != NULL 이면 로그아웃 전송.
*/
int CMessenger::MSGR_LogOUT (BYTE *pOutBUFF)
{
	classPACKET *pCPacket = Packet_AllocNLock ();

	pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_MCMD_STATUS_REPLY );

	pCPacket->m_wsv_MCMD_STATUS_REPLY.m_btCMD = MSGR_CMD_CHANGE_STATUS;
	pCPacket->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG = this->Get_DBID ();
	pCPacket->m_wsv_MCMD_STATUS_REPLY.m_btStatus  = FRIEND_STATUS_OFFLINE;

	int iBuffLEN = 0;
	tagFriend_H *pFR;

	CMessenger *pFindMSGR;
	CDLList< tagMyFriend >::tagNODE *pNODE;
	pNODE = m_ListFRIEND.GetHeadNode();
	while( pNODE ) {
		pFR = (tagFriend_H*)&pOutBUFF[ iBuffLEN ];

		pFR->m_btSTATUS = pNODE->m_VALUE.m_btSTATUS;
		pFR->m_dwDBID   = pNODE->m_VALUE.m_dwDBID;

		iBuffLEN += sizeof( tagFriend_H );
		::CopyMemory( &pOutBUFF[ iBuffLEN ], pNODE->m_VALUE.m_Name.Get(), pNODE->m_VALUE.m_Name.BuffLength() );
		iBuffLEN += pNODE->m_VALUE.m_Name.BuffLength();
		pOutBUFF[ iBuffLEN++ ] = 0;

		pFindMSGR = g_pThreadMSGR->SearchMSGR( pNODE->m_VALUE.m_Name.Get() );
		if ( pFindMSGR ) {
			if ( pFR->m_btSTATUS == FRIEND_STATUS_OFFLINE ) {	
				// 접속해 있는데, 나에게 상태를 등록안했다는것은 나를 삭제했거나, 나를 삭제했다는것...
				pFR->m_btSTATUS = FRIEND_STATUS_DELETED;
			}
			pFindMSGR->MSGR_OnOffLine( pCPacket, NULL, this->Get_DBID(), FRIEND_STATUS_OFFLINE );
		} else {
			if ( pNODE->m_VALUE.m_pMSGR ) {
				// 뭐냐 ???
				g_LOG.CS_ODS( 0xffff, "**ERROR in MSGR_LogOUT owner[ 0x%x:%d:%s ], frined[ 0x%x, %d:%s ]\n", this->m_btMsgrSTATUS, this->m_dwDBID, this->m_Name.Get(), pNODE->m_VALUE.m_btSTATUS, pNODE->m_VALUE.m_dwDBID, pNODE->m_VALUE.m_Name.Get() );
			}
		}
/*
		if ( pNODE->m_VALUE.m_pMSGR ) {
			if ( pFindMSGR == pNODE->m_VALUE.m_pMSGR ) {
				pNODE->m_VALUE.m_pMSGR->MSGR_OnOffLine( pCPacket, NULL, this->Get_DBID(), FRIEND_STATUS_OFFLINE );
			} else {
				// 오류~~~ :: 어떤 경우냐 ???
				g_LOG.CS_ODS( 0xffff, "**ERROR in MSGR_LogOUT owner[ 0x%x:%d:%s ], frined[ 0x%x, %d:%s ]\n", this->m_btMsgrSTATUS, this->m_dwDBID, this->m_Name.Get(), pNODE->m_VALUE.m_btSTATUS, pNODE->m_VALUE.m_dwDBID, pNODE->m_VALUE.m_Name.Get() );
			}
		} else 
		if ( pFindMSGR && pFR->m_btSTATUS == FRIEND_STATUS_OFFLINE ) {	
			// 접속해 있는데, 나에게 상태를 등록안했다는것은 나를 삭제했거나, 나를 삭제했다는것...
			pFR->m_btSTATUS = FRIEND_STATUS_DELETED;
		}
*/
		m_ListFRIEND.DeleteNFree( pNODE );
		pNODE = m_ListFRIEND.GetHeadNode();
	}

	Packet_ReleaseNUnlock( pCPacket );

	return iBuffLEN;
}


bool CMessenger::MSGR_Add( CMessenger *pFriend )
{
	CDLList< tagMyFriend >::tagNODE *pNODE;

	pNODE = m_ListFRIEND.GetHeadNode ();
	while( pNODE ) {
		if ( pNODE->m_VALUE.m_dwDBID == pFriend->Get_DBID() ) {
			// 이미 등록된 사용자다... 삭제되었던넘이 다시 추가할 경우..
			pNODE->m_VALUE.m_btSTATUS   = pFriend->m_btMsgrSTATUS;
			pNODE->m_VALUE.m_pMSGR		= pFriend;
			this->MSGR_Status2ONE( pFriend->Get_DBID(), pFriend->m_btMsgrSTATUS, pFriend );		// CMessenger::MSGR_Add
			return true;
		}
		pNODE = pNODE->GetNext();
	}

	CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pFriend->m_iOwnerSocetIDX );
	if ( pFindUSER && pFindUSER->m_dwDBID == pFriend->m_dwDBID ) {
		this->m_bFriendUPDATE = true;
		pNODE = m_ListFRIEND.AllocNAppend();

		pNODE->m_VALUE.m_btSTATUS   = pFriend->m_btMsgrSTATUS;
		pNODE->m_VALUE.m_dwDBID		= pFriend->Get_DBID();
		pNODE->m_VALUE.m_pMSGR		= pFriend;
		pNODE->m_VALUE.m_Name.Set( pFriend->m_Name.Get() );

		classPACKET *pCPacket = Packet_AllocNLock ();

		pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
		pCPacket->m_HEADER.m_nSize = sizeof( wsv_MCMD_APPEND_ACCEPT );

		pCPacket->m_wsv_MCMD_APPEND_ACCEPT.m_btCMD	   = MSGR_CMD_APPEND_ACCEPT;
		pCPacket->m_wsv_MCMD_APPEND_ACCEPT.m_dwUserTAG = pNODE->m_VALUE.m_dwDBID;
		pCPacket->m_wsv_MCMD_APPEND_ACCEPT.m_btStatus  = pNODE->m_VALUE.m_btSTATUS;
		pCPacket->AppendString( pNODE->m_VALUE.m_Name.Get() );

		this->SendPacket( this->m_iOwnerSocetIDX, this->m_dwDBID, pCPacket );

		Packet_ReleaseNUnlock( pCPacket );
		return true;
	}
	return false;	
}

void CMessenger::MSGR_Del( DWORD dwDBID )
{
	CDLList< tagMyFriend >::tagNODE *pNODE;

	pNODE = m_ListFRIEND.GetHeadNode ();
	while( pNODE ) {
		if ( dwDBID == pNODE->m_VALUE.m_dwDBID ) {
			// 오너클라이언트 알아서 삭제하라고...
			if ( pNODE->m_VALUE.m_pMSGR ) {
				// "대상은 삭제된상태로 설정하고 반드시 m_pFRIEND = NULL로 만들어야...... " )
				pNODE->m_VALUE.m_pMSGR->MSGR_Status2ONE( this->Get_DBID(), FRIEND_STATUS_DELETED, NULL );	// CMessenger::MSGR_Del
			}

			m_ListFRIEND.DeleteNFree( pNODE );
			this->m_bFriendUPDATE = true;
			break;
		}
		pNODE = pNODE->GetNext ();
	}
}

//-------------------------------------------------------------------------------------------------
BYTE CMessenger::MSGR_OnOffLine ( classPACKET *pCPacket, CMessenger *pFriend, DWORD dwDBID, BYTE btNewStatus )
{
	CDLList< tagMyFriend >::tagNODE *pNODE;

	pNODE = m_ListFRIEND.GetHeadNode ();
	while( pNODE ) {
		if ( dwDBID == pNODE->m_VALUE.m_dwDBID ) {
			pNODE->m_VALUE.m_pMSGR = pFriend;

			if ( !( FRIEND_STATUS_REFUSED & pNODE->m_VALUE.m_btSTATUS ) ) {
				// 내가 거부했거나, 나를 삭제한 넘이아니면...
				pNODE->m_VALUE.m_btSTATUS = btNewStatus;
				this->SendPacket( this->m_iOwnerSocetIDX, this->m_dwDBID, pCPacket );
				return this->m_btMsgrSTATUS;
			}
			
			return FRIEND_STATUS_OFFLINE;
		}
		pNODE = pNODE->GetNext ();
	}

	return FRIEND_STATUS_DELETED;
}


//-------------------------------------------------------------------------------------------------
void CMessenger::MSGR_Status2ONE( DWORD dwDBID, BYTE btNewStatus, CMessenger *pMessenger )
{
	// 내목록에서 dwDBID를 찾아 btNewStatus상태로 바꾼다.
	// dwDBID가 나를 삭제한것이라면... 내 목록의 m_pFRIEND를 NULL !!!로 설정해야함.
	CDLList< tagMyFriend >::tagNODE *pNODE;

	pNODE = m_ListFRIEND.GetHeadNode ();
	while( pNODE ) {
		if ( dwDBID == pNODE->m_VALUE.m_dwDBID ) {	// 04. 10. 11 : 0x0013bfe오류...
			pNODE->m_VALUE.m_pMSGR = pMessenger;	// 나를 삭제할때는 pMessenger = NULL로 옮

			if ( !(FRIEND_STATUS_REFUSED & pNODE->m_VALUE.m_btSTATUS ) ) {
				// 내가 거부한 상태거나, 넘이 나를 삭제한 상태가 아니면??? (나를 삭제한 넘이 나한테 보내-삭제할 당시)
				this->m_bFriendUPDATE = ( FRIEND_STATUS_REFUSED & ( pNODE->m_VALUE.m_btSTATUS | btNewStatus ) ) ? true : false;
				pNODE->m_VALUE.m_btSTATUS = btNewStatus;

				classPACKET *pCPacket = Packet_AllocNLock ();

				pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
				pCPacket->m_HEADER.m_nSize = sizeof( wsv_MCMD_STATUS_REPLY );

				pCPacket->m_wsv_MCMD_STATUS_REPLY.m_btCMD = MSGR_CMD_CHANGE_STATUS;
				pCPacket->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG = dwDBID;
				pCPacket->m_wsv_MCMD_STATUS_REPLY.m_btStatus  = btNewStatus;

				this->SendPacket( this->m_iOwnerSocetIDX, this->m_dwDBID, pCPacket );

				Packet_ReleaseNUnlock( pCPacket );
			}
			break;
		}
		pNODE = pNODE->GetNext ();
	}
}

void CMessenger::MSGR_Msg( t_PACKET *pPacket )
{
	CDLList< tagMyFriend >::tagNODE *pNODE;

	pNODE = m_ListFRIEND.GetHeadNode ();
	while( pNODE ) {
		if ( pNODE->m_VALUE.m_dwDBID == pPacket->m_cli_MESSENGER_CHAT.m_dwUserTAG ) {
			if ( pNODE->m_VALUE.m_pMSGR ) {
				classPACKET *pCPacket = Packet_AllocNLock ();
				if ( pCPacket ) {
					pCPacket->m_HEADER.m_wType = WSV_MESSENGER_CHAT;
					pCPacket->m_HEADER.m_nSize = sizeof( wsv_MESSENGER_CHAT );
					pCPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG = this->m_dwDBID;
					pCPacket->AppendString( pPacket->m_cli_MESSENGER_CHAT.m_szMSG );

					this->SendPacket( pNODE->m_VALUE.m_pMSGR->m_iOwnerSocetIDX, pNODE->m_VALUE.m_pMSGR->m_dwDBID, pCPacket );

					Packet_ReleaseNUnlock( pCPacket );
				}
			}
			break;
		}

		pNODE = pNODE->GetNext ();
	}
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void CMessenger::MSGR_Ban( DWORD dwDBID )
{
/*	CDLList< tagMyFriend >::tagNODE *pNODE;

	pNODE = m_ListFRIEND.GetHeadNode ();
	while( pNODE ) {
		if ( dwDBID == pNODE->m_VALUE.m_dwDBID ) {
			// 이넘의 메세지 차단..
			if ( FRIEND_STATUS_DELETED != pNODE->m_VALUE.m_btSTATUS ) {
				if ( FRIEND_STATUS_REFUSED & pNODE->m_VALUE.m_btSTATUS ) {
					if ( pNODE->m_VALUE.m_pMSGR )
						pNODE->m_VALUE.m_btSTATUS = pNODE->m_VALUE.m_pMSGR->m_btMsgrSTATUS;
					else
						pNODE->m_VALUE.m_btSTATUS = FRIEND_STATUS_OFFLINE;
				} else
					pNODE->m_VALUE.m_btSTATUS = FRIEND_STATUS_REFUSED;
			}

			#pragma COMPILE_TIME_MSG ( " 바뀐 상태 전송??? " )

			break;
		}
		pNODE = pNODE->GetNext ();
	} */
}

void CMessenger::MSGR_Status2ALL( BYTE btNewStatus )
{
	this->m_btMsgrSTATUS = btNewStatus;

	classPACKET *pCPacket = Packet_AllocNLock ();

	pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_MCMD_STATUS_REPLY );

	pCPacket->m_wsv_MCMD_STATUS_REPLY.m_btCMD = MSGR_CMD_CHANGE_STATUS;
	pCPacket->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG = this->Get_DBID ();
	pCPacket->m_wsv_MCMD_STATUS_REPLY.m_btStatus  = btNewStatus;

	CDLList< tagMyFriend >::tagNODE *pNODE;

	pNODE = m_ListFRIEND.GetHeadNode ();
	while( pNODE ) {
		if ( pNODE->m_VALUE.m_pMSGR ) {
			this->SendPacket( pNODE->m_VALUE.m_pMSGR->m_iOwnerSocetIDX, pNODE->m_VALUE.m_pMSGR->m_dwDBID, pCPacket );
		}
		pNODE = pNODE->GetNext ();
	}

	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
#endif
