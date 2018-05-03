vs.1.1
//---------------------------------------------------------------
// terrain vertex shader for pass 1
// render only first map and second map
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
//
// oT0 = firstmap
// oT1 = secondmap
// oT2 = lightmap
// oT3 = shadowmap
//---------------------------------------------------------------
dcl_position      V_POSITION
dcl_texcoord0     V_TEXCOORD0
dcl_texcoord1     V_TEXCOORD1
dcl_texcoord2     V_TEXCOORD2

m4x3 r4.xyz, V_POSITION, C_WORLDVIEW_TM // apply worlview matrix
mov r4.w, C_ONE_HALF_ZERO_MAXPOWER.x
m4x4 oPos, r4, C_PROJECTION_TM // apply projection matrix

//------------------------
// use diffuse color
//------------------------
mov oD0.xyz, C_LIGHT_DIFFUSE.xyz
//------------------------------------------------------------------------------
// set alpha blend fog
// compute fog factor f = (alpha_fog_end - dist)*(1/(alpha_fog_end-alpha_fog_start))
//------------------------------------------------------------------------------
add r0.x, r4.z, C_ALPHAFOG_START_END_FACTOR.y // dist already negative because of OpenGL notation (-z axis)
mul oD0.w, r0.x, C_ALPHAFOG_START_END_FACTOR.z

//------------------------------------------------------------------------------
// Texture coordinates
//------------------------------------------------------------------------------
// Copy tex coords
mov oT0.xy, V_TEXCOORD0.xy       // 1st map
