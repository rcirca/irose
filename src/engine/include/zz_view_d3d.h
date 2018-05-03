/** 
 * @file zz_view_d3d.h
 * @brief Direct3D View class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-feb-2002
 *
 * $Header: /engine/include/zz_view_d3d.h 3     04-10-02 3:09p Zho $
 * $History: zz_view_d3d.h $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-10-02   Time: 3:09p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-04-26   Time: 7:39p
 * Updated in $/engine/include
 * 4.26.17½Ã 39ºÐ
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-27   Time: 4:50p
 * Updated in $/znzin11/engine/include
 * - removed zz_resource class, added full-support of resource management
 * into zz_node, and cleaned up some codes.
 */

#ifndef __ZZ_VIEW_D3D_H__
#define __ZZ_VIEW_D3D_H__

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
//#include <time.h>
//#include <ZMouse.h>
//#include <d3d9.h>

#ifndef __ZZ_VIEW_H__
#include "zz_view.h"
#endif

#ifndef __ZZ_POINTER_H__
#include "zz_pointer.h"
#endif

#ifndef __ZZ_RENDERER_D3D_H__
#include "zz_renderer_d3d.h"
#endif


#define ZZ_DEFAULT_INIT_SCRIPT "init.lua"
#define ZZ_DEFAULT_LOAD_SCRIPT "load.lua"

class zz_renderer;

//--------------------------------------------------------------------------------
class zz_view_d3d : public zz_view {
protected:
	//zz_pointer<zz_renderer_d3d> renderer;
	HWND window_handle;
	int restore_window_mode;
	bool scene_began; // same as renderer_d3d's
	
	//UINT uMSH_MOUSEWHEEL = 0;   // Value returned from RegisterWindowMessage()
	//bool install_wheel ();

public:
	zz_view_d3d ();
	~zz_view_d3d ();

	// TODO: client area dismatch screen size
	//virtual int get_width (void);
	//virtual int get_height (void);

	virtual bool attach_window (void);
	virtual bool detach_window (void);

	void render ();

	bool begin_scene ();
	bool end_scene ();

	void set_handle (const void * handle_to_set);
	void * get_handle ();

	HWND get_hwnd ();

	void set_window_text (const char * title);
	ZZ_DECLARE_DYNAMIC(zz_view_d3d)
};

#endif // __ZZ_VIEW_D3D_H__
