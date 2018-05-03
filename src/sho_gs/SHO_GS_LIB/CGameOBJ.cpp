
#include "LIB_gsMAIN.h"

#include "CGameOBJ.h"
#include "ZoneLIST.h"

//-------------------------------------------------------------------------------------------------
CGameOBJ::CGameOBJ ()
{
    m_ZoneNODE.DATA = this;
    m_SectorNODE.DATA = this;
	this->Init_GameOBJ ();
}
CGameOBJ::~CGameOBJ ()
{
	assert(0 == this->Get_INDEX() );
}

//-------------------------------------------------------------------------------------------------
void CGameOBJ::Init_GameOBJ ()
{
    m_iIndex = 0;					// CGameOBJ::Init_GameOBJ
    m_pZONE = NULL;
	m_pGroupSECTOR = NULL;
}

void CGameOBJ::ResetSECTOR (short nSectorSize)
{
    m_PosSECTOR.x = (short) ( m_PosCUR.x / nSectorSize );
    m_PosSECTOR.y = (short) ( m_PosCUR.y / nSectorSize );
}


//-------------------------------------------------------------------------------------------------
int CGameOBJ::GetCur_ZoneTIME (void)
{
	if ( m_pZONE ) {
		short nZoneNO = this->GetZONE()->Get_ZoneNO();
		return ( ::Get_WorldTIME() % ZONE_DAY_CYCLE( nZoneNO ) );
	}

	return 0;
}
int CGameOBJ::GetCur_WorldTIME (void)
{	
	return ::Get_WorldTIME();
}

//-------------------------------------------------------------------------------------------------
bool CGameOBJ::Make_gsv_SUB_OBJECT( classPACKET *pCPacket )
{
    if ( !Get_INDEX() ) {
		assert( 0 );
		return false;
	}

    pCPacket->m_HEADER.m_wType = GSV_SUB_OBJECT;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_SUB_OBJECT );
    
    pCPacket->m_gsv_SUB_OBJECT.m_wObjectIDX[ 0 ] = this->Get_INDEX();

    return true;
}

//-------------------------------------------------------------------------------------------------
