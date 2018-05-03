/** 
 * @file zz_font_texture_d3d.h
 * @brief 3d font texture d3d class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    22-sep-2004
 *
 * $Header: /engine/include/zz_font_texture_d3d.h 2     04-10-14 10:37a Zho $
 */

#ifndef __ZZ_FONT_TEXTURE_D3D_H__
#define __ZZ_FONT_TEXTURE_D3D_H__

#ifndef __ZZ_FONT_TEXTURE_H__
#include "zz_font_texture.h"
#endif

#include <d3d9.h>
#include <d3dx9.h>

class zz_font_texture_d3d : public zz_font_texture {
private:
	LPDIRECT3DTEXTURE9 _texture;
	LPDIRECT3DSURFACE9 _surface;

	static LPDIRECT3DDEVICE9 _device;

protected:
	virtual void render_text_impl (const zz_font& font, const zz_font_text& text);
	virtual void render_to_texture_impl (zz_font& font, const zz_font_text& text);
	virtual void render_to_texture_begin_impl ();
	virtual void render_to_texture_end_impl ();
	void clear_text_texture (const zz_font_text& text);

public:
	zz_font_texture_d3d ();
	virtual ~zz_font_texture_d3d ();

	virtual void render_text_texture (void);

	// restoring device stuff
	virtual bool restore_device_objects (); // create unmanaged objects
	virtual bool init_device_objects (); // create managed objects
	virtual bool invalidate_device_objects (); // destroy unmanaged objects
	virtual bool delete_device_objects (); // destroy managed objects
};

#endif // __ZZ_FONT_TEXTURE_D3D_H__