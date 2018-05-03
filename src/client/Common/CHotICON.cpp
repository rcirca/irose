#include "stdAFX.h"
#include "CHotICON.h"
#ifndef	__SERVER
#include "..\\Object.h"
#endif

//-------------------------------------------------------------------------------------------------
void CHotICONS::Init ()
{
	::ZeroMemory( m_IconLIST, MAX_HOT_ICONS*sizeof(tagHotICON) );
}

//-------------------------------------------------------------------------------------------------
bool CHotICONS::RegHotICON (BYTE btListIDX, tagHotICON sHotICON)
{
	if ( btListIDX >= MAX_HOT_ICONS )
		return false;

	m_IconLIST[ btListIDX ] = sHotICON;
	return true;
}

//-------------------------------------------------------------------------------------------------
void CHotICONS::DelHotICON (BYTE btListIDX)
{
	if ( btListIDX < MAX_HOT_ICONS )
		m_IconLIST[ btListIDX ].m_wHotICON = 0;
}

//-------------------------------------------------------------------------------------------------
#ifndef	__SERVER
/// 만약 핫아이콘이 참조하는 아이템이 사라졌을경우..
void CHotICONS::UpdateHotICON()
{
	for ( int i = 0; i < MAX_HOT_ICONS ; i++ )
	{
		switch( m_IconLIST[ i ].m_cType )
		{
			case INV_ICON:
				{
					if( g_pAVATAR )
					{
						if( m_IconLIST[ i ].m_nSlotNo < 0 || m_IconLIST[ i ].m_nSlotNo >= INVENTORY_TOTAL_SIZE )
						{
							assert( 0 && " UpdateHotICON has invalid slot number" );
							break;
						}

						/// 참조하는 아이템이 무효하면 지워라..
						if( g_pAVATAR->m_Inventory.m_ItemLIST[ m_IconLIST[ i ].m_nSlotNo ].m_cType == 0 )
						{
							DelHotICON( i );
						}
					}else
					{
						assert( 0 && " CHotICONS::UpdateHotICON has invalid pAvatar" );
					}
				}
				break;
		}		
	}
}
#endif

//-------------------------------------------------------------------------------------------------
