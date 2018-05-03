/** 
 * @file zz_material_sky.h
 * @brief material sky class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    19-may-2003
 *
 * $Header: /engine/src/zz_material_sky.cpp 7     04-07-11 3:06p Zho $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_texture.h"
#include "zz_system.h"
#include "zz_material_sky.h"
#include "zz_renderer_d3d.h"

ZZ_IMPLEMENT_DYNCREATE(zz_material_sky, zz_material)

zz_material_sky::zz_material_sky (void) :
	blend_ratio_(1.0f)
{
}

zz_material_sky::~zz_material_sky (void)
{
	// CAUTION: DO NOT INSERT SUB OBJECT RELEASE()
	// because we need auto-cleaning without deleting dependent items
	// in manager destructor
}


void zz_material_sky::set_sky_one (void)
{
	// stage0_
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);

	// ignore alpha
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);
}

void zz_material_sky::set_sky_two (void)
{
	//s_renderer->set_render_state(ZZ_RS_TEXTUREFACTOR, ZZ_TO_D3DRGBA(0, 0, 0, blend_ratio_));
	int factor = int(255.0f*blend_ratio_);
	s_state.tfactor = ZZ_COLOR_RGBA( factor, factor, factor, factor );

	// stage0_rgba = texture1
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);

	// stage1_rgba = texture1*(factor) + texture2*(1 - factor)
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_BLENDFACTORALPHA);

	// disable stage2
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);

	// ignore alpha
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);
}

bool zz_material_sky::set (int pass)
{
	assert(get_cullmode() != zz_render_state::ZZ_CULLMODE_NONE);

	zz_texture * firstmap, * secondmap;
	
	firstmap = get_texture(0);
	secondmap = get_texture(1);

	if (firstmap) {
		firstmap->set(0);
	}
	if (secondmap) {
		secondmap->set(1);
	}

	// do sky texture stage setup
	s_state.copy_from(state);

	if (receive_fog) {
		// blend with dest alpha
		// (invdestalpha, destalpha, add) is not supported in Matrox G400 Fullscreen mode?
		//  .. In G400, the windowed-mode is OK, but in fullscreen mode, sky is not rendered.
		s_state.blend_src = ZZ_BLEND_INVDESTALPHA;
		s_state.blend_dest = ZZ_BLEND_DESTALPHA;

		s_state.blend_op = ZZ_BLENDOP_ADD;
		s_state.alpha_blend = true;
		s_state.blend_type = ZZ_BT_CUSTOM;
	}
	else {
		s_state.alpha_blend = false;
	}

	// set sky texture stage setup
	if (firstmap && !secondmap) {
		set_sky_one();
	}
	else if (firstmap && secondmap) {
		set_sky_two();
	}

	apply_shared_property(2);

	return true;
}