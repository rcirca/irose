/** 
 * @file zz_manager_font.cpp
 * @brief font manager class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    26-aug-2004
 *
 * $Header: /engine/src/zz_manager_font.cpp 1     04-08-31 4:46p Zho $
 */

#include "zz_tier0.h"
#include "zz_manager_font.h"
#include "zz_font.h"

ZZ_IMPLEMENT_DYNCREATE(zz_manager_font, zz_manager)

zz_manager_font::zz_manager_font ()
{
}

zz_manager_font::~zz_manager_font ()
{
}

// traverse all childe nodes and call prepare_font()
void zz_manager_font::prepare_font ()
{
	zz_hash_table<zz_node*>::iterator it, it_end;
	zz_hash_table<zz_node*> * nodes = get_hash_table();
	zz_font * font;

	if (!nodes) return;

	for (it = nodes->begin(), it_end = nodes->end(); it != it_end; ++it) {
		font = static_cast<zz_font*>(*it);
		font->prepare_font();
	}
}