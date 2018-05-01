/** 
 * @file zz_shader_terrain.cpp
 * @brief terrain shader class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    20-may-2004
 *
 * $Header: /engine/src/zz_shader_terrain.cpp 1     04-06-22 9:53a Zho $
 * $History: zz_shader_terrain.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-06-22   Time: 9:53a
 * Created in $/engine/src
 */

#include "zz_tier0.h"
#include "zz_shader.h"

ZZ_IMPLEMENT_DYNCREATE(zz_shader_terrain, zz_shader)

zz_shader_terrain::zz_shader_terrain()
{
	for (int i = 0; i < ZZ_SHADER_TERRAIN_TYPE_MAX; ++i) {
		vshader_terrain_handles[i] = ZZ_HANDLE_NULL;
		vshader_terrain_names[i] = NULL;
	}
}
