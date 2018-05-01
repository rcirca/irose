vs.1.1
//---------------------------------------------------------------
// c0  = world-view matrix
// c4  = projection matrix
// c8  = light texture tm
// c12 = light diffuse
// c13 = light ambient
// c14 = light direction
// c15 = camera position (world)
// c16 = [1.0, .5, 0.0, 127.9961]
// c17 = C_ALPHA_FOG_START_END_FACTOR = [fog_start, fog_end, 1/(fog_end - fog_start), 0]
// c18-21 = world matrix
// c22 = light position(world-view)
// c23 = light attenuation(temporal)
// c24 = world matrix
// c30-95 = bone matrix (max 22( 3x22 = 66) bone's modelview + world matrix)
//
// oT0 = firstmap
// oT1 = secondmap
// oT2 = lightmap
// oT3 = shadowmap
// INPUT : V_POSITION, V_TEXCOORD0, V_TEXCOORD1, V_TEXCOORD2
// CONST : WORLDVIEW, PROJECTION, LIGHT_DIFFUSE, LIGHT_AMBIENT, ETC, FOG, WORLD_TM, LIGHT_POSITION, LIGHT_ATTENUATION
//---------------------------------------------------------------
dcl_position      V_POSITION
dcl_texcoord0     V_TEXCOORD0
dcl_texcoord1     V_TEXCOORD1
dcl_texcoord2     V_TEXCOORD2

//------------------------------------------------------------------------------
// Position
//------------------------------------------------------------------------------
m4x3 r4, V_POSITION, C_WORLDVIEW_TM // apply worlview matrix
mov r4.w, C_ONE_HALF_ZERO_MAXPOWER.x
m4x4 oPos, r4, C_PROJECTION_TM // apply projection matrix

//------------------------------------------------------------------------------
// Texture coordinates
//------------------------------------------------------------------------------
// Copy tex coords
mov oT0.xy, V_TEXCOORD0.xy       // 1st map
mov oT1.xy, V_TEXCOORD1.xy       // 2nd map
mov oT2.xy, V_TEXCOORD2.xy       // light map

//--------------------------------------------------------------------------------
// shadow map texture transform
// - assumes r4 to be modelview position coords
// - divide by w
//--------------------------------------------------------------------------------
m4x4 r0, r4, C_LIGHT_TEX_TM   // shadow map
rcp r0.z, r0.w // 1/w
mul oT3.xy, r0.xy, r0.zz

//--------------------------------------------------------------------------------
// Set diffuse color with point light
// temp reg. : r4, r5, r6, r7
// output reg. : oD0
// r7.w = distance*distance = (x*x) + (y*y) + (z*z)
// r5 : world position
// r6 : distance vector
m4x4 r5, V_POSITION, C_WORLD_TM
add r6.xyz, C_LIGHT_POSITION, -r5.xyz
dp3 r7.w, r6, r6
// r4.w = 1/sqrt(r7.w) = 1/|V| = 1/distance
rsq r4.w, r7.w
dst r7, r7.wwww, r4.wwww
dp3 r7.w, r7, C_LIGHT_ATTENUATION
// r6 = attenuation
rcp r6.w, r7.w
mul r7, r6.www, C_LIGHT_DIFFUSE.xyz
add oD0.xyz, r7.xyz, C_LIGHT_AMBIENT.xyz
mul oD0.w, r0.x, C_FOG_START_END_FACTOR.z
//--------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// compute fog factor f = (fog_end - dist)*(1/(fog_end-fog_start))
//------------------------------------------------------------------------------
add r0.x, r4.z, C_FOG_START_END_FACTOR.y // dist already negative because of OpenGL notation (-z axis)

