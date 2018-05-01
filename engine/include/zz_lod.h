/** 
 * @file zz_lod.cpp
 * @brief lod class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-apr-2004
 *
 * $Header: /engine/include/zz_lod.h 2     04-05-19 8:13p Zho $
 * $History: zz_lod.h $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-05-19   Time: 8:13p
 * Updated in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-04-15   Time: 3:39p
 * Created in $/engine/include
 * Added mesh-material lod.
 * 
 */

#ifndef __ZZ_LOD_H__
#define __ZZ_LOD_H__

struct zz_lod {
	enum e_lod {
		LOD_NEAR = 0,
		LOD_MEDIUM = 1,
		LOD_FAR = 2
	};

	e_lod lod;

	zz_lod ( e_lod lod_in = LOD_NEAR ) : lod(lod_in) {}

	//bool equal ( e_lod lod_range )
	//{
	//	return (lod == lod_range);
	//}

	bool less_equal ( e_lod lod_range )
	{
		return (lod <= lod_range);
	}

	bool greater ( e_lod lod_range )
	{
		return (lod > lod_range);
	}

	bool greater_equal ( e_lod lod_range )
	{
		return (lod >= lod_range);
	}
};

#endif // __ZZ_LOD_H__