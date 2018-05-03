/** 
 * @file zz_ik_limb.h
 * @brief limb IK class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-dec-2002
 *
 * $Header: /engine/include/zz_ik_limb.h 1     03-11-30 8:05p Zho $
 * $History: zz_ik_limb.h $
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
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-28   Time: 2:55p
 * Updated in $/znzin11/engine/include
 * cleanup #include structure
 */

#ifndef __ZZ_IK_LIMB_H__
#define __ZZ_IK_LIMB_H__

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

class zz_bone;

//--------------------------------------------------------------------------------
class zz_ik_limb : public zz_node {
private:
	zz_bone * root;
	zz_bone * mid;
	zz_bone * end;
	mat4 originTM; // TM of parent-of-root
	mat4 originITM; // inverse TM of parent-of-root
	vec3 pos_root;
	vec3 pos_mid;
	vec3 pos_end;
	vec3 pos_target;
	vec3 root_to_mid, mid_to_end, root_to_end;
	vec3 root_to_target;
	float L1, L2, L3;

	float get_mid_angle_rad (void); // get mid segment angle by radian

public:
	zz_ik_limb ();
	~zz_ik_limb () {}

	void set_chain (zz_bone * root, zz_bone * middle, zz_bone * end);
	void set_target (vec3 target_world);
	bool reachable (vec3 target_world);
	zz_bone * get_chain (int index);

	ZZ_DECLARE_DYNAMIC(zz_ik_limb)
};

#endif // __ZZ_IK_LIMB_H__
