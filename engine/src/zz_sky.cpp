/** 
 * @file zz_sky.cpp
 * @brief sky class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-apr-2003
 *
 * $Header: /engine/src/zz_sky.cpp 11    04-08-31 4:46p Zho $
 * $History: zz_sky.cpp $
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-08-31   Time: 4:46p
 * Updated in $/engine/src
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-07-23   Time: 3:29a
 * Updated in $/engine/src
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-06-24   Time: 2:12p
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-06-23   Time: 9:33a
 * Updated in $/engine/src
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-06-22   Time: 9:51a
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-05-31   Time: 5:34p
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-05-19   Time: 8:15p
 * Updated in $/engine/src
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-18   Time: 12:58p
 * Updated in $/engine/src
 * loose camera collision
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
 * User: Zho          Date: 03-11-30   Time: 7:48p
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
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_sky.h"
#include "zz_system.h"
#include "zz_manager.h"
#include "zz_camera.h"
#include "zz_material.h"

ZZ_IMPLEMENT_DYNCREATE(zz_sky, zz_visible)

zz_sky::zz_sky() : rot_angles_delta(10), rot_angles_current(10)
{
	delayed_load = false;
	receive_fog = false;
	receive_fog_now = false;
	inscene = true;
	bv_type = ZZ_BV_OBB;
	collision_level = ZZ_CL_POLYGON;
	ztest = false;
	//glow_color = vec3(.3f, .3f, .3f);

	runits.reserve(1);
}

void zz_sky::update_time (bool recursive, zz_time diff_time)
{
	float time_delta = static_cast<float>(diff_time)/ZZ_TICK_PER_SEC;
	
	// rotate angles
	for (unsigned int i = 0; i < num_runits; ++i) {
		rot_angles_current[i] += time_delta*rot_angles_delta[i];
	}

	zz_camera * cam = znzin->get_camera();
	
	if (cam) {
		set_position(cam->get_eye());
		invalidate_transform();
		//ZZ_LOG("sky: update() position(%f, %f, %f)\n", this->get_position().x, this->get_position().y, this->get_position().z);
	}

	// CAUTION: sky is not in the scene_octree, so you should update manually.
	invalidate_tm_minmax();
}

bool zz_sky::set_rotation_deltas (int index, float angle_degree_per_second)
{
	if (index > (int)rot_angles_delta.size()) { // index cannot be greater than size()+1
		return false;
	}
	if (index == rot_angles_delta.size()) {
		rot_angles_delta.push_back(angle_degree_per_second);
	}
	else {
		rot_angles_delta[index] = angle_degree_per_second;
	}
	return true;
}

void zz_sky::render (bool recursive)
{
	assert(is_visible());
	
	// save rotation
	quat rot = this->get_rotation();
	// rotation axis (z-axis)
	const vec3 axis(0, 0, 1);

	// render the first object only
	rotate_by_axis(this->rot_angles_current[0]*ZZ_TO_RAD, axis);

	invalidate_transform();

	// TODO: extract this code out of render() call
	zz_material * mat = runits[0].material;
	if (mat) {
		mat->set_alpha_test(true);
		mat->set_receive_fog(receive_fog);			
		mat->set_ztest(ztest);
		mat->set_zwrite(false);
	}
	// render unit
	render_runit(0);
	// restore rotation
	set_rotation(rot);
	invalidate_transform();
}


void zz_sky::before_render ()
{
	zz_material * mat;

	zz_runit& ru = runits[0];

	mat = ru.material;

	mat->flush_device(true); // does not render immediately
}

void zz_sky::after_render ()
{
}
