/** 
 * @file zz_font.CPP
 * @brief font class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-nov-2003
 *
 * $Header: /engine/src/zz_font_texture.cpp 1     04-09-22 10:56a Zho $
 */

#include "zz_tier0.h"
#include "zz_font.h"
#include "zz_font_texture.h"

//--------------------------------------------------------------------------------
void zz_font_texture::create (zz_font& font)
{
	// initialize
	int margin = 0;
	switch (font.get_outline_type())
	{
	case zz_font::OUTLINE_TYPE_NONE:
		margin = 0; break;
	case zz_font::OUTLINE_TYPE_SIMPLE:
		margin = 1; break;
	case zz_font::OUTLINE_TYPE_THICKSHADOW:
		margin = 4; break;
	case zz_font::OUTLINE_TYPE_THINSHADOW:
		margin = 2; break;
	}

	line_height = (int)font.get_height();
	int margined_line_height = line_height + margin;

	assert(tex_height > 0);
	assert(line_height > 0);
	num_lines = (int)(tex_height / margined_line_height);
	assert(tex_height >= (num_lines * margined_line_height));
	assert(num_lines > 0);

	zz_texture_element new_elem;
	texts.reserve(num_lines);
	texts.clear();

	for (int i = 0; i < num_lines; ++i) {
		new_elem.text.tex_rect.left = 0;
		new_elem.text.tex_rect.right = tex_width;
		new_elem.text.tex_rect.top = i*margined_line_height ;
		new_elem.text.tex_rect.bottom = new_elem.text.tex_rect.top + margined_line_height;
		texts.push_back(new_elem);
	}
	assert(num_lines == texts.size());
}

void zz_font_texture::destroy ()
{
	// destroy texture in d3d
}

zz_font_texture::zz_element_type::iterator zz_font_texture::find_element (const zz_font_text& text, int state_combination)
{
	zz_texture_element * elm;
	zz_element_type::iterator it(texts.begin()), it_end(texts.end());

	assert(texts.size() == num_lines);

	for (;it != it_end; ++it)
	{
		elm = &(*it);

		if (0 == (state_combination & elm->state)) // skip if state is out of consideration
			continue;

		if (elm->text.same_text(text)) // check all string is the same as
			return it;
	}
	return it_end;
}

bool zz_font_texture::find_text (zz_font_text& text, zz_font_text ** found_text_out)
{
	zz_element_type::iterator it = find_element(text, E_RENDER | E_RERENDER);
	
	assert(texts.size() == this->num_lines);

	if (it == texts.end()) { // not found
		found_text_out = NULL;
		return false;
	}
	*found_text_out = &((*it).text);
	
	assert(*found_text_out);

	return true;
}

zz_font_texture::zz_element_type::iterator zz_font_texture::find_element_any (const zz_font_text& text)
{
	assert(texts.size() == this->num_lines);

	zz_element_type::iterator it(texts.begin()), it_end(texts.end());

	while (it != it_end)
	{
		if ((*it).text.same_text(text))
			return it;
		++it;
	}
	return it_end;
}

bool zz_font_texture::insert (zz_font_text& text)
{
	assert(texts.size() == this->num_lines);

	zz_element_type::iterator it(find_element_any(text)), it_end(texts.end());

	if (it != it_end) { // exists already
		return true;
	}

	zz_texture_element * elm;
	for (it = texts.begin(); it != it_end; ++it)
	{
		elm = &(*it);
		if (elm->is_empty()) {// insert here
			memcpy(&text.tex_rect, &elm->text.tex_rect, sizeof(zz_rect)); // copy tex_rect from slot text
			elm->text = text;
			elm->set_state(E_INSERT);
			//ZZ_LOG("font: [%x] insert(%s)\n", this, text.msg.get());
			return true;
		}
	}

	return false; // no space
}

bool zz_font_texture::draw_text_texture (zz_font& font, zz_font_text& text)
{
	assert(texts.size() == this->num_lines);

	zz_element_type::iterator it = find_element_any(text);

	if (it == texts.end()) { // not found
		return false;
	}

	if (!(*it).is_render() && !(*it).is_rerender()) { // not rendered
		// in list, but not rendered yet and render directly
		return false;
	}

	assert((*it).is_render() || (*it).is_rerender());

	// set render state to rerender
	(*it).set_state(E_RERENDER);
		
	render_text_impl(font, text);

	//ZZ_LOG("font_texture: draw_text(%s) with texture.\n", text.msg.get());
	return true;
}

void zz_font_texture::reset_all ()
{
	zz_texture_element * elem;
	assert(texts.size() == this->num_lines);

	for (zz_element_type::iterator it(texts.begin()), it_end(texts.end()); it != it_end; ++it)
	{
		elem = &(*it);
		elem->set_state(E_EMPTY);
		elem->text.msg.reset();
	}
}

void zz_font_texture::update_font_texture (zz_font& font)
{
	bool render_began = false;

	zz_texture_element * elm;
	assert(texts.size() == this->num_lines);

	for (zz_element_type::iterator it( texts.begin() ), it_end( texts.end() ); it != it_end; ++it)
	{
		elm = &(*it);
		if (elm->is_insert()) {
			// render text to texture actually
			//ZZ_LOG("font_texture: render(%s) to texture\n", elm->text.msg.get());
			// do render-to-texture thing
			elm->text.to_texture = true;

			if (!render_began) { // if not began, first do begin
				render_to_texture_begin_impl(); // with render_to_texture_end_impl()
				render_began = true;
			}

			render_to_texture_impl(font, elm->text);
			elm->set_state(E_RENDER); // check rendered
			//ZZ_LOG("font: [%x] update. set_render(%d, %s)\n", this, count, elm->text.msg.get());
		}
		else if (elm->is_rerender()) {
			elm->set_state(E_RENDER);
		}
		else if (elm->is_render()) { // still render state(it means that it is not rendered in this frame), so we should erase it
			//ZZ_LOG("font: [%x] update. set_empty(%d, %s)\n", this, count, elm->text.msg.get());
			elm->set_state(E_EMPTY);
			elm->text.msg.reset();
		}
		else {
			// do nothing
			assert(elm->is_empty());
		}
	}

	if (render_began)
		render_to_texture_end_impl(); // with render_to_texture_begin_impl()
}

bool zz_font_texture::cacheable (zz_font_text& text)
{
	int text_width(text.rect.right - text.rect.left), text_height(text.rect.bottom - text.rect.top);

	if (text.format & DT_VCENTER) { // if vcentor enabled, do not render to texture
		return false;
	}
	else if (text_width > tex_width) {
		return false;
	}
	else if (text_height > line_height) {
		return false;
	}
	//else if (text_width < (tex_width/8)) {
	//	return false;
	//}
	return true;
}