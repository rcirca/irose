/** 
 * @file zz_mesh_terrain.CPP
 * @brief terrain mesh class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    27-oct-2003
 *
 * $Header: /engine/include/zz_mesh_terrain.h 16    05-01-06 11:20a Choo0219 $
 */

#ifndef __ZZ_MESH_TERRAIN_H__
#define __ZZ_MESH_TERRAIN_H__

#ifndef __ZZ_MESH_H__
#include "zz_mesh.h"
#endif

#ifndef __ZZ_POINTER_H__
#include "zz_pointer.h"
#endif

#ifndef __ZZ_MESH_ISHARED_H__
#include "zz_mesh_ishared.h"
#endif

class zz_manager_mesh_terrain;

//--------------------------------------------------------------------------------
class zz_mesh_terrain : public zz_mesh_ishared {
//--------------------------------------------------------------------------------
	friend class zz_mesh_tool;
	friend class zz_manager_mesh_terrain;

	float orig_x_; // world origin position x 
	float orig_y_; // world origin position y
	int detail_level_; // detail level(=num faces per block). ex) 1, 2, 4, 8.... 
	int uv_type0_; // uv type for map1
	int uv_type1_; // uv type for map2
	int num_heights_; // number of height value
	float * height_list_; // height value array list
	float mapsize_; // lightmap size. ex) 16000
	float blocksize_; // one block unit in world
	float height_diff_; // max height difference

	void update_lod ();

	// if LOD is possible or not
	bool lodable ()
	{
		const float TOO_BIG_CHANGE = .2f;
		return (height_diff_ <	TOO_BIG_CHANGE * blocksize_);
	}

public:
	zz_mesh_terrain (int key_in = TERRAIN_INDEX_ORDER);
	virtual ~zz_mesh_terrain ();

	virtual bool load_mesh ();
	void set_property (
		float scale_in, // client to engine scale
		float orig_x, // world origin position x 
		float orig_y, // world origin position y
		int detail_level, // detail level(=num faces per block). ex) 1, 2, 4, 8.... 
		int uv_type0, // uv type for map1
		int uv_type1, // uv type for map2
		int width, // width size of height_list. number of horizotal heights
		const float * height_list, // height value array list
		float mapsize, // lightmap size. ex) 16000
		float blocksize // one block unit in world
		);
	void set_property_ex (
		float scale_in, // client to engine scale
		float orig_x, // world origin position x 
		float orig_y, // world origin position y
		int detail_level, // detail level(=num faces per block). ex) 1, 2, 4, 8.... 
		int uv_type0, // uv type for map1
		int uv_type1, // uv type for map2
		int width, // width size of height_list. number of horizotal heights
		const float * height_list, // height value array list
		float height_min_max[2],
		float mapsize, // lightmap size. ex) 16000
		float blocksize // one block unit in world
		);



	
	void set_property2 (
		float scale_in, // client to engine scale
		vec3 minmax[2], // minmax 
		int uv_type0, // uv type for map1
		int uv_type1, // uv type for map2
		int width, // width size of height_list. number of horizotal heights
		int numskip, // skipping vertex. if skipsize is 2, we skip 2 vertices every vertex for both horizontal and vertical.
		const float * height_list, // height value array list
		float mapsize, // lightmap size. ex) 16000
		float blocksize // one block unit in world
		);

	ZZ_DECLARE_DYNAMIC(zz_mesh_terrain)
};


//--------------------------------------------------------------------------------
class zz_mesh_terrain_rough : public zz_mesh_terrain {
//--------------------------------------------------------------------------------
public:
	zz_mesh_terrain_rough(int key_in =	ROUGH_TERRAIN_INDEX_ORDER);
	virtual ~zz_mesh_terrain_rough();

	ZZ_DECLARE_DYNAMIC(zz_mesh_terrain_rough)
};

#endif // __ZZ_MESH_TERRAIN_H__