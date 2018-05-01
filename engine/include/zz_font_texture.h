/** 
 * @file zz_font_texture.h
 * @brief 3d font texture class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    22-sep-2004
 *
 * $Header: /engine/include/zz_font_texture.h 2     04-10-14 10:37a Zho $
 */

#ifndef __ZZ_FONT_TEXTURE_H__
#define __ZZ_FONT_TEXTURE_H__

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

#ifndef __ZZ_RENDERER_H__ 
#include "zz_renderer.h" // for viewport
#endif

// for formatting_style
#ifndef WIN32 // from winuser.h
#define DT_TOP              0x00000000
#define DT_LEFT             0x00000000
#define DT_CENTER           0x00000001
#define DT_RIGHT            0x00000002
#define DT_VCENTER          0x00000004
#define DT_BOTTOM           0x00000008
#define DT_WORDBREAK        0x00000010
#define DT_SINGLELINE       0x00000020
#define DT_EXPANDTABS       0x00000040
#define DT_TABSTOP          0x00000080
#define DT_NOCLIP           0x00000100
#define DT_EXTERNALLEADING  0x00000200
#define DT_CALCRECT         0x00000400
#define DT_NOPREFIX         0x00000800
#define DT_INTERNAL         0x00001000
#define DT_EDITCONTROL      0x00002000
#define DT_PATH_ELLIPSIS    0x00004000
#define DT_END_ELLIPSIS     0x00008000
#define DT_MODIFYSTRING     0x00010000
#define DT_RTLREADING       0x00020000
#define DT_WORD_ELLIPSIS    0x00040000
#endif // #ifndef WIN32

#define ZZ_DEFAULT_FORMATTING_STYLE (DT_WORDBREAK | DT_LEFT | DT_TOP | DT_NOCLIP)

struct zz_string;

struct zz_font_text {
	zz_rect rect; // screen space position and size
	zz_rect tex_rect; // texture space position and size(= rect's)
	color32 color; // text color
	color32 color_outline; // outline(shadow) color
	zz_string msg; // text context
	bool dynamic; // cleared every frame(dynamic) or not
	bool use_sprite; // using sprite or not
	dword format; // formatting style
	bool to_texture; // render to texture stage. set this to true when render to texture stage

	zz_font_text () :
		color(0xffff),
        color_outline(0x0),
		msg(),
		dynamic(true),
		use_sprite(false),
		format(ZZ_DEFAULT_FORMATTING_STYLE),
		to_texture(false)
	{
		rect.left = rect.top = rect.right = rect.bottom = tex_rect.left = 0;
		tex_rect.left = tex_rect.top = tex_rect.right = tex_rect.bottom = tex_rect.left = 0;
	}

	zz_font_text (const zz_font_text& t) : 
		rect(t.rect), 
		tex_rect(t.tex_rect),
		color(t.color),
		color_outline(t.color_outline),
		msg(t.msg),
		dynamic(t.dynamic),
		use_sprite(t.use_sprite),
		format(t.format),
		to_texture(t.to_texture)
	{
	}

	zz_font_text& operator=( const zz_font_text& t )
	{
		if (this == &t) return *this;

		rect = t.rect;
		tex_rect = t.tex_rect;
		color = t.color;
		color_outline = t.color_outline;
		msg = t.msg;
		dynamic = t.dynamic;
		use_sprite = t.use_sprite;
		format = t.format;
		to_texture = t.to_texture;
		return (*this);
	}

	// check if same text
	// do not care about position
	bool same_text ( const zz_font_text& t ) const
	{
		int src_width(rect.right - rect.left), src_height(rect.bottom - rect.top);
		int dest_width(t.rect.right - t.rect.left), dest_height(t.rect.bottom - t.rect.top);

		if (src_width != dest_width) return false;
		if (src_height != dest_height) return false;
		if (msg != t.msg) return false;

		return true;
	}
};

class zz_font;

//--------------------------------------------------------------------------------
class zz_font_texture {
//--------------------------------------------------------------------------------
protected:
	enum e_state {
		E_EMPTY = 1, // initial state. no data written. has empty string
		E_INSERT = 2, // created and will be inserted in update()
		E_RENDER = 4, // rendered, but will be erased in update()
		E_RERENDER = 8, // prepared in the texture and also will be valid in the next frame
	};

	struct zz_texture_element {
		e_state state;
		zz_font_text text;
		
		zz_texture_element() : state(E_EMPTY)
		{}

		bool is_empty ()
		{
			return state == E_EMPTY;
		}

		bool is_insert ()
		{
			return state == E_INSERT;
		}

		bool is_render ()
		{
			return (state == E_RENDER);
		}

		bool is_rerender ()
		{
			return (state == E_RERENDER);
		}

		void set_state (e_state state_in)
		{
			state = state_in;
		}
	};

	std::vector<zz_texture_element> texts;
	typedef std::vector<zz_texture_element> zz_element_type;

	int line_height; // font height in pixel. assert(line_height*num_lines <= tex_height)
	int num_lines; // number of total text lines.
	int tex_width; // texture image width in pixel
	int tex_height; // texture image height in pixel

	zz_element_type::iterator find_element (const zz_font_text& text, int state_combination);
	zz_element_type::iterator find_element_any (const zz_font_text& text);

	virtual void render_text_impl (const zz_font& font, const zz_font_text& text) = 0; // render text

	virtual void render_to_texture_impl (zz_font& font, const zz_font_text& text) = 0; // render to texture
	virtual void render_to_texture_begin_impl () = 0;
	virtual void render_to_texture_end_impl () = 0;

	void reset_all (); // reset all element data

public:
	zz_font_texture () :
	  tex_width(0), tex_height(0), line_height(0), num_lines(0)
	{
	}

	virtual ~zz_font_texture ()
	{
	}

	void create (zz_font& font); // initialize initial variables and create texture
	void destroy (); // destroy contents

	bool insert (zz_font_text& text); // insert text to be cached in the texture for the next frame

	void update_font_texture (zz_font& font); // actually render-to-texture

	bool find_text (zz_font_text& text, zz_font_text ** found_text_out); // returns true if this font_texture has this text for rendered
	bool draw_text_texture (zz_font& font, zz_font_text& text); // returns true if draw_text() has done properly.

	int get_tex_width ()
	{
		return tex_width;
	}

	int get_tex_height ()
	{
		return tex_height;
	}

	int get_line_height ()
	{
		return line_height;
	}

	void set_tex_size (int width_in, int height_in)
	{
		tex_width = width_in;
		tex_height = height_in;
	}

	bool cacheable (zz_font_text& text); // whether this text can be used in texture or not

	virtual void render_text_texture (void) = 0;

	// restoring device stuff
	virtual bool restore_device_objects () = 0; // create unmanaged objects
	virtual bool init_device_objects () = 0; // create managed objects
	virtual bool invalidate_device_objects () = 0; // destroy unmanaged objects
	virtual bool delete_device_objects () = 0; // destroy managed objects
};

#endif // __ZZ_FONT_TEXTURE_H__