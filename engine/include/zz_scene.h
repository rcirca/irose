/** 
 * @file zz_scene.h
 * @brief scene interface.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    23-feb-2002
 *
 *$Header: /engine/include/zz_scene.h 13    06-12-29 4:23a Choo0219 $
 * $History: zz_scene.h $
 * 
 * *****************  Version 13  *****************
 * User: Choo0219     Date: 06-12-29   Time: 4:23a
 * Updated in $/engine/include
 * 
 * *****************  Version 12  *****************
 * User: Choo0219     Date: 05-07-11   Time: 10:03a
 * Updated in $/engine/include
 * 
 * *****************  Version 11  *****************
 * User: Choo0219     Date: 05-02-14   Time: 3:19p
 * Updated in $/engine/include
 * 
 * *****************  Version 10  *****************
 * User: Choo0219     Date: 04-12-15   Time: 10:08a
 * Updated in $/engine/include
 * 
 * *****************  Version 9  *****************
 * User: Choo0219     Date: 04-12-12   Time: 6:56p
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Choo0219     Date: 04-12-07   Time: 10:01p
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-08-16   Time: 4:20p
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-07-26   Time: 3:06p
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-06-08   Time: 10:35a
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-05-31   Time: 5:34p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-24   Time: 1:07p
 * Updated in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
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
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_SCENE_H__
#define __ZZ_SCENE_H__

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif


class zz_visible;
struct zz_bounding_obb;
// scene interface
//--------------------------------------------------------------------------------
class zz_scene : public zz_node {
public:
	zz_scene () {};
	~zz_scene () {};
	
	virtual void render () = 0;
	virtual void clear () = 0;
	virtual void build () = 0;
	virtual void insert (zz_visible * vis) = 0;
	virtual void remove (zz_visible * vis) = 0;
	virtual void refresh (zz_visible * vis) = 0;
	virtual void update (zz_time diff_time) = 0;
	virtual void update_object_sort () = 0;
	virtual void update_init_list () = 0;
	virtual void cull () = 0;
	virtual void invalidate () = 0;
	virtual bool is_valid () = 0;
	virtual void increase_num_glows () = 0;
	virtual int get_num_glows () = 0;
	virtual void before_render () = 0;
	virtual void after_render () = 0;
	virtual void input_scene_sphere(float x, float y, float z, float r) = 0;
	virtual void draw_scene_sphere() = 0;
    virtual void input_scene_obb(const zz_bounding_obb *obb) = 0;
	virtual void draw_scene_obb() = 0;
	virtual void reset_scene_obb() = 0;
    virtual void reset_scene_sphere() = 0;
    virtual void input_scene_aabb(float min[3], float max[3], DWORD color) = 0;
	virtual void draw_scene_aabb() = 0;
    virtual void reset_scene_aabb() = 0;
    virtual void input_scene_cylinder(float x, float y, float z, float length, float r) = 0;
	virtual void draw_scene_cylinder() = 0;
    virtual void reset_scene_cylinder() = 0;
    virtual void input_scene_line(vec3& vec1, vec3& vec2) = 0;
	virtual void draw_scene_line() = 0;
    virtual void reset_scene_line() = 0;
    virtual void input_scene_obb2(vec3& center, quat& rotation, float xlength, float ylength, float zlength) = 0;
	virtual void draw_scene_obb2() = 0;
	virtual void reset_scene_obb2() = 0;
    virtual void input_scene_axis(zz_visible *vis,float size) = 0;
	virtual void draw_scene_axis() = 0;
	virtual void reset_scene_axis() = 0;

	// get the sorted list by distance between the viewer position and the visible position
	// list_out        : the new visible list 
	// viewer_position : the position of the current camera
	//void sort_by_distance (zz_list<zz_visible *>& list_out, const vec3& viewer_position_in);

	ZZ_DECLARE_DYNAMIC(zz_scene)
};

#endif // __ZZ_SCENE_H__