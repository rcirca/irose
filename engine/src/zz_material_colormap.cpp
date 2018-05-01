/** 
 * @file zz_material_colormap.h
 * @brief material colormap class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    23-jul-2002
 *
 * $Header: /engine/src/zz_material_colormap.cpp 41    04-09-14 11:21p Zho $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_texture.h"
#include "zz_material_colormap.h"
#include "zz_system.h"
#include "zz_renderer.h"
#include "zz_renderer_d3d.h"
#include "zz_shader.h"
#include "zz_bone.h" // for ZZ_DRAW_BONE

// texture slot index for colormap (textures)
#define DIFFUSEMAP_SLOT		0
#define SPECULARMAP_SLOT	1
#define LIGHTMAP_SLOT			2
#define SHADOWMAP_SLOT	3
#define NUM_TEXTURE_SLOT	4

ZZ_IMPLEMENT_DYNCREATE(zz_material_colormap, zz_material)

zz_material_colormap::zz_material_colormap (void) : zz_material(1)
{
}

zz_material_colormap::~zz_material_colormap (void)
{
	// CAUTION: DO NOT INSERT SUB OBJECT RELEASE()
	// because we need auto-cleaning without deleting dependent items
	// in manager destructor
}

void zz_material_colormap::_set_colormap_specularmap (void)
{ 
	// rgba = diffusemap * diffuselight
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	if (use_light) { // use light
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_MODULATE);
	}
	else {
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1);
	}

	// rgba = arg1_rgb(current) + arg1_a(diffusemap_alpha) x arg2_rgb(irradiance_spheremap)
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_MODULATEALPHA_ADDCOLOR);

	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_tfactor_texture_alpha (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG1, ZZ_TA_TFACTOR);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_SELECTARG1);

	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAARG1, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAARG2, ZZ_TA_TEXTURE); // alpha must be 1
	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAOP,   ZZ_TOP_MODULATE);

	s_renderer->set_texture_stage_state(2, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_texture_texture_alpha (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG2, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_SELECTARG1);

	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAARG1, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAARG2, ZZ_TA_TEXTURE); // alpha must be 1
	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAOP,   ZZ_TOP_SELECTARG2);

	s_renderer->set_texture_stage_state(2, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_irradiancemap (void)
{ 
	// rgb[1] = diffusemap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1);

	// rgb[2] = arg1_rgb(current) x arg2_rgb(irradiance_spheremap)
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_MODULATE);
	//s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_SELECTARG2);
	
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_texture_x_tfactor_alpha (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG2, ZZ_TA_TFACTOR);			
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_MODULATE);
	
	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_texture_alpha (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_SELECTARG1);

	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_tfactor_alpha (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG2, ZZ_TA_TFACTOR);			
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_SELECTARG2);

	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_nop_alpha (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_texture_x_diffuse_alpha (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG2, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_MODULATE);
	
	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE); 
}

void zz_material_colormap::_set_diffuse_alpha (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAARG2, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_ALPHAOP,   ZZ_TOP_SELECTARG2);
	
	s_renderer->set_texture_stage_state(1, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE); 
}

void zz_material_colormap::_set_tfactor_colormap (void)
{ 
	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_TFACTOR);
	
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);

	if (use_light) { // use light
		s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
		s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_CURRENT);
		s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_MODULATE);
	}
	else {
		s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1);
	}

	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_colormap (void)
{ 
	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	if (use_light) { // use light
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_MODULATE);
	}
	else {
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1);
	}

	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_glow (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TFACTOR);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1);
	
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_glow_lit (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TFACTOR);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_MODULATE);
	
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_glow_texture (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TFACTOR);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_MODULATE);
	
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_glow_texture_lit (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_TFACTOR);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_MODULATE);

	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_MODULATE);
	
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_glow_alpha (void)
{
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_TFACTOR);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG2);

	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_shadowmap (void)
{
	s_state.texture_address[0] = ZZ_TADDRESS_CLAMP;
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);

	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_colormap_lightmap (void)
{
	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	if (use_light) {
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	}
	else {
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);
	}

	// lightmap stage
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, s_renderstate->lightmap_blend_style);

	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_colormap_shadowmap (void)
{
	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	if (use_light) {
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	}
	else {
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);
	}

	// secondmap
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG2);
	
	// lightmap stage
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG2);

	// shadowmap stage
	// shadow map uses only 3rd 3
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG2);
	s_state.texture_address[3] = ZZ_TADDRESS_CLAMP;

	s_renderer->set_texture_stage_state(4, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_colormap_lightmap_shadowmap(void)
{
	// colormap
	s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	if (use_light) {
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE);
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	}
	else {
		s_renderer->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1);
	}
	
	// lightmap stage
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(1, ZZ_TSS_COLOROP, znzin->get_rs()->lightmap_blend_style);

	// shadowmap stage
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLORARG2, ZZ_TA_CURRENT);
	s_renderer->set_texture_stage_state(2, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	s_state.texture_address[2] = ZZ_TADDRESS_CLAMP;

	s_renderer->set_texture_stage_state(3, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);
}

void zz_material_colormap::_set_target_shadowmap (void)
{
	s_renderer->enable_alpha_blend(false, state.blend_type);
	s_renderer->enable_alpha_test(false);
	_set_shadowmap();
	_set_nop_alpha();
}

void zz_material_colormap::_set_target_glow (void)
{
	s_state.copy_from(state);

	// set glow color as tfactor
	s_state.tfactor = state.glow_color;

	bool use_glow_texture = texturealpha || 
		(state.glow_type == ZZ_GLOW_TEXTURE) ||
		(state.glow_type == ZZ_GLOW_TEXTURE_LIGHT) ||
		(state.glow_type == ZZ_GLOW_ALPHA);

	if (use_glow_texture) {
		zz_texture * diffusemap = get_texture(DIFFUSEMAP_SLOT);

		if (diffusemap) {
			diffusemap->set(DIFFUSEMAP_SLOT); // use diffusemap only
		}
	}

	if (state.glow_type == ZZ_GLOW_ALPHA) {
		s_state.alpha_blend = true;
		s_state.blend_type = ZZ_BT_NORMAL;
	}
	else {
		s_state.alpha_blend = (texturealpha && !use_specular);
	}

	if (s_state.alpha_blend) {
		_set_texture_alpha();
	}
	else {
		_set_nop_alpha();
	}

	switch (state.glow_type)
	{
	case ZZ_GLOW_NONE:
			return;
			break;
	case ZZ_GLOW_SIMPLE:
		_set_glow();
		break;
	case ZZ_GLOW_LIGHT:
		_set_glow_lit();
		break;
	case ZZ_GLOW_TEXTURE:
		_set_glow_texture();
		break;
	case ZZ_GLOW_TEXTURE_LIGHT:
		_set_glow_texture_lit();
		break;
	case ZZ_GLOW_ALPHA:
		_set_glow_alpha();
		break;
	}

	s_renderer->set_render_state(ZZ_RS_TEXTUREFACTOR, s_state.tfactor );
	s_renderer->enable_alpha_blend(s_state.alpha_blend, s_state.blend_type);
}

int zz_material_colormap::get_shader_format () const
{
	int shader_format(0);
	int num_exist_textures(0);

	if (get_texture(DIFFUSEMAP_SLOT)) {
		shader_format |= SHADER_FORMAT_FIRSTMAP;
		num_exist_textures++;
	}

	if (use_specular && get_texture(SPECULARMAP_SLOT)) { // specular ignores receive_fog
		shader_format |= SHADER_FORMAT_SPECULARMAP;
		num_exist_textures++;
	}
	if (s_renderstate->use_lightmap && get_texture(LIGHTMAP_SLOT)) {
		shader_format |= SHADER_FORMAT_LIGHTMAP;
		num_exist_textures++;
	}
	if (!receive_fog && s_renderstate->use_shadowmap && receive_shadow) {
		shader_format |= SHADER_FORMAT_SHADOWMAP;
		num_exist_textures++;
	}

	if (s_renderstate->use_multipass) {
		if (num_exist_textures > 2) {
			// filter out with available format in current pass
			if (s_renderstate->current_pass == 0)
                shader_format &= (SHADER_FORMAT_FIRSTMAP | SHADER_FORMAT_LIGHTMAP);
			else
				shader_format &= SHADER_FORMAT_SHADOWMAP;
		}
		else if (s_renderstate->current_pass != 0)
			shader_format = SHADER_FORMAT_INVALID; // have done
	}
	return shader_format;
}

void zz_material_colormap::bind_texture ()
{
	int shader_format = get_shader_format();

	assert(shader_format != SHADER_FORMAT_INVALID);

	zz_texture * diffusemap = NULL, * specularmap = NULL, * lightmap = NULL;

	diffusemap = (shader_format & SHADER_FORMAT_FIRSTMAP) ? get_texture(DIFFUSEMAP_SLOT) : 0;
	specularmap = (shader_format & SHADER_FORMAT_SPECULARMAP) ? get_texture(SPECULARMAP_SLOT) : 0;
	lightmap = (shader_format & SHADER_FORMAT_LIGHTMAP) ? get_texture(LIGHTMAP_SLOT) : NULL;

	bool has_shadowmap = (receive_shadow && (shader_format & SHADER_FORMAT_SHADOWMAP));

	state.texture_binding[DIFFUSEMAP_SLOT] = (diffusemap != NULL);
	state.texture_binding[SPECULARMAP_SLOT] = (specularmap != NULL);
	state.texture_binding[LIGHTMAP_SLOT] = (lightmap != NULL);
	state.texture_binding[SHADOWMAP_SLOT] = has_shadowmap;

	int texture_stage = 0;
	// set diffuse texture
	if (diffusemap) {
		diffusemap->set(texture_stage++);
	}
	if (specularmap) {
		assert(!lightmap && !has_shadowmap);
		specularmap->set(texture_stage++);
	}
	if (lightmap) {
		lightmap->set(texture_stage++); // lightmap have to be in stage 1
		s_renderer->set_vertex_shader_constant(ZZ_VSC_LIGHTMAP_TRANSFORM, lightmap_transform, 1);
	}
	if (has_shadowmap) {
		s_renderer->set_texture_shadowmap(texture_stage++);
	}
	
	//for (; texture_stage < ZZ_MAX_TEXTURESTAGE; ++texture_stage) {
	//	s_renderer->set_texture(ZZ_HANDLE_NULL, texture_stage, 0);
	//}
}

void zz_material_colormap::_set_pass_fogged (void)
{
	_set_texture_x_diffuse_alpha();
	_set_colormap();
}

void zz_material_colormap::_set_pass_diffusemap (void)
{
	bool colorblend = false;
	int factor = int(255.0f*state.alphavalue);

	// check if colorblend is on.
	if ((state.blend_src == ZZ_BLEND_SRCCOLOR) || (state.blend_src == ZZ_BLEND_INVSRCCOLOR)) {
		colorblend = true;
	}

	if (state.alphavalue < 1.0f) {
		s_state.alpha_blend = true;
		s_state.alpha_test = true;
		s_state.alpha_ref = 10;

		if (colorblend) { // affects all channels
			s_state.tfactor = ZZ_COLOR_RGBA(factor, factor, factor, factor);
		}
		else { // only for alpha channel
			s_state.tfactor = ZZ_COLOR_RGBA(255, 255, 255, factor);
		}

		if (texturealpha)
			_set_texture_x_tfactor_alpha();
		else if (colorblend) {
			_set_nop_alpha();
			_set_tfactor_colormap();
			return;
		}
		else {
			_set_tfactor_alpha();
		}
	}
	else if (texturealpha) {
		if (receive_fog) {
			s_state.alpha_blend = true;
			s_state.alpha_test = true;
			s_state.alpha_ref = 32; // do small alpha test for fogged objects.
			_set_texture_x_diffuse_alpha();
		}
		else {
			_set_texture_alpha();
		}
	}
	else if (colorblend) {
		s_state.alpha_blend = true;
		if (receive_fog) {
			s_state.alpha_blend = true;
			s_state.alpha_test = true;
			s_state.alpha_ref = 32; // do small alpha test for fogged objects.
			_set_diffuse_alpha();
		}
		else {
			_set_nop_alpha();
		}
	}
	else {
		if (receive_fog) {
			s_state.alpha_blend = true;
			s_state.alpha_test = true;
			s_state.alpha_ref = 32; // do small alpha test for fogged objects.
			_set_diffuse_alpha();
		}
		else {
			_set_nop_alpha();
		}
	}
	_set_colormap();
}

void zz_material_colormap::_set_pass_diffusemap_specularmap (void)
{
	if (state.alphavalue < 1.0f) {
		s_state.alpha_blend = true;
		s_state.alpha_test = true;
		s_state.alpha_ref = state.alpha_ref;
		s_state.blend_src = ZZ_BLEND_BLENDFACTOR;
		s_state.blend_dest = ZZ_BLEND_INVBLENDFACTOR;
		s_state.blend_op = ZZ_BLENDOP_ADD;
		s_state.blend_type = ZZ_BT_CUSTOM;
		s_state.blend_factor = ZZ_COLOR32_ARGB(0xff,
			static_cast<unsigned short>(255.0f*state.alphavalue),
			static_cast<unsigned short>(255.0f*state.alphavalue),
			static_cast<unsigned short>(255.0f*state.alphavalue));

		_set_texture_texture_alpha();
	}
	else if (texturealpha) {
		_set_texture_texture_alpha(); // first stage texture is dummy setting
	}
	else {
		_set_nop_alpha();
	}
	_set_colormap_specularmap();
}

// colormap + lightmap
void zz_material_colormap::_set_pass_diffusemap_lightmap (void)
{
	if (state.alphavalue < 1.0f) {
		s_state.tfactor = ZZ_COLOR_RGBA(255, 255, 255, int(255.0f*state.alphavalue));
		s_state.alpha_blend = true;
		s_state.alpha_test = true;
		s_state.alpha_ref = 10;	
		_set_texture_x_tfactor_alpha();
	}
	else if (texturealpha) {
		if (receive_fog) {
			s_state.alpha_blend = true;
			s_state.alpha_test = true;
			s_state.alpha_ref = 32; // do small alpha test for fogged objects.
			_set_texture_x_diffuse_alpha();
		}
		else _set_texture_alpha();
	}
	else {
		if (receive_fog) {
			s_state.alpha_blend = true;
			s_state.alpha_test = true;
			s_state.alpha_ref = 32; // do small alpha test for fogged objects.
			_set_diffuse_alpha();
		}
		else _set_nop_alpha();
	}
	_set_colormap_lightmap();
}

// only multi shadowmap
void zz_material_colormap::_set_pass_shadowmap (void)
{
	s_state.alpha_blend = true;
	s_state.blend_type = ZZ_BT_CUSTOM;
	s_state.blend_src = ZZ_BLEND_ZERO;
	s_state.blend_dest = ZZ_BLEND_SRCCOLOR;
	s_state.blend_op = ZZ_BLENDOP_ADD;

	_set_nop_alpha();
	_set_shadowmap();
}

void zz_material_colormap::_set_pass_diffusemap_shadowmap (void)
{
	if (texturealpha) {
		_set_texture_alpha();
	}
	else {
		_set_nop_alpha();
	}
	_set_colormap_shadowmap();
}

void zz_material_colormap::_set_pass_diffusemap_lightmap_shadowmap (void)
{
	if (state.alphavalue < 1.0f) {
		int factor = int(255.0f*state.alphavalue);
		s_state.tfactor = ZZ_COLOR_RGBA(factor, factor, factor, factor);
		s_state.alpha_blend = true;
		s_state.alpha_test = true;
		s_state.alpha_ref = 10;	
		_set_texture_x_tfactor_alpha();
	}
	else if (texturealpha) {
		_set_texture_alpha();
	}
	else {
		_set_nop_alpha();
	}
	_set_colormap_lightmap_shadowmap();
}

// no lightmap, no shadowmap
bool zz_material_colormap::set (int pass)
{
	assert(znzin);

	// do shadowmap or glow texture stage setup
	switch (s_renderer->get_render_where())
	{
	case ZZ_RW_SHADOWMAP:
		_set_target_shadowmap();
		return true;
		// RW_GLOW is same as normal material
	case ZZ_RW_GLOW:
		//ZZ_LOG("material_colormap: glow pass(%s)\n", get_texture(0)->get_path());
		if (state.glow_type == ZZ_GLOW_NONE) {
			return false;
		}
		_set_target_glow();
		return true;
	}

	s_state.copy_from(state);
	
	bind_texture();

	if (state.texture_binding[SPECULARMAP_SLOT]) {
		_set_pass_diffusemap_specularmap();
	}
	else if (state.texture_binding[DIFFUSEMAP_SLOT]) {
        if (state.texture_binding[LIGHTMAP_SLOT]) {
			if (state.texture_binding[SHADOWMAP_SLOT]) {
				_set_pass_diffusemap_lightmap_shadowmap();
			}
			else {
				_set_pass_diffusemap_lightmap();
			}
		}
		else if (state.texture_binding[SHADOWMAP_SLOT]) {
			_set_pass_diffusemap_shadowmap();
		}
		else {
			_set_pass_diffusemap();
		}
	}
	else if (state.texture_binding[SHADOWMAP_SLOT]) {
		_set_pass_shadowmap();
	}
	else {
		_set_pass_diffusemap();
	}

	apply_shared_property(NUM_TEXTURE_SLOT);
	return true;
}
