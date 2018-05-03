/** 
 * @file zz_mesh_ocean.CPP
 * @brief ocean mesh class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    27-oct-2003
 *
 * $Header: /engine/src/zz_mesh_ocean.cpp 15    04-07-07 9:57p Zho $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_mesh_ocean.h"
#include "zz_mesh_tool.h"

ZZ_IMPLEMENT_DYNCREATE(zz_mesh_ocean, zz_mesh_ishared)

zz_mesh_ocean::zz_mesh_ocean () :
	width_(0),
	height_(0),
	repeat_hori_(0),
	repeat_vert_(0),
	zz_mesh_ishared(OCEAN_INDEX_ORDER)
{
}

zz_mesh_ocean::~zz_mesh_ocean ()
{
}

void zz_mesh_ocean::set_property (
								  float width,
								  float height,
								  int repeat_hori, // horizontal tiling count
								  int repeat_vert // vertical tiling count
								  )
{
	const float OCEAN_DEPTH = 10000.0f*ZZ_SCALE_IN;
	
	width_ = width;
	height_ = height;
	repeat_hori_ = repeat_hori;
	repeat_vert_ = repeat_vert;
	
	pmin.set((width > 0) ? 0 : width, (height > 0) ? 0 : height, -OCEAN_DEPTH);
	pmax.set((width > 0) ? width : 0, (height > 0) ? height : 0, 0);
}

bool zz_mesh_ocean::load_mesh ()
{
	return zz_mesh_tool::load_ocean_mesh(this);
}
