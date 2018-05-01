/** 
 * @file zz_shader.cpp
 * @brief shader class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-mar-2002
 *
 * $Header: /engine/src/zz_shader.cpp 7     04-09-05 6:51p Zho $
 * $History: zz_shader.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-09-05   Time: 6:51p
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-07-09   Time: 11:58a
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-06-08   Time: 10:35a
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-15   Time: 4:26p
 * Updated in $/engine/src
 * Redesigning mesh structure.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-11-27   Time: 4:50p
 * Updated in $/znzin11/engine/source
 * - removed zz_resource class, added full-support of resource management
 * into zz_node, and cleaned up some codes.
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_shader.h"
#include "zz_system.h"
#include "zz_renderer.h"

ZZ_IMPLEMENT_DYNCREATE(zz_shader, zz_node)

zz_shader * zz_shader::shadow_shader = NULL;
zz_shader * zz_shader::shadow_shader_skin = NULL;
zz_shader * zz_shader::glow_shader = NULL;
zz_shader * zz_shader::glow_shader_skin = NULL;
zz_shader * zz_shader::terrain_shader = NULL;

zz_shader::zz_shader(void) :
	num_format(0),
	vformat(ZZ_VF_NONE),
	is_binary(false)
{
	for (int i = 0; i < ZZ_MAX_SHADER_FORMAT; ++i) {
		vshader_handles[i] = ZZ_HANDLE_NULL;
		pshader_handles[i] = ZZ_HANDLE_NULL;
		vshader_names[i] = NULL;
		pshader_names[i] = NULL;
	}
}

zz_shader::~zz_shader(void)
{
	invalidate_device_objects();
	delete_device_objects();
	for (int i = 0; i < num_format; ++i) {
		ZZ_SAFE_DELETE(vshader_names[i]);
		ZZ_SAFE_DELETE(pshader_names[i]);
	}
}

zz_handle zz_shader::create_vshader (const char * vshader_filename, int format_in)
{
	assert(vshader_filename);
	assert(!ZZ_HANDLE_IS_VALID(vshader_handles[format_in]));
	
	if (format_in >= num_format) {
		num_format = format_in + 1;
	}
	assert(num_format <= ZZ_MAX_SHADER_FORMAT);

    zz_handle handle =  znzin->renderer->create_vertex_shader(
			vshader_filename,
			vformat.get_format(),
			is_binary);
	
	vshader_handles[format_in] = handle;
	
	if (!vshader_names[format_in]) {
		vshader_names[format_in] = zz_new char[strlen(vshader_filename)+1];
		strcpy(vshader_names[format_in], vshader_filename);
	}

	return handle;
}

zz_handle zz_shader::create_pshader (const char * pshader_filename, int format_in)
{
	assert(pshader_filename);
	assert(!ZZ_HANDLE_IS_VALID(pshader_handles[format_in]));
	assert(format_in < num_format);

	zz_handle handle = znzin->renderer->create_pixel_shader(pshader_filename, is_binary);
	
	if (!pshader_names[format_in]) {
		pshader_names[format_in] = zz_new char[strlen(pshader_filename)+1];
		strcpy(pshader_names[format_in], pshader_filename);
	}
	
	pshader_handles[format_in] = handle;

	return handle;
}

// create unmanaged objects
bool zz_shader::restore_device_objects ()
{
	return true;
}

// destroy unmanaged objects
bool zz_shader::invalidate_device_objects ()
{
	return true;
}

// create managed objects
bool zz_shader::init_device_objects ()
{
	for (int i = 0; i < num_format; ++i) {
		if (vshader_names[i] && !ZZ_HANDLE_IS_VALID(vshader_handles[i])) {
			this->create_vshader(vshader_names[i], i);
		}
		if (pshader_names[i] && !ZZ_HANDLE_IS_VALID(pshader_handles[i])) {
			this->create_pshader(pshader_names[i], i);
		}
	}
	return true;
}

// destroy managed objects
bool zz_shader::delete_device_objects ()
{
	for (int i = 0; i < num_format; ++i) {
		if (ZZ_HANDLE_IS_VALID(vshader_handles[i])) {
			znzin->renderer->destroy_vertex_shader(vshader_handles[i]);
			vshader_handles[i] = ZZ_HANDLE_NULL;
		}
		if (ZZ_HANDLE_IS_VALID(pshader_handles[i])) {
			znzin->renderer->destroy_pixel_shader(pshader_handles[i]);
			pshader_handles[i] = ZZ_HANDLE_NULL;
		}
	}
	return true;
}

zz_handle zz_shader::get_vshader (int format_in, ZZ_RENDERWHERE render_where) const
{
	if (render_where == ZZ_RW_SHADOWMAP) {
		if (vformat.use_skin()) {
			zz_assert(shadow_shader_skin);
			return (shadow_shader_skin) ? shadow_shader_skin->vshader_handles[0] : ZZ_HANDLE_NULL;
		}
		else {
			zz_assert(shadow_shader);
			return (shadow_shader) ? shadow_shader->vshader_handles[0] : ZZ_HANDLE_NULL;
		}
	}
	else if (render_where == ZZ_RW_GLOW) {
		if (vformat.use_skin()) {
			zz_assert(glow_shader_skin);
			return (glow_shader_skin) ? glow_shader_skin->vshader_handles[0] : ZZ_HANDLE_NULL;
		}
		else {
			zz_assert(glow_shader);
			return (glow_shader) ? glow_shader->vshader_handles[0] : ZZ_HANDLE_NULL;
		}
	}

	// default render where
	zz_assert(format_in < num_format);
	return vshader_handles[format_in];
}

bool zz_shader::check_system_shaders ()
{
	if (!shadow_shader) return false;
	if (!shadow_shader_skin) return false;
	if (!glow_shader) return false;
	if (!glow_shader_skin) return false;
	if (!terrain_shader) return false;

	return true;
}
