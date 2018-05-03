/** 
 * @file zz_ocean_block.cpp
 * @brief ocean block class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    30-jun-2003
 *
 * $Header: /engine/src/zz_ocean_block.cpp 12    04-08-31 4:46p Zho $
 * $History: zz_ocean_block.cpp $
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-08-31   Time: 4:46p
 * Updated in $/engine/src
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-07-05   Time: 9:38p
 * Updated in $/engine/src
 * vertex buffer redesigned
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-06-23   Time: 9:33a
 * Updated in $/engine/src
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-06-22   Time: 9:51a
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-06-08   Time: 10:35a
 * Updated in $/engine/src
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-05-01   Time: 3:42p
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-04-29   Time: 6:04p
 * Updated in $/engine/src
 * flush_device( delayed) added.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-04-28   Time: 2:03p
 * Updated in $/engine/src
 * ocean vs. camera test more precisely
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-13   Time: 5:22p
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
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_material_ocean.h"
#include "zz_ocean_block.h"
#include "zz_mesh.h"
#include "zz_material.h"
#include "zz_light.h"
#include "zz_shader.h"
#include "zz_renderer.h"
#include "zz_system.h"
#include "zz_mesh_ocean.h"

ZZ_IMPLEMENT_DYNCREATE(zz_ocean_block, zz_visible)

zz_ocean_block::zz_ocean_block()
{
	cast_shadow =false;
	receive_shadow = false;
	receive_fog = true;
	bv_type = ZZ_BV_OBB;
	collision_level = ZZ_CL_OBB; // set_collision_level(ZZ_CL_NONE);
	draw_priority = ZZ_DP_LOW;
	time_interval = 0;
	runits.reserve(1);
}

zz_ocean_block::~zz_ocean_block ()
{
}

void zz_ocean_block::set_interval (zz_time interval)
{
	time_interval = interval;
}

zz_time zz_ocean_block::get_interval (void)
{
	return time_interval;
}

void zz_ocean_block::update_animation (bool recursive, zz_time diff_time)
{
	zz_material_ocean * ocean_mat = reinterpret_cast<zz_material_ocean*>(runits[0].material);
	assert(ocean_mat);
	assert(ZZ_RUNTIME_TYPE(zz_material_ocean) == ocean_mat->get_node_type());
	if (time_interval > 0) {
		int texture_index = znzin->get_current_time() / time_interval;
		texture_index = texture_index % ocean_mat->get_num_textures();
		ocean_mat->set_current_texture_index(texture_index);
	}
	// no need to call visible::update_animation()
	// because ocean_block do not have child visibles
}

// same as terrain_block
void zz_ocean_block::render_runit (unsigned int runit_index)
{	
	assert(runit_index < num_runits);

	// light can be NULL
	assert(is_visible());

	zz_runit& ru = runits[runit_index];
	zz_mesh * mesh = ru.mesh;
	zz_material * mat = ru.material;
	const zz_shader * shader = (mat) ? mat->get_shader() : NULL;
	zz_light * light = ru.light;

	assert(mesh && mat && shader);

	// set modelviewTM from modelview_worldTM
	mat4 modelview_worldTM;
	get_modelview_worldTM(modelview_worldTM);
	znzin->renderer->set_modelview_matrix(modelview_worldTM);
	znzin->renderer->set_world_matrix(get_worldTM());

	if (!mat->get_device_updated())
		return;

	if (!mesh->get_device_updated())
		return;

	znzin->renderer->render(mesh, mat, light);
}

// same as terrain_block
void zz_ocean_block::before_render ()
{
	zz_runit& ru = runits[0];
	zz_mesh * mesh = ru.mesh;
	zz_material * mat = ru.material;

	zz_renderer * r = znzin->renderer;

	apply_lod(mesh, mat); // apply_lod must precede flush_device()

	mat->flush_device(true /* immediate */);

	if (!mesh->get_device_updated()) {
		mesh->flush_device(true /* immediate */);
	}
}

void zz_ocean_block::after_render ()
{
}
