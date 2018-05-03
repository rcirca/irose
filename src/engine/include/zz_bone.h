/** 
 * @file zz_bone.h
 * @brief bone class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-jun-2002
 *
 * $Header: /engine/include/zz_bone.h 10    05-08-25 11:38a Choo0219 $
 * $History: zz_bone.h $
 * 
 * *****************  Version 10  *****************
 * User: Choo0219     Date: 05-08-25   Time: 11:38a
 * Updated in $/engine/include
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-07-27   Time: 3:43p
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-06-30   Time: 6:57p
 * Updated in $/engine/include
 * 6.30.4498
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-06-29   Time: 10:09a
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-06-25   Time: 3:03a
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
 * User: Zho          Date: 04-03-18   Time: 3:19p
 * Updated in $/engine/include
 * bone auto link/del_item at link/unlink_node().
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-29   Time: 12:12p
 * Updated in $/engine/include
 * cleanup
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:05p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:53p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:47p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:19p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 03-11-28   Time: 2:55p
 * Updated in $/znzin11/engine/include
 * cleanup #include structure
 */

#ifndef	__ZZ_BONE_H__
#define __ZZ_BONE_H__

#include <d3d9.h>
#include <d3dx9.h>
// set if need to draw bone
//#define ZZ_DRAW_BONE

#ifndef __ZZ_VISIBLE_H__
#include "zz_visible.h"
#endif

#include <vector>

class zz_motion;
class zz_channel_position;
class zz_channel_rotation;
class zz_model;

class zz_dummy : public zz_visible {
public:
	virtual void unlink_child (zz_node * node_to_remove);

	// debugging only
	virtual void insert_scene ();
	virtual void remove_scene ();

	// not recursive insert/remove scene 
	void insert_scene_one () { inscene = true; }
	void remove_scene_one () { inscene = false; }

	zz_dummy ();
	virtual ~zz_dummy ();
};

//--------------------------------------------------------------------------------
class zz_bone : public zz_visible {
private:
	// bone_offsetTM = worldTM.inverse()
	// from model_space to bone_space
	// new_vertex_world = [new_world_TM] * bone_offsetTM * vertex_in_model_space
	// , where [new_world_TM] is varying in time.
	mat4 bone_offsetTM;

	// boneTM caching
	bool use_cached_boneTM;
	mat4 boneTM; // = worldTM * bone_offsetTM

	// map index to motion's channel
	// channel_index range = [0, motion->channels.size() )
	int position_channel_index; // positive value if mapped to position channel
	int rotation_channel_index; // positive value if mapped to rotation channel

    bool forced_rotation_onoff;
	D3DXVECTOR3  forced_rotation_axis;
    float forced_rotation_angle_;
    float forced_rotation_angle; 
	bool forced_rotation_calculate_onoff;
    quat forcde_rotation_quat;  
    float local_picking_position[3];

	zz_model * model;

public:
	enum zz_joint_type {
		ZZ_JOINT_FREE,
		ZZ_JOINT_SOCKET,
		ZZ_JOINT_HINGE // ...
	};

	zz_bone ();
	virtual ~zz_bone ();

	// invalidate tm and bvolume
	bool invalidate_tm_minmax ()
	{
		use_cached_boneTM = false;
		return zz_visible::invalidate_tm_minmax();
	}

	void set_position_channel_index (int index);
	void set_rotation_channel_index (int index);

	// faster than apply_channel_by_time()
	void apply_channel_by_frame (int frame, zz_motion * motion, float blend_weight = 1.0f);
	// @time = local time after motion start
	void apply_channel_by_time (zz_time time, zz_motion * motion, int custum_fps = 0, float flend_weight = 1.0f);
	void modify_channel_by_time (zz_time time, zz_motion * motion, int custum_fps = 0);

	void calculate_bone_offsetTM ();
	const mat4& get_bone_offsetTM ();
	const mat4& get_boneTM ();

	// boneTM = modelview * worldTM * bone_offsetTM
	// new_vertex_in_modelview_world_space = boneTM * vertex_in_model_space
	void set_boneTM_to_shader (int bone_index);
	void set_boneTM_to_modelviewTM (void);

	// We did not defined zz_bone::link_child(zz_node *), because bone->link_child(non-bone) is always done
	// in zz_model by zz_model::link_bone() that adds visible node into their items list.
	// When we unlink child from bone without zz_model::unlink_bone(),
	// we should check if the to-be-unlinked node is visible-type, and, if it is, 
	// we should do zz_model::del_item().
	virtual void unlink_child (zz_node * node_in);

	// overloading thing for bone
	void link_child (zz_bone * bone_child);
	void link_child (zz_dummy * dummy_child);
	void link_child (zz_visible * vis_child);

	zz_model * get_model ();
	bool set_model (zz_model * model_in); // return false if already set by another model.

	virtual void gather_collidable (std::vector<zz_visible*>& collidable_holder);

	virtual void gather_visible (std::vector<zz_visible*>& visible_holder);

	// debugging only
	virtual void insert_scene ();
	virtual void remove_scene ();

	// not recursive insert/remove scene
	void insert_scene_one () { inscene = true; }
	void remove_scene_one () { inscene = false; }

	void input_forced_rotation_mode(vec4 &position);
	void set_forced_rotation_mode_off();
	
	ZZ_DECLARE_DYNAMIC(zz_bone);
};

inline bool zz_bone::set_model (zz_model * model_in)
{
	// returns false if model was already set.
	// returns true after assigning the model.
	return (model) ? false : (model = model_in, true);
}

inline zz_model * zz_bone::get_model ()
{
	return model;
}

inline void zz_bone::set_position_channel_index (int index)
{
	position_channel_index = index;
}

inline void zz_bone::set_rotation_channel_index (int index)
{
	rotation_channel_index = index;
}

// call this before any animation is applied
inline void zz_bone::calculate_bone_offsetTM ()
{
	bone_offsetTM = zz_bone::get_world_inverseTM();
	// worldTM * Vertex_bonespace = Vertex_world
	// Vertex_bonespace = worldTM.inverse() * Vertex_world
}

inline const mat4& zz_bone::get_bone_offsetTM ()
{
	return bone_offsetTM;
}

// list of bones
typedef std::vector<zz_bone *> zz_bone_list;

#endif // __ZZ_BONE_H__