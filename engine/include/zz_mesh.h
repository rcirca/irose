/** 
 * @file zz_mesh.h
 * @brief mesh class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    05-mar-2002
 *
 * $Header: /engine/include/zz_mesh.h 26    05-05-06 2:27p Choo0219 $
 */

#ifndef __ZZ_MESH_H__
#define __ZZ_MESH_H__

#include <vector>

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

#ifndef __ZZ_DEVICE_RESOURCE_H__
#include "zz_device_resource.h"
#endif

#ifndef __ZZ_VERTEX_FORMAT_H__
#include "zz_vertex_format.h"
#endif

#ifndef __ZZ_VERTEX_BUFFER_H__
#include "zz_vertex_buffer.h"
#endif

#ifndef __ZZ_INDEX_BUFFER_H__
#include "zz_index_buffer.h"
#endif

#ifndef __ZZ_LOD_H__
#include "zz_lod.h"
#endif

#include <assert.h>

enum zz_clip_face {
	ZZ_CLIP_FACE_NONE  = 0, // render all faces
	ZZ_CLIP_FACE_FIRST = 1, // clip first material group faces
	ZZ_CLIP_FACE_LAST  = 2, // clip last material group faces
	ZZ_CLIP_FACE_BOTH  = 3  // clip first and last material group faces
};

enum zz_mesh_usage {
	ZZ_MESH_USAGE_NONE = 0,
	ZZ_MESH_USAGE_TERRAIN = 1
};

//--------------------------------------------------------------------------------
class zz_mesh : public zz_node {
//--------------------------------------------------------------------------------
	friend class zz_mesh_tool;

	
public:
	typedef unsigned int zz_revision;

public:
	enum e_index_type {
		TYPE_NONE = 0,
		TYPE_LIST = 1,
		TYPE_STRIP = 2
	};
protected:
	// index buffer type
	e_index_type index_type;

	//--------------------------------------------------------------------------------
	// Vertex buffer creation parameters
	//--------------------------------------------------------------------------------
	char * vbuf; // vertex data buffer pointer. Created in mesh_tool:load()->set_num_verts(), and deleted in ~zz_mesh()->unload()->set_num_verts(0)
	zz_vertex_buffer * vbuf_res; // vertex buffer resource
	zz_vertex_format vertex_format; // vertex format used in create_vertex_buffer().
	//--------------------------------------------------------------------------------
	
	//--------------------------------------------------------------------------------
	// Index buffer creation parameters
	//--------------------------------------------------------------------------------
	zz_index_buffer * ibuf_res; // index buffer resource
	uint16 * ibuf_list; // index data buffer pointer. Created in mesh_tool:load()->set_num_faces(), and deleted in ~zz_mesh()->unload()->set_num_faces(0)
	uint16 * ibuf_strip; // index buffer pointer for triangle strip. created in mesh_tool:load()->generate_strip(), and deleted in ~zz_mesh()->unload()->set_num_faces(0)
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	zz_revision revision; // For version control. updated by load().
	zz_string path; // the mesh file name

	//--------------------------------------------------------------------------------
	// Mesh properties
	//--------------------------------------------------------------------------------
	uint16 num_indices; // the number of indices. it may not equal to num_faces*3 if triangle strip
	uint16 num_verts; // the number of vertices
	uint16 num_normals; // the number of normals
	uint16 num_faces; // the number of faces
	uint16 num_bones; // the number of bones used in this mesh
	uint16 num_uvs[4]; // the number of uv texture coordinates
	uint16 max_bone_index; // the largest bone index in bone_indices. initialized by -1
	//--------------------------------------------------------------------------------
	
	// for bounding box (AABB)
	vec3 pmin, pmax;

	// The number of material ids. 0 if all have the same material id
	uint16 num_matids;
	
	// Array pointer which indicates the number of faces with the same material id.
	// The memory will be allocated in set_num_matids() and deallocated in destructor.
	uint16 * matid_numfaces;

	// Eye-blinking
	uint16 num_clip_faces; // The number of clipped faces at the end of the index buffer. default is 0.
	zz_clip_face clip_face; // default is NONE

	zz_lod lod;

	bool delayed_rebind; // to unbind and bind before flush_device. its for set_dynamic() for delayed mode

	void set_pool (bool bindable); // set vertex|index buffer pool setting by bindable

	zz_time timestamp; // last update timestamp
	
public:
	// bone indices
	std::vector<uint16> bone_indices; // Bone indices used in this mesh. Each index indicates bone index in skeleton.

	float alpha;
	
	zz_mesh (bool create_buffer_in = true);
	virtual ~zz_mesh (void);

	zz_time get_timestamp (void) const { return timestamp; }
	void set_timestamp (zz_time timestamp_in) { timestamp = timestamp_in; }

	//--------------------------------------------------------------------------------
	// Renderer interface
	//--------------------------------------------------------------------------------
	zz_handle get_vbuffer_handle () const; // Get vertex buffer handle in renderer.
	zz_handle get_ibuffer_handle () const; // Get index buffer handle in renderer.
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// Get mesh property
	//--------------------------------------------------------------------------------
	const usvec3&		get_face					(uint16 index) const;
	const vec3&			get_pos					(uint16 index) const;
	const vec3&			get_normal				(uint16 index) const;
	const zz_color&	get_color					(uint16 index) const;
	const vec4&			get_blend_weight	(uint16 index) const;
	const vec4			get_blend_index		(uint16 index) const;
	const vec3&			get_tangent				(uint16 index) const;
	const vec2&			get_uv						(uint16 index, uint16 mapchannel) const;
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// Set mesh property
	//--------------------------------------------------------------------------------
	void set_face					(uint16 index, const usvec3& face);
	void set_pos					(uint16 index, const vec3& position);
	void set_normal				(uint16 index, const vec3& normal);
	void set_color					(uint16 index, const zz_color& color);
	void set_blend_weight	(uint16 index, const vec4& blend_weight);
	void set_blend_index		(uint16 index, const vec4& blend_index);
	void set_tangent				(uint16 index, const vec3& tangent);
	void set_uv						(uint16 index, uint16 mapchannel, const vec2& uv);
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// Position, normal, color, skin, tangent, uv
	//--------------------------------------------------------------------------------
	uint16 get_num_verts () const;
	uint16 get_num_faces () const;
	uint16 get_vertex_size (void) const;
	uint16 get_index_size (void) const;
	void set_num_verts (uint16 num_verts_in);
	virtual void set_num_faces (uint16 num_faces_in);
	uint16 get_num_indices () const
	{
		return num_indices;
	}
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// Get/set dynamic/bindable property
	//--------------------------------------------------------------------------------
	bool get_dynamic () const;
	void set_dynamic (bool true_or_false); // convert static|dynamic vertex buffer into dynamic|static
	bool get_bindable () const;
	void set_bindable (bool true_or_false);
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// Clip faces
	//--------------------------------------------------------------------------------
	void set_num_clip_faces (uint16 num);
	uint16 get_num_clip_faces () const;
	void set_clip_face (zz_clip_face clip_face_in);
	zz_clip_face get_clip_face () const;
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// set/get the number of material id
	//--------------------------------------------------------------------------------
	uint16 get_num_matids () const;
	void set_num_matids ( uint16 num_matids_in );
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// Set/get the number of faces with same materia id
	//--------------------------------------------------------------------------------
	uint16 get_matid_numfaces (uint16 matid) const;
	void set_matid_numfaces ( uint16 matid_in, uint16 num_faces_in );
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// Resource management from zz_resource
	//--------------------------------------------------------------------------------
	virtual bool load ();
	virtual bool unload ();
	virtual void init_reuse ();
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// Restoring device stuff
	//--------------------------------------------------------------------------------
	virtual bool restore_device_objects (); // create unmanaged objects
	virtual bool init_device_objects (); // create managed objects
	virtual bool invalidate_device_objects (); // destroy unmanaged objects
	virtual bool delete_device_objects (); // destroy managed objects
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// Internal resource management
	//--------------------------------------------------------------------------------
	virtual bool load_mesh ();
	zz_revision get_revision () const;
	bool get_device_ready () const;
	bool get_device_created () const;
	bool get_device_updated () const;
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// Index buffer management
	//--------------------------------------------------------------------------------
	virtual void create_index_buffer (); // overrided in zz_mesh_terrain
	virtual void destroy_index_buffer ();
	virtual void update_index_buffer ();
	//--------------------------------------------------------------------------------
	
	//--------------------------------------------------------------------------------
	// Vertex buffer management
	//--------------------------------------------------------------------------------
	void create_vertex_buffer ();
	void destroy_vertex_buffer ();
	void update_vertex_buffer ();
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// Set/get min/max
	//--------------------------------------------------------------------------------
	const vec3& get_min () const;
	const vec3& get_max () const;
	void set_min (const vec3& p_min);
	void set_max (const vec3& p_max);
	//--------------------------------------------------------------------------------

	const zz_vertex_format& get_vertex_format () const;
	void set_vertex_format (int format_in);
	
	// get the number of bone indices
	uint16 get_num_bone_indices () const;

	// get max bone index
	uint16 get_max_bone_index () const;

	// bind mesh to path
	bool set_path (const char * path_in);
	const char * get_path () const;

	// forcing load and bind to device before render this mesh
	bool flush_device (bool immediate);

	// get/set LOD level
	const zz_lod& get_lod () const;
	void set_lod (const zz_lod& lod_in); // only set lod level

	virtual void apply_lod () {} ; // apply level by lod level

	// return false if generating strip failed
	virtual bool generate_strip ();

	e_index_type get_index_type () { return index_type; }

	uint16 get_strip_index (uint16 index);

	// for debugging
	void dump_indices ();

	bool use_skin ()
	{
		return vertex_format.use_skin();
	}

	// just print out strip indices for num_faces_on_axis x num_faces_on_axis terrain mesh.
	// we hard-code it in zz_mesh_ishared.cpp
	static void print_strip_indices (uint16 input_num_verts, uint16 output_num_verts);

	static void create_strip_indices (
		uint16 num_faces_one_axis, 
		uint16& num_indices_out, 
		uint16 *& buffer_strip_out );

	ZZ_DECLARE_DYNAMIC(zz_mesh)
};

inline void zz_mesh::set_lod (const zz_lod& lod_in)
{
	lod = lod_in;
}

inline const zz_lod& zz_mesh::get_lod () const
{
	return lod;
}

inline zz_mesh::zz_revision zz_mesh::get_revision () const
{
	return revision;
}

inline uint16 zz_mesh::get_vertex_size (void) const
{
	return vertex_format.size;
}

inline uint16 zz_mesh::get_index_size (void) const
{
	return zz_index_buffer::get_index_size();
}

inline void zz_mesh::set_num_clip_faces (uint16 num)
{
	num_clip_faces = num;
}

inline uint16 zz_mesh::get_num_clip_faces () const
{
	return num_clip_faces;
}

inline void zz_mesh::set_clip_face (zz_clip_face clip_face_in)
{
	clip_face = clip_face_in;
}

inline zz_clip_face zz_mesh::get_clip_face () const
{
	return clip_face;
}

// set/get number of material id
inline uint16 zz_mesh::get_num_matids () const
{
	return num_matids;
}

// set/get number of faces with same materia id
inline uint16 zz_mesh::get_matid_numfaces (uint16 matid) const
{
	assert(matid < num_matids);
	assert(matid_numfaces);
	return matid_numfaces[matid];
}

inline void zz_mesh::set_matid_numfaces ( uint16 matid_in, uint16 num_faces_in )
{
	assert(matid_in < num_matids);
	assert(matid_numfaces);
	matid_numfaces[matid_in] = num_faces_in;
}

inline uint16 zz_mesh::get_num_bone_indices () const
{
	return static_cast<uint16>(bone_indices.size());
}

inline uint16 zz_mesh::get_num_verts () const
{
	return num_verts;
}

inline uint16 zz_mesh::get_num_faces () const
{
	return num_faces;
}

inline const vec3& zz_mesh::get_min () const
{
	return pmin;
}

inline const vec3& zz_mesh::get_max () const
{
	return pmax;
}

inline void zz_mesh::set_min (const vec3& p_min)
{
	pmin = p_min;
}

inline void zz_mesh::set_max (const vec3& p_max)
{
	pmax = p_max;
}

inline void zz_mesh::set_vertex_format (int format_in)
{
	vertex_format.set_format(format_in);
}
	
inline const zz_vertex_format& zz_mesh::get_vertex_format () const
{
	return vertex_format;
}

inline bool zz_mesh::get_device_ready () const
{
	assert(vbuf_res);
	return vbuf_res->get_ready();
}

inline bool zz_mesh::get_device_created () const
{
	assert(vbuf_res);
	return vbuf_res->get_created();
}

inline bool zz_mesh::get_device_updated () const
{
	assert(vbuf_res);
	return vbuf_res->get_updated();
}

inline uint16 zz_mesh::get_max_bone_index () const
{
	return max_bone_index;
}

inline const vec3& zz_mesh::get_pos (uint16 index) const
{
	assert(vbuf);
	return *(vec3*)(vbuf + index*vertex_format.size + vertex_format.pos_offset);
}

inline const vec3& zz_mesh::get_normal (uint16 index) const
{
	assert(vbuf);
	return *(vec3*)(vbuf + index*vertex_format.size + vertex_format.normal_offset);
}

inline const zz_color& zz_mesh::get_color (uint16 index) const
{
	assert(vbuf);
	return *(zz_color*)(vbuf + index*vertex_format.size + vertex_format.color_offset);
}

inline const vec4& zz_mesh::get_blend_weight (uint16 index) const
{
	assert(vbuf);
	return *(vec4*)(vbuf + index*vertex_format.size + vertex_format.blend_weight_offset);
}

inline const vec4 zz_mesh::get_blend_index (uint16 index) const
{
	assert(vbuf);
	// to direct-access bone matrix index, multiply 3 with bone index.
	// each bone matrix consumes 3 indices.
	vec4 new_index = *(vec4*)(vbuf + index*vertex_format.size + vertex_format.blend_index_offset);
	new_index.x = float(int(new_index.x) / 3); // revert for set_blend_index()
	new_index.y = float(int(new_index.y) / 3); // revert for set_blend_index()
	new_index.z = float(int(new_index.z) / 3); // revert for set_blend_index()
	new_index.w = float(int(new_index.w) / 3); // revert for set_blend_index()
	return new_index;
}

inline const vec3& zz_mesh::get_tangent (uint16 index) const
{
	assert(vbuf);
	assert(index < num_verts);
	return *(vec3*)(vbuf + index*vertex_format.size + vertex_format.tangent_offset);
}

inline const vec2& zz_mesh::get_uv (uint16 index, uint16 mapchannel) const
{
	assert(vbuf);
	assert(index < num_verts);
	return *(vec2*)(vbuf + index*vertex_format.size + vertex_format.uv_offset[mapchannel]);
}

inline const usvec3& zz_mesh::get_face (uint16 index) const
{
	assert(ibuf_list);
	assert(index < num_faces);
	return *(usvec3*)(ibuf_list + index*3);
}

inline void zz_mesh::set_face (uint16 index, const usvec3& face)
{
	assert(ibuf_list);
	assert(index < num_faces);
	*(usvec3*)(ibuf_list + index*3) = face; // unsigned short
}

inline void zz_mesh::set_pos (uint16 index, const vec3& position)
{
	assert(vbuf);
	assert(index < this->num_verts);
	*(vec3*)(vbuf + index*vertex_format.size + vertex_format.pos_offset) = position;
}

inline void zz_mesh::set_normal (uint16 index, const vec3& normal)
{
	assert(vbuf);
	assert(index < this->num_verts);
	*(vec3*)(vbuf + index*vertex_format.size + vertex_format.normal_offset) = normal;	
}

inline void zz_mesh::set_color (uint16 index, const zz_color& color)
{
	assert(vbuf);
	assert(index < this->num_verts);
	*(zz_color*)(vbuf + index*vertex_format.size + vertex_format.color_offset) = color;		
}

inline void zz_mesh::set_blend_weight (uint16 index, const vec4& blend_weight)
{
	assert(vbuf);
	assert(index < this->num_verts);
	*(vec4*)(vbuf + index*vertex_format.size + vertex_format.blend_weight_offset) = blend_weight;	
}

inline void zz_mesh::set_blend_index (uint16 index, const vec4& blend_index)
{
	assert(vbuf);
	assert(index < this->num_verts);
	*(vec4*)(vbuf + index*vertex_format.size + vertex_format.blend_index_offset) = 3.0f*blend_index; // /3 in get_blend_index()
}

inline void zz_mesh::set_tangent (uint16 index, const vec3& tangent)
{
	assert(vbuf);
	assert(index < this->num_verts);
	*(vec3*)(vbuf + index*vertex_format.size + vertex_format.tangent_offset) = tangent;
}

inline void zz_mesh::set_uv (uint16 index, uint16 mapchannel, const vec2& uv)
{
	assert(vbuf);
	assert(index < this->num_verts);
	*(vec2*)(vbuf + index*vertex_format.size + vertex_format.uv_offset[mapchannel]) = uv;	
}

inline uint16 zz_mesh::get_strip_index (uint16 index)
{
	assert(index < num_indices);
	return ibuf_strip[index];
}

inline zz_handle zz_mesh::get_vbuffer_handle () const
{
	return vbuf_res->get_device_handle();
}

inline zz_handle zz_mesh::get_ibuffer_handle () const
{
	return ibuf_res->get_device_handle();
}
#endif //__ZZ_MESH_H__