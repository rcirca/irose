/** 
 * @file zz_mesh_tool.h
 * @brief mesh tool class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    05-sep-2002
 *
 * $Header: /engine/include/zz_mesh_tool.h 16    05-02-14 3:16p Choo0219 $
 */

#ifndef __ZZ_MESH_TOOL_H__
#define __ZZ_MESH_TOOL_H__

#ifndef __ZZ_MESH_H__
#include "zz_mesh.h"
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

// mesh tool feature:
// - load mesh
// - build mesh
// - optimize mesh
// - build terrain mesh

class zz_visible;
struct zz_bounding_sphere;
class zz_mesh_terrain;
class zz_mesh_ocean;
class zz_skeleton;
class zz_model;

//--------------------------------------------------------------------------------
class zz_mesh_tool {
public:
		// uv put type
	enum zz_uv_type {
		ZZ_UV_NORMAL = 1,
		ZZ_UV_LR     = 2,
		ZZ_UV_TB     = 3,
		ZZ_UV_LRTB   = 4,
		ZZ_UV_90CW   = 5,
		ZZ_UV_90CCW  = 6
	};

protected:
	// get transformed uv by uv-type
	static vec2 get_uv_by_type(int face_width, int ix, int iy, zz_uv_type uv_type);

public:

	enum zz_select_method {
		ZZ_SM_ANY = 0,
		ZZ_SM_NEAREST = 1,
		ZZ_SM_FARTHEST = 2
	};

	struct zz_vertex_bubble {
		vec3 pos;
		vec3 normal;
		zz_color color;
		vec4 blend_weight;
		vec4 blend_index;
		vec3 tangent;
		vec2 uv[4];

		void set_from (const zz_mesh& mesh, uint16 ivertex) {
			const zz_vertex_format& format = mesh.get_vertex_format();
			pos = mesh.get_pos(ivertex);
			normal = (format.use_normal()) ? mesh.get_normal(ivertex) : vec3_null;
			color = (format.use_color()) ? mesh.get_color(ivertex) : zz_color(0, 0, 0, 0);
			blend_weight = (format.use_skin()) ? mesh.get_blend_weight(ivertex) : vec4_null;
			blend_index = (format.use_skin()) ? mesh.get_blend_index(ivertex) : vec4_null;
			tangent = (format.use_tangent()) ? mesh.get_tangent(ivertex) : vec3_null;
			uv[0] = (format.use_uv(0)) ? mesh.get_uv(ivertex, 0) : vec2_null;
			uv[1] = (format.use_uv(1)) ? mesh.get_uv(ivertex, 1) : vec2_null;
			uv[2] = (format.use_uv(2)) ? mesh.get_uv(ivertex, 2) : vec2_null;
			uv[3] = (format.use_uv(3)) ? mesh.get_uv(ivertex, 3) : vec2_null;
		}

		void set_to (zz_mesh& mesh, uint16 ivertex) {
			const zz_vertex_format& format = mesh.get_vertex_format();
			mesh.set_pos(ivertex, pos);
			if (format.use_normal()) mesh.set_normal(ivertex, normal);
			if (format.use_color()) mesh.set_color(ivertex, color);
			if (format.use_skin()) {
				mesh.set_blend_weight(ivertex, blend_weight);
				mesh.set_blend_index(ivertex, blend_index);
			}
			if (format.use_tangent()) mesh.set_tangent(ivertex, tangent);
			if (format.use_uv(0)) mesh.set_uv(ivertex, 0, uv[0]);
			if (format.use_uv(1)) mesh.set_uv(ivertex, 1, uv[1]);
			if (format.use_uv(2)) mesh.set_uv(ivertex, 2, uv[2]);
			if (format.use_uv(3)) mesh.set_uv(ivertex, 3, uv[3]);
		}
	};

	struct zz_epsilon {
		float position;
		float normal;
		float color;
		float blend_weight;
		float tangent;
		float uv[4];
	};

private:
	static int found_same (int vertex_format, const std::vector<zz_vertex_bubble> & bubbles, zz_vertex_bubble& bubble);
	static bool is_same (int vertex_format, const zz_vertex_bubble& a, const zz_vertex_bubble& b);
	static void get_intersect_pos (const vec3& p0, const vec3& p1, const vec3& p2, float u, float v, vec3& pos);
	static void get_intersect_normal (const vec3& p0, const vec3& p1, const vec3& p2, vec3& normal);
	static bool intersect_tri_d3d (const vec3& p0, const vec3& p1, const vec3& p2, 
						const vec3& ray_pos, const vec3& ray_dir, float& u, float& v, float& dist);

public:
	zz_mesh_tool ();
	~zz_mesh_tool ();
	static bool load_mesh_6 ( const char * file_name, zz_mesh * mesh, bool text_mode = false );
	static bool load_mesh_8 ( const char * file_name, zz_mesh * mesh, bool text_mode = false );
	static bool load_mesh (const char * file_name, zz_mesh * mesh, bool text_mode = false);
	static bool save_mesh (const char * file_name, zz_mesh * mesh, bool text_mode = false);
	static bool export_mesh_xml (const char* filename, zz_mesh* mesh);
	
	static bool load_mesh_minmax (const char * file_name, zz_mesh * mesh, bool text_mode = false);

	static bool load_terrain_mesh ( zz_mesh_terrain * mesh );

	// this just make simple quad with tiling coordinates
	static bool load_ocean_mesh ( zz_mesh_ocean * mesh );

	static void build_minmax (zz_mesh * mesh, vec3& pmin, vec3& pmax);

	// for transparent object
	// to render furthest objects first
	// this call rearranges face indices and update index buffer
	// TODO: optimize it!
	static void sort_transparent_faces (zz_visible * vis);

	// if *point* is NULL, do not calculate contact point.
	// if *normal* is NULL, do not calculate contact face normal.
	// else, get the closest contact point
	static bool get_intersection_mesh_ray (const zz_mesh * mesh, const mat4 * worldtm, const vec3 * ray_origin, const vec3 * ray_direction, vec3 * point, vec3 * normal, zz_select_method select_method);
    
	static bool get_intersection_mesh_ray_ex (const zz_mesh * mesh, const mat4 * worldtm, const vec3 * ray_origin, const vec3 * ray_direction, vec3 * point, vec3 * normal, vec3 *p01, vec3 *p02, vec3 *p03, zz_select_method select_method);

	
	// test intersect test mesh-sphere
	static bool test_intersection_mesh_sphere (const zz_mesh * mesh, const mat4 * worldtm, const zz_bounding_sphere * sphere);

	static bool weld_vertices (zz_mesh * mesh);
	static bool unweld_vertices (zz_mesh * mesh);

	static bool build_normal (zz_mesh * mesh);

	// repose mesh that is posed in original source skeleton(src_skel) into other modified skeleton
	static bool repose_mesh (zz_mesh * mesh, zz_model * model);
};

inline void zz_mesh_tool::get_intersect_pos (const vec3& p0, const vec3& p1, const vec3& p2, float u, float v, vec3& pos)
{
	//V1 + U(V2-V1) + V(V3-V1).
	pos = p0 + u*(p1-p0) + v*(p2-p0);
}

inline void zz_mesh_tool::get_intersect_normal (const vec3& p0, const vec3& p1, const vec3& p2, vec3& normal)
{
	// assumes right-hand order
	vec3 v01 = p1 - p0; v01.normalize();
	vec3 v02 = p2 - p0; v02.normalize();
	normal.cross(v01, v02);
}

inline bool zz_mesh_tool::intersect_tri_d3d (const vec3& p0, const vec3& p1, const vec3& p2, 
						const vec3& ray_pos, const vec3& ray_dir, float& u, float& v, float& dist)
{
	return D3DXIntersectTri(
		reinterpret_cast<const D3DXVECTOR3*>(&p0),
		reinterpret_cast<const D3DXVECTOR3*>(&p1),
		reinterpret_cast<const D3DXVECTOR3*>(&p2),
        reinterpret_cast<const D3DXVECTOR3*>(&ray_pos),
		reinterpret_cast<const D3DXVECTOR3*>(&ray_dir),
		&u,
		&v,
		&dist) ? true : false;
}

#endif // __ZZ_MESH_TOOL_H__
