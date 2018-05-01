/** 
 * @file zz_bone.cpp
 * @brief bone class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-jun-2002
 *
 * $Header: /engine/src/zz_bone.cpp 16    05-08-25 11:38a Choo0219 $
 * $History: zz_bone.cpp $
 * 
 * *****************  Version 16  *****************
 * User: Choo0219     Date: 05-08-25   Time: 11:38a
 * Updated in $/engine/src
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 04-07-27   Time: 3:43p
 * Updated in $/engine/src
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 04-07-16   Time: 7:53p
 * Updated in $/engine/src
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 04-06-30   Time: 6:57p
 * Updated in $/engine/src
 * 6.30.4498
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-06-29   Time: 10:09a
 * Updated in $/engine/src
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-06-25   Time: 3:03a
 * Updated in $/engine/src
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-06-22   Time: 9:51a
 * Updated in $/engine/src
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-06-08   Time: 10:35a
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-05-29   Time: 9:47p
 * Updated in $/engine/src
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-03-18   Time: 3:19p
 * Updated in $/engine/src
 * bone auto link/del_item at link/unlink_node().
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-29   Time: 12:12p
 * Updated in $/engine/src
 * cleanup
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-19   Time: 1:47a
 * Updated in $/engine/src
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-08   Time: 7:56p
 * Updated in $/engine/src
 * motion blending reset
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
 * *****************  Version 15  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_channel_position.h"
#include "zz_channel_rotation.h"
#include "zz_motion.h"
#include "zz_bone.h"
#include "zz_system.h"
#include "zz_renderer.h"
#include "zz_animatable.h"
#include "zz_model.h"
#include "zz_profiler.h"

ZZ_IMPLEMENT_DYNCREATE(zz_bone, zz_visible)

zz_bone::zz_bone(void) :
	zz_visible(),
	position_channel_index(-1),
	rotation_channel_index(-1),
	use_cached_boneTM(false),
	model(NULL)
{
	visibility = 0;
	bv_type = ZZ_BV_NONE;
	forced_rotation_onoff = false;
	forced_rotation_calculate_onoff = false;

#ifdef ZZ_DRAW_BONE
	visibility = 1.0f;
	zz_mesh * mesh    = static_cast<zz_mesh *>(znzin->meshes->find("etc/bone.zms"));
	zz_material * mat = static_cast<zz_material *>(znzin->materials->find("etc/bone.dds"));
	zz_light * lit    = static_cast<zz_light *>(znzin->lights->find("light_01"));
	if (mesh && mat && lit) {
		add_runit(mesh, mat, lit);
		mat->set_ztest(false);
	}
#endif
}

zz_bone::~zz_bone(void)
{
	zz_assert(!get_onode());
}

void zz_bone::apply_channel_by_frame (int frame, zz_motion * motion, float blend_weight)
{
	assert(motion && "apply_channel_by_frame");
	assert(blend_weight >= 0.0f && blend_weight <= 1.0f && "apply_channel_by_frame");

	// the pelvis bone may use both the position and rotation channel.
	if (position_channel_index >= 0) {
		zz_assertf(position_channel_index < motion->get_num_channels(), "[%s]-[%s]-[%d]-[%f]pos",
			get_name(), motion->get_path(), position_channel_index, blend_weight);

		if (blend_weight < 1.0f) { // if use blend_weight
			vec3 last_position = position;
			motion->get_channel_data(position_channel_index, frame, &position);
			position = znzin->motion_tool.blend_position(last_position, position, blend_weight);
		}
		else { // no blend
			motion->get_channel_data(position_channel_index, frame, &position);
		}
	}
	if (rotation_channel_index >= 0) { // CAUTION: DO NOT INSERT *else* 
		zz_assertf(position_channel_index < motion->get_num_channels(), "[%s]-[%s]-[%d]-[%f]rot",
			get_name(), motion->get_path(), position_channel_index, blend_weight);

		if (blend_weight < 1.0f) { // if use blend_weight
            quat last_rotation = rotation;
			motion->get_channel_data(rotation_channel_index, frame, &rotation);
			rotation = znzin->motion_tool.blend_rotation(last_rotation, rotation, blend_weight);
		}
		else {
			motion->get_channel_data(rotation_channel_index, frame, &rotation);
		}
	}
}

void zz_bone::apply_channel_by_time (zz_time time, zz_motion * motion, int custum_fps, float blend_weight)
{
	assert(motion && "apply_channel_by_time");
	assert(blend_weight >= 0.0f && blend_weight <= 1.0f && "apply_channel_by_time");

	if (position_channel_index >= 0) {
		if (blend_weight < 1.0f) { // if use blend_weight
			vec3 last_position = position;
			motion->get_channel_data(position_channel_index, time, &position, custum_fps);
			position = znzin->motion_tool.blend_position(last_position, position, blend_weight);
		}
		else { // no blend
			motion->get_channel_data(position_channel_index, time, &position, custum_fps);
		}
	}

	if (rotation_channel_index >= 0) {
		if(!forced_rotation_onoff)
		{
			if (blend_weight < 1.0f) { // if use blend_weight
				quat last_rotation = rotation;
				motion->get_channel_data(rotation_channel_index, time, &rotation, custum_fps);
				rotation = znzin->motion_tool.blend_rotation(last_rotation, rotation, blend_weight);
			
			}
			else { // no blend
				motion->get_channel_data(rotation_channel_index, time, &rotation, custum_fps);
			}
		}
		else
		{			
			if(forced_rotation_calculate_onoff)
			{			
				float diff_time = znzin->get_diff_time();
				const float angle_per_time = 0.3f*3.141592f / ZZ_TICK_PER_SEC;  
				float new_angle_diff = angle_per_time * diff_time;	 
	            
				if( forced_rotation_angle_ + new_angle_diff > forced_rotation_angle)
				{
					forced_rotation_angle_ = forced_rotation_angle;
				    forced_rotation_calculate_onoff = false; 
				}
				else
				{
					forced_rotation_angle_ += new_angle_diff;

				}
			
			}
			
			D3DXQUATERNION dx_quat;
			D3DXMATRIX buffer_m;
			D3DXMATRIX buffer_m2, buffer_m3;
			dx_quat.x = forcde_rotation_quat.x; dx_quat.y = forcde_rotation_quat.y; dx_quat.z = forcde_rotation_quat.z; dx_quat.w = forcde_rotation_quat.w;
            D3DXMatrixRotationQuaternion(&buffer_m, &dx_quat);
            D3DXMatrixRotationAxis(&buffer_m2, &forced_rotation_axis, forced_rotation_angle_);
            buffer_m3 = buffer_m2 * buffer_m;
            
			D3DXQuaternionRotationMatrix(&dx_quat, &buffer_m3);
			rotation.x = dx_quat.x; rotation.y = dx_quat.y; rotation.z = dx_quat.z; rotation.w = dx_quat.w;
            
				
		}
	}
	//invalidate_transform();
}

const mat4& zz_bone::get_boneTM ()
{
	if (use_cached_boneTM == true) {
		return boneTM;
	}
	use_cached_boneTM = true;
	boneTM = get_worldTM() * get_bone_offsetTM();
	return boneTM;
}

void zz_bone::set_boneTM_to_shader (int bone_index)
{
	mat4 modelview_worldTM, bone_modelviewTM;
	
	get_modelview_worldTM(modelview_worldTM);
	
	// bone_offsetTM = (original)world_inverseTM
	// new_Vertex_world = modelviewTM * new_bone_worldTM * bone_offsetTM * [old_vertex_world]
	znzin->renderer->set_vertex_shader_constant_matrix(
		ZZ_VSC_BONE_TM + bone_index * 3,
		mult(bone_modelviewTM, modelview_worldTM, get_bone_offsetTM()),
		3); // just 3 columns available

//#ifdef _DEBUG
//	// testcode
//	ZZ_LOG("bone: set_boneTM_to_shader(%f, %f ,%f)\n", 
//		bone_modelviewTM.x, bone_modelviewTM.y, bone_modelviewTM);
//#endif
}

void zz_bone::set_boneTM_to_modelviewTM (void)
{
	mat4 modelview_worldTM, boneTM;
	
	get_modelview_worldTM(modelview_worldTM);

	// new_Vertex_world = modelviewTM * new_bone_worldTM * (world_inverseTM) * old_vertex_world
	znzin->renderer->set_modelview_matrix(mult(boneTM, modelview_worldTM, get_bone_offsetTM()));
}

void zz_bone::modify_channel_by_time (zz_time time, zz_motion * motion, int custum_fps)
{
	if (position_channel_index >= 0) {
		motion->set_channel_data(position_channel_index, time, &position, custum_fps);
	}
	if (rotation_channel_index >= 0) {
		motion->set_channel_data(rotation_channel_index, time, &rotation, custum_fps);
	}
}

void zz_bone::unlink_child(zz_node * node_in)
{
	zz_visible::unlink_child(node_in);

	// ignore bone-bone link
	if (node_in->is_a(ZZ_RUNTIME_TYPE(zz_bone))) return;

	assert(model);
	assert(node_in->is_a(ZZ_RUNTIME_TYPE(zz_visible)));
	model->del_item(static_cast<zz_visible*>(node_in));
}


void	zz_bone::gather_visible (std::vector<zz_visible *>& visible_holder)
{
	zz_assert(0);

	if (children.size() == 0) return;
	zz_visible * child_vis;
	for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it) {
		child_vis = static_cast<zz_visible*>(*it);
		child_vis->gather_visible(visible_holder);
	}
}

void	zz_bone::gather_collidable (std::vector<zz_visible *>& collidable_holder)
{
	zz_assert(0);

	if (children.size() == 0) return;	
	zz_visible * child_vis;
	for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it) {
		child_vis = static_cast<zz_visible*>(*it);
		child_vis->gather_collidable(collidable_holder);
	}
}

void zz_bone::link_child (zz_bone * bone_child)
{
	zz_assert(bone_child->get_num_children() == 0);
	zz_node::link_child(static_cast<zz_node*>(bone_child));
}

void zz_bone::link_child (zz_visible * vis_child)
{
	zz_visible::link_child(static_cast<zz_node*>(vis_child));
}

void zz_bone::link_child (zz_dummy * dummy_child)
{
	zz_assert(dummy_child->get_num_children() == 0);
	zz_node::link_child(static_cast<zz_node*>(dummy_child));
}

// same effect as model::unlink_dummy() (not exists)
// unlink visible from dummy in model
void zz_dummy::unlink_child (zz_node * node_to_remove)
{
	zz_bone * bone = static_cast<zz_bone*>(parent_node);
	zz_visible * vis = static_cast<zz_visible*>(node_to_remove);

	assert(bone);
	zz_model * model = bone->get_model();

	assert(bone->is_a(ZZ_RUNTIME_TYPE(zz_bone)));
	assert(model);
	assert(model->is_a(ZZ_RUNTIME_TYPE(zz_model)));
	assert(vis);
	assert(vis->is_a(ZZ_RUNTIME_TYPE(zz_visible)));

	model->del_item(vis);
	zz_visible::unlink_child(node_to_remove);
}

void zz_bone::input_forced_rotation_mode(vec4 &position)
{
	const mat4 wtm = this->get_world_inverseTM();;
    vec4 local_pos;
    float buffer_angle;
	D3DXVECTOR3 buffer_v, buffer_v2, buffer_v3;   
    buffer_v2.x = 0.0f; buffer_v2.y = -1.0f; buffer_v2.z = 0.0f;
  
	mult(local_pos, wtm, position);  
    local_picking_position[0] = local_pos.x;
	local_picking_position[1] = local_pos.y;
	local_picking_position[2] = local_pos.z;
	
	
	buffer_v.x = local_pos.x; buffer_v.y = local_pos.y; buffer_v.z = 0.0f;//local_pos.z;
	buffer_v /= D3DXVec3Length(&buffer_v);
    
	buffer_angle = acosf(D3DXVec3Dot(&buffer_v, &buffer_v2));
    D3DXVec3Cross(&buffer_v3, &buffer_v2, &buffer_v);   
    
	forced_rotation_onoff = true;
	forced_rotation_axis = buffer_v3 / D3DXVec3Length(&buffer_v3) ;
    forced_rotation_angle_ = 0.0f;
    forced_rotation_angle = buffer_angle; 
	forced_rotation_calculate_onoff = true;
    forcde_rotation_quat = rotation;
}

void zz_bone::set_forced_rotation_mode_off()
{
	forced_rotation_onoff = false;
}

void zz_bone::insert_scene()
{
	zz_assert(0);
}

void zz_bone::remove_scene()
{
	zz_assert(0);
}

void zz_dummy::insert_scene()
{
	zz_assert(0);
}

void zz_dummy::remove_scene()
{
	zz_assert(0);
}

zz_dummy::zz_dummy ()
{
	visibility = 0;
	bv_type = ZZ_BV_NONE;
}

zz_dummy::~zz_dummy ()
{
	zz_assert(!get_onode());
}
