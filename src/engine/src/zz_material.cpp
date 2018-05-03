/** 
 * @file zz_material.cpp
 * @brief material class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    17-may-2002
 *
 * $Header: /engine/src/zz_material.cpp 31    05-05-12 10:16a Choo0219 $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_shader.h"
#include "zz_system.h"
#include "zz_renderer_d3d.h"
#include "zz_manager.h"
#include "zz_texture.h"
#include "zz_material.h"
#include "memory.h" // for memcpy

ZZ_IMPLEMENT_DYNAMIC(zz_material, zz_node)

zz_renderer * zz_material::s_renderer = NULL;
zz_render_state * zz_material::s_renderstate = NULL;
zz_material_state zz_material::s_state;

void zz_material_state::init ()
{
	cullmode = zz_render_state::ZZ_CULLMODE_CW;
	blend_src = ZZ_BLEND_ONE;
	blend_dest = ZZ_BLEND_ZERO;
	blend_op = ZZ_BLENDOP_ADD;
	blend_type = ZZ_BT_NORMAL;
	alpha_blend  = false;
	alpha_test = true;
	alpha_ref = 128;
	ztest = true;
	zwrite = true;
	tfactor = 0xffffffff;
	blend_factor = 0xffffffff;
	glow_color = 0xffffffff;
	alphavalue = 1.0f;
	glow_type = ZZ_GLOW_NONE;

	// make wrap as default for 0 stage
	texture_address[0] = ZZ_TADDRESS_WRAP;
	texture_binding[0] = false;

	for (int i = 1; i < ZZ_MAX_TEXTURESTAGE; i++) {
		texture_address[i] = ZZ_TADDRESS_CLAMP;
		texture_binding[i] = false;
	}
}

void zz_material_state::copy_to (zz_material_state& dest)
{
	memcpy(this, &dest, sizeof(zz_material_state));
}

void zz_material_state::copy_from (const zz_material_state& src)
{
	memcpy(this, &src, sizeof(zz_material_state));
}

zz_material::zz_material(int texture_size) : 
	shader(NULL),
	texturealpha(false),
	objectalpha(1.0f),
	cast_shadow(false),
	receive_shadow(false),
	receive_fog(true),
	textures(texture_size),
	use_light(true),
	use_specular(false),
	glow_type(ZZ_GLOW_NONE),
	glow_color(vec3_one),
	materialalpha(1.0f)
{
	state.init();
}

zz_material::~zz_material(void)
{
	ZZ_SAFE_RELEASE(shader);
	zz_texture * tex;
	for (int i = 0; i < (int)textures.size(); i++) {
		// CAUTION: do not simply release texture
		// texture object is created only by material, not by user (ex. loadTexture()...)
		tex = textures[i];
		if (tex) {
			znzin->textures->kill(tex);
		}
	}
}

void zz_material::apply_shared_property (int num_stage)
{
	s_renderer->enable_zbuffer(s_state.ztest);
	
	s_renderer->set_zfunc(s_state.ztest ? ZZ_CMP_LESSEQUAL : ZZ_CMP_ALWAYS);

	s_renderer->enable_zwrite(s_state.zwrite);
	
	s_renderer->set_render_state(ZZ_RS_TEXTUREFACTOR, s_state.tfactor );

	s_renderer->set_render_state(ZZ_RS_BLENDFACTOR, s_state.blend_factor);

	s_renderer->enable_alpha_blend(s_state.alpha_blend, s_state.blend_type);
	if (s_state.alpha_blend) {
		s_renderer->enable_alpha_test(s_state.alpha_test, s_state.alpha_ref);
		// set alpha blend property
		// some materials has false-texturealpha but uses color-alpha, thus needs blend option
		if (s_state.blend_type == ZZ_BT_CUSTOM) {
			s_renderer->set_blend_type(
				static_cast<ZZ_BLEND>(s_state.blend_src),
				static_cast<ZZ_BLEND>(s_state.blend_dest), 
				static_cast<ZZ_BLENDOP>(s_state.blend_op)
				);
		}
	}

	s_renderer->set_cullmode(s_state.cullmode);

	// set texture address mode
	for (int i = 0; i < num_stage; ++i) {
		s_renderer->set_sampler_state(i, ZZ_SAMP_ADDRESSU, s_state.texture_address[i]);
		s_renderer->set_sampler_state(i, ZZ_SAMP_ADDRESSV, s_state.texture_address[i]);
	}
}

void zz_material::set_blendtype (int blend_src_in, int blend_dest_in, int blend_op_in)
{
	state.blend_src = blend_src_in;
	state.blend_dest = blend_dest_in;
	state.blend_op = blend_op_in;
	state.blend_type = ZZ_BT_CUSTOM;
}

bool zz_material::push_texture (const char * filename)
{
	if (!filename) {
		_push_texture((zz_texture*)NULL);
		return false;
	}
    zz_texture * tex = (zz_texture*)znzin->textures->find(filename);
	
	if (tex) {
		tex->addref();
	}
	else {
		tex = (zz_texture*)znzin->textures->spawn(filename, ZZ_RUNTIME_TYPE(zz_texture), false /* not to autoload */);
		// same codeblock as set_texture()
		assert(tex);
		tex->set_path(filename);
	}

	return _push_texture((zz_texture*)tex);
}


bool zz_material::set_texture (int index, const char * filename)
{
	//ZZ_LOG("material:[%s]->set_texture(%d, %s)\n", get_name(), index, filename);
	if (!filename) {
		return _set_texture(index, (zz_texture*)NULL);
	}
    zz_texture * tex = (zz_texture*)znzin->textures->find(filename);
	
	if (tex) {
		tex->addref();
	}
	else {
		tex = (zz_texture*)znzin->textures->spawn(filename, ZZ_RUNTIME_TYPE(zz_texture), false /* not to autoload */);
		// same codeblock as push_texture()
		assert(tex);
	}

	if (!tex->get_path()) { // if not already set.
		tex->set_property(
			filename, 0, 0, false /* dynamic */, -1 /* miplevels */, true /* usefilter */, 
			zz_device_resource::ZZ_POOL_MANAGED,
			ZZ_FMT_UNKNOWN, false /* for_image */ );
	}
	return _set_texture(index, (zz_texture*)tex);
}

bool zz_material::set_texture (int index, int width, int height)
{
	// CAUTION: cannot check if already same texture

    zz_texture * tex = (zz_texture*)znzin->textures->spawn(NULL, ZZ_RUNTIME_TYPE(zz_texture), false /* not to autoload */);

	assert(tex);
	tex->set_property(NULL, width, height, true /* dynamic */, 1 /* miplevels */, false /* use_filter */, 
		zz_device_resource::ZZ_POOL_DEFAULT, ZZ_FMT_A8R8G8B8, true /* for_image */ );

	if (_set_texture(index, (zz_texture*)tex)) {
		// force immediate load
		znzin->textures->flush_entrance(tex);
		tex->lock_texture();
		return true;
	}
	return false;
}

bool zz_material::_set_texture (int index, zz_texture * tex)
{
	int num_textures = (int)textures.size();

	for (int i = num_textures-1; i < index; ++i) {
		textures.push_back(NULL);
	}

	zz_texture * old = textures[index];

	// release old, and addref it!
	if (tex == old) return true; // nothing to do

	if (old) {
		znzin->textures->kill(old); // CAUTION: Never ZZ_SAFE_RELEASE(old)! Because that the texture is created only by material.
	}
	textures[index] = tex;

	if (tex) {
		znzin->textures->load(tex);
	}
	return true;
}

bool zz_material::_push_texture (zz_texture * tex)
{
	int index = (int)textures.size();

	textures.push_back(NULL);
	
	textures[index] = tex;
	
	if (tex) {
		znzin->textures->load(tex);
		tex->lock_texture(); // zhotest as this push_texture is only used in ocean material for now.
	}
	return true;
}

// reference increased
bool zz_material::set_texture (int index, zz_texture * tex)
{
	if (tex) {
		tex->addref();
	}
	return _set_texture(index, tex);
}

// reference increased
bool zz_material::push_texture (zz_texture * tex)
{
	if (tex) {
		tex->addref();
	}
	return _push_texture(tex);
}

void zz_material::set_shader (zz_shader * shader_in)
{
	if (shader) {
		shader->release();
	}
	shader = shader_in;
	shader->addref();
}


bool zz_material::get_device_updated () const
{
	zz_texture * tex;
	size_t count = textures.size();
	for (size_t i = 0; i < count; ++i) {
		tex = textures[i];
		if (tex && !tex->get_device_updated()) {
			return false;
		}
	}
	return true;
}

bool zz_material::flush_device (bool immediate)
{
	zz_texture * tex;
	bool ok = true;
	for (size_t i = 0, count = textures.size(); i < count; ++i) {
		tex = textures[i];
		if (!tex)
			continue;
		if (!tex->flush_device(immediate))
			ok = false;
	}
	return ok;
}

void zz_material::set_lod ( const zz_lod& lod_in )
{
	lod = lod_in;
}

void zz_material::set_load_priroty ( int priority_in )
{
	for (std::vector<zz_texture*>::iterator it = textures.begin(), it_end = textures.end();
		it != it_end; ++it)
	{
		if (*it) {
			(*it)->set_load_priority(priority_in);
		}
	}
}

void zz_material::reset_texture(void)
{
	
	zz_texture * tex;
	for (int i = 0; i < (int)textures.size(); i++) {
		
		tex = textures[i];
		if (tex) {
			znzin->textures->kill(tex);
		
		}
	}
    textures.clear();
    textures.push_back(NULL);
}