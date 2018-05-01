/** 
 * @file zz_shader.h
 * @brief shader class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-mar-2002
 *
 * $Header: /engine/include/zz_shader.h 10    04-09-05 6:51p Zho $
 * $History: zz_shader.h $
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-09-05   Time: 6:51p
 * Updated in $/engine/include
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-08-11   Time: 6:20p
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-07-16   Time: 7:53p
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-07-10   Time: 4:31p
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-06-08   Time: 10:35a
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-06-03   Time: 11:18a
 * Updated in $/engine/include
 * glow code updated.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-05-19   Time: 8:15p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-15   Time: 4:26p
 * Updated in $/engine/include
 * Redesigning mesh structure.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 03-11-27   Time: 4:50p
 * Updated in $/znzin11/engine/include
 * - removed zz_resource class, added full-support of resource management
 * into zz_node, and cleaned up some codes.
 */

#ifndef __ZZ_SHADER_H__
#define __ZZ_SHADER_H__

#ifndef __ZZ_MESH_H__
#include "zz_mesh.h"
#endif

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

#ifndef __ZZ_VERTEX_FORMAT_H__
#include "zz_vertex_format.h"
#endif

#ifndef __ZZ_LOD_H__
#include "zz_lod.h"
#endif

#ifndef __ZZ_RENDERER_H__
#include "zz_renderer.h"
#endif

// shader format
#define SHADER_FORMAT_FIRSTMAP					(1 << 0)
#define SHADER_FORMAT_SECONDMAP				(1 << 1)
#define SHADER_FORMAT_SPECULARMAP			(1 << 2)
#define SHADER_FORMAT_IRADIANCEMAP			(1 << 3)
#define SHADER_FORMAT_LIGHTMAP					(1 << 4)
#define SHADER_FORMAT_SHADOWMAP				(1 << 5)
//#define SHADER_FORMAT_GLOW						(1 << 6)
#define SHADER_FORMAT_FOG							(1 << 7)
#define ZZ_MAX_SHADER_FORMAT						(1 << 15)

#define SHADER_FORMAT_DEFAULT			(0)
#define SHADER_FORMAT_INVALID				ZZ_MAX_SHADER_FORMAT

//--------------------------------------------------------------------------------
class zz_shader : public zz_node {
protected:
	// handle starts from 0. -1 means no shader
	zz_handle vshader_handles[ZZ_MAX_SHADER_FORMAT]; // vertex shader handle array  
	zz_handle pshader_handles[ZZ_MAX_SHADER_FORMAT]; // pixel shader handle array

	// names for recovery
	char * vshader_names[ZZ_MAX_SHADER_FORMAT];
	char * pshader_names[ZZ_MAX_SHADER_FORMAT];

	int num_format; // the number of shader pairs

	zz_vertex_format vformat; // vertex format which this shader uses

	bool is_binary;

public:
	static zz_shader * shadow_shader;
	static zz_shader * shadow_shader_skin;
	static zz_shader * glow_shader;
	static zz_shader * glow_shader_skin;
	static zz_shader * terrain_shader;

	zz_shader(void);
	virtual ~zz_shader(void);

	static bool init_shader (); // load default shaders. called by system
	static bool dest_shader (); // cleanup default shaders. called by system

	bool has_skin() const
	{
		return vformat.use_skin();
	}

	zz_handle get_vshader (int format_in = SHADER_FORMAT_DEFAULT, ZZ_RENDERWHERE render_where = ZZ_RW_SCENE) const;
	zz_handle get_pshader (int format_in = SHADER_FORMAT_DEFAULT, ZZ_RENDERWHERE render_where = ZZ_RW_SCENE) const
	{
		assert(format_in < num_format);
		return pshader_handles[format_in];
	}

	// set current format
	void set_num_format (int num_format_in)
	{
		num_format= num_format_in;
	}

	int get_num_format () const
	{
		return num_format;
	}
	
	void set_vertex_format (int vertex_format_in)
	{
		vformat.set_format(vertex_format_in);	
	}

	const zz_vertex_format& get_vertex_format () const
	{
		return vformat;
	}

	void set_is_binary (bool is_binary_in)
	{
		is_binary = is_binary_in;
	}

	bool get_is_binary () const
	{
		return is_binary;
	}

	zz_handle create_vshader (const char * vshader_filename, int format_in);
	zz_handle create_pshader (const char * pshader_filename, int format_in);

	// managing device objects
	virtual void set_load_weight (int weight_in) { }
	virtual int get_load_weight () const { return 0; }
	virtual bool bind_device () { return true; }
	virtual bool unbind_device () { return true; }
	virtual bool restore_device_objects (); // create unmanaged objects
	virtual bool invalidate_device_objects (); // destroy unmanaged objects
	virtual bool init_device_objects (); // create managed objects
	virtual bool delete_device_objects (); // destroy managed objects

	static bool check_system_shaders (); // check if has all system default shaders

	ZZ_DECLARE_DYNAMIC(zz_shader)
};

#endif // __ZZ_SHADER_H__