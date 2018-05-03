/** 
 * @file zz_font.h
 * @brief 3d font class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-nov-2003
 *
 * $Header: /engine/include/zz_font.h 18    04-10-14 10:37a Zho $
 */

#ifndef __ZZ_FONT_H__
#define __ZZ_FONT_H__

#ifndef __ZZ_POOL_H__
#include "zz_pool.h"
#endif

#ifndef __ZZ_FONT_TEXTURE_H__
#include "zz_font_texture.h"
#endif

#include <vector>

//--------------------------------------------------------------------------------
class zz_font : public zz_node {
public:
	enum zz_font_style {
		FONT_STYLE_NONE = 0,
		FONT_STYLE_ITALIC = 1,
		FONT_STYLE_BOLD = 2,
	};

	enum zz_outline_type {
		OUTLINE_TYPE_NONE = 0,
		OUTLINE_TYPE_SIMPLE = 1, // 1-pixel outline
		OUTLINE_TYPE_THINSHADOW = 2, // 1-pixel outline and 1-pixel bottom shadow
		OUTLINE_TYPE_THICKSHADOW = 4 // 4-pixel outline shadow
	};

protected:
	zz_string font_name; // system font name. static property.
	color32 font_color; // text color. dynamic property.
	color32 font_color_outline; // outline(or shadow) color. dynamic property.
	unsigned int font_size; // font size in pixel. static property.
	unsigned int font_height; // font height in logical unit. static property.
	zz_pool<zz_font_text> font_texts; // internal managed text fonts. set by add_text(), interface:drawFontLater()
	zz_font_style font_style; // static property.
	zz_outline_type font_outline_type; // outline type.
	unsigned char font_charset; // charecter set

	void set_bold (bool bold);
	void set_italic (bool italic);
	void set_outline_type (zz_outline_type outline_type_in);

	void draw_text (zz_font_text& text); // all drawing text meets this method

	virtual void draw_text_prim (const zz_font_text& text) = 0; // draw text primitive function without outline
	virtual void draw_text_prim_offset (const zz_font_text& text_item, float offsetx, float offsety) = 0;
	virtual void draw_text_prim_outline_simple (const zz_font_text& text_item) = 0;

	// calc rect by ix and iy, and return false if width or height is zero.
	bool get_rect (zz_rect& rect_out, int ix, int iy, const char * msg);

	virtual zz_font_texture * get_font_texture () = 0; // get font texture

public:
	zz_font();
	virtual ~zz_font();

	// set static property
	virtual void set_font_property (
		const char * font_name_in,
		unsigned char font_charset_in,
		int font_size_in,
		color32 text_color_in, 
		color32 outline_color_in,
		bool italic_in,
		bool bold_in,
		zz_outline_type outline_type_in);

	bool get_bold () const;
	bool get_italic () const;

	color32 get_color () const;
	color32 get_color_outline () const;

	void set_color (color32 color);
	void set_color_outline (color32 color_outline_in);

	zz_outline_type get_outline_type () const; // is outline-ed font

	unsigned int get_size () const // get font size registered
	{
		return font_size;
	}

	unsigned int get_height () const // get font height in logical unit
	{
		return font_height;
	}

	virtual bool get_text_extent (const char * text, zz_size * extent) = 0;

	void render ();
	
	// push text in queue to render at rendering time.
	// returns new handle
	zz_handle add_text (bool dynamic, int left, int top, const char * msg);

	// delete text in queue
	bool del_text ( zz_handle htext );

	// delete all texts in queue
	void clear_text (bool dynamic);

	// not implemented push() by rect and formatting_style

	// draw text immediately
	void draw (bool use_sprite, int left, int top, const char * msg);
	void draw (bool use_sprite, zz_rect * rect_in, dword formatting_style_in, const char * msg);

	void prepare_font (); // render fonts to texture. this call should be out of beginscene/endscene

	void draw_text_direct (const zz_font_text& text);

	virtual void set_cache_property (int num_textures_in, int texture_width_in, int texture_height_in) = 0;

	ZZ_DECLARE_DYNAMIC(zz_font)
};

#endif __ZZ_FONT_H__ 