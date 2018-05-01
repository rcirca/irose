/** 
 * @file zz_light_omni.cpp
 * @brief light omni class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    18-may-2002
 *
 * $Header: /engine/src/zz_light_omni.cpp 2     04-02-11 2:05p Zho $
 * $History: zz_light_omni.cpp $
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
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_system.h"
#include "zz_light_omni.h"

ZZ_IMPLEMENT_DYNCREATE(zz_light_omni, zz_light)

zz_light_omni::zz_light_omni(void) : zz_light()
{
	position = vec4(0, 0, 0, 0);
//	znzin->renderer->set_light_type(zz_light::light_index_in_renderer, zz_renderer::ZZ_RLT_OMNI);
}

zz_light_omni::~zz_light_omni(void)
{
}

//vec3& zz_light_omni::get_position ()
//{
//	return position;
//}
//
//vec3& zz_light_omni::get_ambient ()
//{
//	return ambient;
//}
//
//vec3& zz_light_omni::get_specular ()
//{
//	return specular;
//}
//
//vec3& zz_light_omni::get_diffuse ()
//{
//	return diffuse;
//}

//void zz_light_omni::set_position (vec3& position_to_set)
//{
//	position = position_to_set;
//	znzin->renderer->set_light_property_by_vector(light_index_in_renderer, zz_renderer::ZZ_RLP_POSITION, position_to_set);
//}
//
//void zz_light_omni::set_ambient (vec3& ambient_to_set)
//{
//	ambient = ambient_to_set;
//	znzin->renderer->set_light_property_by_vector(light_index_in_renderer, zz_renderer::ZZ_RLP_AMBIENT, ambient_to_set);
//}
//
//void zz_light_omni::set_specular (vec3& specular_to_set)
//{
//	specular = specular_to_set;
//	znzin->renderer->set_light_property_by_vector(light_index_in_renderer, zz_renderer::ZZ_RLP_SPECULAR, specular_to_set);
//}
//
//void zz_light_omni::set_diffuse (vec3& diffuse_to_set)
//{
//	diffuse = diffuse_to_set;
//	znzin->renderer->set_light_property_by_vector(light_index_in_renderer, zz_renderer::ZZ_RLP_DIFFUSE, diffuse_to_set);
//}