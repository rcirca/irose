/** 
 * @file zz_view_d3d.cpp
 * @brief Direct3D View class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-feb-2002
 *
 * $Header: /engine/src/zz_view_d3d.cpp 27    05-01-10 2:56p Choo0219 $
 * $History: zz_view_d3d.cpp $
 * 
 * *****************  Version 27  *****************
 * User: Choo0219     Date: 05-01-10   Time: 2:56p
 * Updated in $/engine/src
 * 
 * *****************  Version 26  *****************
 * User: Choo0219     Date: 04-11-07   Time: 5:56p
 * Updated in $/engine/src
 * 
 * *****************  Version 25  *****************
 * User: Zho          Date: 04-10-03   Time: 5:22p
 * Updated in $/engine/src
 * 
 * *****************  Version 24  *****************
 * User: Zho          Date: 04-10-02   Time: 3:09p
 * Updated in $/engine/src
 * 
 * *****************  Version 23  *****************
 * User: Zho          Date: 04-10-01   Time: 1:43p
 * Updated in $/engine/src
 * 
 * *****************  Version 22  *****************
 * User: Zho          Date: 04-09-21   Time: 2:05p
 * Updated in $/engine/src
 * 
 * *****************  Version 21  *****************
 * User: Zho          Date: 04-09-21   Time: 11:20a
 * Updated in $/engine/src
 * 
 * *****************  Version 20  *****************
 * User: Zho          Date: 04-09-21   Time: 10:46a
 * Updated in $/engine/src
 * 
 * *****************  Version 19  *****************
 * User: Zho          Date: 04-09-13   Time: 7:01p
 * Updated in $/engine/src
 * 
 * *****************  Version 18  *****************
 * User: Zho          Date: 04-09-07   Time: 7:21p
 * Updated in $/engine/src
 * 
 * *****************  Version 17  *****************
 * User: Zho          Date: 04-08-31   Time: 4:46p
 * Updated in $/engine/src
 * 
 * *****************  Version 16  *****************
 * User: Zho          Date: 04-08-16   Time: 4:20p
 * Updated in $/engine/src
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 04-07-19   Time: 12:15p
 * Updated in $/engine/src
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 04-06-29   Time: 10:09a
 * Updated in $/engine/src
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 04-06-25   Time: 3:03a
 * Updated in $/engine/src
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-06-11   Time: 3:16p
 * Updated in $/engine/src
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-06-08   Time: 10:35a
 * Updated in $/engine/src
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-06-03   Time: 11:18a
 * Updated in $/engine/src
 * glow code updated.
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-05-19   Time: 8:15p
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-04-28   Time: 6:32p
 * Updated in $/engine/src
 * device restore things
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-04-26   Time: 7:39p
 * Updated in $/engine/src
 * 4.26.17½Ã 39ºÐ
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-04-25   Time: 1:17p
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-04-22   Time: 12:02p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-04-22   Time: 11:57a
 * Updated in $/engine/src
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-21   Time: 12:07p
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
 * *****************  Version 16  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 03-11-27   Time: 4:50p
 * Updated in $/znzin11/engine/source
 * - removed zz_resource class, added full-support of resource management
 * into zz_node, and cleaned up some codes.
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_font.h"
#include "zz_view_d3d.h"
#include "zz_log.h"
#include "zz_node_type.h"
#include "zz_system.h"
#include "zz_profiler.h"
#include "zz_manager_font.h"
#include "zz_error.h"
#include <string.h> // for memcpy

#include "zz_interface.h"  //test

HWND g_hWnd = NULL; // global window handle

ZZ_IMPLEMENT_DYNCREATE(zz_view_d3d, zz_view)

zz_view_d3d::zz_view_d3d () : zz_view(), window_handle(NULL), restore_window_mode(0), scene_began(false)
{
	//ZZ_LOG("view_d3d: zz_view_d3d created\n");
}

zz_view_d3d::~zz_view_d3d ()
{
	//ZZ_LOG("view_d3d: zz_view_d3d destroyed\n");
}

void zz_view_d3d::set_handle (const void * handle_to_set)
{
	window_handle = ((HWND)handle_to_set);
	g_hWnd = window_handle;
}

void * zz_view_d3d::get_handle ()
{
	return window_handle;
}

HWND zz_view_d3d::get_hwnd ()
{
	return window_handle;
}

bool zz_view_d3d::attach_window ()
{
	znzin->renderer->set_view(znzin->view);

	try {
		if (!znzin->renderer->initialize()) {
			return false;
		}
	}
	catch (const char * log) {
		ZZ_LOG(log);
		ZZ_LOG("view_d3d: attach_window() failed. iNiTfAiL\n");
		strcat(znzin->sysinfo.video_string," <Window Create failed>");
		zz_msgboxf(zz_error::get_text(zz_error::MSG_ERROR), zz_error::get_text(zz_error::MSG_EXIT_D3DERROR), znzin->sysinfo.video_string);
		exit(EXIT_FAILURE);
	}
	
	znzin->timer_start();
	return true;
}

bool zz_view_d3d::detach_window ()
{
	return true;
}

void zz_view_d3d::set_window_text (const char * text)
{
	if (this->get_fullscreen()) return;
	SetWindowText(this->window_handle, text);
}


void zz_view_d3d::render (void)
{
	if (!scene_began)
		return;

	if (!znzin)
		return;

	if (!znzin->renderer)
		return;

	znzin->scene.render();

	Sleep(znzin->renderer->get_state()->time_delay); // waiting until swap complete
}

bool zz_view_d3d::begin_scene ()
{
	assert(!scene_began);

	
	if (!znzin->scene.is_valid()) {
		znzin->scene.update(zz_time(0));
	}

	if (znzin->renderer->reset_device()) { // if device_lost
		scene_began = false;
		return false; // device lost
	}

	znzin->renderer->init_render_state();

	znzin->fonts->prepare_font();

	znzin->scene.before_render();

	znzin->renderer->init_textures(); // ?

	((zz_scene_octree*)&znzin->scene)->render_shadowmap(); // includes begin_scene()/end_scene()

	znzin->renderer->init_scene();
	znzin->renderer->init_render_state();
	znzin->renderer->begin_scene(ZZ_RW_SCENE);
	scene_began = true;

	return true;
}

bool zz_view_d3d::end_scene ()
{
	if (!scene_began)
		return false;

	scene_began = false;

	znzin->renderer->end_scene();

	if (znzin->get_rs()->draw_shadowmap_viewport) {
		((zz_scene_octree*)&znzin->scene)->render_shadowmap_viewport();
	}
	znzin->scene.after_render();

	return true;
}

//int zz_view_d3d::get_height (void)
//{
//	int screen_height;
//	RECT screen_rect;
//	GetClientRect(window_handle, &screen_rect);
//	screen_height = screen_rect.bottom - screen_rect.top;
//	return screen_height;
//}
//
//int zz_view_d3d::get_width (void)
//{
//	int screen_width;
//	RECT screen_rect;
//	GetClientRect(window_handle, &screen_rect);
//	screen_width = screen_rect.right - screen_rect.left;
//	return screen_width;
//}

//bool zz_view_d3d::install_wheel (void)
//{
//	// mouse wheel setting
//	uMSH_MOUSEWHEEL = RegisterWindowMessage(MSH_MOUSEWHEEL);
//	if (!uMSH_MOUSEWHEEL) {
//		MessageBox(NULL,"RegisterWindowMessag Failed!",	"Error",MB_OK);
//		return 1;
//	}
//}


//int APIENTRY WinMain (HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
//{
//	// new system
//	assert(!znzin);
//	znzin = zz_new zz_system;
//	((zz_view_d3d *)znzin->view)->set_instance_handle(hThisInst);
//
//	// invoke client main
//	znzin_main();
//	
//	assert(znzin);
//	zz_delete znzin;
//	return 0;
//}

