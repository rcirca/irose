/*
	$Header: /7HeartsOnline/Server/SHO_GS/Sho_gs_lib/CObjEVENT.cpp 8     05-01-17 4:47p Icarus $
	$History: CObjEVENT.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Icarus       Date: 05-01-17   Time: 4:47p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib
 * 
 * *****************  Version 7  *****************
 * User: Icarus       Date: 04-11-15   Time: 11:24a
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib
 * 
 * *****************  Version 6  *****************
 * User: Icarus       Date: 04-10-07   Time: 8:00p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib
 * 
 * *****************  Version 5  *****************
 * User: Icarus       Date: 04-05-13   Time: 5:31p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib
 * 
 * *****************  Version 4  *****************
 * User: Icarus       Date: 04-05-03   Time: 6:46p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib
 * 
 * *****************  Version 3  *****************
 * User: Icarus       Date: 04-04-01   Time: 5:43p
 * Updated in $/7HeartsOnline/Server/SHO_GS/SHO_GS_LIB
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-03-31   Time: 3:10p
 * Updated in $/7HeartsOnline/Server/SHO_GS/SHO_GS_LIB
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:20a
 * Created in $/7HeartsOnline/Server/Sho_gs/Sho_gs_lib
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:47a
 * Created in $/SevenHearts/Server/Sho_gs/Sho_gs_lib
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:35a
 * Created in $/SHO/Server/Sho_gs/Sho_gs_lib
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:26a
 * Created in $/SevenHearts/Server/SHO_GS_LIB/Server/Sho_gs/Sho_gs_lib
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-03-24   Time: 9:43p
 * Updated in $/SevenHearts/Server/SHO_GS_LIB
*/
#include "LIB_gsMAIN.h"
#include "OBJECT.h"
#include "GS_ThreadZONE.h"
#include "IO_Quest.h"

extern t_HASHKEY Make_EventObjectID(int iZoneNO, int iMapX, int iMapY, int iEventID);


//-------------------------------------------------------------------------------------------------
CObjEVENT::CObjEVENT ()
{
}
CObjEVENT::~CObjEVENT ()
{
}

short CObjEVENT::VGetCur_ZoneNO()
{
	return this->GetZONE()->Get_ZoneNO();
}

//-------------------------------------------------------------------------------------------------
bool CObjEVENT::Init (CZoneTHREAD *pZONE, char cMapX, char cMapY, float fWorldX, float fWorldY, WORD wMapEventID)
{
	// TODO:: mapx, mapy로 존 트리거 데이타 검색해서 없으면 스킵...
	m_PosCUR.x = fWorldX;
	m_PosCUR.y = fWorldY;

	m_cMapX = cMapX;
	m_cMapY = cMapY;
	m_wMapEventID = wMapEventID;
	m_nEventSTATUS = 0;

	m_HashID = ::Make_EventObjectID( pZONE->Get_ZoneNO(), cMapX, cMapY, wMapEventID );

	return pZONE->Add_OBJECT( this );		// 존에 추가..
}

//-------------------------------------------------------------------------------------------------
bool CObjEVENT::Send_gsv_SET_EVENT_STATUS ()
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType	= GSV_SET_EVENT_STATUS;
	pCPacket->m_HEADER.m_nSize	= sizeof( gsv_SET_EVENT_STATUS );
	pCPacket->m_gsv_SET_EVENT_STATUS.m_wObjectIDX	= this->Get_INDEX ();
	pCPacket->m_gsv_SET_EVENT_STATUS.m_nEventSTATUS	= this->m_nEventSTATUS;

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjEVENT::Make_gsv_ADD_OBJECT( classPACKET *pCPacket )
{
    pCPacket->m_HEADER.m_wType = GSV_ADD_EVENTOBJ;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_ADD_EVENTOBJ );

    pCPacket->m_gsv_ADD_EVENTOBJ.m_wObjectIDX	= this->Get_INDEX ();
    pCPacket->m_gsv_ADD_EVENTOBJ.m_cMapX		= this->m_cMapX;
	pCPacket->m_gsv_ADD_EVENTOBJ.m_cMapY		= this->m_cMapY;
    pCPacket->m_gsv_ADD_EVENTOBJ.m_wMapEventID	= this->m_wMapEventID;
    pCPacket->m_gsv_ADD_EVENTOBJ.m_nEventSTATUS	= this->m_nEventSTATUS;

	return true;
}

//-------------------------------------------------------------------------------------------------
int	 CObjEVENT::Proc (void)
{
	if ( CObjVAR::ProcVAR( (this->GetZONE())->GetPassTIME() )  ) {
		// 다음 이벤트 처리...
		// m_HashCurrentTrigger = m_HashNextTrigger;
		g_QuestList.CheckQUEST( NULL, m_HashNextTrigger, true, 0, this );
		m_HashNextTrigger = 0;
	}

	return 1;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
