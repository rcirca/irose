/** 
 * @file zz_light_omni.h
 * @brief omni light class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    18-may-2002
 *
 * $Header: /engine/include/zz_light_omni.h 1     03-11-30 8:05p Zho $
 * $History: zz_light_omni.h $
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

#ifndef __ZZ_LIGHT_OMNI_H__
#define __ZZ_LIGHT_OMNI_H__

#ifndef __ZZ_LIGHT_H__
#include "zz_light.h"
#endif

//--------------------------------------------------------------------------------
class zz_light_omni : public zz_light {
public:

	zz_light_omni(void);
	virtual ~zz_light_omni(void);

	ZZ_DECLARE_DYNAMIC(zz_light_omni)

	//vec3& get_position ();
	//vec3& get_ambient ();
	//vec3& get_specular ();
	//vec3& get_diffuse ();

	//void set_position (vec3& position_to_set);
	//void set_ambient (vec3& ambient_to_set);
	//void set_specular (vec3& specular_to_set);
	//void set_diffuse (vec3& diffuse_to_set);
};

#endif // __ZZ_LIGHT_OMNI_H__