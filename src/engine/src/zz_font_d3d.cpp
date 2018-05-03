/** 
 * @file zz_font_d3d.CPP
 * @brief d3d font class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-nov-2003
 *
 * $Header: /engine/src/zz_font_d3d.cpp 44    06-09-15 5:56p Choo0219 $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_font_d3d.h"
#include "zz_renderer_d3d.h"
#include "zz_system.h"
#include "zz_font_texture_d3d.h"
#include "zz_error.h"

ZZ_IMPLEMENT_DYNCREATE(zz_font_d3d, zz_font)

//--------------------------------------------------------------------------------
zz_font_d3d::zz_font_d3d() :
	_d3d_device(NULL),
	_d3d_font(NULL),
	_num_font_textures(0)
{
}

zz_font_d3d::~zz_font_d3d()
{
	unload();
}

// create managed objects
bool zz_font_d3d::init_device_objects ()
{
	if (!znzin) return true;
	if (!znzin->renderer) return true;
	if (!znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d))) return false;

	_d3d_device = ((zz_renderer_d3d*)(znzin->renderer))->get_device();
	if (!_d3d_device) return true;

	D3DXFONT_DESC fontDesc;

	ZeroMemory( &fontDesc, sizeof( D3DXFONT_DESC ) );

    HDC hDC = GetDC( NULL );
    _logical_pixels_y = GetDeviceCaps( hDC, LOGPIXELSY );
    ReleaseDC( NULL, hDC );
	font_height = (unsigned int)MulDiv( font_size, _logical_pixels_y, 72 ); // why minus in other documents? -MulDiv(?)

	fontDesc.Height					= font_height;
	fontDesc.Width					= 0;
	fontDesc.Weight					= this->get_bold() ? FW_BOLD : FW_THIN; 
	fontDesc.MipLevels				= 1;
	fontDesc.Italic						= this->get_italic();
	fontDesc.CharSet					= this->font_charset;
	fontDesc.OutputPrecision		= OUT_DEFAULT_PRECIS;	
	fontDesc.Quality					= PROOF_QUALITY;
	fontDesc.PitchAndFamily		= FIXED_PITCH | FF_MODERN;
	strcpy( fontDesc.FaceName, font_name.get() );

	HRESULT hr;

	try {
		if (FAILED(hr = D3DXCreateFontIndirect( _d3d_device, &fontDesc, &_d3d_font))) {
			ZZ_LOG("zz_font_d3d: init_device_objects(%s, %d, %d) failed. [%s]\n",
				font_name.get(), font_height, font_charset,
				zz_renderer_d3d::get_hresult_string(hr) );
			zz_msgbox(zz_error::get_text(zz_error::MSG_ERROR), zz_error::get_text(zz_error::CREATE_FONT_FAIL));
			
			strcat(znzin->sysinfo.video_string," <Font failed>");

			zz_msgboxf(zz_error::get_text(zz_error::MSG_ERROR), zz_error::get_text(zz_error::MSG_EXIT_D3DERROR), znzin->sysinfo.video_string);
			exit(EXIT_FAILURE);
			return false;
		}
	}
	catch (...) {
		ZZ_LOG("font_d3d: createfont failed. [%x, %d, %s, %x, %x]\n",
			this, fontDesc.Height, fontDesc.FaceName, _d3d_device, &_d3d_font);
		throw;
	}

	if (_num_font_textures > 0) {
		_d3d_font_texture.init_device_objects();
		_d3d_font_texture.create(*this); // zhotest
	}

	return true;
}

// destroy unmanaged objects
bool zz_font_d3d::invalidate_device_objects ()
{
	if (_num_font_textures) {
		if (!_d3d_font_texture.invalidate_device_objects()) {
			_num_font_textures = 0;
		}
	}

	if (!_d3d_font) return true;
	_d3d_font->OnLostDevice();

	return true;
}

// create unmanaged objects
bool zz_font_d3d::restore_device_objects ()
{
	if (_num_font_textures) {
		if (!_d3d_font_texture.restore_device_objects()) {
			_num_font_textures = 0;
		}
	}

	// reset font
	if (!_d3d_font) return true;
	_d3d_font->OnResetDevice();
	return true;
}

// destroy managed objects
bool zz_font_d3d::delete_device_objects ()
{
	if (_num_font_textures) {
		if (!_d3d_font_texture.delete_device_objects()) {
			_num_font_textures = 0;
		}
	}

	SAFE_RELEASE(_d3d_font);
	return true;
}

bool zz_font_d3d::load ()
{
	if (init_device_objects()) {
		if (restore_device_objects()) {
			return true;
		}
	}
	return false;
}

bool zz_font_d3d::unload ()
{
	if (delete_device_objects()) {
		if (invalidate_device_objects()) {
			return true;
		}
	}
	return false;
}

void zz_font_d3d::draw_text_prim_outline_simple (const zz_font_text& text_item)
{
	assert(!text_item.to_texture);

	zz_renderer_d3d * r = static_cast<zz_renderer_d3d*>(znzin->renderer);

	if (!text_item.use_sprite) { // begin sprite
		zz_assert(!r->sprite_began());
		r->begin_sprite(ZZ_SPRITE_ALPHABLEND, "draw_text_prim_outline_simple");
		// we assumes that identity matrix transform is the default
	}
	else {
		zz_assert(r->sprite_began());
	}

	zz_font_text item(text_item);

	// overwrite color with outline color
	item.color = font_color_outline;
	
	draw_text_prim_offset(item,	-.5f,	-.5f);
	draw_text_prim_offset(item,	.5f,	.5f);
	draw_text_prim_offset(item,	-.5f,	.5f);
	draw_text_prim_offset(item,	.5f,	-.5f);

	item.color = font_color;
	draw_text_prim_offset(item, 0, 0);

	if (!text_item.use_sprite) { // end sprite
		r->end_sprite();
	}
}

INT zz_font_d3d::_draw_text ( LPD3DXSPRITE pSprite, const char * pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color )
{
	assert(_d3d_font);
	INT result;
	
	try {
		result = _d3d_font->DrawText( pSprite, pString, Count, pRect, Format, Color );
	}
	catch (...) {
		result = -1; // error
		ZZ_LOG("font_d3d: exception occured in _draw_text(%x, %s, %d)", pSprite, pString, Count);
		// reset sprite and font
		zz_renderer_d3d * r = static_cast<zz_renderer_d3d *>(znzin->renderer);
		r->recreate_default_sprite();

		// we dont't know that we should recreate font itself when the exception was occured rendering sprite
		// so, we keep font-recreating in comments until we find that the exception occures in font, too.
#if (0) // uncomment this if exception is still occured here.
		unload();
		load();
#endif
	}
	return result;
}

void zz_font_d3d::draw_text_prim_offset (const zz_font_text& text_item, float offsetx, float offsety)
{
	zz_assert(!text_item.to_texture);
	zz_assert(text_item.msg.get());
	zz_assert(text_item.msg.size());
	zz_assert(_d3d_font);
	
	if (!_d3d_font)
		return;

	zz_renderer_d3d * r = (zz_renderer_d3d *)(znzin->renderer);

	assert(r->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));

	LPD3DXSPRITE sprite = r->get_sprite();
	zz_assert(sprite);
	HRESULT hr;

	D3DXMATRIX saved_tm;
	D3DXMATRIX new_tm;

	zz_assert(r->sprite_began());

	r->flush_sprite();
	r->get_sprite_transform((float*)&saved_tm); // save tm

	// adjust offset
	new_tm = saved_tm;

	// assert not-zero-scale
	assert(saved_tm._11 != 0);
	assert(saved_tm._22 != 0);
	assert(saved_tm._33 != 0);

	// we does not support rotation
	assert(saved_tm._12 == 0);
	assert(saved_tm._13 == 0);
	assert(saved_tm._21 == 0);
	assert(saved_tm._23 == 0);
	assert(saved_tm._31 == 0);
	assert(saved_tm._32 == 0);
	new_tm._41 += offsetx * saved_tm._11;
	new_tm._42 += offsety * saved_tm._22;

	r->set_sprite_transform((float*)&new_tm);

	RECT rect = text_item.rect;
	if (FAILED(hr = _draw_text(
		sprite,
		text_item.msg.get(), -1,
		&rect, 
		text_item.format,
		text_item.color )))
	{
		ZZ_LOG("font_d3d: DrawText() failed\n", zz_renderer_d3d::get_hresult_string(hr));
	}

	sprite->SetTransform(&saved_tm); // restore tm
}

// called after clear_text_texture
void zz_font_d3d::draw_text_prim (const zz_font_text& text_item)
{
	if (!text_item.msg.get()) return;

	size_t length;
	length = text_item.msg.size();

	if (length <= 0) return; // draw nothing
	
	assert(_d3d_font);
	if (!_d3d_font)
		return;
	
	zz_renderer_d3d * r = static_cast<zz_renderer_d3d*>(znzin->renderer);
	LPD3DXSPRITE sprite = r->get_sprite();
	assert(sprite);

	if (!text_item.use_sprite) { // begin sprite
		
		zz_assert(!r->sprite_began());
		r->begin_sprite(ZZ_SPRITE_ALPHABLEND, "draw_text_prim");  //조성현 2006 04 25 Text출력..
		// we assumes that identity matrix transform is the default
	   D3DXMATRIX mat, mat2;
	   D3DXMatrixIdentity(&mat2);
	   sprite->GetTransform(&mat);
	   
	   mat2._41 = mat._41;
	   mat2._42 = mat._42;
	   mat2._43 = mat._43;
	
	   sprite->SetTransform(&mat2);

	    
	}
	else {
		zz_assert(r->sprite_began());
	  
	}

	HRESULT hr;

	if (text_item.to_texture) { // render to texture stage
		r->enable_zbuffer(false);
		r->enable_zwrite(false);

		zz_rect new_rect;
		int width, height;
		width = text_item.rect.right - text_item.rect.left;
		height = text_item.rect.bottom - text_item.rect.top;
		SetRect(&new_rect, text_item.tex_rect.left, text_item.tex_rect.top,
			text_item.tex_rect.left + width, text_item.tex_rect.top + height);

		assert((text_item.tex_rect.top != 0) || (text_item.tex_rect.bottom != 0));

		//ZZ_LOG("font: draw_text_prim(%s), [%d, %d]\n", text_item.msg.get(),	text_item.tex_rect.top, text_item.tex_rect.top + height);
		//ZZ_LOG("font: draw_text_prim(%s), [%d, %d, %d, %d]\n",
		//	text_item.msg.get(),
		//	new_rect.left, new_rect.top, new_rect.right, new_rect.bottom);

		DWORD frontcolor = 0xFFFFFFFF;

		if (FAILED(hr = _draw_text(
			sprite,
			text_item.msg.get(), -1,
			&new_rect, 
			text_item.format,
			frontcolor )))
		{
			ZZ_LOG("font_d3d: DrawText() failed\n", zz_renderer_d3d::get_hresult_string(hr));
		}
		
		// restore states
		r->enable_zbuffer(true);
		r->enable_zwrite(true);
	}
	else {
		RECT rect = text_item.rect;
		if (FAILED(hr = _draw_text(
			sprite,
			text_item.msg.get(), -1,
			&rect, 
			text_item.format,
			text_item.color )))
		{
			ZZ_LOG("font_d3d: DrawText() failed\n", zz_renderer_d3d::get_hresult_string(hr));
		}
	}

	if (!text_item.use_sprite) { // end sprite
		r->end_sprite();
	}
}

bool zz_font_d3d::get_text_extent (const char * text, zz_size * extent)
{
	assert(_d3d_font);
	assert(extent);

	if (!_d3d_font) 
		return false;

	HDC hDC = _d3d_font->GetDC(); // Do not get the device context from ::GedDC(NULL), use d3d font's.

	D3DXFONT_DESC font_desc;
	_d3d_font->GetDesc(&font_desc);

	SelectObject( hDC, (HFONT)GetStockObject( font_desc.PitchAndFamily ) );
	GetTextExtentPoint32( hDC, text, (int)strlen(text), extent );

	ReleaseDC(NULL, hDC);

	return true;
}

void zz_font_d3d::set_cache_property (int num_textures_in, int texture_width_in, int texture_height_in)
{
	_num_font_textures = num_textures_in;
	_d3d_font_texture.set_tex_size(texture_width_in, texture_height_in);
}
