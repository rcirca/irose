#ifndef	__CRIDEITEM_H
#define	__CRIDEITEM_H
#include "IO_STB.h"
//-------------------------------------------------------------------------------------------------

#define	RIDE_ITEM_PART_IDX(I)				g_RideITEM.GetValue(I,  2)
#define	RIDE_ITEM_USE_RIGHT(I)				g_RideITEM.GetValue(I,  3)

/*
	4~15번 컬럼은 공통 아이템에서...
#define	RIDE_ITEM_TYPE(I)					g_RideITEM.GetValue(I,  4)

#define	RIDE_ITEM_BASE_PRICE(I)				g_RideITEM.GetValue(I,  5)
#define	RIDE_ITEM_PRICE_RATE(I)				g_RideITEM.GetValue(I,  6)
#define	RIDE_ITEM_WEIGHT(I)					g_RideITEM.GetValue(I,  7)
#define	RIDE_ITEM_QUALITY(I)				g_RideITEM.GetValue(I,  8)

#define	RIDE_ITEM_ICON_NO(I)				g_RideITEM.GetValue(I,  9)
#define	RIDE_ITEM_FIELD_IMG(I)				g_RideITEM.GetValue(I, 10)
#define	RIDE_ITEM_EQUIP_SOUND(I)			g_RideITEM.GetValue(I, 11)
*/

#define	RIDE_ITEM_PART_TYPE(I)				g_RideITEM.GetValue(I, 16)
#define	RIDE_ITEM_PART_VER(I)				g_RideITEM.GetValue(I, 17)

#define	RIDE_ITEM_NEED_SKILL_IDX(I)			g_RideITEM.GetValue(I, 19)
#define	RIDE_ITEM_NEED_SKILL_LEV(I)			g_RideITEM.GetValue(I, 20)

#define	RIDE_ITEM_NEED_ABILITY_IDX(I)		g_RideITEM.GetValue(I, 21)
#define	RIDE_ITEM_NEED_ABILITY_VALUE(I)		g_RideITEM.GetValue(I, 22)

#define	RIDE_ITEM_MAX_FUEL(I)				g_RideITEM.GetValue(I, 31)
#define RIDE_ITEM_USE_FUEL_RATE(I)			g_RideITEM.GetValue(I, 32)

#define	RIDE_ITEM_MOV_SPD(I)				g_RideITEM.GetValue(I, 33)

#define RIDE_ITEM_ATK_RANGE(I)				g_RideITEM.GetValue(I, 35)
#define	RIDE_ITEM_ATK_POW(I)				g_RideITEM.GetValue(I, 36)
#define	RIDE_ITEM_ATK_SPD(I)				g_RideITEM.GetValue(I, 37)

#define	RIDE_ITEM_SEAT_CNT(I)				g_RideITEM.GetValue(I, 38)		// 추가 좌석수

class CRideITEM
{
private:

public :
	STBDATA		m_ItemDATA;

	CRideITEM ();
	~CRideITEM ();

	bool  LoadRideITEM(const char *szFileName);
	void  Free ();

	short GetValue(WORD wLine, WORD wCol)
	{
		_ASSERT( wLine < m_ItemDATA.m_nDataCnt );

		return m_ItemDATA.m_ppDATA[ wLine ][ wCol ];
	}
} ;
extern CRideITEM g_RideITEM;

//-------------------------------------------------------------------------------------------------
#endif
