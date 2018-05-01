/** 
 * @file zz_view.cpp
 * @brief view class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    23-feb-2002
 *
 * $Header: /engine/src/zz_view.cpp 3     04-06-29 10:09a Zho $
 * $History: zz_view.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-06-29   Time: 10:09a
 * Updated in $/engine/src
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:49p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 * $History: zz_view.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-06-29   Time: 10:09a
 * Updated in $/engine/src
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:49p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_font.h"
#include "zz_view.h"
#include "zz_system.h"
#include "zz_render_state.h"

ZZ_IMPLEMENT_DYNAMIC(zz_view, zz_node)

zz_view::zz_view () : zz_node(), is_active(false)
{
}

void zz_view::set_width (int width)
{
	assert(znzin);
	znzin->get_rs()->screen_width = width;
}

int zz_view::get_width (void)
{
	assert(znzin);
	return znzin->get_rs()->screen_width;
}

void zz_view::set_height (int height)
{
	assert(znzin);
	znzin->get_rs()->screen_height = height;
}

int zz_view::get_height (void)
{
	assert(znzin);
	return znzin->get_rs()->screen_height;
}

void zz_view::set_depth (int bpp)
{
	assert(znzin);
	znzin->get_rs()->screen_depth = bpp;
}

int zz_view::get_depth (void)
{
	assert(znzin);
	return znzin->get_rs()->screen_depth;
}

void zz_view::set_fullscreen (bool true_or_false)
{
	assert(znzin);
	znzin->get_rs()->use_fullscreen = true_or_false;
}

bool zz_view::get_fullscreen (void)
{
	assert(znzin);
	return znzin->get_rs()->use_fullscreen;
}