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
dcl_texcoord0     V_TEXCOORD0

//-------------------------------------------------
// compute position (oPos)
//-------------------------------------------------
m4x3 r4, V_POSITION, C_WORLDVIEW_TM // apply worlview matrix
mov r4.w, C_ONE_HALF_ZERO_MAXPOWER.x
m4x4 oPos, r4, C_PROJECTION_TM // apply projection matrix

// transform normal for world & projection matrix
m3x3 r5, V_NORMAL, C_WORLDVIEW_TM

//-------------------------------------------------
// normalize normals (r5)
//-------------------------------------------------
//dp3 r5.w, r5, r5
//rsq r5.w, r5.w
//mul r5, r5, r5.w

//-------------------------------------------------
// transform light vector to world space
//-------------------------------------------------
mov r7, C_LIGHT_DIRECTION

//-------------------------------------------------
// oD0 = ambient term
//-------------------------------------------------
dp3 r0, r5, r7       // N dot L in world space  
//mul r0.w, r0.w, r0.w // r0 = r0^2
//rsq r0.w, r0.w       // r0 = sqrt(r0^2) = | r0 |
max r0.w, r0.w, C_ONE_HALF_ZERO_MAXPOWER.z  // clamp to [0..w]
mul r1, r0.wwww, C_LIGHT_DIFFUSE      // multiply by diffuse
add oD0, r1, C_LIGHT_AMBIENT       // add in ambient
//------------------------------------------------------------------------------
// compute fog factor f = (fog_end - dist)*(1/(fog_end-fog_start))
//------------------------------------------------------------------------------
add r0.x, r4.z, C_FOG_START_END_FACTOR.y // dist already negative because of OpenGL notation (-z axis)
mul oD0.w, r0.x, C_FOG_START_END_FACTOR.z

//------------------------------------------------------------------------------
// Texture coordinates
//------------------------------------------------------------------------------
// Copy tex coords
mov oT0.xy, V_TEXCOORD0.xy       // color map
// shadow map texture transform
// - assumes r6 to be modelview position coords
// - divide by w
//m4x4 r0, r6, C_LIGHT_TEX_TM   // shadow map
//rcp r0.z, r0.w // 1/w
//mul oT1.xy, r0.xy, r0.zz

//------------------------------------------------------------------------------
// compute fog factor f = (fog_end - dist)*(1/(fog_end-fog_start))
//------------------------------------------------------------------------------
//add r0.x, r6.z, C_FOG_START_END_FACTOR.y // dist already negative because of OpenGL notation (-z axis)
//mul r0.x, r0.x, C_FOG_START_END_FACTOR.z
//max r0.x, r0.x, C_ONE_HALF_ZERO_MAXPOWER.z       // clamp fog to > 0.0
//min oD0.w, r0.x, C_ONE_HALF_ZERO_MAXPOWER.x       // clamp fog to < 1.0