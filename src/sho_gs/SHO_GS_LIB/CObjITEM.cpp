
#include "LIB_gsMAIN.h"
#include "OBJECT.h"
#include "GS_ThreadZONE.h"
#include "GS_ThreadLOG.h"

//-------------------------------------------------------------------------------------------------
/*
CObjITEM::CObjITEM ()
{
	m_ITEM.m_nItemNo = 0;
	m_ITEM.m_cType	 = 0;
}
*/
CObjITEM::~CObjITEM ()
{
//    LogString (0xffff, "CObjITEM::~CObjITEM :: ObjIDX: %d \n", this->Get_INDEX () );
}

//-------------------------------------------------------------------------------------------------
void CObjITEM::InitItemOBJ (classUSER *pDropper, tPOINTF &PosCUR, POINTS &PosSEC, tagITEM &sITEM,  classUSER *pOwner, bool bActive, CParty *pOwnerPARTY)
{
	m_PosSECTOR = PosSEC;
	m_PosCUR = PosCUR;
	m_ITEM   = sITEM;
	m_pOwnerPARTY = pOwnerPARTY;

	if ( pOwner ) {
		m_iOwnerObjIDX	= pOwner->Get_INDEX();
		m_dwOwnerDBID	= pOwner->m_dwDBID;
	} else {
		m_iOwnerObjIDX	= 0;
		m_dwOwnerDBID	= 0;
	}

	if ( pDropper ) {
		// 로그를 남길 만한 가치가 있는 아이템인가는 로그에서 판단..
		m_DropperACCOUNT.Set( pDropper->Get_ACCOUNT() );
		m_DropperNAME.Set( pDropper->Get_NAME() );
		m_DropperIP.Set( pDropper->Get_IP() );
		m_dwDropperDBID = pDropper->m_dwDBID;
		m_bDropperIsUSER  = true;
		m_dwDropperClanID = pDropper->GetClanID ();
	} else {
		m_bDropperIsUSER  = false;
		m_dwDropperClanID = 0;
	}

	if ( bActive )
		m_iRemainTIME = ITEM_OBJ_LIVE_TIME;
	else
		m_iRemainTIME = 0;		// 몹죽을때 드롭되는 경우 데미지 패킷과 같이 가도록...
}

//-------------------------------------------------------------------------------------------------
void CObjITEM::SetACTIVE ()
{
	if ( m_pOwnerPARTY ) {
		// 파티 소유 아이템일경우 소유권 길게...
		m_iRemainTIME = ITEM_OBJ_LIVE_TIME + ( ITEM_OBJ_LIVE_TIME-ITEM_OBJ_FREE_TIME );
	} else
		m_iRemainTIME = ITEM_OBJ_LIVE_TIME;
}

//-------------------------------------------------------------------------------------------------
void CObjITEM::Update_OwnerObjIDX ( classUSER *pUSER )
{
	// 이 패킷을 받을 넘이 오너냐 ???
	if ( pUSER->m_dwDBID == this->m_dwOwnerDBID ) {
		this->m_iOwnerObjIDX = pUSER->Get_INDEX();
	}
}

//-------------------------------------------------------------------------------------------------
bool CObjITEM::Make_gsv_ADD_OBJECT( classPACKET *pCPacket )
{
	if ( m_iRemainTIME <= 0 )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_ADD_FIELDITEM;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_ADD_FIELDITEM );

    pCPacket->m_gsv_ADD_FIELDITEM.m_wServerItemIDX = this->Get_INDEX ();
    pCPacket->m_gsv_ADD_FIELDITEM.m_ITEM		   = this->m_ITEM;
	pCPacket->m_gsv_ADD_FIELDITEM.m_PosCUR		   = this->m_PosCUR;
    pCPacket->m_gsv_ADD_FIELDITEM.m_wOwnerObjIDX   = this->m_iOwnerObjIDX;
    pCPacket->m_gsv_ADD_FIELDITEM.m_wRemainTIME    = m_iRemainTIME;

	return true;
}


//-------------------------------------------------------------------------------------------------
int	 CObjITEM::Proc (void)
{
	// 생성된후 일정 시간이 경과 되면 필드에서 삭제한다...
	m_iRemainTIME -= (this->GetZONE())->GetPassTIME ();
	if ( m_iRemainTIME < 0 ) {
		if ( m_bDropperIsUSER ) {
			g_pThreadLOG->When_DeletedITEM( this );
		}

		return 0;
	}

	return 1;
}

//-------------------------------------------------------------------------------------------------
