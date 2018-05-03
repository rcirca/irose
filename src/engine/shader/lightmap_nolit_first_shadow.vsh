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
// c18 = C_LIGHTMAP_TRANSFORM (scale and translation) : [uv_index_x, uv_index_y, 0, 1/width]
// ...
// c30-95 = bone matrix (max 22( 3x22 = 66) bone's modelview + world matrix)
//---------------------------------------------------------------
dcl_position      V_POSITION
dcl_normal        V_NORMAL
dcl_texcoord0     V_TEXCOORD0
dcl_texcoord1     V_TEXCOORD1

//-------------------------------------------------
// compute position (oPos)
//-------------------------------------------------
m4x3 r4, V_POSITION, C_WORLDVIEW_TM // apply worlview matrix
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
mov oT0.xy, V_TEXCOORD0.xy // diffuse texture coords

// shadow map texture transform
// - assumes r4 to be modelview position coords
// - divide by w
m4x4 r0, r4, C_LIGHT_TEX_TM   // shadow map
rcp r0.z, r0.w // 1/w
mul oT1.xy, r0.xy, r0.zz

