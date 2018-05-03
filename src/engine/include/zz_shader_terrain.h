/** 
 * @file zz_shader_terrain.h
 * @brief terrain shader class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    20-may-2004
 *
 * $Header: /engine/include/zz_shader_terrain.h 1     04-06-22 9:53a Zho $
 * $History: zz_shader_terrain.h $
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-06-22   Time: 9:53a
 * Created in $/engine/include
 */

#ifndef __ZZ_SHADER_TERRAIN_H__
#define __ZZ_SHADER_TERRAIN_H__

#ifndef __ZZ_SHADER_H__
#include "zz_shader.h"
#endif

#define ZZ_SHADER_TERRAIN_TYPE_NONE					0
#define ZZ_SHADER_TERRAIN_TYPE_FIRSTMAP			(1 << 0)
#define ZZ_SHADER_TERRAIN_TYPE_SECONDMAP		(1 << 1)
#define ZZ_SHADER_TERRAIN_TYPE_LIGHTMAP			(1 << 2)
#define ZZ_SHADER_TERRAIN_TYPE_SHADOWMAP		(1 << 3)
#define ZZ_SHADER_TERRAIN_TYPE_FOG					(1 << 4)
#define ZZ_SHADER_TERRAIN_TYPE_MAX					(1 << 5)

//--------------------------------------------------------------------------------
class zz_shader_terrain : public zz_shader {
protected:
	zz_handle vshader_terrain_handles[ZZ_SHADER_TERRAIN_TYPE_MAX];
	char * vshader_terrain_names[ZZ_SHADER_TERRAIN_TYPE_MAX];

public:
	zz_shader_terrain(void);
	virtual ~zz_shader_terrain(void);

	zz_handle create_vshader_terrain (const char * vshader_filename, int shader_terrain_type);

	zz_handle get_vshader_terrain (int shader_terrain_type);

	ZZ_DECLARE_DYNAMIC(zz_shader_terrain)
};

#endif // __ZZ_SHADER_TERRAIN_H__