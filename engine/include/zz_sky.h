/** 
 * @file zz_sky.h
 * @brief sky class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-apr-2003
 *
 * $Header: /engine/include/zz_sky.h 5     04-08-31 4:46p Zho $
 * $History: zz_sky.h $
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-08-31   Time: 4:46p
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-07-23   Time: 3:29a
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-05-31   Time: 5:34p
 * Updated in $/engine/include
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

#ifndef __ZZ_SKY_H__
#define __ZZ_SKY_H__

#ifndef __ZZ_VISIBLE_H__
#include "zz_visible.h"
#endif

#include <vector>

class zz_sky : public zz_visible {
private:
	std::vector<float> rot_angles_delta;
	std::vector<float> rot_angles_current;
	bool ztest;

public:
	zz_sky();

	virtual void before_render ();
	virtual void after_render ();
	virtual void update_time (bool recursive, zz_time diff_time);
	virtual void render (bool recursive = false);
	void render_glow ();

	void set_ztest (bool ztest_in)
	{
		ztest = ztest_in;
	}

	bool set_rotation_deltas (int index, float angle_degree_per_second);

	ZZ_DECLARE_DYNAMIC(zz_sky)	
};

#endif // __ZZ_SKY_H__