/** 
 * @file zz_mesh_terrain.CPP
 * @brief terrain mesh class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    27-oct-2003
 *
 * $Header: /engine/src/zz_mesh_terrain.cpp 25    05-01-06 3:17p Choo0219 $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_mesh_terrain.h"
#include "zz_mesh_tool.h"
#include "zz_log.h"
#include "zz_profiler.h"

ZZ_IMPLEMENT_DYNCREATE(zz_mesh_terrain, zz_mesh_ishared)
ZZ_IMPLEMENT_DYNCREATE(zz_mesh_terrain_rough, zz_mesh_terrain)

zz_mesh_terrain::zz_mesh_terrain(int key_in) :
	orig_x_(0),
	orig_y_(0),
	detail_level_(0),
	uv_type0_(0),
	uv_type1_(0),
	height_list_(NULL),
	mapsize_(0),
	blocksize_(0),
	height_diff_(0),
	num_heights_(0),
	zz_mesh_ishared(key_in)
{
}

zz_mesh_terrain::~zz_mesh_terrain()
{
	if (height_list_) {
		ZZ_SAFE_DELETE_ARRAY(height_list_);
	}
}

void zz_mesh_terrain::set_property
	(
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
	)
{
	orig_x_ = orig_x*scale_in;
	orig_y_ = orig_y*scale_in;
	detail_level_ = detail_level;
	uv_type0_ = uv_type0;
	uv_type1_ = uv_type1;
	mapsize_ = mapsize*scale_in;
	blocksize_ = blocksize*scale_in;

	int num_heights = (detail_level+1)*(detail_level+1);
	
	if (height_list_) {
		zz_assert(num_heights == num_heights_);
	}
	else {
		height_list_ = zz_new float[num_heights];
		num_heights_ = num_heights;
	}

	if (num_heights == (width*width)) { // same size, just copy
		memcpy(height_list_, height_list, num_heights*sizeof(float));
		for (int i = 0; i < num_heights; ++i) {
			height_list_[i] *= scale_in;
		}
	}
	else {
		int x, y;
		int is = 0, it = 0;
		assert(detail_level < width);
		int skipsize = (width-1)/detail_level;
		for (y = 0; y < width; y += skipsize) {
			for (x = 0; x < width; x += skipsize) {
				is = y*width + x;
				assert(is < (width*width));
				height_list_[it] = scale_in*height_list[is];
				++it;
			}
		}
	}

	float minz, maxz;

	int i;
	maxz = minz = height_list_[0];
	for (i = 1; i < num_heights; ++i) {
		if (height_list_[i] < minz) {
			minz = height_list_[i];
		}
		if (height_list_[i] > maxz) {
			maxz = height_list_[i];
		}
	}

	pmin = vec3(orig_x_, orig_y_ - blocksize_, minz);
	pmax = vec3(orig_x_ + blocksize_, orig_y_, maxz);
}


void zz_mesh_terrain::set_property_ex
	(
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
	)
{
	orig_x_ = orig_x*scale_in;
	orig_y_ = orig_y*scale_in;
	detail_level_ = detail_level;
	uv_type0_ = uv_type0;
	uv_type1_ = uv_type1;
	mapsize_ = mapsize*scale_in;
	blocksize_ = blocksize*scale_in;
    
	float height_[2];
	height_[0] = height_min_max[0]*scale_in;
    height_[1] = height_min_max[1]*scale_in;
	
 
	int num_heights = (detail_level+1)*(detail_level+1);
	
	if (height_list_) {
		zz_assert(num_heights == num_heights_);
	}
	else {
		height_list_ = zz_new float[num_heights];
		num_heights_ = num_heights;
	}

	if (num_heights == (width*width)) { // same size, just copy
		memcpy(height_list_, height_list, num_heights*sizeof(float));
		for (int i = 0; i < num_heights; ++i) {
			height_list_[i] *= scale_in;
		}
	}
	else {
		int x, y;
		int is = 0, it = 0;
		assert(detail_level < width);
		int skipsize = (width-1)/detail_level;
		for (y = 0; y < width; y += skipsize) {
			for (x = 0; x < width; x += skipsize) {
				is = y*width + x;
				assert(is < (width*width));
				height_list_[it] = scale_in*height_list[is];
				++it;
			}
		}
	}

	pmin = vec3(orig_x_, orig_y_ - blocksize_, height_[0]);
	pmax = vec3(orig_x_ + blocksize_, orig_y_, height_[1]);
}
void zz_mesh_terrain::set_property2
	(
		float scale_in, // client to engine scale
		vec3 minmax[2], // minmax 
		int uv_type0, // uv type for map1
		int uv_type1, // uv type for map2
		int width, // width size of height_list. number of horizotal heights
		int numskip, // skipping vertex. if skipsize is 2, we skip 2 vertices every vertex for both horizontal and vertical.
		const float * height_list, // height value array list
		float mapsize, // lightmap size. ex) 16000
		float blocksize // one block unit in world
	)
{
	pmin = scale_in*minmax[0];
	pmax = scale_in*minmax[1];

	orig_x_ = pmin.x;
	orig_y_ = pmax.y;

	detail_level_ = (width - 1) / (numskip + 1); // number of faces
	assert(detail_level_);
	uv_type0_ = uv_type0;
	uv_type1_ = uv_type1;
	mapsize_ = mapsize*scale_in;
	blocksize_ = blocksize*scale_in;

	int num_heights = (detail_level_ + 1) * (detail_level_ + 1);
	
	if (height_list_) {
		zz_assert(num_heights < num_heights_);
	}
	else {
		height_list_ = zz_new float[num_heights];
		num_heights_ = num_heights;
	}

	if (numskip == 0) { // same size, just copy
		memcpy(height_list_, height_list, num_heights*sizeof(float));
		for (int i = 0; i < num_heights; ++i) {
			height_list_[i] *= scale_in;
		}
	}
	else {
		int x, y;
		int is = 0, it = 0;
		for (y = 0; y < width; y += (numskip+1)) {
			for (x = 0; x < width; x += (numskip+1)) {
				is = y*width + x;
				assert(is < (width*width));
				height_list_[it] = scale_in*height_list[is];
				++it;
			}
		}
		assert(it == num_heights_);
	}
}

bool zz_mesh_terrain::load_mesh ()
{
	return zz_mesh_tool::load_terrain_mesh(this);
}


zz_mesh_terrain_rough::zz_mesh_terrain_rough(int key_in) : zz_mesh_terrain(key_in)
{
}

zz_mesh_terrain_rough::~zz_mesh_terrain_rough()
{
}
