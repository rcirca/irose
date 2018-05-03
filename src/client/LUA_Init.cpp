/*
	$Header: /Client/LUA_Init.cpp 6     04-04-07 10:27a Jeddli $
*/
#include "stdAFX.h"
#include "Event\Quest_FUNC.h"
#include "Game_FUNC.h"
//-------------------------------------------------------------------------------------------------

#include "Game_FUNC_DEF.inc"
#include "Event\Quest_FUNC_DEF.inc"

// Global
void GF_Init (lua_State *L)
{
	#include "Game_FUNC_REG.inc"
}

// Quest
void QF_Init (lua_State *L)
{
	#include "Game_FUNC_REG.inc"
	#include "Event\Quest_FUNC_REG.inc"

	ZL_SETVAR( SV_SEX	);
	ZL_SETVAR( SV_BIRTH	);
	ZL_SETVAR( SV_CLASS	);
	ZL_SETVAR( SV_UNION	);
	ZL_SETVAR( SV_RANK	);
	ZL_SETVAR( SV_FAME	);
	ZL_SETVAR( SV_STR	);
	ZL_SETVAR( SV_DEX	);
	ZL_SETVAR( SV_INT	);
	ZL_SETVAR( SV_CON	);
	ZL_SETVAR( SV_CHA	);
	ZL_SETVAR( SV_SEN	);
	ZL_SETVAR( SV_EXP	);
	ZL_SETVAR( SV_LEVEL	);
	ZL_SETVAR( SV_POINT	);

/*
	ZL_SETVAR( BODY_PART_FACE		);
	ZL_SETVAR( BODY_PART_HAIR		);
	ZL_SETVAR( BODY_PART_HELMET		);
	ZL_SETVAR( BODY_PART_ARMOR		);
	ZL_SETVAR( BODY_PART_GAUNTLET	);
	ZL_SETVAR( BODY_PART_BOOTS		);
	ZL_SETVAR( BODY_PART_GOGGLE		);
	ZL_SETVAR( BODY_PART_KNAPSACK	);
	ZL_SETVAR( BODY_PART_WEAPON_R	);
	ZL_SETVAR( BODY_PART_WEAPON_L	);
*/

	ZL_SETVAR( ITEM_TYPE_FACE_ITEM	);
	ZL_SETVAR( ITEM_TYPE_HELMET		);
	ZL_SETVAR( ITEM_TYPE_ARMOR		);
	ZL_SETVAR( ITEM_TYPE_GAUNTLET	);
	ZL_SETVAR( ITEM_TYPE_BOOTS		);
	ZL_SETVAR( ITEM_TYPE_KNAPSACK	);
	ZL_SETVAR( ITEM_TYPE_JEWEL		);
	ZL_SETVAR( ITEM_TYPE_WEAPON		);
	ZL_SETVAR( ITEM_TYPE_SUBWPN		);
	ZL_SETVAR( ITEM_TYPE_USE		);
	ZL_SETVAR( ITEM_TYPE_ETC		);
	ZL_SETVAR( ITEM_TYPE_GEM		);
	ZL_SETVAR( ITEM_TYPE_NATURAL	);
/*
ITEM_TYPE_QUEST
ITEM_TYPE_SPECIAL
ITEM_TYPE_MONEY = 0x1f
*/

}


//-------------------------------------------------------------------------------------------------
