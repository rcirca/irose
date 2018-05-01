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
dcl_blendindices  V_BLENDINDICES
dcl_blendweight   V_BLENDWEIGHT
dcl_texcoord0     V_TEXCOORD0

//-------------------------------------------------
// Set 1
//-------------------------------------------------
mov a0.x, V_BLENDINDICES.x
m4x3 r4.xyz, V_POSITION, c[a0.x + C_BONESTART] // r4 = vertex transform
m3x3 r5.xyz, V_NORMAL, c[a0.x + C_BONESTART] // r5 = normal transform

//blend them
mul r4.xyz, r4.xyz, V_BLENDWEIGHT.xxxx
mul r5.xyz, r5.xyz, V_BLENDWEIGHT.xxxx

//-------------------------------------------------
// Set 2
//-------------------------------------------------
mov a0.x, V_BLENDINDICES.y
m4x3 r2.xyz, V_POSITION, c[a0.x + C_BONESTART]
m3x3 r3, V_NORMAL, c[a0.x + C_BONESTART]

//add them in
mad r4.xyz, r2.xyz, V_BLENDWEIGHT.yyyy, r4
mad r5.xyz, r3.xyz, V_BLENDWEIGHT.yyyy, r5

//-------------------------------------------------
// compute position (oPos)
//-------------------------------------------------
mov r4.w, C_ONE_HALF_ZERO_MAXPOWER.x
m4x4 oPos, r4, C_PROJECTION_TM // apply projection matrix

//-------------------------------------------------
// transform light vector to world space
//-------------------------------------------------
mov r7, C_LIGHT_DIRECTION

//-------------------------------------------------
// oD0 = ambient term
//-------------------------------------------------
dp3 r0, r5, r7       // N dot L in world space  
max r0.w, r0.w, C_ONE_HALF_ZERO_MAXPOWER.z  // clamp to [0..w]
add oD0, r0.wwww, C_ONE_HALF_ZERO_MAXPOWER.yyyy

//-------------------------------------------------
//tex coords
//-------------------------------------------------
mov oT0.xy, V_TEXCOORD0.xy       // color map
