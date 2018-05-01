/** 
 * @file zz_mesh_tool.cpp
 * @brief mesh tool class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    05-sep-2002
 *
 * $Header: /engine/src/zz_mesh_tool.cpp 41    05-02-14 3:16p Choo0219 $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_font.h"
#include "zz_system.h"
#include "zz_script_simple.h"
#include "zz_terrain_block.h"
#include "zz_visible.h"
#include "zz_profiler.h"
#include "zz_renderer.h"
#include "zz_manager.h"
#include "zz_camera.h"
#include "zz_material.h"
#include "zz_mesh_tool.h"
#include "zz_mesh_terrain.h"
#include "zz_mesh_ocean.h"
#include "zz_autolock.h"
#include "zz_vfs_thread.h"
#include "zz_vfs_pkg.h"
#include "zz_model.h"

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

#define ZZ_MESH_VERSION5	"ZMS0005"
#define ZZ_MESH_VERSION6	"ZMS0006"
#define ZZ_MESH_VERSION7	"ZMS0007"
#define ZZ_MESH_VERSION8	"ZMS0008"
#define ZZ_MESH_VERSION		ZZ_MESH_VERSION8 // current version

#include <vector>
#include <algorithm>

using namespace std;

// for debugging
#define DEBUG_MESHFILE
bool error_logged = false;

zz_mesh_tool::zz_mesh_tool ()
{
}

zz_mesh_tool::~zz_mesh_tool ()
{
}

//
// +-----> u-axis
// |
// |
// V v-axis
vec2 zz_mesh_tool::get_uv_by_type(int face_width, int ix, int iy, zz_uv_type uv_type)
{
	vec2 ouv, uv; // original uv

	ouv.x = float(ix)/face_width;
	ouv.y = float(iy)/face_width;

	switch (uv_type) {
		case ZZ_UV_LR:
			uv.x = 1.0f - ouv.x;
			uv.y = ouv.y;
			break;
		case ZZ_UV_TB:
			uv.x = ouv.x;
			uv.y = 1.0f - ouv.y;
			break;
		case ZZ_UV_LRTB:
			uv.x = 1.0f - ouv.x;
			uv.y = 1.0f - ouv.y;
			break;
		case ZZ_UV_90CW:
			uv.x = ouv.y;
			uv.y = 1.0f - ouv.x;
			break;
		case ZZ_UV_90CCW:
			uv.x = 1.0f - ouv.y;
			uv.y = ouv.x;
			break;
		default:
			uv = ouv;
	}
	return uv;
}

// read only min-max info
bool zz_mesh_tool::load_mesh_minmax (const char * file_name, zz_mesh * mesh, bool text_mode)
{
	//ZZ_LOG("mesh_tool: load_mesh_minmax(%s) begin\n", file_name);
	char str_buf[ZZ_MAX_STRING];
	uint32 uint32_buf;
	int version = 0;

	zz_vfs_pkg mesh_file;
	
	if (!mesh_file.open(file_name, zz_vfs::ZZ_VFS_READ)) {
		ZZ_LOG("mesh_tool: load_mesh_minmax(%s) failed. cannot open file\n", file_name);
		return false;
	}

	// [version]
	if (!mesh_file.read_string(str_buf)) {
		return false;
	}
	if (strcmp(str_buf, ZZ_MESH_VERSION8) == 0) {
		version = 8;
	}
	else 	if (strcmp(str_buf, ZZ_MESH_VERSION7) == 0) {
		version = 7;
	}
	else if (strcmp(str_buf, ZZ_MESH_VERSION6) == 0) {
		version = 6;
	}
	else if (strcmp(str_buf, ZZ_MESH_VERSION5) == 0) {
		version = 5;
	}
	else {
		ZZ_LOG("mesh_tool: load_mesh(%s) failed. version dismatched(%s)\n",
			str_buf, ZZ_MESH_VERSION);
		return false;
	}

	// [vertex_format]
	mesh_file.read_uint32(uint32_buf);
	mesh->set_vertex_format(uint32_buf);

	// [mix]
	mesh_file.read_float3(mesh->pmin.vec_array);

	if (version < 7) {
		mesh->pmin.x = ZZ_XFORM_IN(mesh->pmin.x);
		mesh->pmin.y = ZZ_XFORM_IN(mesh->pmin.y);
		mesh->pmin.z = ZZ_XFORM_IN(mesh->pmin.z);
	}

	// [max]
	mesh_file.read_float3(mesh->pmax.vec_array);
	if (version < 7) {
		mesh->pmax.x = ZZ_XFORM_IN(mesh->pmax.x);
		mesh->pmax.y = ZZ_XFORM_IN(mesh->pmax.y);
		mesh->pmax.z = ZZ_XFORM_IN(mesh->pmax.z);
	}
	
	//ZZ_LOG("mesh_tool: load_mesh_minmax(%s) end\n", file_name);
	return true;
}

bool zz_mesh_tool::load_mesh_6 ( const char * file_name, zz_mesh * mesh, bool text_mode )
{
	if (text_mode) return false; // Currently, does not support text mode for performance.

	int version = 0;
	zz_vfs_pkg mesh_file;
	
	if (!mesh_file.open(file_name, zz_vfs::ZZ_VFS_READ)) {
		ZZ_LOG("mesh_tool: load_mesh(%s) failed. cannot open file\n", file_name);
		return false;
	}

	// [version]
	char str_buf[ZZ_MAX_STRING] = "";
	if (!mesh_file.read_string(str_buf)) {
		return false;
	}
	else if (strcmp(str_buf, ZZ_MESH_VERSION6) == 0) {
		version = 6;
	}
	else if (strcmp(str_buf, ZZ_MESH_VERSION5) == 0) {
		version = 5;
	}
	else {
		assert(!"mesh vertios dismatched");
		ZZ_LOG("mesh_tool: load_mesh(%s) failed. version dismatched(%s)\n",
			str_buf, ZZ_MESH_VERSION);
		return false;
	}
	
	uint16 i;
	uint32 uint32_buf;
	uint16 num_verts(0);
	uint16 num_faces(0);
	uint16 num_bones(0);
	vec2 xy;
	vec3 xyz;
	vec4 xyzw;
	usvec3 usxyz;
	usvec4 usxyzw;
	uivec3 uixyz;
	uivec4 uixyzw;
	uint32 index;

	// [vertex_format]
	mesh_file.read_uint32(uint32_buf);
	mesh->set_vertex_format(uint32_buf);
	
	zz_vertex_format format = mesh->get_vertex_format();

	bool force_normal = false;
	if (!format.use_normal()) {
		force_normal = true;
		mesh->set_vertex_format(uint32_buf | ZZ_VF_NORMAL);
	}

	// [mix/max]
	mesh_file.read_float3(mesh->pmin.vec_array);
	mesh_file.read_float3(mesh->pmax.vec_array);

	mesh->pmin.x = ZZ_XFORM_IN(mesh->pmin.x);
	mesh->pmin.y = ZZ_XFORM_IN(mesh->pmin.y);
	mesh->pmin.z = ZZ_XFORM_IN(mesh->pmin.z);
	mesh->pmax.x = ZZ_XFORM_IN(mesh->pmax.x);
	mesh->pmax.y = ZZ_XFORM_IN(mesh->pmax.y);
	mesh->pmax.z = ZZ_XFORM_IN(mesh->pmax.z);

	// [num_bones]
	mesh_file.read_uint32(uint32_buf);
	num_bones = uint32_buf;
	mesh->bone_indices.reserve(num_bones);
	mesh->bone_indices.clear();

	// [bone]
	for (i = 0; i < num_bones; i++) {
		mesh_file.read_uint32(index); // index
		mesh_file.read_uint32(uixyz.x); // bone index
		if (index != i) return false; // index does not match
		mesh->bone_indices.push_back(uixyz.x);
	}

	// [num_verts]
	mesh_file.read_uint32(uint32_buf);
	num_verts = uint32_buf;
	
	// [positions]
	mesh->set_num_verts(num_verts);

	for (i = 0; i < num_verts; ++i) {
		mesh_file.read_uint32(index);		
		mesh_file.read_float3(xyz.vec_array);
		xyz.x = ZZ_XFORM_IN(xyz.x);
		xyz.y = ZZ_XFORM_IN(xyz.y);
		xyz.z = ZZ_XFORM_IN(xyz.z);
		mesh->set_pos(i, xyz);
	}

	// [normal]
	const vec3 zaxis(0, 0, 1);
	if (force_normal) {
		for (i = 0; i < num_verts; ++i) {
			mesh->set_normal(i, zaxis);
		}
	}
	else if (format.use_normal()) {
		//mesh->normals.reserve(num_verts);
		//mesh->normals.clear();
		for (i = 0; i < num_verts; ++i) {
			mesh_file.read_uint32(index);
			mesh_file.read_float3(xyz.vec_array);
			mesh->set_normal(i, xyz);
		}
	}

	// [color]
	if (format.use_color()) {
		for (i = 0; i < num_verts; ++i) {
			mesh_file.read_uint32(index);
			mesh_file.read_float4(xyzw.vec_array);
			mesh->set_color(i, zz_color(
				(char)(xyzw.x*255.0f), 
				(char)(xyzw.y*255.0f), 
				(char)(xyzw.z*255.0f),
				(char)(xyzw.w*255.0f))); // A >> R >> G >> B
		}
	}

	// [skin]
	int max_bone_index = -1;
	int bone_index = 0;
	float blend_sum;
	int j;
	int indexes[4];
	float weights[4];
	float big_weight;
	int big_index;

	if (format.use_skin()) {
		blend_sum = 0.0f;
		for (i = 0; i < num_verts; ++i) {
			mesh_file.read_uint32(index);
			mesh_file.read_float4(xyzw.vec_array);
			mesh_file.read_uint324(uixyzw.uint_array);

#ifdef DEBUG_MESHFILE
			assert(uixyzw.x < MAX_NUM_BONES);
			assert(uixyzw.y < MAX_NUM_BONES);
			assert(uixyzw.z < MAX_NUM_BONES);
			assert(uixyzw.w < MAX_NUM_BONES);
			if ((uixyzw.x >= MAX_NUM_BONES) || (uixyzw.y >= MAX_NUM_BONES) || (uixyzw.z >= MAX_NUM_BONES) || (uixyzw.w >= MAX_NUM_BONES)) {
				if (!error_logged) {
					assert(!"mesh_tool: load_mesh() failed. index exceeded max_num_bones");
					ZZ_LOG("mesh_tool: load_mesh(%s) failed. bones(%d). skin(%d), uix(%d), uiy(%d), uiz(%d), uiw(%d)\n",
						file_name, i, index, uixyzw.x, uixyzw.y, uixyzw.z, uixyzw.w);
					ZZ_LOG("...num_verts(%d), num_bones(%d)\n", num_verts, num_bones);
					error_logged = true;
				}
				index = 0;
				uixyzw.x = uixyzw.y = uixyzw.z = uixyzw.w = 0;
			}
#endif

			// confirm data
			blend_sum = xyzw.x + xyzw.y + xyzw.z + xyzw.w;

			if (1) { // sort by weight
				indexes[0] = uixyzw.x; indexes[1] = uixyzw.y; indexes[2] = uixyzw.z; indexes[3] = uixyzw.w;
				weights[0] = xyzw.x; weights[1] = xyzw.y; weights[2] = xyzw.z; weights[3] = xyzw.w;
				big_weight = weights[0];
				big_index = indexes[0];
				for (j = 1; j < 4; j++) {
					if (weights[j] > big_weight) { // swap
						indexes[0] = indexes[j];
						weights[0] = weights[j];
						indexes[j] = big_index;
						weights[j] = big_weight;
						big_index = indexes[0];
						big_weight = weights[0];
					}
				}
				big_index = indexes[1];
				big_weight = weights[1];
				for (j = 2; j < 4; j++) {
					if (weights[j] > big_weight) { // swap
						indexes[1] = indexes[j];
						weights[1] = weights[j];
						indexes[j] = big_index;
						weights[j] = big_weight;
						big_index = indexes[1];
						big_weight = weights[1];
					}
				}
				big_index = indexes[2];
				big_weight = weights[2];
				for (j = 3; j < 4; j++) {
					if (weights[j] > big_weight) { // swap
						indexes[2] = indexes[j];
						weights[2] = weights[j];
						indexes[j] = big_index;
						weights[j] = big_weight;
						big_index = indexes[2];
						big_weight = weights[2];
					}
				}
				xyzw.x = weights[0]; xyzw.y = weights[1]; xyzw.z = weights[2]; xyzw.w = weights[3];
				uixyzw.x = indexes[0]; uixyzw.y = indexes[1]; uixyzw.z = indexes[2]; uixyzw.w = indexes[3];
			}

			if (blend_sum < 0.99f) {
				// Maybe, there is some "skin not affected" vertices when exporting by 3dsmax znzin plugin.
				assert(!"mesh_tool: load_mesh_6() blend_sum error!");
				ZZ_LOG("mesh_tool: load_mesh(%s) failed. This mesh has skin, but blend_sum is just %f.\n", mesh->get_path(), blend_sum);
				// force skinned by 0
				xyzw.x = 1.0f;
				xyzw.y = xyzw.z = xyzw.w = 0.0f;
			}

			// set max bone index
			assert(uixyzw.x < mesh->bone_indices.size());
			assert(uixyzw.y < mesh->bone_indices.size());
			assert(uixyzw.z < mesh->bone_indices.size());
			assert(uixyzw.w < mesh->bone_indices.size());

#ifdef DEBUG_MESHFILE
			if ((uixyzw.x >= num_bones) || (uixyzw.y >= num_bones) || (uixyzw.z >= num_bones) || (uixyzw.w >= num_bones)) {
				assert(!"mesh_tool: load_mesh() failed. bone index exceed num_bones");
				ZZ_LOG("mesh_tool: load_mesh() failed. uix(%d)/y(%d)/z(%d)/w(%d) >= num_bones(%d)\n",
					uixyzw.x, uixyzw.y, uixyzw.z, uixyzw.w, num_bones);
				uixyzw.x = 0; uixyzw.y = 0; uixyzw.z = 0; uixyzw.w = 0;
			}
#endif

			bone_index = mesh->bone_indices[uixyzw.x]; max_bone_index = (bone_index > max_bone_index) ? bone_index : max_bone_index;
			bone_index = mesh->bone_indices[uixyzw.y]; max_bone_index = (bone_index > max_bone_index) ? bone_index : max_bone_index;
			bone_index = mesh->bone_indices[uixyzw.z]; max_bone_index = (bone_index > max_bone_index) ? bone_index : max_bone_index;
			bone_index = mesh->bone_indices[uixyzw.w]; max_bone_index = (bone_index > max_bone_index) ? bone_index : max_bone_index;

			mesh->set_blend_weight(i, xyzw);
			mesh->set_blend_index(i, vec4((float)(uixyzw.x), (float)(uixyzw.y), (float)(uixyzw.z), (float)(uixyzw.w)));
		}
	}
	mesh->max_bone_index = max_bone_index;

	// [tangent]
	if (format.use_tangent()) {
		for (i = 0; i < num_verts; ++i) {
			mesh_file.read_uint32(index);
			mesh_file.read_float3(xyz.vec_array);
			mesh->set_tangent(i, xyz);
		}
	}

	assert("no mapchannel" && (format.get_num_mapchannel() > 0)); 

	// [uv0-4]
	int num_mapchannel = format.get_num_mapchannel();
	for (uint16 i_channel = 0; i_channel < num_mapchannel; ++i_channel) {
		for (i = 0; i < num_verts; ++i) {
			mesh_file.read_uint32(index);
			mesh_file.read_float2(xy.vec_array);
			mesh->set_uv(i, i_channel, xy);
		}
	}

	// [num_faces]
	mesh_file.read_uint32(uint32_buf);
	num_faces = uint32_buf;
	assert(num_faces < MAX_NUM_FACES);
	mesh->set_num_faces(num_faces);

	for (i = 0; i < num_faces; i++) {
		mesh_file.read_uint32(index);
		mesh_file.read_uint323(uixyz.uint_array);

#ifdef DEBUG_MESHFILE
		assert(index < num_faces);
		assert(uixyz.x < num_verts);
		assert(uixyz.y < num_verts);
		assert(uixyz.z < num_verts);
		assert(i == index);
		if ((i != index) || (index >= num_faces) || (uixyz.x >= num_verts) || (uixyz.y >= num_verts) || (uixyz.z >= num_verts)) {
			if (!error_logged) {
				assert(!"mesh_tool: load_mesh() failed. face index > num_faces");
				ZZ_LOG("mesh_tool: load_mesh(%s) failed. face(%d >? %d), uix(%d), uiy(%d), uiz(%d) >? num_faces(%d)\n",
					file_name, index, num_faces, uixyz.x, uixyz.y, uixyz.z, num_verts);
				// other info
				ZZ_LOG("...num_verts(%d), num_bones(%d), num_faces(%d)\n", num_verts, num_bones, num_faces);
				error_logged = true;
			}

			index = 0;
			uixyz.x = uixyz.y = uixyz.z = 0;
		}
#endif

		mesh->set_face(index, usvec3(uixyz.x, uixyz.y, uixyz.z));
	}
	
	// added in version ZMS0006
	if (version >= 6) { // support material id
		uint32 num_matids, mat_index, matid_numfaces;

		mesh_file.read_uint32(num_matids);
		
		mesh->set_num_matids(num_matids);

		for (i = 0; i < num_matids; i++) {
			mesh_file.read_uint32(mat_index);
			mesh_file.read_uint32(matid_numfaces);
			mesh->set_matid_numfaces(i, matid_numfaces);
		}
		// set clipfaces
		if (num_matids > 0) {
			mesh->set_num_clip_faces(mesh->get_matid_numfaces(num_matids-1));
		}
	}

	mesh->index_type = zz_mesh::TYPE_LIST;

	return true;
}


bool zz_mesh_tool::load_mesh_8 ( const char * file_name, zz_mesh * mesh, bool text_mode )
{
	if (text_mode) return false; // Currently, does not support text mode for performance.

	int version = 0;
	zz_vfs_pkg mesh_file;
	
	if (!mesh_file.open(file_name, zz_vfs::ZZ_VFS_READ)) {
		ZZ_LOG("mesh_tool: load_mesh(%s) failed. cannot open file\n", file_name);
		return false;
	}

	// [version]
	char str_buf[ZZ_MAX_STRING] = "";
	if (!mesh_file.read_string(str_buf)) {
		return false;
	}

	if (strcmp(str_buf, ZZ_MESH_VERSION8) == 0) {
		version = 8;
	}
	else if (strcmp(str_buf, ZZ_MESH_VERSION7) == 0) {
		version = 7;
	}
	else {
		assert(!"mesh version dismatched");
		ZZ_LOG("mesh_tool: load_mesh(%s) failed. version dismatched(%s)\n",
			str_buf, ZZ_MESH_VERSION);
		return false;
	}

	uint16 i;
	uint16 uint16_buf;
	uint32 uint32_buf;
	uint16 num_verts(0);
	uint16 num_faces(0);
	uint16 num_bones(0);
	vec2 xy;
	vec3 xyz;
	vec4 xyzw;
	usvec3 usxyz;
	usvec4 usxyzw;
	uivec3 uixyz;
	uivec4 uixyzw;

	// [vertex_format]
	mesh_file.read_uint32(uint32_buf);
	mesh->set_vertex_format(uint32_buf);
	
	zz_vertex_format format = mesh->get_vertex_format();

	bool force_normal = false;
	if (!format.use_normal()) {
		force_normal = true;
		mesh->set_vertex_format(uint32_buf | ZZ_VF_NORMAL);
	}

	// [mix/max]
#ifdef _DEBUG
	// check equlity from one at load_mesh_minmax()
	vec3 saved_min(mesh->pmin), saved_max(mesh->pmax);
#endif

	mesh_file.read_float3(mesh->pmin.vec_array);
	mesh_file.read_float3(mesh->pmax.vec_array);

//#ifdef _DEBUG
//	// check equlity from one at load_mesh_minmax()
//	assert(mesh->pmin == saved_min);
//	assert(mesh->pmax == saved_max);
//#endif

	// [num_bones]
	mesh_file.read_uint16(num_bones);
	mesh->bone_indices.reserve(num_bones);
	mesh->bone_indices.clear();

	// [bone]
	for (i = 0; i < num_bones; i++) {
		mesh_file.read_uint16(uint16_buf); // bone index
		mesh->bone_indices.push_back(uint16_buf);
	}

	// [num_verts]
	mesh_file.read_uint16(num_verts);
	
	// [positions]
	mesh->set_num_verts(num_verts);

	for (i = 0; i < num_verts; ++i) {
		mesh_file.read_float3(xyz.vec_array);
		mesh->set_pos(i, xyz);
	}

	// [normal]
	const vec3 zaxis(0, 0, 1);
	if (force_normal) {
		for (i = 0; i < num_verts; ++i) {
			mesh->set_normal(i, zaxis);
		}
	}
	else if (format.use_normal()) {
		for (i = 0; i < num_verts; ++i) {
			mesh_file.read_float3(xyz.vec_array);
			mesh->set_normal(i, xyz);
		}
	}

	// [color]
	if (format.use_color()) {
		for (i = 0; i < num_verts; ++i) {
			mesh_file.read_float4(xyzw.vec_array);
			mesh->set_color(i, zz_color((char)(xyzw.x*255.0f), (char)(xyzw.y*255.0f), (char)(xyzw.z*255.0f), (char)(xyzw.w*255.0f))); // A >> R >> G >> B
		}
	}

	// [skin]
	uint16 max_bone_index = -1;
	uint16 bone_index = 0;
	float blend_sum;
	uint16 j;
	uint16 indexes[4];
	float weights[4];
	float big_weight;
	uint16 big_index;

	if (format.use_skin()) {
		blend_sum = 0.0f;
		for (i = 0; i < num_verts; ++i) {
			mesh_file.read_float4(xyzw.vec_array);
			mesh_file.read_uint164(usxyzw.ushort_array);

			// confirm data
			blend_sum = xyzw.x + xyzw.y + xyzw.z + xyzw.w;

			if (1) { // sort by weight
				indexes[0] = usxyzw.x; indexes[1] = usxyzw.y; indexes[2] = usxyzw.z; indexes[3] = usxyzw.w;
				weights[0] = xyzw.x; weights[1] = xyzw.y; weights[2] = xyzw.z; weights[3] = xyzw.w;
				big_weight = weights[0];
				big_index = indexes[0];
				for (j = 1; j < 4; j++) {
					if (weights[j] > big_weight) { // swap
						indexes[0] = indexes[j];
						weights[0] = weights[j];
						indexes[j] = big_index;
						weights[j] = big_weight;
						big_index = indexes[0];
						big_weight = weights[0];
					}
				}
				big_index = indexes[1];
				big_weight = weights[1];
				for (j = 2; j < 4; j++) {
					if (weights[j] > big_weight) { // swap
						indexes[1] = indexes[j];
						weights[1] = weights[j];
						indexes[j] = big_index;
						weights[j] = big_weight;
						big_index = indexes[1];
						big_weight = weights[1];
					}
				}
				big_index = indexes[2];
				big_weight = weights[2];
				for (j = 3; j < 4; j++) {
					if (weights[j] > big_weight) { // swap
						indexes[2] = indexes[j];
						weights[2] = weights[j];
						indexes[j] = big_index;
						weights[j] = big_weight;
						big_index = indexes[2];
						big_weight = weights[2];
					}
				}
				xyzw.x = weights[0]; xyzw.y = weights[1]; xyzw.z = weights[2]; xyzw.w = weights[3];
				usxyzw.x = indexes[0]; usxyzw.y = indexes[1]; usxyzw.z = indexes[2]; usxyzw.w = indexes[3];
			}

			if (blend_sum < 0.99f) {
				// Maybe, there is some "skin not affected" vertices when exporting by 3dsmax znzin plugin.
				assert(!"mesh_tool: load_mesh_7() blend_sum error!");
				ZZ_LOG("mesh_tool: load_mesh(%s) failed. This mesh has skin, but blend_sum is just %f.\n", mesh->get_path(), blend_sum);
				// force skinned by 0
				xyzw.x = 1.0f;
				xyzw.y = xyzw.z = xyzw.w = 0.0f;
			}

			// set max bone index
			assert(usxyzw.x < mesh->bone_indices.size());
			assert(usxyzw.y < mesh->bone_indices.size());
			assert(usxyzw.z < mesh->bone_indices.size());
			assert(usxyzw.w < mesh->bone_indices.size());

			bone_index = mesh->bone_indices[usxyzw.x]; max_bone_index = (bone_index > max_bone_index) ? bone_index : max_bone_index;
			bone_index = mesh->bone_indices[usxyzw.y]; max_bone_index = (bone_index > max_bone_index) ? bone_index : max_bone_index;
			bone_index = mesh->bone_indices[usxyzw.z]; max_bone_index = (bone_index > max_bone_index) ? bone_index : max_bone_index;
			bone_index = mesh->bone_indices[usxyzw.w]; max_bone_index = (bone_index > max_bone_index) ? bone_index : max_bone_index;

			mesh->set_blend_weight(i, xyzw);
			mesh->set_blend_index(i, vec4((float)(usxyzw.x), (float)(usxyzw.y), (float)(usxyzw.z), (float)(usxyzw.w)));
		}
	}
	mesh->max_bone_index = max_bone_index;

	// [tangent]
	if (format.use_tangent()) {
		for (i = 0; i < num_verts; ++i) {
			mesh_file.read_float3(xyz.vec_array);
			mesh->set_tangent(i, xyz);
		}
	}

	assert("no mapchannel" && (format.get_num_mapchannel() > 0)); 

	// [uv0-4]
	int num_mapchannel = format.get_num_mapchannel();
	for (uint16 i_channel = 0; i_channel < num_mapchannel; ++i_channel) {
		for (i = 0; i < num_verts; ++i) {
			mesh_file.read_float2(xy.vec_array);
			mesh->set_uv(i, i_channel, xy);
		}
	}

	// [num_faces]
	mesh_file.read_uint16(num_faces);
	assert(num_faces < MAX_NUM_FACES);
	mesh->set_num_faces(num_faces);

	for (i = 0; i < num_faces; i++) {
		mesh_file.read_uint163(usxyz.ushort_array);
		mesh->set_face(i, usxyz);
	}

	// [material id]
	uint16 num_matids, matid_numfaces;

	mesh_file.read_uint16(num_matids);
	mesh->set_num_matids(num_matids);

	for (i = 0; i < num_matids; i++) {
		mesh_file.read_uint16(matid_numfaces);
		mesh->set_matid_numfaces(i, matid_numfaces);
	}
	// set clipfaces
	if (num_matids > 0) {
		mesh->set_num_clip_faces(mesh->get_matid_numfaces(num_matids-1));
		mesh->index_type = zz_mesh::TYPE_LIST;
	}
	
	// [triangle strip]
	uint16 num_strip_indices;
	mesh_file.read_uint16(num_strip_indices);
	
	// if num_matids > 0, then we cannot use triangle strip.
	if (num_strip_indices > 0) {
		assert(num_matids == 0);
		mesh->num_indices = num_strip_indices;
		mesh->index_type = zz_mesh::TYPE_STRIP;
		assert(!mesh->ibuf_strip);
		mesh->ibuf_strip = zz_new unsigned short[num_strip_indices];
		mesh_file.read(reinterpret_cast<char*>(mesh->ibuf_strip), num_strip_indices*sizeof(unsigned short));
	}
	else {
		mesh->index_type = zz_mesh::TYPE_LIST;
	}

	if (version >= 8) { // read pool type
		uint16 pool_type;
		mesh_file.read_uint16(pool_type);
		switch (pool_type)
		{
		case 0: // static mesh
			mesh->set_dynamic(false);
			mesh->set_bindable(true);
			break;
		case 1: // dynamic mesh
			mesh->set_dynamic(true);
			mesh->set_bindable(true);
			break;
		case 2: // system mesh
			mesh->set_dynamic(false);
			mesh->set_bindable(false);
			break;
		}
	}

	return true;
}

bool zz_mesh_tool::load_mesh (const char * file_name, zz_mesh * mesh, bool text_mode)
{
	if (text_mode) return false; // Currently, does not support text mode for performance.

	int version = 0;
	zz_vfs_pkg mesh_file;
	
	if (!mesh_file.open(file_name, zz_vfs::ZZ_VFS_READ)) {
		ZZ_LOG("mesh_tool: load_mesh(%s) failed. cannot open file\n", file_name);
		return false;
	}

	// [version]
	char str_buf[ZZ_MAX_STRING] = "";
	if (!mesh_file.read_string(str_buf)) {
		return false;
	}
	if (strcmp(str_buf, ZZ_MESH_VERSION8) == 0) {
		version = 8;
	}
	else if (strcmp(str_buf, ZZ_MESH_VERSION7) == 0) {
		version = 7;
	}
	else if (strcmp(str_buf, ZZ_MESH_VERSION6) == 0) {
		version = 6;
	}
	else if (strcmp(str_buf, ZZ_MESH_VERSION5) == 0) {
		version = 5;
	}
	else {
		assert(!"mesh version dismatched");
		ZZ_LOG("mesh_tool: load_mesh(%s) failed. version dismatched(%s)\n",
			str_buf, ZZ_MESH_VERSION);
		return false;
	}
	
	mesh_file.close();

	if (version < 7) {
		return load_mesh_6(file_name, mesh, text_mode);
	}
	return load_mesh_8(file_name, mesh, text_mode);
}

bool zz_mesh_tool::save_mesh (const char * file_name, zz_mesh * mesh, bool text_mode)
{
	uint16 i;
	
	const zz_vertex_format& format = mesh->get_vertex_format();

	//zz_script_simple mesh_file(file_name, zz_vfs::ZZ_VFS_WRITE, false /* binary mode */);
	zz_script_simple mesh_file(file_name, zz_vfs::ZZ_VFS_WRITE, text_mode);

	// [version]
	mesh_file.write_string("version", ZZ_MESH_VERSION);
	mesh_file.write_endl();

	// [vertex_format]
	mesh_file.write_uint32("vertex_format", format.get_format());
	mesh_file.write_endl();

	// CAUTION: minmax could be modified by load_mesh() in skinned mesh
	//build_minmax(mesh, mesh->pmin, mesh->pmax);

	// [min]
	mesh_file.write_float3("min", mesh->get_min().x, mesh->get_min().y, mesh->get_min().z);
	mesh_file.write_endl();

	// [max]
	mesh_file.write_float3("max", mesh->get_max().x, mesh->get_max().y, mesh->get_max().z);
	mesh_file.write_endl();

	// [bone_indices]
	mesh_file.write_uint16("num_bones", mesh->bone_indices.size());
	mesh_file.write_endl();
	for (i = 0; i < mesh->bone_indices.size(); ++i) {
		//mesh_file.write_uint32("bone", i);
		mesh_file.write_uint16("bone", mesh->bone_indices[i]);
		mesh_file.write_endl();
	}

	// [num_verts]
	uint16 num_verts = mesh->get_num_verts();
	mesh_file.write_uint16("num_verts", num_verts);
	mesh_file.write_endl();

	// [positions]
	for (i = 0; i < num_verts; ++i) {
		//mesh_file.write_uint32("pos", i);
		mesh_file.write_float3("pos",
			mesh->get_pos(i).x, 
			mesh->get_pos(i).y, 
			mesh->get_pos(i).z);
		mesh_file.write_endl();
	}

	// [normal]
	if (format.use_normal()) {
		for (i = 0; i < num_verts; ++i) {
			//mesh_file.write_uint32("norm", i);
			mesh_file.write_float3("norm",
				mesh->get_normal(i).x, 
				mesh->get_normal(i).y, 
				mesh->get_normal(i).z);
			mesh_file.write_endl();
		}
	}

	// [color]
	if (format.use_color()) {
		//mesh->colors.reserve(num_verts);
		//mesh->colors.clear();
		for (i = 0; i < num_verts; ++i) {
			//mesh_file.write_uint32("col", i);
			mesh_file.write_float4("col",
				float(mesh->get_color(i).r)/255.0f,
				float(mesh->get_color(i).g)/255.0f, 
				float(mesh->get_color(i).b)/255.0f,
				float(mesh->get_color(i).a)/255.0f);
		}
	}

	// [skin]
	if (format.use_skin()) {
		for (i = 0; i < num_verts; ++i) {
			//mesh_file.write_uint16("skin", static_cast<uint16>(i));
			mesh_file.write_float4("skin",
				mesh->get_blend_weight(i).x, 
				mesh->get_blend_weight(i).y, 
				mesh->get_blend_weight(i).z, 
				mesh->get_blend_weight(i).w);
			// CAUTION:
			// *3 in mesh_tool::load_mesh()
			mesh_file.write_uint16(NULL,	(uint16)mesh->get_blend_index(i).x);
			mesh_file.write_uint16(NULL,	(uint16)mesh->get_blend_index(i).y);
			mesh_file.write_uint16(NULL,	(uint16)mesh->get_blend_index(i).z);
			mesh_file.write_uint16(NULL,	(uint16)mesh->get_blend_index(i).w);
			mesh_file.write_endl();
		}
	}

	// [tangent]
	if (format.use_tangent()) {
		for (i = 0; i < num_verts; ++i) {
			//mesh_file.write_uint32("tan", i);
			mesh_file.write_float3("tan",
				mesh->get_tangent(i).x, 
				mesh->get_tangent(i).y, 
				mesh->get_tangent(i).z);
			mesh_file.write_endl();
		}
	}

	// [uv0]
	if (format.get_num_mapchannel() > 0) {
		for (i = 0; i < num_verts; ++i) {
			//mesh_file.write_uint32("uv0", i);
			mesh_file.write_float("uv0", mesh->get_uv(i, 0).x);
			mesh_file.write_float(NULL,	mesh->get_uv(i, 0).y);
			mesh_file.write_endl();
		}
	}

	// [uv1]
	if (format.get_num_mapchannel() > 1) {
		for (i = 0; i < num_verts; ++i) {
			//mesh_file.write_uint32("uv1", i);
			mesh_file.write_float("uv1", mesh->get_uv(i, 1).x);
			mesh_file.write_float(NULL,	mesh->get_uv(i, 1).y);
			mesh_file.write_endl();
		}
	}

	// [uv2]
	if (format.get_num_mapchannel() > 2) {
		for (i = 0; i < num_verts; ++i) {
			//mesh_file.write_uint32("uv2", i);
			mesh_file.write_float("uv2", mesh->get_uv(i, 2).x);
			mesh_file.write_float(NULL,	mesh->get_uv(i, 2).y);
			mesh_file.write_endl();
		}
	}

	// [uv3]
	if (format.get_num_mapchannel() > 3) {
		for (i = 0; i < num_verts; ++i) {
			//mesh_file.write_uint32("uv3", i);
			mesh_file.write_float("uv3", mesh->get_uv(i, 3).x);
			mesh_file.write_float(NULL,	mesh->get_uv(i, 3).y);
			mesh_file.write_endl();
		}
	}

	// [num_faces]
	uint16 num_faces = mesh->get_num_faces();
	mesh_file.write_uint16("num_faces", num_faces);
	mesh_file.write_endl();

	// [face]
	for (i = 0; i < num_faces; i++) {
		//mesh_file.write_uint32("face", i);
		mesh_file.write_uint16("face", mesh->get_face(i).x);
		mesh_file.write_uint16(NULL, mesh->get_face(i).y);
		mesh_file.write_uint16(NULL, mesh->get_face(i).z);
		mesh_file.write_endl();
	}

	// [material id]
	uint16 num_matids, matid_numfaces;
	num_matids = mesh->get_num_matids();
	if (num_matids == 1) num_matids = 0; // ignore only one material id

	mesh_file.write_uint16("num_matid", num_matids);
	mesh_file.write_endl();

	for (i = 0; i < num_matids; i++) {
		matid_numfaces = mesh->get_matid_numfaces(i);
		//mesh_file.write_uint16("matid", i);
        mesh_file.write_uint16("matid", matid_numfaces);
		mesh_file.write_endl();
	}

	// [tristrip info]
	uint16 num_strip_index = 0; // number of triangle strip indices
	if (mesh->get_index_type() == zz_mesh::TYPE_STRIP) {
		num_strip_index = mesh->get_num_indices();
	}
	mesh_file.write_uint16("num_strip_indices", num_strip_index);
	mesh_file.write_endl();
	for (i = 0; i < num_strip_index; ++i) {
		mesh_file.write_uint16("strip_index", mesh->get_strip_index(i));
		mesh_file.write_endl();
	}

	uint16 pool_type;
	if (!mesh->get_dynamic() && mesh->get_bindable()) {
		pool_type = 0;
	}
	else if (mesh->get_dynamic() && mesh->get_bindable()) {
		pool_type = 1;
	}
	else if (!mesh->get_dynamic() && !mesh->get_bindable()) {
		pool_type = 2;
	}
	else {
		assert(!"mesh_tool: save_mesh failed. invalid type");
	}
	mesh_file.write_uint16("pool_type", pool_type);

	return true;
}

bool zz_mesh_tool::export_mesh_xml(const char* filename, zz_mesh* mesh)
{
	const zz_vertex_format& format = mesh->get_vertex_format();

	FILE* fxml = fopen(filename, "wt");
	if (!fxml) {
		return false;
	}

	fprintf(fxml, "<mesh>\n");
	fprintf(fxml, "  <submeshes count=\"%i\">\n", 1);
	fprintf(fxml, "    <submesh material=\"%s\" usesharedvertices=\"%s\">\n", "Default", "false");

	uint16 num_faces = mesh->get_num_faces();
	fprintf(fxml, "      <faces count=\"%i\">\n", num_faces);
	for (int i = 0; i < num_faces; i++) {
		fprintf(fxml, "        <face v1=\"%i\" v2=\"%i\" v3=\"%i\">\n", 
			mesh->get_face(i).x, mesh->get_face(i).y, mesh->get_face(i).z);
	}
	fprintf(fxml, "      </faces>\n");

	uint16 num_verts = mesh->get_num_verts();
	fprintf(fxml, "      <geometry vertexcount=\"%i\">\n", num_verts);


	fprintf(fxml, "        <vertexbuffer position=\"%s\" normals=\"%s\">\n", "true", "true");
	fprintf(fxml, "          <vertex>\n");
	// [positions]
	for (int i = 0; i < num_verts; ++i) {
		fprintf(fxml, "            <position x=\"%f\" y=\"%f\" z=\"%f\" />\n", 
			mesh->get_pos(i).x, 
			mesh->get_pos(i).y, 
			mesh->get_pos(i).z);
		fprintf(fxml, "            <normal x=\"%f\" y=\"%f\" z=\"%f\" />\n", 
			mesh->get_normal(i).x, 
			mesh->get_normal(i).y, 
			mesh->get_normal(i).z);
	}
	fprintf(fxml, "          </vertex>\n");
	fprintf(fxml, "        </vertexbuffer>\n");
	fprintf(fxml, "      </geometry>\n");
	fprintf(fxml, "    </submesh>\n");
	fprintf(fxml, "  </submeshes>\n");
	fprintf(fxml, "</mesh>\n");

	fclose(fxml);

	return true;
}

// build mesh
bool zz_mesh_tool::load_terrain_mesh ( zz_mesh_terrain * mesh )
{
	//ZZ_PROFILER_INSTALL(Pload_terrain_mesh);
	//ZZ_LOG("mesh_tool: load_terrain_mesh(%dx%d)\n", orig_x, orig_y);
	
	assert(mesh);

	// set vertex format
	mesh->set_vertex_format(ZZ_VERTEX_FORMAT_TERRAIN );

	int ix, iy; // x|y index of block
	int vwidth = mesh->detail_level_ + 1; // vertex num per block
	int fwidth = mesh->detail_level_; // face num per block

	// per vertex unit size
	float unit = (float)mesh->blocksize_ / mesh->detail_level_;

	// temp variables
	vec3 pos;
	vec2 uv;
	usvec3 indices;

	mesh->set_num_verts(vwidth*vwidth);
	int i = 0;
	
	// for integer math in texture coordinates calculation.
	int iunit = int(unit*ZZ_SCALE_OUT);
	int imapsize = int(mesh->mapsize_*ZZ_SCALE_OUT);
	int iorig_x = int(mesh->orig_x_*ZZ_SCALE_OUT);
	int iorig_y = int(mesh->orig_y_*ZZ_SCALE_OUT);

	for (iy = 0; iy < vwidth; iy++) {
		for (ix = 0; ix < vwidth; ix++) {
			// calculate vertex position
			pos.x = mesh->orig_x_ + ix*unit;
			pos.y = mesh->orig_y_ - iy*unit; // x-z plane to x-y plane

			// height_list[iy][ix]
			pos.z = *(mesh->height_list_ + iy*vwidth + ix);
			
            //mesh->positions.push_back(pos);
			mesh->set_pos(i, pos);
			
			// set 1st mapchannel
			uv = get_uv_by_type(fwidth, ix, iy, zz_uv_type(mesh->uv_type0_));
			//mesh->uv0s.push_back(uv);
			mesh->set_uv(i, 0, uv);

			// set 2nd mapchannel
			uv = get_uv_by_type(fwidth, ix, iy, zz_uv_type(mesh->uv_type1_));
			//mesh->uv1s.push_back(uv);
			mesh->set_uv(i, 1, uv);

			// set light mapchannel
			if (mesh->orig_x_ < 0) { // negative coordinates
				uv.x = float(iorig_x%(imapsize+1)) + ix*unit;
				uv.x /= imapsize;
				uv.x += 1.0f;
			}
			else {
				uv.x = float(iorig_x%imapsize) + ix*iunit;
				uv.x /= imapsize;
			}
			if ((mesh->orig_y_ - mesh->blocksize_) < 0) { // negative coordinates
				uv.y = float(iorig_y%imapsize) - iy*iunit;
				uv.y /= imapsize;
				uv.y += 1.0f;
			}
			else {
				uv.y = float(iorig_y%(imapsize+1)) - iy*iunit;
				uv.y /= imapsize;
			}
			uv.y = 1.0f - uv.y; // for d3d texture coordinate
			//mesh->uv2s.push_back(uv);
			mesh->set_uv(i, 2, uv);

			i++;
		}
	}

	mesh->height_diff_ = mesh->pmax.z - mesh->pmin.z;

	// set face
	if (1) { // full-level
		mesh->set_num_faces(fwidth*fwidth*2);
		i = 0;
		for (iy = 0; iy < fwidth; ++iy) {
			for (ix = 0; ix < fwidth; ++ix) {
				///////////////////////////////////////
				// CAUTION: counter-clock-wise welding
				///////////////////////////////////////

/*
face order:

lower face (0-1-2) : clockwise welding
upper face (1-2-3) : counter-clockwise welding

vertex order = (vertex sequence index):
0 = (x, y+1)
1 = (x, y)
2 = (x+1, y+1)
3 = (x+1, y)

1   3
+---+
| \ |
+---+
0   2

*/

				// for lower face (0-1-2)
				indices.x = ix + (iy+1)*vwidth;
				indices.y = ix + iy*vwidth;
				indices.z = (ix+1) + (iy+1)*vwidth;
				mesh->set_face(i++, indices);

				// for upper face (1-2-3)
				indices.x = ix + iy*vwidth;
				indices.y = (ix+1) + (iy+1)*vwidth;
				indices.z = (ix+1) + iy*vwidth;
				mesh->set_face(i++, indices);
			}
		}
	}

	return true;
}

void zz_mesh_tool::build_minmax (zz_mesh * mesh, vec3& pmin, vec3& pmax)
{
	int vertex_count = (int)mesh->get_num_verts();

	//assert(mesh->positions.size() > 0);

	pmin = mesh->get_pos(0);
	pmax = pmin;
	const vec3 * pos;
	for (int i = 0; i < vertex_count; ++i) {
		pos = &mesh->get_pos(i);
		// for min
		if (pos->x < pmin.x) pmin.x = pos->x;
		if (pos->y < pmin.y) pmin.y = pos->y;
		if (pos->z < pmin.z) pmin.z = pos->z;
		// for max
		if (pos->x > pmax.x) pmax.x = pos->x;
		if (pos->y > pmax.y) pmax.y = pos->y;
		if (pos->z > pmax.z) pmax.z = pos->z;
	}
}

// for compare function
zz_mesh * current_mesh;
vec3 camera_pos;


bool compare_face_farther_than (const usvec3& a, const usvec3& b)
{
	float dist_a, dist_b;
	vec3 pos_a, pos_b;
	pos_a =	current_mesh->get_pos(a.x) + current_mesh->get_pos(a.y) + current_mesh->get_pos(a.z);
	pos_a = 1.0f/3.0f * pos_a;

	pos_b =	current_mesh->get_pos(b.x) + current_mesh->get_pos(b.y) + current_mesh->get_pos(b.z);
	pos_b = 1.0f/3.0f * pos_b;

	dist_a = camera_pos.distance_square(pos_a);
	dist_b = camera_pos.distance_square(pos_b);
	return (dist_a > dist_b);
}

// sort transparent faces with distance to the camera postion
void zz_mesh_tool::sort_transparent_faces (zz_visible * vis)
{
	unsigned int i, j;
	vector<usvec3> faces;
	zz_material * mat;

	// get current valid camera
	zz_camera * current_camera = znzin->get_camera();
	assert(current_camera);

	// transform camera position in world-space to object-space
	mult(camera_pos, vis->get_world_inverseTM(), current_camera->get_eye());
	
	//ZZ_LOG("mesh_tool: sort_transparent_faces(%s) camera_pos=(%f, %f, %f)\n",
	//	vis->get_name(), camera_pos.x, camera_pos.y, camera_pos.z);
	// for each render unit
	for (i = 0; i < vis->get_num_runits(); i++) {
		mat = vis->get_material(i);
		// if this is a opaque object, skip.
		// because we do not need for transparent-sorting
		if (!mat || !mat->get_texturealpha()) {
			continue;
		}

		// get current mesh by render unit index
		current_mesh = vis->get_mesh(i);
	
		faces.clear();

		// TODO: do not copy faces, and implement sort algorithm
		// copy to faces
		for (j = 0; j < current_mesh->get_num_faces(); j++) {
			faces.push_back((usvec3)(current_mesh->get_face(j)));
		}

		// sort faces by distance to the camera position
		sort(faces.begin(), faces.end(), compare_face_farther_than);

		// copy to current_mesh
		for (j = 0; j < current_mesh->get_num_faces(); j++) {
			current_mesh->set_face(j, faces[j]);
		}

		// update index buffer
		current_mesh->update_index_buffer();
	}
}

bool zz_mesh_tool::is_same (int vertex_format, const zz_vertex_bubble& a, const zz_vertex_bubble& b)
{
	// setup weld-epsilons. for now, does not adapt these values
	zz_epsilon e;
	e.position = 0.01f;
	e.normal = 0.01f;
	e.color = 0.01f;
	e.blend_weight = 0.01f;
	e.tangent = 0.0001f;
	e.uv[0] = 0.0001f;
	e.uv[1] = 0.0001f;
	e.uv[2] = 0.0001f;
	e.uv[3] = 0.0001f;

	if (memcmp(&a, &b, sizeof(zz_vertex_bubble)) == 0) {
		return true;
	}
	return false;
}

int zz_mesh_tool::found_same (int vertex_format, const vector<zz_vertex_bubble> & bubbles, zz_vertex_bubble& bubble)
{
	int index = 0;

	for (index = 0; index < (int)bubbles.size(); index++) {
		if (is_same(vertex_format, bubble, bubbles[index])) {
			return index;
		}
	}
	return -1; // not found
}

// not verified yet code
bool zz_mesh_tool::unweld_vertices (zz_mesh * mesh)
{
	unsigned int num_faces = mesh->get_num_faces();
	unsigned int num_verts = mesh->get_num_verts();

	if (num_faces == num_verts*3) return false;

	vector<zz_vertex_bubble> bubbles;
	zz_vertex_bubble bubble;
	
	unsigned int iface;
	unsigned int ivertex;
	unsigned int i;
	usvec3 face;

	// build bubbles
	for (iface = 0; iface < num_faces; ++iface) {
		face = mesh->get_face(iface);
		for (i = 0; i < 3; ++i) {
			ivertex = face[i];
			bubble.set_from(*mesh, ivertex);
			// insert this vertex into bubbles
			face[i] = (int)bubbles.size();
			bubbles.push_back(bubble);
		}
		// update face index
		mesh->set_face(iface, face);
	}
	assert(bubbles.size() == num_faces*3);

	// copy bubbles into new vertex
	mesh->set_num_verts(0);	
	mesh->set_num_verts(bubbles.size());

	for (ivertex = 0; ivertex < (int)bubbles.size(); ivertex++) {
		bubbles[ivertex].set_to(*mesh, ivertex);
	}

	return true;
}

bool zz_mesh_tool::weld_vertices (zz_mesh * mesh)
{
	vector<zz_vertex_bubble> bubbles;
	zz_vertex_bubble bubble;
	
	int iface;
	int ivertex;
	int i;
	usvec3 face;
	int found_index;
	bool face_changed;

	ZZ_LOG("mesh_tool: weld_vertices(%s). before(#%d)", mesh->get_name(), mesh->get_num_verts());

	// build bubbles
	for (iface = 0; iface < mesh->get_num_faces(); iface++) {
		face = mesh->get_face(iface);
		face_changed = false;
		for (i = 0; i < 3; i++) {
			ivertex = face[i];
			// check duplicate
			bubble.set_from(*mesh, ivertex);
			found_index = found_same(mesh->get_vertex_format().get_format(), bubbles, bubble);
			if (found_index < 0) { // if not found same
				// insert this vertex into bubbles
				face[i] = (int)bubbles.size();
				bubbles.push_back(bubble);
			}
			else { // if found same
				// adjust face index
				face[i] = found_index;
			}
		}
		// update face index
		mesh->set_face(iface, face);
		//ZZ_LOG("mesh_tool: weld_vertices. set_face(%d)\n", iface);
	}
	
	// copy bubbles into new vertex
	mesh->set_num_verts((int)bubbles.size());
	ZZ_LOG("-> after(#%d). faces(%d)\n", mesh->get_num_verts(), mesh->get_num_faces());

	for (ivertex = 0; ivertex < (int)bubbles.size(); ivertex++) {
		bubbles[ivertex].set_to(*mesh, ivertex);
	}

	ZZ_LOG("mesh_tool: weld_vertices() done\n");
	return true;
}

// simple quad that has wrapped texture coordinates.
bool zz_mesh_tool::load_ocean_mesh ( zz_mesh_ocean * mesh )
{
	assert(mesh);
	
	// set vertex format
	mesh->set_vertex_format(ZZ_VERTEX_FORMAT_OCEAN);
	
	mesh->set_num_verts(4);
	
	vec3 pos;
	vec2 uv;

	pos.set(0,				mesh->height_,	0); mesh->set_pos(0, pos);
	pos.set(0,				0,				0); mesh->set_pos(1, pos);
	pos.set(mesh->width_,	mesh->height_,	0); mesh->set_pos(2, pos);
	pos.set(mesh->width_,	0,				0); mesh->set_pos(3, pos);

	uv.set(0.f,			0.f);										mesh->set_uv(0, 0, uv);
	uv.set(0.f,			(float)mesh->repeat_vert_);					mesh->set_uv(1, 0, uv);
	uv.set((float)mesh->repeat_hori_,	0.f);						mesh->set_uv(2, 0, uv);
	uv.set((float)mesh->repeat_hori_,	(float)mesh->repeat_vert_);	mesh->set_uv(3, 0, uv);

	// no need to calculate pmin/pmax because these values were update in set_property()

	mesh->set_num_faces(2);

	usvec3 indices;
	// CAUTION: assuming counter-clock-wise welding
	indices.x = 0; indices.y = 1; indices.z = 2; mesh->set_face(0, indices);
	indices.x = 3; indices.y = 2; indices.z = 1; mesh->set_face(1, indices);

	return true;
}

bool zz_mesh_tool::build_normal (zz_mesh * mesh)
{
	const zz_vertex_format& format = mesh->get_vertex_format();
	int num_faces = mesh->get_num_faces();
	if (num_faces == 0) return true;

	// add normal to vertex format
	if (!format.use_normal()) {
		mesh->set_vertex_format(format.get_format() | ZZ_VF_NORMAL);
	}

	int num_verts = mesh->get_num_verts();

	vector<int> num_adjoint_faces; // size = num_verts
	vector<vec3> vertex_normals; // size = num_verts
	vector<vec3> face_normals; // size = num_faces

	num_adjoint_faces.assign(num_verts, 0);
	face_normals.assign(num_faces, vec3_null);
	vertex_normals.assign(num_verts, vec3_null);

	// calculate face normals
	vec3 face_normal;
	usvec3 iface3;
	int ivert, iface;
	vec3 right, left;
	for (iface = 0; iface < num_faces; iface++) {
		// CAUTION: assuming counter-clock-wise welding
		iface3 = mesh->get_face(iface);
		right = mesh->get_pos(iface3.z) - mesh->get_pos(iface3.y);
		left = mesh->get_pos(iface3.x) - mesh->get_pos(iface3.y);
		cross(face_normal, right, left);
		face_normal.normalize();
		face_normals.push_back(face_normal);

		// increase reference count of the adjoint face
		num_adjoint_faces[iface3.x]++;
		num_adjoint_faces[iface3.y]++;
		num_adjoint_faces[iface3.z]++;

		// add face normal into vertex normal
		vertex_normals[iface3.x] += face_normal;
		vertex_normals[iface3.y] += face_normal;
		vertex_normals[iface3.z] += face_normal;
	}

	// set mesh normal
	for (ivert = 0; ivert < num_verts; ivert++) {
		// divide vertex normal by reference count
		vertex_normals[ivert] *= 1.0f/(float)num_adjoint_faces[ivert];
		mesh->set_normal(ivert, vertex_normals[ivert]);
	}

	return true;
}
 
// fast version but inaccurate
/*
bool zz_mesh_tool::get_intersection_mesh_ray (
	const zz_mesh * mesh,
	const mat4 * worldtm,
	const vec3 * ray_origin,
	const vec3 * ray_direction,
	vec3 * point,
	zz_select_method select_method)
{
	int iface;
	int num_faces = mesh->get_num_faces();
	usvec3 iface_vertex;
	float u, v;
	const vec3 * p0;
	const vec3 * p1;
	const vec3 * p2;
	
	float min_distance, cur_distance;
	vec3 cur_point, min_point; // local tm
	bool first = true, found = false;

	mat4 iworldtm;
	invert(iworldtm, *worldtm);
	
	vec3 ray_origin_local;
	vec3 ray_direction_local;
	vec3 world_origin_local;
	
	mult(ray_origin_local, iworldtm, *ray_origin);
	// recalc direction local
	mult(ray_direction_local, iworldtm, *ray_direction); // not a direction vector
	mult(world_origin_local, iworldtm, vec3_null);
	ray_direction_local -= world_origin_local; // direction vector to local origin
	
	for (iface = 0; iface < num_faces; iface++) {
		iface_vertex = mesh->get_face(iface);
		p0 = &mesh->get_pos(iface_vertex.x);
		p1 = &mesh->get_pos(iface_vertex.y);
		p2 = &mesh->get_pos(iface_vertex.z);

		if (intersect_tri_d3d(
			*p0, *p1, *p2,
			ray_origin_local,
			ray_direction_local,
			u, v, cur_distance)) // if intersect this triangle
		{
			found = true;
			if (!point) return true;
			
			// get intersecting point
			get_intersect_pos(*p0, *p1, *p2, u, v, cur_point);

			if (first) {
				min_point = cur_point;
				min_distance = cur_distance;
			}
			if (select_method == ZZ_SM_ANY) {
				min_point = cur_point;
				min_distance = cur_distance;
				break;
			}
			else if ((select_method == ZZ_SM_NEAREST) && (cur_distance < min_distance)) {
				min_point = cur_point;
				min_distance = cur_distance;
			}
			else if ((select_method == ZZ_SM_FARTHEST) && (cur_distance > min_distance)) {
				min_point = cur_point;
				min_distance = cur_distance;
			}
			first = false;
		}
	}

	if (!found) return false;

	mult(*point, *worldtm, min_point);
	return true;
}
*/
//

// SLOW VERSION
bool zz_mesh_tool::get_intersection_mesh_ray (
	const zz_mesh * mesh,
	const mat4 * worldtm,
	const vec3 * ray_origin,
	const vec3 * ray_direction,
	vec3 * point,
	vec3 * normal,
	zz_select_method select_method)
{
	int iface;
	int num_faces = mesh->get_num_faces();
	usvec3 iface_vertex;
	float u, v, dist;
	
    float min_distance, cur_distance;
	vec3 cur_point, min_point, min_normal;
	bool first = true, found = false;
	
    mat4 iworldtm;
	invert(iworldtm, *worldtm);
	
	vec3 ray_origin_local;
	vec3 ray_direction_local;
	vec3 world_origin_local;
	const vec3 * lp0;
	const vec3 * lp1;
	const vec3 * lp2;
	vec3 local_origin_world;
	
	mult(ray_origin_local, iworldtm, *ray_origin);
	// recalc direction local
	mult(ray_direction_local, iworldtm, *ray_direction); // not a direction vector
	mult(world_origin_local, iworldtm, vec3_null);
	ray_direction_local -= world_origin_local; // direction vector to local origin


	for (iface = 0; iface < num_faces; iface++) {
		iface_vertex = mesh->get_face(iface);
		lp0 = &mesh->get_pos(iface_vertex.x);
		lp1 = &mesh->get_pos(iface_vertex.y);
		lp2 = &mesh->get_pos(iface_vertex.z);
	
		if (intersect_tri_d3d(*lp0, *lp1, *lp2, ray_origin_local, ray_direction_local, u, v, dist)) // if intersect this triangle
		{
			found = true;
			if (!point) return true;
			
			// get intersecting point
			get_intersect_pos(*lp0, *lp1, *lp2, u, v, cur_point);
			cur_distance = ray_origin_local.distance_square(cur_point);

			if
				(
					(first) ||
					(select_method == ZZ_SM_ANY) ||
					((select_method == ZZ_SM_NEAREST) && (cur_distance < min_distance)) ||
					((select_method == ZZ_SM_FARTHEST) && (cur_distance > min_distance))
				)
			{
				min_point = cur_point;
				min_distance = cur_distance;
				if (normal) {
					get_intersect_normal(*lp0, *lp1, *lp2, min_normal);
				}
				if (select_method == ZZ_SM_ANY) break;
			}
			first = false;
		}	
	}

	if (!found) return false;

	
	mult(*point, *worldtm, min_point);
	mult(*normal, *worldtm, min_normal);
	local_origin_world = worldtm->get_position();
	*normal -= local_origin_world;
	

	return true;
}


bool zz_mesh_tool:: get_intersection_mesh_ray_ex (
	const zz_mesh * mesh, 
	const mat4 * worldtm, 
	const vec3 * ray_origin, 
	const vec3 * ray_direction, 
	vec3 * point, vec3 *normal, vec3 *p01, vec3 *p02, vec3 *p03,
	
	zz_select_method select_method)

{
	int iface;
	int num_faces = mesh->get_num_faces();
	usvec3 iface_vertex;
	float u, v, dist;
	
    float min_distance, cur_distance;
	vec3 cur_point, min_point, min_normal;
	bool first = true, found = false;
	
    mat4 iworldtm;
	mat4 test_m;
	invert(iworldtm, *worldtm);
	
	mult(test_m,iworldtm,*worldtm);
	
	vec3 ray_origin_local;
	vec3 ray_direction_local;
	vec3 world_origin_local;
	const vec3 * lp0;
	const vec3 * lp1;
	const vec3 * lp2;
	vec3 min_p01, min_p02, min_p03;
	vec3 local_origin_world;
	
	mult(ray_origin_local, iworldtm, *ray_origin);
	// recalc direction local
	mult(ray_direction_local, iworldtm, *ray_direction); // not a direction vector
	mult(world_origin_local, iworldtm, vec3_null);
	ray_direction_local -= world_origin_local; // direction vector to local origin


	for (iface = 0; iface < num_faces; iface++) {
		iface_vertex = mesh->get_face(iface);
		lp0 = &mesh->get_pos(iface_vertex.x);
		lp1 = &mesh->get_pos(iface_vertex.y);
		lp2 = &mesh->get_pos(iface_vertex.z);
	
		if (intersect_tri_d3d(*lp0, *lp1, *lp2, ray_origin_local, ray_direction_local, u, v, dist)) // if intersect this triangle
		{
			found = true;
			if (!point) return true;
			
			// get intersecting point
			get_intersect_pos(*lp0, *lp1, *lp2, u, v, cur_point);
			cur_distance = ray_origin_local.distance_square(cur_point);

			if
				(
					(first) ||
					(select_method == ZZ_SM_ANY) ||
					((select_method == ZZ_SM_NEAREST) && (cur_distance < min_distance)) ||
					((select_method == ZZ_SM_FARTHEST) && (cur_distance > min_distance))
				)
			{
				min_point = cur_point;
				min_distance = cur_distance;
				min_p01 = *lp0;
                min_p02 = *lp1;
				min_p03 = *lp2;
							
				if (normal) {
					get_intersect_normal(*lp0, *lp1, *lp2, min_normal);
				}
				
				if (select_method == ZZ_SM_ANY) break;
			}
			first = false;
		}	
	}

	if (!found) return false;

	
	mult(*point, *worldtm, min_point);
	mult( *p01, *worldtm, min_p01);
    mult( *p02, *worldtm, min_p02);
    mult( *p03, *worldtm, min_p03);
    
	mult(*normal, *worldtm, min_normal);
	local_origin_world = worldtm->get_position();
	*normal -= local_origin_world;

	return true;
}


// mesh-sphere intersection test
bool zz_mesh_tool::test_intersection_mesh_sphere (
	const zz_mesh * mesh,
	const mat4 * worldtm, // visible's world tm
	const zz_bounding_sphere * sphere)
{
	int iface;
	int num_faces = mesh->get_num_faces();
	usvec3 iface_vertex;
	const vec3 * p0;
	const vec3 * p1;
	const vec3 * p2;	

	mat4 iworldtm;
	invert(iworldtm, *worldtm);
	
	zz_bounding_sphere sphere_local; // sphere in local coordinate
	
	// transform sphere radius
	vec3 radius_scale_vector; // for sphere radius scale
	mat3 iworldtm_rotation; // inverse world rotation matrix
	iworldtm.get_rot(iworldtm_rotation); // extract mat3 rotation matrix from mat4 transform matrix
	mult(radius_scale_vector, iworldtm_rotation, vec3(0, 0, sphere->radius)); // rotate into local coordsys
	sphere_local.radius = radius_scale_vector.norm(); // get vector length, and save to sphere_local radius

	// transform sphere center
	mult(sphere_local.center, iworldtm, sphere->center);
	
	for (iface = 0; iface < num_faces; iface++) {
		iface_vertex = mesh->get_face(iface);
		p0 = &mesh->get_pos(iface_vertex.x);
		p1 = &mesh->get_pos(iface_vertex.y);
		p2 = &mesh->get_pos(iface_vertex.z);

		if (intersect_sphere_tri(sphere_local, *p0, *p1, *p2)) // if intersect this triangle
		{
			//ZZ_LOG("zz_mesh_tool::test_intersection_mesh_sphere(%s, %f, %f, %f, %f) succeed\n", mesh->get_name(),
			//	sphere->center.x, sphere->center.y, sphere->center.z, sphere->radius);
			return true;
		}
	}

	return false;
}

bool zz_mesh_tool::repose_mesh (zz_mesh * mesh, zz_model * model)
{
	uint16 v, num_verts = mesh->get_num_verts();
	vec3 pos, post;
	vec3 pos4[4]; // each position to be weighted
	vec4 weights;
	vec4 indices;
	zz_bone * bones[4];
	int bone_index, bone_index_local;
	int i = 0;
	for (v = 0; v < num_verts; ++v) {
		pos = mesh->get_pos(v);
		indices = mesh->get_blend_index(v);
		weights = mesh->get_blend_weight(v);

		for (i = 0; i < 4; ++i) {
			bone_index_local = int(indices[i]);
			bone_index = mesh->bone_indices[bone_index_local];
			bones[i] = model->get_bone(bone_index);
			mult(pos4[i], bones[i]->get_boneTM(), pos);
		}
		post = weights[0]*pos4[0] + weights[1]*pos4[1] + weights[2]*pos4[2] + weights[3]*pos4[3];
		mesh->set_pos(v, post);
	}

	return true;
}

