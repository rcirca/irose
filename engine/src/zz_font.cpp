/** 
 * @file zz_font.CPP
 * @brief font class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-nov-2003
 *
 * $Header: /engine/src/zz_font.cpp 30    04-10-19 1:58p Zho $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_string.h"
#include "zz_font.h"
#include "zz_renderer.h"
#include "zz_system.h"
#include "zz_profiler.h"

#include <algorithm>

ZZ_IMPLEMENT_DYNAMIC(zz_font, zz_node)

#define ZZ_DEFAULT_FONTNAME "±¼¸²Ã¼"
#define ZZ_DEFAULT_FONTSIZE (9)

#define ZZ_MAX_FONTS 1000

zz_font::zz_font() :
	font_name(ZZ_DEFAULT_FONTNAME),
	font_size(ZZ_DEFAULT_FONTSIZE),
	font_height(ZZ_DEFAULT_FONTSIZE),
	font_color(0xFFFFFFFF),
	font_color_outline(0xff000000),
	font_style(zz_font::FONT_STYLE_NONE),
	font_outline_type(OUTLINE_TYPE_NONE),
	font_charset(DEFAULT_CHARSET)
{
}

zz_font::~zz_font()
{
}

void zz_font::render ()
{
	zz_pool<zz_font_text>::iterator it(font_texts.begin()), it_end(font_texts.end());
	for (; it != it_end; ++it) {
		draw_text((*it));
	}
	clear_text(true);
}

zz_handle zz_font::add_text (bool dynamic, int left, int top, const char * msg)
{
	zz_font_text item;
	
	if (!get_rect(item.rect, left, top, msg)) return ZZ_HANDLE_NULL;

	item.msg.set(msg);
	item.color = font_color;
	item.color_outline = font_color_outline;
	item.dynamic = dynamic;
	item.format = ZZ_DEFAULT_FORMATTING_STYLE;
	item.use_sprite = false;
	
	if (font_texts.get_num_running() < ZZ_MAX_FONTS) {
		return font_texts.add(item);
	}
	return ZZ_HANDLE_NULL;
}

bool zz_font::del_text ( zz_handle htext )
{
	if (!ZZ_HANDLE_IS_VALID(htext)) return false;

	font_texts.del(htext);
	return true;
}

void zz_font::clear_text ( bool dynamic )
{
	zz_pool<zz_font_text>::iterator it;

	if (dynamic) {
		it = font_texts.begin();
		while (it != font_texts.end()) {
			if ((*it).dynamic == dynamic) {
				font_texts.erase(it);
				it = font_texts.begin();
			}
			else { // for not considered node, skip it
				++it; // if we do not increase it, it will cause infinite loop.
			}
		}
	}
}

void zz_font::set_font_property (
	const char * font_name_in,
	unsigned char font_charset_in,
	int font_size_in,
	color32 text_color_in,
	color32 outline_color_in,
	bool italic_in,
	bool bold_in,
	zz_outline_type outline_type_in)
{
	font_name.set(font_name_in);
	font_size = font_size_in;
	font_color = text_color_in;
	font_color_outline = outline_color_in;
	font_style = FONT_STYLE_NONE;
	font_charset = font_charset_in;
	set_bold(bold_in);
	set_italic(italic_in);
	set_outline_type(outline_type_in);
}


bool zz_font::get_bold () const
{
	return ((font_style & zz_font::FONT_STYLE_BOLD) != 0);
}

bool zz_font::get_italic () const
{
	return ((font_style & zz_font::FONT_STYLE_ITALIC) != 0);
}

void zz_font::set_color (color32 color)
{
	font_color = color;
}

void zz_font::set_color_outline (color32 color_outline_in)
{
	font_color_outline = color_outline_in;
}

color32 zz_font::get_color () const
{
	return font_color;
}

color32 zz_font::get_color_outline () const
{
	return font_color_outline;
}

void zz_font::set_bold (bool bold)
{
	if (bold) {
		font_style = zz_font_style(font_style | zz_font::FONT_STYLE_BOLD);
	}
	else {
		font_style = zz_font_style(font_style - (font_style & zz_font::FONT_STYLE_BOLD));
	}
}

void zz_font::set_italic (bool italic)
{
	if (italic) {
		font_style = zz_font_style(font_style | zz_font::FONT_STYLE_ITALIC);
	}
	else {
		font_style = zz_font_style(font_style - (font_style & zz_font::FONT_STYLE_ITALIC));
	}
}

zz_font::zz_outline_type zz_font::get_outline_type () const
{
	return font_outline_type;
}

void zz_font::set_outline_type (zz_outline_type outline_type_in)
{
	font_outline_type = outline_type_in;
}

void zz_font::draw (bool use_sprite, int left, int top, const char * msg)
{
	if (!msg) return;
	if (msg[0] == '\0') return; // if null-string, skip

	zz_font_text item;
	
	if (!get_rect(item.rect, left, top, msg)) return;

	item.color = font_color;
	item.color_outline = font_color_outline;
	item.format = ZZ_DEFAULT_FORMATTING_STYLE;
	item.msg.set(msg);
	item.use_sprite = use_sprite;
	item.dynamic = true;

	draw_text(item);
}

void zz_font::draw_text_direct (const zz_font_text& text)
{
	draw_text_prim(text);
}

void zz_font::draw_text (zz_font_text& text)
{
	assert(znzin);

	zz_renderer * r = znzin->renderer;

	zz_assert(r);
	assert(r->scene_began());
	assert(!r->get_device_lost());

	if (!r->scene_began()) return;
	if (r->get_device_lost()) return;

	//ZZ_LOG("draw_text(%04d,%04d,%x, %s)\n", text.rect.left, text.rect.right, text.color, text.msg.get());
	//ZZ_PROFILER_BEGIN(Pdraw_text);

	// check if the font has a texture for this text
	zz_font_texture * ftex = get_font_texture();

	if (ftex && r->get_state()->use_font_texture) { // if has font texture
		if (ftex->cacheable(text)) { // if we can hold text in texture
			zz_font_text * text_found = NULL;
			if (ftex->find_text(text, &text_found)) { // search existing one with same text
				text.tex_rect = text_found->tex_rect; // copy tex_rect as new text does not know texture position 
				if (ftex->draw_text_texture(*this, text)) { // draw text within texture
#if defined(_DEBUG)
					goto draw_text_end; // do some finishing job if any
#else
					return;
#endif
				}
			}
			else { // not found matching text
				// we insert this text into the list to render with texture in the next time.
				// for now, we have to render it directly.
				ftex->insert(text);
			}
		}
	}

	// render directly
	text.to_texture = false; // set to backbuffer stage
	switch (get_outline_type())
	{
	case zz_font::OUTLINE_TYPE_NONE:
		draw_text_direct(text);
		break;
	case zz_font::OUTLINE_TYPE_SIMPLE:
		draw_text_prim_outline_simple(text);
		break;
	default:
		zz_assertf(0, "not supported font type");
		break;
	}

#ifdef _DEBUG
draw_text_end:
	{
		zz_font_texture * ftex = get_font_texture();
		static zz_font * font_addr = 0;
		//if (ftex) {
		if (ftex && (font_addr == this)) {
			ftex->render_text_texture();
		}
	}
#endif // _DEBUG

	//ZZ_PROFILER_END(Pdraw_text);
}

void zz_font::draw (bool use_sprite, zz_rect * rect_in, dword formatting_style_in, const char * msg)
{
	if (!msg) return;
	if (msg[0] == '\0') return; // if null string, skip

	zz_font_text item;

	item.color = font_color;
	item.color_outline = font_color_outline;
	item.format = formatting_style_in;
	item.msg.set(msg);
	item.rect = *rect_in;
	item.use_sprite = use_sprite;

	draw_text(item);
}

bool zz_font::get_rect (zz_rect& rect_out, int ix, int iy, const char * msg)
{
	size_t msg_length = strlen(msg);
	if (msg_length <= 0) return false; // draw nothing

	rect_out.left = ix;
	rect_out.top = iy;

#if (1)
	zz_size extent;
	bool ret = get_text_extent(msg, &extent);
	assert(ret);
	int margin = 0;
	switch (font_outline_type)
	{
	case OUTLINE_TYPE_SIMPLE:
		margin = 1;
		break;
	case OUTLINE_TYPE_THICKSHADOW:
		margin = 4;
		break;
	case OUTLINE_TYPE_THINSHADOW:
		margin = 2;
		break;
	}

	rect_out.right = ix + extent.cx + margin;
	rect_out.bottom = iy + extent.cy;
#else
	rect_out.right = ix + font_size * msg_length;
	rect_out.bottom = iy + font_size; // + margin ?
#endif

	return true;
}

// do jobs to be done before rendering fonts
void zz_font::prepare_font ()
{
	zz_font_texture * ftex = get_font_texture();

	if (ftex) {
		ftex->update_font_texture(*this);
	}
}

