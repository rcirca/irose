/*
@file zz_vshader_constants.h
@brief vertex shader constants
@author Jiho Choi (zho@korea.com)
@version 1.0
@date    18-jun-2002
$Id: vshader_constants.h,v 1.1.1.1 2002/09/06 00:44:05 Jiho Choi Exp $
*/

// CAUTION: this file have to syncronize with "zz_renderer.h"
// vertex
#define V_VERTEX_POS		v0
#define V_NORMAL_VECTOR		v1
#define V_TEXTURE_UV0		v2
#define V_TEXTURE_UV1		v3
#define V_TANGENT_VECTOR	v4
#define V_BLEND_INDICES		v5
#define V_BLEND_WEIGHTS		v6

// constant : must sync with zz_renderer.h
#define C_WORLDVIEW_TM		c0      // use only 3 registers (use mat4x3)
#define C_PROJECTION_TM		c3      // use mat4x4
#define C_LIGHT_TEX_TM      c7      // use mat4x4
#define C_LIGHT_DIFFUSE		c11
//#define C_ONE				c11      // 1.0 in .w component. In shader, use as C_ONE.w
#define C_LIGHT_AMBIENT		c12
//#define C_HALF				c12      // .5 in .w component. In shader, use as C_HALF.w
#define C_LIGHT_DIRECTION	c13		// vertex to light source
#define C_CAMERA_POSITION	c14		// world
#define C_ONE_HALF_ZERO_MAXPOWER     c15     // [1.0, .5, 0.0, 127.9961]
#define C_BONESTART			16		// max 3x27 = 81 bone's modilview + world matrix starting point

