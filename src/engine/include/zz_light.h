/** 
 * @file zz_light.h
 * @brief light class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    18-may-2002
 *
 * $Header: /engine/include/zz_light.h 2     04-02-20 12:17p Zho $
 * $History: zz_light.h $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-20   Time: 12:17p
 * Updated in $/engine/include
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
 * $History: zz_light.h $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-20   Time: 12:17p
 * Updated in $/engine/include
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

#ifndef __ZZ_LIGHT_H__
#define __ZZ_LIGHT_H__

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif

//--------------------------------------------------------------------------------
class zz_light : public zz_node {
public:
	//int light_index_in_renderer;
	vec4 ambient;
	vec4 specular;
	vec4 diffuse;
	vec4 position;
	
	bool shadowable;
	bool enabled;

	zz_light(void);
	virtual ~zz_light(void);

	ZZ_DECLARE_DYNAMIC(zz_light)
//	virtual void set_light ();
};

#endif //__ZZ_LIGHT_H__