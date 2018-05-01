/** 
 * @file zz_ocean_block.h
 * @brief ocean block class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    30-jun-2003
 *
 * $Header: /engine/include/zz_ocean_block.h 4     04-08-31 4:46p Zho $
 * $History: zz_ocean_block.h $
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-08-31   Time: 4:46p
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-04-29   Time: 6:04p
 * Updated in $/engine/include
 * flush_device( delayed) added.
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
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_OCEAN_BLOCK_H__
#define __ZZ_OCEAN_BLOCK_H__

#ifndef __ZZ_VISIBLE_H__
#include "zz_visible.h"
#endif

class zz_ocean_block : public zz_visible {
	zz_time time_interval;

public:
	zz_ocean_block();
	virtual ~zz_ocean_block();

	virtual void update_animation (bool recursive, zz_time diff_time);
	virtual void render_runit (unsigned int runit_index);

	void set_interval (zz_time interval);
	zz_time get_interval (void);

	virtual void before_render ();
	virtual void after_render ();

	ZZ_DECLARE_DYNAMIC(zz_ocean_block);
};

#endif // __ZZ_OCEAN_BLOCK_H__