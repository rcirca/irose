/** 
 * @file zz_font_d3d.CPP
 * @brief d3d font class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-nov-2003
 *
 * $Header: /engine/src/zz_font_outline.cpp 4     04-08-17 5:11p Zho $
 */

#include "zz_tier0.h"
#include "zz_font_outline.h"

ZZ_IMPLEMENT_DYNCREATE(zz_font_outline, zz_font_d3d)

zz_font_outline::zz_font_outline() : zz_font_d3d()
{
	font_color_outline = 0xff000000;
}

zz_font_outline::~zz_font_outline()
{
}

void zz_font_outline::set_property (
	const char * font_name_in,
	int font_size_in,
	color32 font_color_in,
	color32 font_color_outline_in,
	bool italic_in,
	bool bold_in,
	bool outline_in)
{
	font_name.set(font_name_in);
	font_size = font_size_in;
	font_color = font_color_in;
	font_color_outline = font_color_outline_in;
	font_style = STYLE_NONE;
	set_bold(bold_in);
	set_italic(italic_in);
	set_outline(outline_in);
}

void zz_font_outline::_draw_text (bool use_sprite, zz_rect * rect_in, color32 color_in, dword formatting_style, const char * msg, size_t msg_length)
{
	zz_text_item item;

	item.color = font_color_outline;
	item.formatting_style = formatting_style;
	item.msg = msg;
	item.msg_length = msg_length;
	item.rect = *rect_in;
	item.use_sprite = use_sprite;

	if (get_outline()) {
		// draw left/right/top/bottom outline
		_draw_text_offset(item, -1, 0);
		_draw_text_offset(item, 1, 0);
		_draw_text_offset(item, 0, -1);
		_draw_text_offset(item, 0, 1);

		// shrink alpha value by 1/2.
		//item.color = ZZ_COLOR32_ARGB(
		//	ZZ_COLOR32_ALPHA(font_color_outline) >> 2, // divide 4
		//	ZZ_COLOR32_RED(font_color_outline),
		//	ZZ_COLOR32_GREEN(font_color_outline),
		//	ZZ_COLOR32_BLUE(font_color_outline));
		item.color = (font_color_outline & 0x00FFFFFF) | ((ZZ_COLOR32_ALPHA(font_color_outline) >> 1) << 24);

		// draw lefttop/righttop/leftbottom/rightbottom outline
		_draw_text_offset(item, -1, -1);
		_draw_text_offset(item, 1, -1);
		_draw_text_offset(item, -1, 1);
		_draw_text_offset(item, 1, 1);

		// shrink alpha value by 1/4.
		item.color = (font_color_outline & 0x00FFFFFF) | ((ZZ_COLOR32_ALPHA(font_color_outline) >> 2) << 24);

		// draw bottom shadow
		_draw_text_offset(item, -1, 2);
		_draw_text_offset(item, 0, 2);
		_draw_text_offset(item, 1, 2);
	}

	// draw normal
	item.color = color_in;
	zz_font_d3d::_draw_text(item);
}

bool zz_font_outline::get_outline ()
{
	return ((font_style & zz_font::STYLE_OUTLINE) != 0);
}

void zz_font_outline::set_outline (bool outline)
{
	if (outline) {
		font_style = zz_font_style(font_style | zz_font::STYLE_OUTLINE);
	}
	else {
		font_style = zz_font_style(font_style - (font_style & zz_font::STYLE_OUTLINE));
	}
}