/** 
 * @file zz_material_ocean.cpp
 * @brief ocean material class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    30-jun-2003
 *
 * $Header: /engine/src/zz_material_ocean.cpp 5     04-05-26 9:13p Zho $
 * $History: zz_material_ocean.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-03-19   Time: 5:28p
 * Updated in $/engine/src
 * Added special effect.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-03-19   Time: 11:27a
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
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_texture.h"
#include "zz_material_ocean.h"
#include "zz_system.h"
#include "zz_renderer.h"

ZZ_IMPLEMENT_DYNCREATE(zz_material_ocean, zz_material)

zz_material_ocean::zz_material_ocean (void) :
	zz_material(1),
	current_texture_index(0)
{
	state.blend_type = ZZ_BT_CUSTOM;
	state.blend_src = ZZ_BLEND_ONE;
	state.blend_dest = ZZ_BLEND_ONE;
}

zz_material_ocean::~zz_material_ocean (void)
{
}

void zz_material_ocean::set_first_light()
{
	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);
		
	s_state.texture_address[0] = ZZ_TADDRESS_WRAP;

	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_ocean::set_first()
{
	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);
		
	s_state.texture_address[0] = ZZ_TADDRESS_WRAP;

	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

// meaning of *pass* is in [zz_material.h]
bool zz_material_ocean::set (int pass)
{
	zz_texture * firstmap = get_texture(current_texture_index);

	s_state.copy_from(state);

	if (firstmap) {
		firstmap->set(0);
	}
	
	s_state.cullmode = zz_render_state::ZZ_CULLMODE_NONE;

	//s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG2, ZZ_TA_DIFFUSE); // to alpha fog
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_SELECTARG2);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);

	set_first();
	
	apply_shared_property(1);

	return true;
}
