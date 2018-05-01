/** 
 * @file zz_ik_limb.cpp
 * @brief limb IK class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-dec-2002
 *
 * $Header: /engine/src/zz_ik_limb.cpp 3     04-06-22 9:51a Zho $
 * $History: zz_ik_limb.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-06-22   Time: 9:51a
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
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_ik_limb.h"
#include "zz_profiler.h"
#include "zz_bone.h"

ZZ_IMPLEMENT_DYNCREATE(zz_ik_limb, zz_node)

zz_ik_limb::zz_ik_limb () : originTM(mat4_id), originITM(mat4_id)
{
}

void zz_ik_limb::set_chain (zz_bone * root, zz_bone * middle, zz_bone * end)
{
	assert(root && middle && end);
	this->root = root;
	this->mid = middle;
	this->end = end;

	// setup originTM
	assert(root->get_parent()->is_a(ZZ_RUNTIME_TYPE(zz_visible)));
	originTM = ((zz_visible *)(root->get_parent()))->get_worldTM();
	originITM = ((zz_visible *)(root->get_parent()))->get_world_inverseTM();

	// set length
	mult_pos(pos_root, originITM, root->get_worldTM().get_position());
	mult_pos(pos_mid, originITM, mid->get_worldTM().get_position());
	mult_pos(pos_end, originITM, end->get_worldTM().get_position());	
	root_to_mid = pos_mid - pos_root;
	mid_to_end = pos_end - pos_mid;
	L1 = root_to_mid.norm();
	L2 = mid_to_end.norm();
}

float zz_ik_limb::get_mid_angle_rad (void)
{
	// L2^2 = L1^2 + L2^2 - 2*L1*L2*cos(theta)
	// L1 : hip to knee, L2 : knee to ankle, L3 : hip to target
    L3 = root_to_target.norm();
	const float minimum_angle_rad = 5.0f*ZZ_TO_RAD;
	
	//ZZ_LOG("ik_limb: L1(%f) + L2(%f) = L3(%f). length(%f)\n", L1, L2, L3, root_to_end.norm());

	if (L3 > (L1 + L2)) {
        L3 = L1 + L2; // do not exceed total length, cannot reach the target
		//ZZ_LOG("ik_limb: cannot reach. end(%f, %f, %f), target(%f, %f, %f)\n",
		//	pos_end.x, pos_end.y, pos_end.z,
		//	pos_target.x, pos_target.y, pos_target.z);
		return minimum_angle_rad;
	}
    //float angle_rad = acosf((L1*L1 + L2*L2 - L3*L3) / (2.0f*L1*L2));
	float angle_rad = acosf(0.5f * (L1*L1 + L2*L2 - L3*L3) / (L1*L2));
	// angle constraint
    if (angle_rad < minimum_angle_rad) angle_rad = minimum_angle_rad;
    if (angle_rad > (ZZ_PI - minimum_angle_rad)) angle_rad = ZZ_PI - minimum_angle_rad;

    // current angle_rad means inner angle, thus convert to outer angle
    angle_rad = ZZ_PI - angle_rad;
    return angle_rad;
}

void zz_ik_limb::set_target (vec3 target_world)
{
	//ZZ_PROFILER_BEGIN_CYCLE(Pset_target, 100);

	quat rot_root;
	quat rot_mid;
	
	// setup originTM
	assert(root->get_parent()->is_a(ZZ_RUNTIME_TYPE(zz_visible)));
	originTM = ((zz_visible *)(root->get_parent()))->get_worldTM();
	originITM = ((zz_visible *)(root->get_parent()))->get_world_inverseTM();

	// set positions
	mult_pos(pos_root, originITM, root->get_worldTM().get_position());
	mult_pos(pos_mid, originITM, mid->get_worldTM().get_position());
	mult_pos(pos_end, originITM, end->get_worldTM().get_position());
	mult_pos(pos_target, originITM, target_world);
	
	//ZZ_LOG("ik_limb: end(%f, %f, %f), target(%f, %f, %f)\n",
	//	pos_end.x, pos_end.y, pos_end.z,
	//	pos_target.x, pos_target.y, pos_target.z);

	// set direction vectors
	root_to_mid = pos_mid - pos_root;
	mid_to_end = pos_end - pos_mid;
	root_to_end = pos_end - pos_root;
	root_to_target = pos_target - pos_root;

	// update mid orientation
	mat3 mid_rot(mat3_id);
	quat q_mid_rot(quat_id);
	float theta(get_mid_angle_rad());
	//ZZ_LOG("ik_limb: theta = %f\n", theta*ZZ_TO_DEG);
	mid_rot.set_rot(-theta, vec3(0, 0, 1)); // negative rotation around z-axis(only in max biped)
	
	// rotate mid angle
	q_mid_rot.from_matrix(mid_rot);
	q_mid_rot.normalize();
	mid->set_rotation(q_mid_rot);

	// reset positions
	mid->invalidate_transform();
	mult_pos(pos_mid, originITM, mid->get_worldTM().get_position());
	mult_pos(pos_end, originITM, end->get_worldTM().get_position());
	
	// reset direction vectors
	root_to_mid = pos_mid - pos_root;
	mid_to_end = pos_end - pos_mid;
	root_to_end = pos_end - pos_root;
	
	// update root orientation
	quat q_end_to_target;
	mat3 end_to_target = mat3_id;
	root_to_end.normalize();
	root_to_target.normalize();
	end_to_target.set_rot(root_to_end, root_to_target);
	q_end_to_target.from_matrix(end_to_target);
	root->set_rotation(q_end_to_target * root->get_rotation());
	
	// update
	root->invalidate_transform();
	//ZZ_PROFILER_END(Pset_target);
}

bool zz_ik_limb::reachable (vec3 pos_world)
{
	//TODO: code it
	return true;
}

zz_bone * zz_ik_limb::get_chain (int index)
{
	switch (index) {
		case 0:
			return root;
			break;
		case 1:
			return mid;
			break;
		case 2:
			return end;
			break;
	}
	return NULL;
}
