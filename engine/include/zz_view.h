/** 
 * @file zz_view.h
 * @brief view class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-feb-2002
 *
 * $Header: /engine/include/zz_view.h 3     04-10-02 3:09p Zho $
 * $History: zz_view.h $
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
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_VIEW_H__
#define __ZZ_VIEW_H__

#ifndef __ZZ_POINTER_H__
#include "zz_pointer.h"
#endif

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif


// display view base class
// will be inherited by those classes
// : zz_view_d3d, zz_view_ogl, zz_view_image...etc...
//class zz_scene;
class zz_view : public zz_node {
protected:
	bool is_active;

public:
	zz_view ();
	virtual ~zz_view () {};
    
	virtual bool attach_window (void) = 0;
	virtual bool detach_window (void) = 0;

	//virtual void main_loop (void) = 0;
	virtual bool begin_scene () = 0; // returns false if device is lost
	virtual bool end_scene () = 0; // returns false if not began already
	virtual void render () = 0;
	
	virtual void	set_handle (const void * handle_to_set) = 0;
	virtual void *	get_handle () = 0;
	virtual void	set_width (int width);
	virtual int		get_width (void);
	virtual void	set_height (int height);
	virtual int		get_height (void);
	virtual void	set_depth (int bpp);
	virtual int		get_depth (void);
	virtual void	set_fullscreen (bool true_or_false);
	virtual bool	get_fullscreen (void);
	virtual void	set_window_text (const char * title) = 0;

	bool set_active (bool true_false)
	{
		bool saved = is_active;
		is_active = true_false;
		return saved;
	}

	ZZ_DECLARE_DYNAMIC(zz_view)
};

#endif // __ZZ_VIEW_H__
