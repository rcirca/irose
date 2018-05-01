/** 
 * @file zz_morpher.cpp
 * @brief mesh morpher class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    08-oct-2002
 *
 * $Header: /engine/src/zz_morpher.cpp 17    05-12-24 10:23p Choo0219 $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_morpher.h"
#include "zz_mesh.h"
#include "zz_motion.h"
#include "zz_system.h"
#include "zz_renderer.h"
#include "zz_profiler.h"
#include "zz_material.h"

ZZ_IMPLEMENT_DYNCREATE(zz_morpher, zz_animatable);

zz_morpher::zz_morpher ()
{
	runits.reserve(1);
	bv_type = ZZ_BV_OBB;
	collision_level = ZZ_CL_NONE;
	receive_fog = false;
	receive_shadow = false;
	cast_shadow = false	;
}

bool zz_morpher::apply_motion (void)
{
	//ZZ_PROFILER_INSTALL(PMORPHER_APPLYMOTION);

	if (!motion) return false;

	assert(num_runits == 1);
	zz_mesh * mesh = runits[0].mesh;
	
	if (mesh->get_timestamp() == znzin->get_current_time()) {
		
		if(!forced_visibility_mode)
			set_visibility(mesh->alpha);  // 조성현 2005 11 - 12 effect 숨기기
		return false; // mesh was already updated
	   
	}

	if (!mesh->get_device_updated()) {
		mesh->flush_device(true);
	}

	int num_verts = (int)mesh->get_num_verts();
	int current_frame = motion_controller.get_current_frame();	
	float alpha;
	//
	//ZZ_LOG("morpher: [%s]->apply_motion(%s, %d) num_channels(%d), num_verts(%d)\n", mesh->get_path(),
	//	motion->get_path(), current_frame, motion->get_num_channels(), num_verts);

	if (motion->update_mesh(mesh, current_frame, num_verts, alpha)) {
		if(!forced_visibility_mode)
		set_visibility(alpha);
	}
	
	
	// update mesh animation
	zz_assert(mesh->get_dynamic());
	
	mesh->update_vertex_buffer();
	
	mesh->set_timestamp(znzin->get_current_time());

	return false; // not to invalidate transform
}

// added for debugging
void zz_morpher::render (bool recursive)
{
	zz_animatable::render(recursive);
}

// added for debugging
void zz_morpher::update_animation (bool recursive, zz_time diff_time)
{
	//ZZ_LOG("morpher:update_animation(%s, %x, %s, %d)\n", get_name(), get_mesh(0), get_mesh(0)->get_path(), get_mesh(0)->get_num_faces());
	zz_animatable::update_animation(recursive, diff_time);
}
