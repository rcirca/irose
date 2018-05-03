/** 
 * @file zz_font_texture_d3d.CPP
 * @brief d3d font texture d3d class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    22-sep-2004
 *
 * $Header: /engine/src/zz_font_texture_d3d.cpp 10    04-10-19 4:23p Zho $
 */

#include "zz_tier0.h"
#include "zz_font_texture_d3d.h"
#include "zz_font_d3d.h"
#include "zz_renderer_d3d.h"
#include "zz_system.h"

// simple version
// with RHW, no need to SetTransform()
struct FONT_TEXT_VERTEX4_TEXTURE
{
    D3DXVECTOR4 position;
    D3DCOLOR color;
    float tu, tv;

    static DWORD FVF;
};
DWORD FONT_TEXT_VERTEX4_TEXTURE::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

// sprite TM multiplied version
// need SetTransform()
struct FONT_TEXT_VERTEX3_TEXTURE
{
    D3DXVECTOR3 position;
    D3DCOLOR color;
    float tu, tv;

    static DWORD FVF;
};
DWORD FONT_TEXT_VERTEX3_TEXTURE::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

// to clear text
struct FONT_TEXT_VERTEX_DIFFUSE
{
    D3DXVECTOR4 position;
    D3DCOLOR color;

    static DWORD FVF;
};
DWORD FONT_TEXT_VERTEX_DIFFUSE::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

LPDIRECT3DDEVICE9 zz_font_texture_d3d::_device;

zz_font_texture_d3d::zz_font_texture_d3d () : 
	 _texture(NULL), _surface(NULL), zz_font_texture()
{
}

zz_font_texture_d3d::~zz_font_texture_d3d ()
{
}

// create rendertarget texture and surface
bool zz_font_texture_d3d::restore_device_objects ()
{
	if (tex_height <= 0) return true;
	if (tex_width <= 0) return true;


	if (!znzin) return true;
	if (!znzin->renderer) return true;

	zz_renderer_d3d * r = static_cast<zz_renderer_d3d*>(znzin->renderer);
	if (!r->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d))) return false;

	_device = r->get_device();
	if (!_device) return true;

	HRESULT hr;

	// create rendertarget texture
	if (FAILED(hr = _device->CreateTexture(
		tex_width, tex_height, 1, // level
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT, // rendertarget must set this to D3DPOOL_DEFAULT
		&_texture, NULL)))
	{
		zz_render_state * rs = znzin->get_rs();
		if (rs) {
			ZZ_LOG("font_texture_d3d: createtexture(%dx%d)-(%dx%dx%d) failed(%s)\n",
				tex_width, tex_height,
				rs->buffer_width, rs->buffer_height, rs->buffer_depth,
				zz_renderer_d3d::get_hresult_string(hr));
		}
		else {
			ZZ_LOG("font_texture_d3d: createtexture() failed(%s). no rs\n", zz_renderer_d3d::get_hresult_string(hr));
		}
		return false;
	}

	// get texture surface
	if (FAILED(hr = _texture->GetSurfaceLevel(0, &_surface))) {
		zz_assert(0);
		return false;
	}

	return true;
}

bool zz_font_texture_d3d::init_device_objects ()
{
	return true;
}

bool zz_font_texture_d3d::invalidate_device_objects ()
{
	SAFE_RELEASE(_texture);
	SAFE_RELEASE(_surface);

	reset_all();

	return true;
}

bool zz_font_texture_d3d::delete_device_objects ()
{
	return true;
}

// render text from texture
void zz_font_texture_d3d::render_text_impl (const zz_font& font, const zz_font_text& text)
{
	const RECT& rect = text.rect;
	float u, v;
	float width, height;

	u = float(text.tex_rect.left) / this->tex_width;
	v = float(text.tex_rect.top + line_height) / tex_height; // should be aligned by tex_rect.top
	width = float(text.rect.right - text.rect.left) / tex_width;
	height = float(line_height)/ tex_height;
	assert(line_height <= (text.tex_rect.bottom - text.tex_rect.top));

	float top = (float)rect.top;
	float bottom = (float)top + line_height;

	HRESULT hr;

	zz_renderer_d3d * r = static_cast<zz_renderer_d3d*>(znzin->renderer);
	assert(r);

	//assert(SUCCEEDED(hr));
	D3DXMATRIX saved_view, saved_proj, saved_world;
	D3DVIEWPORT9 saved_viewport;
	D3DXMATRIX sprite_tm; // sprite transform

	// we assume that init_sprite_state() has already been called.

	if (text.use_sprite) { // save states
		assert(r->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));
		LPD3DXSPRITE sprite = r->get_sprite();
		assert(sprite);

		_device->GetTransform(D3DTS_VIEW, &saved_view);
		_device->GetTransform(D3DTS_PROJECTION, &saved_proj);
		_device->GetTransform(D3DTS_WORLD, &saved_world);
		_device->GetViewport(&saved_viewport);
		r->get_sprite_transform((float*)&sprite_tm); // save transform
		r->flush_sprite(); // flushing out previous sprite rendering to avoid rendering disorder
	}
	else { // not using sprite
		D3DXMatrixIdentity(&sprite_tm); // get identity matrix for sprite_tm
	}

	r->set_cullmode(zz_render_state::ZZ_CULLMODE_CW);
	
	r->invalidate_texture( 0 ); // invalidate 0-th texture to call SetTexture directly.
	hr = _device->SetTexture( 0, _texture );  
	assert(SUCCEEDED(hr));

	if (text.use_sprite) {
		FONT_TEXT_VERTEX3_TEXTURE vertices3 [4] =
		{
			D3DXVECTOR3((float)rect.left,		(float)bottom,	.0f), text.color, u,  v, 
			D3DXVECTOR3((float)rect.right,	(float)bottom,	.0f), text.color, u + width, v,
			D3DXVECTOR3((float)rect.right,	(float)top,			.0f), text.color, u + width, v - height, 
			D3DXVECTOR3((float)rect.left,		(float)top,			.0f), text.color, u, v - height, 
		};
		const int hm = -1, hp = 1;
		const int vm = -1, vp = 1;
		const float adjm = .5f, adjp = -.5f;
		FONT_TEXT_VERTEX3_TEXTURE vertices3_outline_simple [22] =
		{
			D3DXVECTOR3((float)(rect.left+hm)+adjm,			(float)(top+vm)+adjm,				.0f ), text.color_outline, u, v - height, 
			D3DXVECTOR3((float)(rect.left+hm)+adjm,			(float)(bottom+vm)+adjm,		.0f ), text.color_outline, u,  v, 
			D3DXVECTOR3((float)(rect.right+hm)+adjm,		(float)(top+vm)+adjm	,			.0f ), text.color_outline, u + width, v - height, 
			D3DXVECTOR3((float)(rect.right+hm)+adjm,		(float)(bottom+vm)+adjm,		.0f ), text.color_outline, u + width, v,
			D3DXVECTOR3((float)(rect.right+hm)+adjm,		(float)(bottom+vm)+adjm,		.0f ), text.color_outline, u + width, v,

			D3DXVECTOR3((float)(rect.left+hp)+adjp,			(float)(top+vp)+adjp,				.0f ), text.color_outline, u, v - height, 
			D3DXVECTOR3((float)(rect.left+hp)+adjp,			(float)(top+vp)+adjp,				.0f ), text.color_outline, u, v - height, 
			D3DXVECTOR3((float)(rect.left+hp)+adjp,			(float)(bottom+vp)+adjp,			.0f ), text.color_outline, u,  v, 
			D3DXVECTOR3((float)(rect.right+hp)+adjp,			(float)(top+vp)+adjp,				.0f ), text.color_outline, u + width, v - height, 
			D3DXVECTOR3((float)(rect.right+hp)+adjp,			(float)(bottom+vp)+adjp,			.0f ), text.color_outline, u + width, v,
			D3DXVECTOR3((float)(rect.right+hp)+adjp,			(float)(bottom+vp)+adjp,			.0f ), text.color_outline, u + width, v,

			D3DXVECTOR3((float)(rect.left+hm)+adjm,			(float)(top+vp)+adjp,				.0f ), text.color_outline, u, v - height, 
			D3DXVECTOR3((float)(rect.left+hm)+adjm,			(float)(top+vp)+adjp,				.0f ), text.color_outline, u, v - height, 
			D3DXVECTOR3((float)(rect.left+hm)+adjm,			(float)(bottom+vp)+adjp,			.0f ), text.color_outline, u,  v, 
			D3DXVECTOR3((float)(rect.right+hm)+adjm,		(float)(top+vp)+adjp,				.0f ), text.color_outline, u + width, v - height, 
			D3DXVECTOR3((float)(rect.right+hm)+adjm,		(float)(bottom+vp)+adjp,			.0f ), text.color_outline, u + width, v,
			D3DXVECTOR3((float)(rect.right+hm)+adjm,		(float)(bottom+vp)+adjp,			.0f ), text.color_outline, u + width, v,

			D3DXVECTOR3((float)(rect.left+hp)+adjp,			(float)(top+vm)+adjm,				.0f ), text.color_outline, u, v - height, 
			D3DXVECTOR3((float)(rect.left+hp)+adjp,			(float)(top+vm)+adjm,				.0f ), text.color_outline, u, v - height, 
			D3DXVECTOR3((float)(rect.left+hp)+adjp,			(float)(bottom+vm)+adjm,		.0f ), text.color_outline, u,  v, 
			D3DXVECTOR3((float)(rect.right+hp)+adjp,			(float)(top+vm)+adjm,				.0f ), text.color_outline, u + width, v - height, 
			D3DXVECTOR3((float)(rect.right+hp)+adjp,			(float)(bottom+vm)+adjm,		.0f ), text.color_outline, u + width, v,
		};

		_device->SetTransform(D3DTS_WORLD, &sprite_tm);

		hr = _device->SetFVF( FONT_TEXT_VERTEX3_TEXTURE::FVF );
		assert(SUCCEEDED(hr));

		try {
			if (font.get_outline_type() == zz_font::OUTLINE_TYPE_SIMPLE) {
				hr = _device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 20, vertices3_outline_simple, sizeof(FONT_TEXT_VERTEX3_TEXTURE) );
				assert(SUCCEEDED(hr));
			}
			hr = _device->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices3, sizeof(FONT_TEXT_VERTEX3_TEXTURE) );
		}
		catch (...) {
			// do nothing. maybe device lost state
			ZZ_LOG("font_texture_d3d: render_tex_impl(). dp exception\n");
		}

		assert(SUCCEEDED(hr));

		if (r->get_state()->use_draw_text_rect) {
			// changing render states
			{
				r->set_texture_stage_state( 0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG2 ); // DIFFUSE
				r->set_texture_stage_state( 0, ZZ_TSS_ALPHAOP, ZZ_TOP_DISABLE );
			}
			try {
				_device->DrawPrimitiveUP( D3DPT_LINESTRIP, 3, vertices3, sizeof(FONT_TEXT_VERTEX3_TEXTURE) );
			}
			catch (...) {
				ZZ_LOG("font_texture_d3d: render_tex_impl(). dp exception.2\n");
				// do nothing. maybe device lost state
			}
		}
	}
	else { // not using sprite

		FONT_TEXT_VERTEX4_TEXTURE vertices4 [4] =
		{
			D3DXVECTOR4((float)rect.left - .5f,		(float)bottom - .5f,	.0f, 1.0f), text.color, u,  v, 
			D3DXVECTOR4((float)rect.right - .5f,	(float)bottom - .5f,	.0f, 1.0f), text.color, u + width, v,
			D3DXVECTOR4((float)rect.right - .5f,	(float)top - .5f,			.0f, 1.0f), text.color, u + width, v - height, 
			D3DXVECTOR4((float)rect.left - .5f,		(float)top - .5f,			.0f, 1.0f), text.color, u, v - height, 
		};

		const int hm = -1, hp = 1;
		const int vm = -1, vp = 1;
		const float adjm = .0f, adjp = -1.0f;

		FONT_TEXT_VERTEX4_TEXTURE vertices4_outline_simple [22] =
		{
			D3DXVECTOR4((float)(rect.left+hm)+adjm,			(float)(top+vm)+adjm,				.0f, 1.0f), text.color_outline, u, v - height, 
			D3DXVECTOR4((float)(rect.left+hm)+adjm,			(float)(bottom+vm)+adjm,		.0f, 1.0f), text.color_outline, u,  v, 
			D3DXVECTOR4((float)(rect.right+hm)+adjm,		(float)(top+vm)+adjm	,			.0f, 1.0f), text.color_outline, u + width, v - height, 
			D3DXVECTOR4((float)(rect.right+hm)+adjm,		(float)(bottom+vm)+adjm,		.0f, 1.0f), text.color_outline, u + width, v,
			D3DXVECTOR4((float)(rect.right+hm)+adjm,		(float)(bottom+vm)+adjm,		.0f, 1.0f), text.color_outline, u + width, v,

			D3DXVECTOR4((float)(rect.left+hp)+adjp,			(float)(top+vp)+adjp,				.0f, 1.0f), text.color_outline, u, v - height, 
			D3DXVECTOR4((float)(rect.left+hp)+adjp,			(float)(top+vp)+adjp,				.0f, 1.0f), text.color_outline, u, v - height, 
			D3DXVECTOR4((float)(rect.left+hp)+adjp,			(float)(bottom+vp)+adjp,			.0f, 1.0f), text.color_outline, u,  v, 
			D3DXVECTOR4((float)(rect.right+hp)+adjp,			(float)(top+vp)+adjp,				.0f, 1.0f), text.color_outline, u + width, v - height, 
			D3DXVECTOR4((float)(rect.right+hp)+adjp,			(float)(bottom+vp)+adjp,			.0f, 1.0f), text.color_outline, u + width, v,
			D3DXVECTOR4((float)(rect.right+hp)+adjp,			(float)(bottom+vp)+adjp,			.0f, 1.0f), text.color_outline, u + width, v,

			D3DXVECTOR4((float)(rect.left+hm)+adjm,			(float)(top+vp)+adjp,				.0f, 1.0f), text.color_outline, u, v - height, 
			D3DXVECTOR4((float)(rect.left+hm)+adjm,			(float)(top+vp)+adjp,				.0f, 1.0f), text.color_outline, u, v - height, 
			D3DXVECTOR4((float)(rect.left+hm)+adjm,			(float)(bottom+vp)+adjp,			.0f, 1.0f), text.color_outline, u,  v, 
			D3DXVECTOR4((float)(rect.right+hm)+adjm,		(float)(top+vp)+adjp,				.0f, 1.0f), text.color_outline, u + width, v - height, 
			D3DXVECTOR4((float)(rect.right+hm)+adjm,		(float)(bottom+vp)+adjp,			.0f, 1.0f), text.color_outline, u + width, v,
			D3DXVECTOR4((float)(rect.right+hm)+adjm,		(float)(bottom+vp)+adjp,			.0f, 1.0f), text.color_outline, u + width, v,

			D3DXVECTOR4((float)(rect.left+hp)+adjp,			(float)(top+vm)+adjm,				.0f, 1.0f), text.color_outline, u, v - height, 
			D3DXVECTOR4((float)(rect.left+hp)+adjp,			(float)(top+vm)+adjm,				.0f, 1.0f), text.color_outline, u, v - height, 
			D3DXVECTOR4((float)(rect.left+hp)+adjp,			(float)(bottom+vm)+adjm,		.0f, 1.0f), text.color_outline, u,  v, 
			D3DXVECTOR4((float)(rect.right+hp)+adjp,			(float)(top+vm)+adjm,				.0f, 1.0f), text.color_outline, u + width, v - height, 
			D3DXVECTOR4((float)(rect.right+hp)+adjp,			(float)(bottom+vm)+adjm,		.0f, 1.0f), text.color_outline, u + width, v,
		};
		hr = _device->SetFVF( FONT_TEXT_VERTEX4_TEXTURE::FVF );
		assert(SUCCEEDED(hr));

		try {
			if (font.get_outline_type() == zz_font::OUTLINE_TYPE_SIMPLE) {
				hr = _device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 20, vertices4_outline_simple, sizeof(FONT_TEXT_VERTEX4_TEXTURE) );
				assert(SUCCEEDED(hr));
			}
			hr =_device->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices4, sizeof(FONT_TEXT_VERTEX4_TEXTURE) );
			assert(SUCCEEDED(hr));

			if (r->get_state()->use_draw_text_rect) {
				r->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG2 ); // DIFFUSE
				r->set_texture_stage_state( 0, ZZ_TSS_ALPHAOP, ZZ_TOP_DISABLE);
				hr = _device->DrawPrimitiveUP( D3DPT_LINESTRIP, 3, vertices4, sizeof(FONT_TEXT_VERTEX4_TEXTURE) );
				assert(SUCCEEDED(hr));
			}
		}
		catch (...) {
			// do nothing. maybe device lost state
			ZZ_LOG("font_texture_d3d: render_tex_impl(). dp exception. 3\n");
		}
	}

	// restore sprite state
	if (text.use_sprite) {
		_device->SetTransform(D3DTS_WORLD, &saved_world);

		r->set_cullmode(zz_render_state::ZZ_CULLMODE_NONE);

		r->set_texture_stage_state( 0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE );
		r->set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE );
		r->set_texture_stage_state( 0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE );

		r->set_texture_stage_state( 0, ZZ_TSS_ALPHAOP, ZZ_TOP_MODULATE);
		r->set_texture_stage_state( 0, ZZ_TSS_ALPHAARG1, ZZ_TA_TEXTURE );
		r->set_texture_stage_state( 0, ZZ_TSS_ALPHAARG2, ZZ_TA_DIFFUSE );
	}
}

void zz_font_texture_d3d::clear_text_texture (const zz_font_text& text)
{
	const RECT& rect = text.tex_rect;

	D3DCOLOR clearcolor = text.color & 0x00FFFFFF;

	FONT_TEXT_VERTEX_DIFFUSE vertices[4] =
    {
		D3DXVECTOR4((float)rect.left - .5f,		(float)rect.bottom - .5f,	1.0f, 1.0f), clearcolor, 
		D3DXVECTOR4((float)rect.right - .5f,	(float)rect.bottom - .5f,	1.0f, 1.0f), clearcolor,
		D3DXVECTOR4((float)rect.right - .5f,	(float)rect.top - .5f,			1.0f, 1.0f), clearcolor,
		D3DXVECTOR4((float)rect.left - .5f,		(float)rect.top - .5f,			1.0f, 1.0f), clearcolor,
    };

	zz_renderer * r = znzin->renderer;
	{
		r->enable_alpha_blend(false, ZZ_BT_NORMAL);
		r->enable_alpha_test(false);

		r->set_cullmode(zz_render_state::ZZ_CULLMODE_CW);

		r->enable_zbuffer(false);
		r->enable_zwrite(false);

		r->set_texture_stage_state( 0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1 );
		r->set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_DIFFUSE );

		r->set_texture_stage_state( 1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE );
		r->set_texture_stage_state( 0, ZZ_TSS_ALPHAOP, ZZ_TOP_DISABLE );
	}

	// set transform
  	_device->SetFVF( FONT_TEXT_VERTEX_DIFFUSE::FVF );
	try {
		_device->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices, sizeof(FONT_TEXT_VERTEX_DIFFUSE) );
	}
	catch (...) {
		// do nothing. maybe device lost state
		ZZ_LOG("font_texture_d3d: clear_text_texture(). dp exception\n");
	}

	// restore
	{
		r->enable_zbuffer(true);
		r->enable_zwrite(true);
		r->enable_alpha_blend(true, ZZ_BT_NORMAL);
		r->enable_alpha_test(true, 0, ZZ_CMP_GREATER);
		r->set_cullmode(zz_render_state::ZZ_CULLMODE_NONE);
		
		// wrap0 0
		r->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE );
		r->set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE );
		r->set_texture_stage_state( 0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE );

		r->set_texture_stage_state( 0, ZZ_TSS_ALPHAOP, ZZ_TOP_MODULATE);
		r->set_texture_stage_state( 0, ZZ_TSS_ALPHAARG1, ZZ_TA_TEXTURE );
		r->set_texture_stage_state( 0, ZZ_TSS_ALPHAARG2, ZZ_TA_DIFFUSE );
	}
}

// render texture for text into screen for debugging purpose
void zz_font_texture_d3d::render_text_texture (void)
{
	float eps_width = 0; //.5f/float(tex_width);
	float eps_height = 0; // .5f/float(tex_height);

    FONT_TEXT_VERTEX4_TEXTURE vertices[4] =
    {
		D3DXVECTOR4(-.5f,								200.f - 0.5f,									.5f,		1.0f), 0xFF0000FF, eps_width,		eps_height,
		D3DXVECTOR4(float(tex_width) - .5f,	200.f - 0.5f,									.5f,		1.0f), 0xFF0000FF, 1.0f + eps_width,	eps_height,
		D3DXVECTOR4(float(tex_width) - .5f,	200.f + float(tex_height) - .5f,		.5f,		1.0f), 0xFF0000FF, 1.0f + eps_width,	1.0f + eps_height,
		D3DXVECTOR4(-.5f,								200.f + float(tex_height) - .5f,		.5f,		1.0f), 0xFF0000FF, eps_width,	1.0f + eps_height,
    };

	assert(_device);

	{
		zz_renderer_d3d * r = static_cast<zz_renderer_d3d*>(znzin->renderer);
		assert(r);

		r->init_sprite_state();

		r->enable_alpha_blend(true, ZZ_BT_NORMAL);
		r->enable_alpha_test(false);

		r->set_vertex_shader(ZZ_HANDLE_NULL);
		r->set_pixel_shader(ZZ_HANDLE_NULL);

		r->enable_zbuffer(false);
		r->enable_zwrite(false);

		r->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_SELECTARG1 );
		r->set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);

		r->set_texture_stage_state( 1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE );
		r->set_texture_stage_state( 0, ZZ_TSS_ALPHAOP, ZZ_TOP_DISABLE );

		r->invalidate_texture( 0 );
	}

	// set transform
  	_device->SetFVF( FONT_TEXT_VERTEX4_TEXTURE::FVF );
	_device->SetTexture( 0, _texture );  

	try {
		_device->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices, sizeof(FONT_TEXT_VERTEX4_TEXTURE) );
	}
	catch (...) {
		ZZ_LOG("font_texture_d3d: render_text_texture(). dp exception\n");
		// do nothing. maybe device lost state
	}
}

void zz_font_texture_d3d::render_to_texture_begin_impl ()
{
	assert(!znzin->renderer->scene_began());

	assert(_device);

	zz_renderer_d3d * r = static_cast<zz_renderer_d3d*>(znzin->renderer);
	
	assert(r);

	_device->SetRenderTarget(0, _surface);
	_device->SetDepthStencilSurface(NULL);

	r->begin_scene(ZZ_RW_TEXTURE);

	assert(!r->sprite_began());

	r->begin_sprite(ZZ_SPRITE_ALPHABLEND, "render_to_texture");
	r->init_sprite_transform(tex_width, tex_height);
}

void zz_font_texture_d3d::render_to_texture_end_impl ()
{
	zz_renderer_d3d * r = static_cast<zz_renderer_d3d*>(znzin->renderer);

	assert(r);
	assert(r->scene_began());
	assert(r->sprite_began());

	r->end_sprite();

	r->end_scene();

	// no need to restore renderstates
	// because, the following process(begin_scene()) includes initializing render state.
}

void zz_font_texture_d3d::render_to_texture_impl (zz_font& font, const zz_font_text& text)
{
	clear_text_texture(text);

	font.draw_text_direct(text);
}
