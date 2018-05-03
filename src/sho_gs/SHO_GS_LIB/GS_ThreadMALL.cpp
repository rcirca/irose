
#include "LIB_gsMAIN.h"
#include "GS_ThreadMALL.h"
#include "GS_ListUSER.h"

//-------------------------------------------------------------------------------------------------
GS_CThreadMALL::GS_CThreadMALL () : CSqlTHREAD( true )
{
}
GS_CThreadMALL::~GS_CThreadMALL ()
{
}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadMALL::Add_SqlPacketWithACCOUNT (classUSER *pUSER, t_PACKET *pPacket)
{
	return CSqlTHREAD::Add_SqlPACKET((int)pUSER->m_iSocketIDX, pUSER->Get_ACCOUNT(), (BYTE*)pPacket, pPacket->m_HEADER.m_nSize);
}
bool GS_CThreadMALL::Add_SqlPacketWithAVATAR (classUSER *pUSER, t_PACKET *pPacket)
{
	return CSqlTHREAD::Add_SqlPACKET((int)pUSER->m_iSocketIDX, pUSER->Get_NAME(), (BYTE*)pPacket, pPacket->m_HEADER.m_nSize);
}

//-------------------------------------------------------------------------------------------------
void GS_CThreadMALL::Execute ()
{
	g_LOG.CS_ODS( 0xffff, ">  > >> GS_CThreadMALL::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );

	CDLList< tagQueryDATA >::tagNODE *pSqlNODE;

    while( TRUE ) {
		if ( !this->Terminated ) {
			m_bWaiting = true;
			m_pEVENT->WaitFor( INFINITE );
			m_bWaiting = false;
		} else {
			int iCnt1;
			this->m_CS.Lock ();
				iCnt1 = m_AddPACKET.GetNodeCount();
			this->m_CS.Unlock ();

			if ( iCnt1 <= 0 ) {
				m_bWaiting = true;
				break;
			}
		}

		this->m_CS.Lock ();
			m_RunPACKET.AppendNodeList( &m_AddPACKET );
			m_AddPACKET.Init ();
		m_pEVENT->ResetEvent ();
		this->m_CS.Unlock ();

		for( pSqlNODE = m_RunPACKET.GetHeadNode(); pSqlNODE; ) {
			if ( Run_SqlPACKET( &pSqlNODE->m_VALUE ) )
				pSqlNODE = this->Del_SqlPACKET( pSqlNODE );
			else
				pSqlNODE = m_RunPACKET.GetNextNode ( pSqlNODE );
		}
	}

	g_LOG.CS_ODS( 0xffff, "<  < << GS_CThreadMALL::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );
}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadMALL::Run_SqlPACKET( tagQueryDATA *pSqlPACKET )
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	switch( pPacket->m_cli_MALL_ITEM_REQ.m_btReqTYPE ) {
		case REQ_MALL_ITEM_LIST		:
		{
			this->List_MallITEM ( pSqlPACKET->m_iTAG, pSqlPACKET->m_Name.Get() );
			break;
		}
		case REQ_MALL_ITEM_BRING	:
		{
			// SQL에서 정상 리턴되면 사용자 인벤에 추가...
			this->Bring_MallITEM ( pSqlPACKET, 
						pPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0],
						pPacket->m_cli_MALL_ITEM_REQ.m_nDupCnt );
			break;
		}
		case REQ_MALL_ITEM_GIVE		:
		{
			this->Give_MallITEM( pSqlPACKET, 
						pPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0],
						1 /* pPacket->m_cli_MALL_ITEM_REQ.m_nDupCnt */ );
			break;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadMALL::Give_MallITEM ( tagQueryDATA *pSqlPACKET, BYTE btMallInvIDX, short nDupCnt )
{
	classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
	if ( NULL == pUSER )
		return true;

	if ( 0 == pUSER->m_MALL.m_ITEMS[ btMallInvIDX ].m_ui64ID ) {
		this->Send_gsv_MALL_ITEM_REPLAY( pUSER, REPLY_MALL_ITEM_NOT_FOUND );
		return true;
	}

	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	short nOffset = 1 + sizeof( cli_MALL_ITEM_REQ );
	char *szDesc;
	Packet_GetStringPtr( pPacket, nOffset );	// skip char name
	szDesc = Packet_GetStringPtr( pPacket, nOffset );

	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;
	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );
	if ( this->m_pSQL->QuerySQL( "{?=call GiveITEM(%I64d,\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%d)}", 
			pUSER->m_MALL.m_ITEMS[ btMallInvIDX ].m_ui64ID,
			pUSER->Get_ACCOUNT(), 
			pUSER->Get_NAME(), 
			pUSER->m_MALL.m_DestACCOUNT.Get(), 
			pUSER->m_MALL.m_DestCHAR.Get(), 
			szDesc, 
			pUSER->Get_IP(),
			CLIB_GameSRV::GetInstance()->GetServerIP(),
			nDupCnt ) ) {
//		short nItemType, nItemNo, nDupCnt;
		while ( this->m_pSQL->GetMoreRESULT() ) {
			if ( this->m_pSQL->BindRESULT() ) {
				;
			}
		}

		switch ( iResultSP ) {
			case -1 :	// db error
				// failed
				this->Send_gsv_MALL_ITEM_REPLAY( pUSER, REPLY_MALL_ITEM_GIVE_FAILED );
				return true;
			case  0 :	// clear item
				// clear
				pUSER->m_MALL.m_ITEMS[ btMallInvIDX ].m_ui64ID = 0;
				pUSER->m_MALL.m_ITEMS[ btMallInvIDX ].m_ITEM.Clear();
				break;
			default :
				pUSER->m_MALL.m_ITEMS[ btMallInvIDX ].m_ITEM.m_uiQuantity = iResultSP;
				break;
		}

		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( pCPacket ) {
			pCPacket->m_HEADER.m_wType = GSV_MALL_ITEM_REPLY;
			pCPacket->m_HEADER.m_nSize = sizeof(gsv_MALL_ITEM_REPLY) + sizeof(tag_SET_INVITEM);

			pCPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE = REPLY_MALL_ITME_GIVE_SUCCESS;
			pCPacket->m_gsv_MALL_ITEM_REPLY.m_btCntOrIdx  = btMallInvIDX;

			// 변경된 몰아이템..
			pCPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_btInvIDX = btMallInvIDX;
			pCPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_ITEM = pUSER->m_MALL.m_ITEMS[ btMallInvIDX ].m_ITEM;

			pUSER->Send_Start( pCPacket );
			Packet_ReleaseNUnlock( pCPacket );
			return true;
		}
	} else {
		this->Send_gsv_MALL_ITEM_REPLAY( pUSER, REPLY_MALL_ITEM_GIVE_FAILED );
	}

	return true;
}

bool GS_CThreadMALL::Bring_MallITEM ( tagQueryDATA *pSqlPACKET, BYTE btMallInvIDX, short nDupCnt )
{
	classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
	if ( NULL == pUSER )
		return true;
	
	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;
	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );
	if ( this->m_pSQL->QuerySQL( "{?=call PickOutITEM(\'%s\',\'%s\',\'%s\',\'%s\',%I64d,%d)}", pUSER->Get_ACCOUNT(), pUSER->Get_NAME(), pUSER->Get_IP(), 
				CLIB_GameSRV::GetInstance()->GetServerIP(), pUSER->m_MALL.m_ITEMS[ btMallInvIDX ].m_ui64ID, nDupCnt ) ) {
		short nItemType, nItemNo, nDupCnt, nRemainCnt;
		while ( this->m_pSQL->GetMoreRESULT() ) {
			if ( this->m_pSQL->BindRESULT() ) {
				if ( this->m_pSQL->GetNextRECORD() ) {
					nItemType	= this->m_pSQL->GetInteger(0);
					nItemNo		= this->m_pSQL->GetInteger(1);
					nDupCnt		= this->m_pSQL->GetInteger(2);
					nRemainCnt	= this->m_pSQL->GetInteger(3);		// 디비에 남은 개수
				}
			}
		}

		short nUserInvIDX;
		tagITEM sOutItem;
		if ( iResultSP ) {
			// clear
			if ( nRemainCnt > 0 ) {
				pUSER->m_MALL.m_ITEMS[ btMallInvIDX ].m_ITEM.m_uiQuantity = nRemainCnt;
			} else {
				pUSER->m_MALL.m_ITEMS[ btMallInvIDX ].m_ui64ID = 0;
				pUSER->m_MALL.m_ITEMS[ btMallInvIDX ].m_ITEM.Clear();
			}

			// success
			sOutItem.Clear ();
			sOutItem.m_cType   = nItemType;
			sOutItem.m_nItemNo = nItemNo;
			if ( sOutItem.IsEnableDupCNT() ) {
				sOutItem.m_uiQuantity = nDupCnt;
			} else {
				sOutItem.m_bIsAppraisal = 1;
				sOutItem.m_cDurability = ITEM_DURABITY( nItemType, nItemNo );
				sOutItem.m_nLife = MAX_ITEM_LIFE;
			}

			pUSER->Set_ItemSN( sOutItem );	// 쇼핑몰에서 구입시...
			nUserInvIDX = pUSER->Add_ITEM( sOutItem );
			if ( nUserInvIDX <= 0 ) {
				// 30분 * 2 * 10 -> 10시간
				pUSER->Save_ItemToFILED( sOutItem, ITEM_OBJ_DROP_TIME*2*10 );	// 몰 구입후 인벤토리가 추가가 불가능해서 밖에 30분간 보관...
			}
		} else {
			// failed
			nUserInvIDX = -1;
		}

		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( pCPacket ) {
			pCPacket->m_HEADER.m_wType = GSV_MALL_ITEM_REPLY;
			pCPacket->m_gsv_MALL_ITEM_REPLY.m_btCntOrIdx = btMallInvIDX;

			if ( iResultSP ) {
				pCPacket->m_HEADER.m_nSize = sizeof(gsv_MALL_ITEM_REPLY) + sizeof(tag_SET_INVITEM)*2;
				pCPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE = REPLY_MALL_ITEM_BRING_SUCCESS;

				// 사용자 인벤...
				pCPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_btInvIDX = static_cast<BYTE>( nUserInvIDX );
				pCPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_ITEM = sOutItem;

				// 변경된 몰아이템..
				pCPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[1].m_btInvIDX = btMallInvIDX;
				pCPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[1].m_ITEM     = pUSER->m_MALL.m_ITEMS[ btMallInvIDX ].m_ITEM;
			} else {
				pCPacket->m_HEADER.m_nSize = sizeof(gsv_MALL_ITEM_REPLY);
				pCPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE = REPLY_MALL_ITEM_BRING_FAILED;
			}

			pUSER->Send_Start( pCPacket );
			Packet_ReleaseNUnlock( pCPacket );
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadMALL::Send_gsv_MALL_ITEM_REPLAY(classUSER *pUSER, BYTE btType, BYTE btData)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;
	
	pCPacket->m_HEADER.m_wType = GSV_MALL_ITEM_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof(gsv_MALL_ITEM_REPLY);
	pCPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE = btType;
	pCPacket->m_gsv_MALL_ITEM_REPLY.m_btCntOrIdx = btData;

	pUSER->Send_Start( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

bool GS_CThreadMALL::List_MallITEM( int iScoketIDX, char *szAccount )
{
	__int64 biMallID;
	short	nItemTYPE, nItemNO, nDupCNT;
	char   *pFROM, *pTO, *pDESC/*, *pIP*/;
	//BYTE	btWorldIDX, btChannelIDX;

	//__int64 biStartID=0;
	//if ( this->m_pSQL->QuerySQL( "{call GetItemLIST(\'%s\',%I64d)}", pSqlPACKET->m_Name.Get(), biStartID ) ) {
	if ( this->m_pSQL->QuerySQL( "{call GetItemLIST(\'%s\',0)}", szAccount ) ) {
		classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( iScoketIDX );
		if ( NULL == pUSER )
			return true;

		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( pCPacket ) {
			// 목록 시작...
			this->Send_gsv_MALL_ITEM_REPLAY( pUSER, REPLY_MALL_ITEM_LIST_START );
	
			pCPacket->m_HEADER.m_wType = GSV_MALL_ITEM_REPLY;
			pCPacket->m_HEADER.m_nSize = sizeof(gsv_MALL_ITEM_REPLY);
			pCPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE = REPLY_MALL_ITEM_LIST_DATA;

			BYTE btCnt=0;
			while ( this->m_pSQL->GetNextRECORD() ) {
				biMallID	= this->m_pSQL->GetInt64(0);
				nItemTYPE	= this->m_pSQL->GetInteger(1);
				nItemNO		= this->m_pSQL->GetInteger(2);
				nDupCNT		= this->m_pSQL->GetInteger(3);
				pFROM		= this->m_pSQL->GetStrPTR(4,true);
				pTO			= this->m_pSQL->GetStrPTR(5,true);
				pDESC		= this->m_pSQL->GetStrPTR(6,true);

				// pIP		= this->m_pSQL->GetStrPTR(7,true);
				// btWorldIDX  = this->m_pSQL->GetInteger(7);
				// btChannelIDX= this->m_pSQL->GetInteger(8);

				// 선물 한 아이템이다. 월드가 틀리면 안되지~~~
				//if ( pFROM && btWorldIDX != xxx ) 
				//	continue;
				//}

				pUSER->m_MALL.m_ITEMS[ btCnt ].m_ui64ID = biMallID;
				pUSER->m_MALL.m_ITEMS[ btCnt ].m_ITEM.Clear ();

				pUSER->m_MALL.m_ITEMS[ btCnt ].m_ITEM.m_cType	= nItemTYPE;
				pUSER->m_MALL.m_ITEMS[ btCnt ].m_ITEM.m_nItemNo= nItemNO;

				if ( pUSER->m_MALL.m_ITEMS[ btCnt ].m_ITEM.IsEnableDupCNT() ) {
					pUSER->m_MALL.m_ITEMS[ btCnt ].m_ITEM.m_uiQuantity = nDupCNT;
				} else {
					pUSER->m_MALL.m_ITEMS[ btCnt ].m_ITEM.m_cDurability  = 100;
					pUSER->m_MALL.m_ITEMS[ btCnt ].m_ITEM.m_nLife		 = MAX_ITEM_LIFE;
					pUSER->m_MALL.m_ITEMS[ btCnt ].m_ITEM.m_bIsAppraisal = 1;
				}
				pCPacket->AppendData( &pUSER->m_MALL.m_ITEMS[ btCnt ].m_ITEM, sizeof( tagBaseITEM ) );
				pCPacket->AppendString( pFROM );

				if( ++btCnt >= MAX_MALL_ITEM_COUNT )
					break;

				if ( pFROM ) {
					pCPacket->AppendString( pDESC );
					pCPacket->AppendString( pTO );
				}
				if ( pCPacket->m_HEADER.m_nSize >= MAX_PACKET_SIZE - 140 ) {
					pCPacket->m_gsv_MALL_ITEM_REPLY.m_btCntOrIdx = btCnt;

					pUSER->Send_Start( pCPacket );
					Packet_ReleaseNUnlock( pCPacket );

					btCnt = 0;
					pCPacket = Packet_AllocNLock ();

					pCPacket->m_HEADER.m_wType = GSV_MALL_ITEM_REPLY;
					pCPacket->m_HEADER.m_nSize = sizeof(gsv_MALL_ITEM_REPLY);
					pCPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE = REPLY_MALL_ITEM_LIST_DATA;
				}
			}
			if ( btCnt ) {
				pCPacket->m_gsv_MALL_ITEM_REPLY.m_btCntOrIdx = btCnt;
				pUSER->Send_Start( pCPacket );
			}
			Packet_ReleaseNUnlock( pCPacket );

			// 목록끝...
			this->Send_gsv_MALL_ITEM_REPLAY( pUSER, REPLY_MALL_ITEM_LIST_END );

			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


