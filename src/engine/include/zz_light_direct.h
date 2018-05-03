/** 
 * @file zz_light_direct.h
 * @brief direct light class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-may-2002
 *
 * $Header: /engine/include/zz_light_direct.h 1     03-11-30 8:05p Zho $
 * $History: zz_light_direct.h $
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
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:55p
 * Updated in $/znzin11/engine/include
 * cleanup #include structure
 */

#ifndef __ZZ_LIGHT_DIRECT_H__
#define __ZZ_LIGHT_DIRECT_H__

#ifndef __ZZ_LIGHT_H__
#include "zz_light.h"
#endif

//--------------------------------------------------------------------------------
class zz_light_direct :	public zz_light {
public:
	vec4 direction;

	zz_light_direct(void);
	virtual ~zz_light_direct(void);
	ZZ_DECLARE_DYNAMIC(zz_light_direct)
};

#endif //__ZZ_LIGHT_DIRECT_H__