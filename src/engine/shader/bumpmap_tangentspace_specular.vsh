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


m4x3 r4, V_POSITION, C_WORLDVIEW_TM // apply worldview matrix
m3x3 r5, V_NORMAL, C_WORLDVIEW_TM // r5 = normal transform

//-------------------------------------------------
// compute position (oPos)
//-------------------------------------------------
mov r4.w, C_ONE_HALF_ZERO_MAXPOWER.x
mov r6, r4
m4x4 oPos, r4, C_PROJECTION_TM // apply projection matrix

//-------------------------------------------------
// normalize normals (r5)
//-------------------------------------------------
dp3 r5.w, r5, r5
rsq r5.w, r5.w
mul r5, r5, r5.w

//-------------------------------------------------
//tangent & normal generation
// r3 = world * tangent_vector
// r5 = world * normal_vector
//-------------------------------------------------
m3x3 r3, V_TANGENT, C_WORLDVIEW_TM          // generate tangent

//normalize tangent (if scaling was applied)
//dp3 r3.w, r3.xyz,r3.xyz	 // load the square into r11
//rsq r3.w, r3.w  		 // get the inverse of the square
//mul r3.xyz, r3.xyz, r3.www // multiply, r2 = vertex to camera

//-------------------------------------------------
//generate binormal via Cross product
// r4 = r5 x (r3)
//-------------------------------------------------
mul r0, r5.zxyw, r3.yzxw
mad r4, r5.yzxw, r3.zxyw, -r0

// normalize r4
//dp3 r4.w, r4, r4	 // load the square into r11
//rsq r4.w, r4.w  		 // get the inverse of the square
//mul r4, r4, r4.w // multiply, r2 = vertex to camera

//-------------------------------------------------
// oD1 = half-angle for specular term
//-------------------------------------------------
// get a vector toward the camera = r2
// r2 = r1(camera_position) - world * ZZ_SM_VERTEX_POS
m4x3 r2, V_POSITION, C_WORLDVIEW_TM// transform position
mov r1, C_CAMERA_POSITION
add r2, -r2, r1

// r2 = normalize(r2)
dp3 r2.w,  r2.xyz,r2.xyz	// load the square into r11
rsq r2.w, r2.w              // get the inverse of the square
mul r2.xyz, r2.xyz, r2.www	// multiply, r2 = vertex to camera

// r2 = r2 + light_direction
add r2.xyz, r2.xyz, C_LIGHT_DIRECTION		// to get half angle

//normalize
dp3 r2.w, r2.xyz,r2.xyz		// load the square into r1
rsq r2.w, r2.w				// get the inverse of the square
mul r2.xyz, r2.xyz, r2.www	// multiply, r2 = HalfAngle

// transform the half angle vector -> r8
dp3 r8.x,r3,r2
dp3 r8.y,r4,r2
dp3 r8.z,r5,r2

// normalize r8
dp3 r8.w, r8.xyz,r8.xyz		// load the square into r1
rsq r8.w, r8.w				// get the inverse of the square
mul r8.xyz, r8.xyz, r8.www	// multiply, r8 = HalfAngle

// r8[-1..1] -> oD1[0..1]
// oD1 = r8 / 2 + 1/2
mad oD1.xyz, r8.xyz, C_ONE_HALF_ZERO_MAXPOWER.yyyy, C_ONE_HALF_ZERO_MAXPOWER.yyyy // multiply by a half to bias, then add half	

//-------------------------------------------------
// oD0 = ambient term
//-------------------------------------------------
dp3 r0, r5, C_LIGHT_DIRECTION       // N dot L in world space  
mul r3.x, r0.x, r0.x
mul r1, r3.x, C_LIGHT_DIFFUSE      // multiply by diffuse
add oD0, r1, C_LIGHT_AMBIENT       // add in ambient

//-------------------------------------------------
//tex coords
//-------------------------------------------------
mov oT0.xy, V_TEXCOORD0.xy       // color map
mov oT1.xy, V_TEXCOORD1.xy       // light map
m4x4 oT2, r6, C_LIGHT_TEX_TM   // shadow map
// shadow map texture transform
// - assumes r6 to be modelview position coords
// - divide by w
m4x4 r0, r6, C_LIGHT_TEX_TM   // shadow map
rcp r0.z, r0.w // 1/w
mul oT3.xy, r0.xy, r0.zz

//------------------------------------------------------------------------------
// compute fog factor f = (fog_end - dist)*(1/(fog_end-fog_start))
//------------------------------------------------------------------------------
add r0.x, r6.z, C_FOG_START_END_FACTOR.y // dist already negative because of OpenGL notation (-z axis)
mul r0.x, r0.x, C_FOG_START_END_FACTOR.z
max r0.x, r0.x, C_ONE_HALF_ZERO_MAXPOWER.z       // clamp fog to > 0.0
min oFog, r0.x, C_ONE_HALF_ZERO_MAXPOWER.x       // clamp fog to < 1.0
//mov oFog, C_ONE_HALF_ZERO_MAXPOWER.y