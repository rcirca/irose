/** 
 * @file zz_material_terrain.h
 * @brief terrain material class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-feb-2003
 *
 * $Header: /engine/src/zz_material_terrain.cpp 27    04-09-05 6:51p Zho $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_texture.h"
#include "zz_material_terrain.h"
#include "zz_system.h"
#include "zz_renderer.h"
#include "zz_shader.h"
#include "zz_light.h"

// texture slot index for colormap (textures)
#define FIRSTMAP_SLOT			0
#define SECONDMAP_SLOT			1
#define LIGHTMAP_SLOT			2
#define SHADOWMAP_SLOT			3
#define NUM_TEXTURE_SLOT		4

ZZ_IMPLEMENT_DYNCREATE(zz_material_terrain, zz_material)
ZZ_IMPLEMENT_DYNCREATE(zz_material_terrain_rough, zz_material_terrain)

zz_material_terrain::e_material_state zz_material_terrain::color_state(zz_material_terrain::MS_NONE);
zz_material_terrain::e_material_state zz_material_terrain::alpha_state(zz_material_terrain::MS_NONE);

zz_material_terrain::zz_material_terrain (void) :
	zz_material(3)
{
	// do not define state-variables.
	// state-variables are set in begin()
}

zz_material_terrain::~zz_material_terrain (void)
{
}

void zz_material_terrain::set_first_second()
{
	if (color_state == MS_FIRST_SECOND) return;
	color_state = MS_FIRST_SECOND;

	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);
	
	// secondmap
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_BLENDTEXTUREALPHA);

	//// color
	// CAUTION: do not use tfactor for light diffuse color blending.
	// why? GeForce 4 MX Me driver does not support TFACTOR in multi stage state. (only support one stage tfactor)
	// that can cause TOOMANYOPERATION error(after ValidateDevice()).
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG1, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);

	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_terrain::set_first_light ()
{	
	if (color_state == MS_FIRST_LIGHT) return;
	color_state = MS_FIRST_LIGHT;

	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	
	// lightmap stage
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, s_renderstate->lightmap_blend_style);

	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_terrain::set_first_light_shadow ()
{	
	if (color_state == MS_FIRST_LIGHT_SHADOW) return;
	color_state = MS_FIRST_LIGHT_SHADOW;

	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_TFACTOR);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	
	// lightmap stage
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, s_renderstate->lightmap_blend_style);

	// shadowmap stage
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG1, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG2, ZZ_TA_TEXTURE);

	if (s_renderstate->use_blackshadowmap) {
		s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_SUBTRACT);
	}
	else {
		s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	}

	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_terrain::set_light()
{
	if (color_state == MS_LIGHT) return;
	color_state = MS_LIGHT;

	// lightmap stage
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);

	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_terrain::set_light_shadow()
{
	if (color_state == MS_LIGHT_SHADOW) return;
	color_state = MS_LIGHT_SHADOW;

	// lightmap stage
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);

	// shadowmap stage
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_TEXTURE);

	if (s_renderstate->use_blackshadowmap)
		s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_SUBTRACT);
	else
		s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}


void zz_material_terrain::set_shadow()
{
	if (color_state == MS_SHADOW) return;
	color_state = MS_SHADOW;

	// shadowmap stage
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG2);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_terrain::set_first()
{
	if (color_state == MS_FIRST) return;
	color_state = MS_FIRST;

	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_TFACTOR);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
		
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_terrain::set_first_second_light()
{
	if (color_state == MS_FIRST_SECOND_LIGHT) return;
	color_state = MS_FIRST_SECOND_LIGHT;

	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);

	// secondmap
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_BLENDTEXTUREALPHA);
	
	// lightmap stage
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, s_renderstate->lightmap_blend_style);

	// light color stage
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLORARG1, ZZ_TA_TFACTOR);
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);

	s_renderer->set_texture_stage_state(4, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_terrain::set_first_second_shadow()
{
	if (color_state == MS_FIRST_SECOND_SHADOW) return;
	color_state = MS_FIRST_SECOND_SHADOW;	

	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);
	
	// secondmap
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_BLENDTEXTUREALPHA);

	// shadowmap stage
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	if (s_renderstate->use_blackshadowmap) {
		s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_SUBTRACT);
	}
	else {
		s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	}

	// secondmap
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLORARG1, ZZ_TA_TFACTOR);
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);

	s_renderer->set_texture_stage_state(4, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_terrain::set_first_shadow()
{
	if (color_state == MS_FIRST_SHADOW) return;
	color_state = MS_FIRST_SHADOW;	

	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	
	// shadowmap stage
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	if (s_renderstate->use_blackshadowmap) {
		s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_SUBTRACT);
	}
	else {
		s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	}

	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_terrain::set_first_second_light_shadow ()
{	
	if (color_state == MS_FIRST_SECOND_LIGHT_SHADOW) return;
	color_state = MS_FIRST_SECOND_LIGHT_SHADOW;

	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);
	
	// secondmap
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_BLENDTEXTUREALPHA);

	// lightmap stage
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, s_renderstate->lightmap_blend_style);

	// shadowmap stage
	// shadow map uses only 3rd 3
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLORARG1, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLORARG2, ZZ_TA_TEXTURE);

	if (s_renderstate->use_blackshadowmap) {
		s_renderer->set_texture_stage_state(3, ZZ_TSS_COLOROP, ZZ_TOP_SUBTRACT);
	}
	else {
		s_renderer->set_texture_stage_state(3, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	}

	// secondmap
	s_renderer->set_texture_stage_state(4, ZZ_TSS_COLORARG1, ZZ_TA_TFACTOR);
	s_renderer->set_texture_stage_state(4, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(4, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);

	s_renderer->set_texture_stage_state(5, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}


void zz_material_terrain::set_test_blend ()
{	
	if (color_state == MS_TEST_BLEND) return;
	color_state = MS_TEST_BLEND;
 
	// A*(1-C) + B*C = A - A*C + B*C = A - (A - B)*C
	// A
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE); // A 
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);
	
	// (A - B)
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_TEXTURE); // B
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_SUBTRACT);
	
	// (A - B) * c
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG1, ZZ_TA_CURRENT); // (A - B)
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG2, ZZ_TA_TEXTURE); // C
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	
	// A - (A - B) * C
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE); // A
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT); // (A - B)*C
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLOROP, ZZ_TOP_SUBTRACT);

	s_renderer->set_texture_stage_state(4, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_terrain::set_alpha_stage (int pass, bool use_lightmap, bool use_shadowmap)
{
	bool use_alpha_fog = znzin->get_rs()->use_alpha_fog;

	// check alpha fog
	if (use_lightmap) {
		if (use_shadowmap) {
			if (use_alpha_fog) {
				if (alpha_state == MS_ALPHA_LIGHT_SHADOW_FOG) return;
				alpha_state = MS_ALPHA_LIGHT_SHADOW_FOG;
			}
			else {
				if (alpha_state == MS_ALPHA_LIGHT_SHADOW) return;
				alpha_state = MS_ALPHA_LIGHT_SHADOW;
			}
		}
		else {
			if (use_alpha_fog) {
				if (alpha_state == MS_ALPHA_LIGHT_FOG) return;
				alpha_state = MS_ALPHA_LIGHT_FOG;
			}
			else {
				if (alpha_state == MS_ALPHA_LIGHT) return;
				alpha_state = MS_ALPHA_LIGHT;
			}
		}
	}
	else {
		if (use_shadowmap) {
			if (use_alpha_fog) {
				if (alpha_state == MS_ALPHA_SHADOW_FOG) return;
				alpha_state = MS_ALPHA_SHADOW_FOG;
			}
			else {
				if (alpha_state == MS_ALPHA_SHADOW) return;
				alpha_state = MS_ALPHA_SHADOW;
			}
		}
		else {
			if (use_alpha_fog) {
				if (alpha_state == MS_ALPHA_FOG) return;
				alpha_state = MS_ALPHA_FOG;
			}
			else {
				if (alpha_state == MS_ALPHA) return;
				alpha_state = MS_ALPHA;
			}
		}
	}

	// set blend style
	if (!s_renderstate->use_multipass) { // if one pass
		if (use_alpha_fog) {
			s_renderer->enable_alpha_blend(true, ZZ_BT_CUSTOM);
			s_renderer->set_blend_type(ZZ_BLEND_SRCALPHA, ZZ_BLEND_INVSRCALPHA, ZZ_BLENDOP_ADD);
			s_renderer->enable_alpha_test(false);

			s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG1, ZZ_TA_DIFFUSE);
			s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_SELECTARG1);
			s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE); 
		}
		else {
			s_renderer->enable_alpha_blend(false, ZZ_BT_NORMAL);
			s_renderer->enable_alpha_test(false);
			s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);
		}
	}
	else { // multi pass
		if (pass == 0) {
			if (use_alpha_fog) {
				//s_renderer->enable_alpha_blend(true, ZZ_BT_CUSTOM);
				s_renderer->enable_alpha_blend(true, ZZ_BT_NORMAL); // SRCALPHA, INVSRCALPHA
				s_renderer->enable_alpha_test(false);

				// CAUTION: geforce4 mx. must be over 2-stage alpha operation.
				// why? i dont know.
				// if not using over 2-stage, then we cannot alpha blending by diffuse.
				s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG1, ZZ_TA_DIFFUSE);
				s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_SELECTARG1);

				s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAARG1, ZZ_TA_DIFFUSE);
				s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAOP,   ZZ_TOP_SELECTARG1);

				s_renderer->set_texture_stage_state(2, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE); 
			}
			else {
				s_renderer->enable_alpha_blend(false, ZZ_BT_NORMAL);
				s_renderer->enable_alpha_test(false);

				s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);
			}
		}
		else { // if second pass
			s_renderer->enable_alpha_blend(true, ZZ_BT_NONE);
			// modulate
			// final = src*src_blend + dest*dest_blend
			//       = src*(dest_color) + dest*(zero)
			// modulate2x
			// final = src*(dest_color) + dest*(src_color)
			if (use_lightmap) {
				if (s_renderstate->lightmap_blend_style == ZZ_TOP_MODULATE) {
					s_renderer->set_blend_type(ZZ_BLEND_DESTCOLOR, ZZ_BLEND_ZERO, ZZ_BLENDOP_ADD); // modulate1x
				}
				else if (s_renderstate->lightmap_blend_style == ZZ_TOP_MODULATE2X) { // default MODULATE2X
					s_renderer->set_blend_type(ZZ_BLEND_DESTCOLOR, ZZ_BLEND_SRCCOLOR, ZZ_BLENDOP_ADD); // modulate2x
				}
				else {
					ZZ_LOG("material_terrain: invalid lightmap_blend_style");
					// set default
					s_renderer->set_blend_type(ZZ_BLEND_DESTCOLOR, ZZ_BLEND_SRCCOLOR, ZZ_BLENDOP_ADD); // modulate2x
				}
			}
			else { // only shadowmap
				assert(use_shadowmap);
				// shadowmap test
				s_renderer->enable_alpha_blend(true, ZZ_BT_NONE);
				if (s_renderstate->use_blackshadowmap) {
					s_renderer->set_blend_type(ZZ_BLEND_ONE, ZZ_BLEND_ONE, ZZ_BLENDOP_REVSUBTRACT); // = SUBTRACT
				}
				else {
					s_renderer->set_blend_type(ZZ_BLEND_ZERO, ZZ_BLEND_SRCCOLOR, ZZ_BLENDOP_ADD); // MODULATE
				}
			}

			// alpha texture stage setup
			s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);
		}
	}
}

void zz_material_terrain::set_first_rough()
{
	if (color_state == MS_FIRST) return;
	color_state = MS_FIRST;

	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
		
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_terrain::set_blend_rterrain_alpha_stage ()
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG2, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_SELECTARG2); 
	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);
}

#ifdef _DEBUG
#define TEST_FLAGS_MULTI( INDEX ) if (!debug_flags_multi[ INDEX ]) return false;
#define TEST_FLAGS_SINGLE( INDEX ) if (!debug_flags_single[ INDEX ]) return false;
#else
#define TEST_FLAGS_MULTI( INDEX )
#define TEST_FLAGS_SINGLE( INDEX )
#endif

// meaning of *pass* is in [zz_material.h]
bool zz_material_terrain::set (int pass)
{
	zz_texture * firstmap, * secondmap, * lightmap;

	int shader_format = get_shader_format();

	if (shader_format == SHADER_FORMAT_DEFAULT) return false;

	firstmap = (shader_format & SHADER_FORMAT_FIRSTMAP) ? textures[0] : 0;
	secondmap = (shader_format & SHADER_FORMAT_SECONDMAP) ? textures[1] : 0;
	lightmap = (shader_format & SHADER_FORMAT_LIGHTMAP) ? textures[2] : 0;

	bool l_use_shadow = (shader_format & SHADER_FORMAT_SHADOWMAP) > 0;

	// texture binding
	int texture_stage = 0;

	if (firstmap) firstmap->set(texture_stage++);
	if (secondmap) secondmap->set(texture_stage++);
	if (lightmap) lightmap->set(texture_stage++);
	if (l_use_shadow) s_renderer->set_texture_shadowmap(texture_stage++);

#ifdef _DEBUG
	static bool debug_flags_multi [] = {true, true, true, true, true, true, true};
	static bool debug_flags_single [] = {true, true, true, true, true, true, true};
#endif

	// set texture stage
	if (s_renderstate->use_multipass) {
		if (firstmap) {
			if (secondmap) {
				TEST_FLAGS_MULTI(0);
				set_first_second();
			}
			else if (lightmap) {
				TEST_FLAGS_MULTI(1);
				set_first_light();
			}
			else if (l_use_shadow) {
				TEST_FLAGS_MULTI(2);
				set_first_shadow();
			}
			else {
				TEST_FLAGS_MULTI(3);
				set_first();
			}
		}
		else if (lightmap) {
			if (l_use_shadow) {
				TEST_FLAGS_MULTI(4);
				set_light_shadow();
			}
			else {
				TEST_FLAGS_MULTI(5);
				set_light();
			}
		}
		else if (l_use_shadow) {
			TEST_FLAGS_MULTI(6);
			set_shadow();
		}
		else {
			return false;
		}
	}
	else {
		if (firstmap) {
			if (secondmap) {
				if (lightmap) {
					if (l_use_shadow) {
						TEST_FLAGS_SINGLE(0);
						set_first_second_light_shadow();
					}
					else {
						TEST_FLAGS_SINGLE(1);
						set_first_second_light();
					}
				}
				else {
					if (l_use_shadow) {
						TEST_FLAGS_SINGLE(2);
						set_first_second_shadow();
					}
					else {
						TEST_FLAGS_SINGLE(3);
						set_first_second();
					}
				}
			}
			else {
				if (lightmap) {
					if (l_use_shadow) {
						TEST_FLAGS_SINGLE(4);
						set_first_light_shadow();
					}
					else {
						TEST_FLAGS_SINGLE(5);
						set_first_light();
					}
				}
				else {
					TEST_FLAGS_SINGLE(6);
					set_first();
				}
			}
		}
		else {
			return false;
		}
	}
	set_alpha_stage(pass, lightmap ? true : false, l_use_shadow);
	return true;
}

void zz_material_terrain::begin ()
{
	color_state = MS_NONE;
	alpha_state = MS_NONE;

	s_state.init();

	for (int i = 0; i < ZZ_MAX_TEXTURESTAGE; i++) {
		s_state.texture_address[i] = ZZ_TADDRESS_CLAMP;
	}
	s_state.cullmode = zz_render_state::ZZ_CULLMODE_CCW; // uses clockwise welding. see zz_mesh_tool.cpp

	zz_light * light = znzin->get_light();
	zz_assert(light);

	s_state.tfactor = zz_color(light->diffuse.r, light->diffuse.g, light->diffuse.b, light->diffuse.a);

	apply_shared_property(4);
}

void zz_material_terrain::end ()
{
}

int zz_material_terrain::get_shader_format () const
{
	int shader_format(0);
	int num_textures(0);

	if (get_texture(FIRSTMAP_SLOT)) {
		shader_format |= SHADER_FORMAT_FIRSTMAP;
		num_textures++;
	}
	if (get_texture(SECONDMAP_SLOT)) {
		shader_format |= SHADER_FORMAT_SECONDMAP;
		num_textures++;
	}
	if (s_renderstate->use_lightmap && get_texture(LIGHTMAP_SLOT)) {
		shader_format |= SHADER_FORMAT_LIGHTMAP;
		num_textures++;
	}
	if (s_renderstate->use_shadowmap && receive_shadow) {
		shader_format |= SHADER_FORMAT_SHADOWMAP;
		num_textures++;
	}
	if (s_renderstate->use_alpha_fog) {
		shader_format |= SHADER_FORMAT_FOG;
	}

	if (s_renderstate->use_multipass) {
		if (num_textures > 2) { // it is for two-pass only
			if (s_renderstate->current_pass == 0) {
                shader_format &= (SHADER_FORMAT_FIRSTMAP | SHADER_FORMAT_SECONDMAP | SHADER_FORMAT_FOG);
			}
			else {
				shader_format &= (SHADER_FORMAT_LIGHTMAP | SHADER_FORMAT_SHADOWMAP);
			}
		}
		else if (s_renderstate->current_pass != 0) {
			shader_format = SHADER_FORMAT_DEFAULT; // have done
		}
	}
	return shader_format;
}


void zz_material_terrain_rough::begin ()
{
	color_state = MS_NONE;
	alpha_state = MS_NONE;

	s_state.init();

	for (int i = 0; i < ZZ_MAX_TEXTURESTAGE; i++) {
		s_state.texture_address[i] = ZZ_TADDRESS_CLAMP;
	}
	s_state.cullmode = zz_render_state::ZZ_CULLMODE_CCW; // uses clockwise welding. see zz_mesh_tool.cpp

	zz_light * light = znzin->get_light();
	zz_assert(light);

	s_state.tfactor = zz_color(light->diffuse.r, light->diffuse.g, light->diffuse.b, light->diffuse.a);

	apply_shared_property(1);

	s_renderer->enable_alpha_test( true, 254, ZZ_CMP_LESSEQUAL );
	s_renderer->enable_alpha_blend(true, ZZ_BT_CUSTOM);
	s_renderer->set_blend_type(ZZ_BLEND_INVSRCALPHA, ZZ_BLEND_SRCALPHA, ZZ_BLENDOP_ADD);
}

void zz_material_terrain_rough::end ()
{
	s_renderer->enable_alpha_test( false );
}


bool zz_material_terrain_rough::set (int pass)
{
	zz_texture * firstmap;

	int shader_format = get_shader_format();

	if (shader_format == SHADER_FORMAT_DEFAULT) return false;

	firstmap = (shader_format & SHADER_FORMAT_FIRSTMAP) ? textures[0] : 0;

	// texture binding
	int texture_stage = 0;

	if (firstmap) firstmap->set(texture_stage++);

	set_first_rough();
	set_blend_rterrain_alpha_stage(); // it's blended alpha rough terrain material
	return true;
}
