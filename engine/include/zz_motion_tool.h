/** 
 * @file zz_motion_tool.h
 * @brief motion tool class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    19-sep-2002
 *
 * $Header: /engine/include/zz_motion_tool.h 2     03-12-08 7:56p Zho $
 * $History: zz_motion_tool.h $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-08   Time: 7:56p
 * Updated in $/engine/include
 * motion blending reset
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
 * User: Zho          Date: 03-11-30   Time: 7:53p
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

#ifndef	__ZZ_MOTION_TOOL_H__
#define __ZZ_MOTION_TOOL_H__

class zz_motion;
class zz_motion_mixer;

//--------------------------------------------------------------------------------
class zz_motion_tool {
public:
	zz_motion_tool();
	~zz_motion_tool();

	zz_motion_mixer * create_blend_motion (const char * name, zz_motion * motion_arg1, zz_motion * motion_arg2, float blend_weight);
	vec3 blend_position (const vec3& pos1, const vec3& pos2, float t_0_to_1);
	quat blend_rotation (const quat& rot1, const quat& rot2, float t_0_to_1);

	// sigmoid-like functions
	// x = (0...1) -> (0..1). start slow. reach fast
	float scurve (float x) { return (3.0f*x*x - 2.0f*x*x*x); }
	float sinusoid (float x) { return float(sin(ZZ_PI*(x - .5f))); }
	// sigmoid ()

	bool load_motion (zz_motion * motion, const char * motion_file_name, bool use_loop, int interp_position, int interp_rotation, float scale);
};

#endif __ZZ_MOTION_TOOL_H__