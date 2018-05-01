/** 
 * @file zz_mesh_ocean.CPP
 * @brief ocean mesh class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    27-oct-2003
 *
 * $Header: /engine/include/zz_mesh_ocean.h 8     04-07-06 3:27p Zho $
 */

#ifndef __ZZ_MESH_OCEAN_H__
#define __ZZ_MESH_OCEAN_H__

#ifndef __ZZ_MESH_H__
#include "zz_mesh.h"
#endif

#ifndef __ZZ_MESH_ISHARED_H__
#include "zz_mesh_ishared.h"
#endif

class zz_manager_mesh_ocean;

//--------------------------------------------------------------------------------
class zz_mesh_ocean : public zz_mesh_ishared {
//--------------------------------------------------------------------------------
public:
	zz_mesh_ocean ();
	virtual ~zz_mesh_ocean ();

	virtual bool load_mesh ();
	void set_property ( float width, float height, int repeat_hori, int repeat_vert );

protected:
	float width_;
	float height_;
	int repeat_hori_; // horizontal tiling count
	int repeat_vert_; // vertical tiling count
	friend class zz_mesh_tool;
	friend class zz_manager_mesh_ocean;

public:
	ZZ_DECLARE_DYNAMIC(zz_mesh_ocean)
};


#endif // __ZZ_MESH_OCEAN_H__

