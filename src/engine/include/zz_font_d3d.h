/** 
 * @file zz_font_d3d.h
 * @brief 3d font class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-nov-2003
 *
 * $Header: /engine/include/zz_font_d3d.h 15    04-10-14 10:37a Zho $
 */

#ifndef __ZZ_FONT_D3D_H__
#define __ZZ_FONT_D3D_H__

#ifndef __ZZ_FONT_H__
#include "zz_font.h"
#endif

#ifndef __ZZ_STRING_H__
#include "zz_string.h"
#endif

#ifndef __ZZ_FONT_TEXTURE_D3D_H__
#include "zz_font_texture_d3d.h"
#endif

#include <d3d9.h>
#include <d3dx9.h>

//--------------------------------------------------------------------------------
class zz_font_d3d : public zz_font {
//--------------------------------------------------------------------------------
private:
	INT _draw_text ( LPD3DXSPRITE pSprite, const char * pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color ); // just wrapper for DrawText

protected:
	LPD3DXFONT _d3d_font;
	LPDIRECT3DDEVICE9 _d3d_device;

	int _logical_pixels_y; // get by GetDeviceCaps
	int _num_font_textures;
	zz_font_texture_d3d _d3d_font_texture;

	// this is primitive virtual functions
	virtual void draw_text_prim (const zz_font_text& text_item);
	virtual void draw_text_prim_offset (const zz_font_text& text_item, float offsetx, float offsety);
	virtual void draw_text_prim_outline_simple (const zz_font_text& text_item);

	virtual zz_font_texture * get_font_texture ()
	{
		if (_num_font_textures == 0) return NULL;
		return static_cast<zz_font_texture*>(&_d3d_font_texture);
	}

public:
	zz_font_d3d();
	virtual ~zz_font_d3d();

	// from node
	virtual bool load ();
	virtual bool unload ();

	virtual bool get_text_extent (const char * text, zz_size * extent);

	// restoring device stuff
	virtual bool restore_device_objects (); // create unmanaged objects
	virtual bool init_device_objects (); // create managed objects
	virtual bool invalidate_device_objects (); // destroy unmanaged objects
	virtual bool delete_device_objects (); // destroy managed objects

	virtual void set_cache_property (int num_textures_in, int texture_width_in, int texture_height_in);

	ZZ_DECLARE_DYNAMIC(zz_font_d3d);
};


#endif // __ZZ_FONT_D3D_H__