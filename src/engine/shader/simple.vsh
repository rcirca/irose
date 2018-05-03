vs.1.1
//---------------------------------------------------------------
// c0  = world-view matrix
// c4  = projection matrix
// c8  = light texture tm
// c12 = light diffuse
// c13 = light ambient
// c14 = light direction (or position)
// c15 = camera position (world)
// c16 = [1.0, .5, 0.0, 127.9961]
// c17 = C_ALPHA_FOG_START_END_FACTOR = [fog_start, fog_end, 1/(fog_end - fog_start), 0]
// ...
// c30-95 = bone matrix (max 22( 3x22 = 66) bone's modelview + world matrix)
//---------------------------------------------------------------
dcl_position      V_POSITION
dcl_normal        V_NORMAL
dcl_tangent       V_TANGENT
dcl_blendindices  V_BLENDINDICES
dcl_blendweight   V_BLENDWEIGHT
dcl_texcoord0     V_TEXCOORD0
dcl_texcoord1     V_TEXCOORD1

m4x3 r4, V_POSITION, C_WORLDVIEW_TM // apply worlview matrix
mov r4.w, C_ONE_HALF_ZERO_MAXPOWER.x
m4x4 oPos, r4, C_PROJECTION_TM // apply projection matrix

// transform normal for world & projection matrix
m3x3 r5, V_NORMAL, C_WORLDVIEW_TM

//-------------------------------------------------
// normalize normals (r5)
//-------------------------------------------------
dp3 r5.w, r5, r5
rsq r5.w, r5.w
mul r5, r5, r5.w

//-------------------------------------------------
// oD0 = ambient term
//-------------------------------------------------
dp3 r0, r5, C_LIGHT_DIRECTION       // N dot L in world space  
mul r3.x, r0.x, r0.x
mul r1, r3.x, C_LIGHT_DIFFUSE      // multiply by diffuse
add oD0, r1, C_LIGHT_AMBIENT       // add in ambient

//------------------------------------------------------------------------------
// Texture coordinates
//------------------------------------------------------------------------------
// Copy tex coords
mov oT0.xy, V_TEXCOORD0.xy       // color map
mov oT1.xy, V_TEXCOORD1.xy       // light map
// shadow map texture transform
// - assumes r4 to be modelview position coords
// - divide by w
m4x4 r0, r4, C_LIGHT_TEX_TM   // shadow map
rcp r0.z, r0.w // 1/w
mul oT3.xy, r0.xy, r0.zz
