/**
 * @file zz_algebra.h
 * @brief algebra classes and functions
 * @author Jiho Choi(zho@korea.com)
 * @version 2.0
 * @date 12-jun-2002
 * $Header: /engine/src/zz_algebra.cpp 7     04-09-01 2:59p Zho $
 * $History: zz_algebra.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-09-01   Time: 2:59p
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-05-19   Time: 8:15p
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-04-17   Time: 2:56p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-06   Time: 1:23p
 * Updated in $/engine/src
 * Added double version.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-04   Time: 5:10p
 * Updated in $/engine/src
 * comments fixed
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 19  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

/* quaternion interpolation
 * "Portions Copyright (C) Jason Shankel, 2000"
 */

#include "zz_tier0.h"
#include "zz_algebra.h"

//-----------------------------------------------
// mat3
//-----------------------------------------------

mat3::mat3(const quat& q)
{
	q.to_matrix(*this);
}
	
mat3& mat3::operator=(const quat& q)
{
	q.to_matrix(*this);
	return *this;
}

// v is normalized
// theta in radians
void mat3::set_rot(const float& theta, const vec3& v) 
{
	float ct = float(cos(theta));
	float st = float(sin(theta));

	float xx = v.x * v.x;
	float yy = v.y * v.y;
	float zz = v.z * v.z;
	float xy = v.x * v.y;
	float xz = v.x * v.z;
	float yz = v.y * v.z;

	a00 = xx + ct*(1-xx);
	a01 = xy + ct*(-xy) + st*-v.z;
	a02 = xz + ct*(-xz) + st*v.y;

	a10 = xy + ct*(-xy) + st*v.z;
	a11 = yy + ct*(1-yy);
	a12 = yz + ct*(-yz) + st*-v.x;

	a20 = xz + ct*(-xz) + st*-v.y;
	a21 = yz + ct*(-yz) + st*v.x;
	a22 = zz + ct*(1-zz);
}

// make rotation matrix that rotate u-vector to v-vector
void mat3::set_rot(const vec3& u, const vec3& v)
{
	float phi;
	float h;
	float lambda;
	vec3 w;

    cross(w,u,v);
    dot(phi,u,v);
    dot(lambda,w,w);
	if (lambda > ZZ_EPS)
		h = (1.0f - phi) / lambda;
	else
		h = lambda;

	float hxy = w.x * w.y * h;
	float hxz = w.x * w.z * h;
	float hyz = w.y * w.z * h;

	a00 = phi + w.x * w.x * h;
	a01 = hxy - w.z;
	a02 = hxz + w.y;

	a10 = hxy + w.z;
	a11 = phi + w.y * w.y * h;
	a12 = hyz - w.x;

	a20 = hxz - w.y;
	a21 = hyz + w.x;
	a22 = phi + w.z * w.z * h;
}

mat3& mat3::mult_scale (const vec3& scale)
{
	a00 *= scale.x;
	a10 *= scale.x;
	a20 *= scale.x;

	a01 *= scale.y;
	a11 *= scale.y;
	a21 *= scale.y;

	a02 *= scale.z;
	a12 *= scale.z;
	a22 *= scale.z;

	return *this;
}

//-----------------------------------------------
// mat4
//-----------------------------------------------

mat4::mat4(const mat3& M)
{
	this->set_identity();
	set_rot(M);
}

mat4::mat4( const vec3& pos, const quat& rot )
{
	set(pos, rot);
}

void mat4::set(const vec3& pos, const quat& rot)
{
	set_identity();
	set_rot(rot);
	set_translation(pos);
}

void mat4::set_identity (void)
{
	memcpy(mat_array, array16_id, sizeof(float)*16);
}

mat4 mat4::inverse (void)
{
	mat4 m;
	invert(m, *this);
	return m;
}

quat & mat4::get_rot(quat& q) const
{
    float tr = a00 + a11 + a22;
      
    // check the diagonal
    if ( tr > .0f) 
    {
        // Diagonal is positive.
        float s = sqrtf(tr + 1.0f);
        q.w = s * .5f;
        s = .5f / s;
        q.x = (a21 - a12) * s;
        q.y = (a02 - a20) * s;
        q.z = (a10 - a01) * s;
    } // have positive trace
    else
    {                
        // Diagonal is negative.
        int nxt[3] = {1, 2, 0};
   
        // Find the largest diagonal.
        int i = 0;
        if ( a11 > a00 )
            i = 1;
        if ( a22 > (*this)[i][i] )
            i = 2;
        int j = nxt[i];
        int k = nxt[j];
      
        float s = sqrtf(( (*this)[i][i] - ( (*this)[j][j] + (*this)[k][k] )) + 1.0f );
      
        q[i] = s * .5f;
        s = .5f / s;
      
        q.w  = ((*this)[k][j] - (*this)[j][k]) * s;
        q[j] = ((*this)[j][i] + (*this)[i][j]) * s;
        q[k] = ((*this)[k][i] + (*this)[i][k]) * s;
    } // have negative or zero trace
    return q;
}

// this code has a problem
//quat & mat4::get_rot(quat& q) const
//{
//    float tr = a00 + a11 + a22;
//      
//    // check the diagonal
//    if ( tr > .0f) 
//    {
//        // Diagonal is positive.
//        float s = sqrtf(tr + 1.0f);
//        q.w = s * .5f;
//        s = .5f / s;
//        q.x = (a12 - a21) * s;
//        q.y = (a20 - a02) * s;
//        q.z = (a01 - a10) * s;
//    } // have positive trace
//    else
//    {                
//        // Diagonal is negative.
//        int nxt[3] = {1, 2, 0};
//   
//        // Find the largest diagonal.
//        int i = 0;
//        if ( a11 > a00 )
//            i = 1;
//        if ( a22 > (*this)[i][i] )
//            i = 2;
//        int j = nxt[i];
//        int k = nxt[j];
//      
//        float s = sqrtf(( (*this)[i][i] - ( (*this)[j][j] + (*this)[k][k] )) + 1.0f );
//      
//        q[i] = s * .5f;
//        s = .5f / s;
//      
//        q.w  = ((*this)[j][k] - (*this)[k][j]) * s;
//        q[j] = ((*this)[i][j] + (*this)[j][i]) * s;
//        q[k] = ((*this)[i][k] + (*this)[k][i]) * s;
//    } // have negative or zero trace
//    return q;
//}

// zho patched this!
void mat4::set_rot(const quat& q)
{
    float x2 = q.x * q.x * 2.0f;
    float y2 = q.y * q.y * 2.0f;
    float z2 = q.z * q.z * 2.0f;

    a00 = 1.0f - y2 - z2;
    a11 = 1.0f - x2 - z2;
    a22 = 1.0f - x2 - y2;

	float wz = q.w*q.z, xy = q.x*q.y;        
    a01 = 2.0f * (xy - wz);
    a10 = 2.0f * (xy + wz);

    float wx = q.w*q.x, yz = q.y*q.z;
    a12 = 2.0f * (yz - wx);
    a21 = 2.0f * (yz + wx);

    float wy = q.w*q.y, xz = q.x*q.z;
    a02 = 2.0f * (xz + wy);
    a20 = 2.0f * (xz - wy);
}

// this code has a problem
//void mat4::set_rot(const quat& q)
//{
//    float x2 = q.x * q.x * 2.0f;
//    float y2 = q.y * q.y * 2.0f;
//    float z2 = q.z * q.z * 2.0f;
//
//    a00 = 1.0f - y2 - z2;
//    a11 = 1.0f - x2 - z2;
//    a22 = 1.0f - x2 - y2;
//
//	float wz = q.w*q.z, xy = q.x*q.y;        
//    a01 = 2.0f * (xy + wz);
//    a10 = 2.0f * (xy - wz);
//
//    float wx = q.w*q.x, yz = q.y*q.z;
//    a12 = 2.0f * (yz + wx);
//    a21 = 2.0f * (yz - wx);
//
//    float wy = q.w*q.y, xz = q.x*q.z;
//    a02 = 2.0f * (xz - wy);
//    a20 = 2.0f * (xz + wy);
//}

// v is normalized
// theta in radians
void mat4::set_rot(const float& theta, const vec3& v) 
{
    float ct = float(cos(theta));
    float st = float(sin(theta));

    float xx = v.x * v.x;
    float yy = v.y * v.y;
    float zz = v.z * v.z;
    float xy = v.x * v.y;
    float xz = v.x * v.z;
    float yz = v.y * v.z;

    a00 = xx + ct*(1-xx);
    a01 = xy + ct*(-xy) + st*-v.z;
    a02 = xz + ct*(-xz) + st*v.y;

    a10 = xy + ct*(-xy) + st*v.z;
    a11 = yy + ct*(1-yy);
    a12 = yz + ct*(-yz) + st*-v.x;

    a20 = xz + ct*(-xz) + st*-v.y;
    a21 = yz + ct*(-yz) + st*v.x;
    a22 = zz + ct*(1-zz);
}

void mat4::set_rot(const vec3& u, const vec3& v)
{
    float phi;
    float h;
    float lambda;
    vec3 w;

    cross(w,u,v);
    dot(phi,u,v);
    dot(lambda,w,w);
    if (lambda > ZZ_EPS)
        h = (1.0f - phi) / lambda;
    else
        h = lambda;
    
    float hxy = w.x * w.y * h;
    float hxz = w.x * w.z * h;
    float hyz = w.y * w.z * h;

    a00 = phi + w.x * w.x * h;
    a01 = hxy - w.z;
    a02 = hxz + w.y;

    a10 = hxy + w.z;
    a11 = phi + w.y * w.y * h;
    a12 = hyz - w.x;

    a20 = hxz - w.y;
    a21 = hyz + w.x;
    a22 = phi + w.z * w.z * h;
}

mat4 mat4::operator*(const mat4& B) const
{
    mat4 C;
    C.a00 = a00 * B.a00 + a01 * B.a10 + a02 * B.a20 + a03 * B.a30;
    C.a10 = a10 * B.a00 + a11 * B.a10 + a12 * B.a20 + a13 * B.a30;
    C.a20 = a20 * B.a00 + a21 * B.a10 + a22 * B.a20 + a23 * B.a30;
    C.a30 = a30 * B.a00 + a31 * B.a10 + a32 * B.a20 + a33 * B.a30;
    C.a01 = a00 * B.a01 + a01 * B.a11 + a02 * B.a21 + a03 * B.a31;
    C.a11 = a10 * B.a01 + a11 * B.a11 + a12 * B.a21 + a13 * B.a31;
    C.a21 = a20 * B.a01 + a21 * B.a11 + a22 * B.a21 + a23 * B.a31;
    C.a31 = a30 * B.a01 + a31 * B.a11 + a32 * B.a21 + a33 * B.a31;
    C.a02 = a00 * B.a02 + a01 * B.a12 + a02 * B.a22 + a03 * B.a32;
    C.a12 = a10 * B.a02 + a11 * B.a12 + a12 * B.a22 + a13 * B.a32;
    C.a22 = a20 * B.a02 + a21 * B.a12 + a22 * B.a22 + a23 * B.a32;
    C.a32 = a30 * B.a02 + a31 * B.a12 + a32 * B.a22 + a33 * B.a32;
    C.a03 = a00 * B.a03 + a01 * B.a13 + a02 * B.a23 + a03 * B.a33;
    C.a13 = a10 * B.a03 + a11 * B.a13 + a12 * B.a23 + a13 * B.a33;
    C.a23 = a20 * B.a03 + a21 * B.a13 + a22 * B.a23 + a23 * B.a33;
    C.a33 = a30 * B.a03 + a31 * B.a13 + a32 * B.a23 + a33 * B.a33;
    return C;
}

mat4& mat4::mult_scale (const vec3& scale)
{
	a00 *= scale.x;
	a10 *= scale.x;
	a20 *= scale.x;

	a01 *= scale.y;
	a11 *= scale.y;
	a21 *= scale.y;

	a02 *= scale.z;
	a12 *= scale.z;
	a22 *= scale.z;

	return *this;
}

//-----------------------------------------------
// quat
//-----------------------------------------------

quat::quat(const mat3& rot)
{
	from_matrix(rot);
}

quat& quat::operator=(const quat& q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
	return *this;
}

quat& quat::operator=(const mat3& mat)
{
	this->from_matrix(mat);
	return *this;
}

void quat::set_rot(const vec3& u, const vec3& v)
{
	mat3 mat_rot(mat3_id);
	mat_rot.set_rot(u, v);
	from_matrix(mat_rot);
}

void quat::from_matrix(const mat3& mat)
{
	float trace = mat(0, 0) + mat(1, 1) + mat(2, 2);
	if (trace > 0) {
		float scale = sqrtf(trace + 1.0f);
		w = 0.5f * scale;
		scale = 0.5f / scale;
		x = scale * (mat(2, 1) - mat(1, 2));
		y = scale * (mat(0, 2) - mat(2, 0));
		z = scale * (mat(1, 0) - mat(0, 1));
	}
	else {
		static int next[] = { 1, 2, 0 };
		int i = 0;
		if (mat(1, 1) > mat(0, 0))
			i = 1;
		if (mat(2, 2) > mat(i, i))
			i = 2;
		int j = next[i];
		int k = next[j];
		float scale = sqrtf(mat(i, i) - mat(j, j) - mat(k, k) + 1);
		float* q[] = { &x, &y, &z };
		*q[i] = 0.5f * scale;
		scale = 0.5f / scale;
		w = scale * (mat(k, j) - mat(j, k));
		*q[j] = scale * (mat(j, i) + mat(i, j));
		*q[k] = scale * (mat(k, i) + mat(i, k));
	}
}

void quat::to_matrix(mat3& mat) const
{
	float x2 = x * 2;
	float y2 = y * 2;
	float z2 = z * 2;
	float wx = x2 * w;
	float wy = y2 * w;
	float wz = z2 * w;
	float xx = x2 * x;
	float xy = y2 * x;
	float xz = z2 * x;
	float yy = y2 * y;
	float yz = z2 * y;
	float zz = z2 * z;
	mat(0, 0) = 1 - (yy + zz);
	mat(0, 1) = xy - wz;
	mat(0, 2) = xz + wy;
	mat(1, 0) = xy + wz;
	mat(1, 1) = 1 - (xx + zz);
	mat(1, 2) = yz - wx;
	mat(2, 0) = xz - wy;
	mat(2, 1) = yz + wx;
	mat(2, 2) = 1 - (xx + yy);
}


mat3 & quat_2_mat(mat3& M, const quat& q )
{
    float x2 = q.x * q.x * 2.0f;
    float y2 = q.y * q.y * 2.0f;
    float z2 = q.z * q.z * 2.0f;

    M.a00 = 1.0f - y2 - z2;
    M.a11 = 1.0f - x2 - z2;
    M.a22 = 1.0f - x2 - y2;

    float wz = q.w*q.z, xy = q.x*q.y;        
    M.a01 = 2.0f * (xy - wz);
    M.a10 = 2.0f * (xy + wz);

    float wx = q.w*q.x, yz = q.y*q.z;
    M.a12 = 2.0f * (yz - wx);
    M.a21 = 2.0f * (yz + wx);

    float wy = q.w*q.y, xz = q.x*q.z;
    M.a02 = 2.0f * (xz + wy);
    M.a20 = 2.0f * (xz - wy);
    return M;
}

quat & mat_2_quat(quat& q,const mat3& M)
{
    float tr = M.a00 + M.a11 + M.a22;
      
    // check the diagonal
    if ( tr > .0f) 
    {
        // Diagonal is positive.
        float s = sqrtf(tr + 1.0f);
        q.w = s * .5f;
        s = .5f / s;
        q.x = (M.a21 - M.a12) * s;
        q.y = (M.a02 - M.a20) * s;
        q.z = (M.a10 - M.a01) * s;
    } // have positive trace
    else
    {                
        // Diagonal is negative.
        int nxt[3] = {1, 2, 0};
   
        // Find the largest diagonal.
        int i = 0;
        if ( M.a11 > M.a00 )
            i = 1;
        if ( M.a22 > M[i][i] )
            i = 2;
        int j = nxt[i];
        int k = nxt[j];
      
        float s = sqrtf( M[i][i] - ( M[j][j] + M[k][k] )) + 1.0f;
      
        q[i] = s * .5f;
        s = .5f / s;
      
        q.w  = (M[k][j] - M[j][k]) * s;
        q[j] = (M[j][i] + M[i][j]) * s;
        q[k] = (M[k][i] + M[i][k]) * s;
    } // have negative or zero trace
    return q;
} 

quat & mat_2_quat(quat& q,const mat4& M)
{
    float tr = M.a00 + M.a11 + M.a22;
      
    // check the diagonal
    if ( tr > .0f) 
    {
        // Diagonal is positive.
        float s = sqrtf(tr + 1.0f);
        q.w = s * .5f;
        s = .5f / s;
        q.x = (M.a21 - M.a12) * s;
        q.y = (M.a02 - M.a20) * s;
        q.z = (M.a10 - M.a01) * s;
    } // have positive trace
    else
    {                
        // Diagonal is negative.
        int nxt[3] = {1, 2, 0};
   
        // Find the largest diagonal.
        int i = 0;
        if ( M.a11 > M.a00 )
            i = 1;
        if ( M.a22 > M[i][i] )
            i = 2;
        int j = nxt[i];
        int k = nxt[j];
      
        float s = sqrtf( M[i][i] - ( M[j][j] + M[k][k] )) + 1.0f;
      
        q[i] = s * .5f;
        s = .5f / s;
      
        q.w  = (M[k][j] - M[j][k]) * s;
        q[j] = (M[j][i] + M[i][j]) * s;
        q[k] = (M[k][i] + M[i][k]) * s;
    } // have negative or zero trace
    return q;
} 


// Convert euler(yaw/pitch/roll) angle to quatertion.
// The order of euler rotation is roll first, then pitch, then yaw.
// This operates same as D3DXQuaternionRotationYawPitchRoll 
// refer: D3DXMatrixRotationYawPitchRoll()
// yaw = around the y-axis, in radians. 
// pitch = around the x-axis, in radians. 
// roll = around the z-axis, in radians. 
// CAUTION: This returns quaternion in D3D coordinate system, then you have to convert.
// by zho 2003.09.19
/*
convert euler angle(in radians) to quaternion
D3D coordinate system (left-hand):

  ^ y(yaw)
  |
  +--> z(roll)
 /
v x(pitch)

*/
quat & euler_2_quat_d3d(quat &q, const float yaw, const float pitch, const float roll)
{
	quat qpitch(vec3(1, 0, 0), pitch), qyaw(vec3(0, 1, 0), yaw), qroll(vec3(0, 0, 1), roll);

	q = qyaw * qpitch * qroll;
	return q;
}

/*
convert euler angle(in radians) to quaternion
OpenGL coordinate system (right-hand):

  ^ z(yaw)
  |
  +--> y(roll)
 /
v x(pitch)

*/
quat & euler_2_quat_ogl(quat &q, const float yaw, const float pitch, const float roll)
{
	quat qpitch(vec3(1, 0, 0), pitch), qyaw(vec3(0, 0, 1), yaw), qroll(vec3(0, 1, 0), roll);

	q = qyaw * qpitch * qroll;
	return q;
}

/*
z-axis rotation:
[cos  -sin  0]
[sin   cos  0]
[  0     0  1]

x-axis rotation:
[1    0     0]
[0  cos  -sin]
[0  sin   cos]

y-axis rotation:
[ cos  0  sin]
[   0  1    0]
[-sin  0  cos]
*/

/// convert quaternion to euler(in radians)
/// order = (roll(y)->pitch(x)->yaw(z)) in ogl coordinate system
/// yaw(z-axis), pitch(x-axis), roll(y-axis)
/// from http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
/*
RotationMatrix = 
Rz * Rx * Ry = 
[c3c2-s3s1s2  -s3c1   s3s1c2]
[s3c2+c3s1s2   c3c1  -c3s1c2]
[      -c1s2     s1     c1c2]

where,
c1 = cos(theta_x), c2 = cos(theta_y), c3 = cos(theta_z)
s1 = sin(theta_x), s2 = sin(theta_y), s3 = sin(theta_z).

quaternion matrix = 
[1-2xx-2zz   2(xy-wz)   2(xz+wy)]
[ 2(xy+wz)  1-2xx-2zz   2(yz-wx)]
[ 2(xz-wy)   2(yz+wx)  1-2xx-2yy]

x-axis-rotation = asin(s1) = asin(2(yz+wx))
y-axis-rotation = atan(-(-c1s2), c1c2) = atan(-2(xz-wy), 1-2xx-2yy) = atan(2(wy-xz), xx+yy+zz+ww-2xx-2yy)
				= atan(2(wy-xz), -xx-yy+zz+ww)
z-axis-rotation = atan(-s3c1, c3c1) = atan(-2(xy-wz), 1-2xx-2zz) = atan(2(wz-xy), xx+yy+zz+ww-2xx-2zz)
				= atan(2(wz-xy), -xx+yy-zz+ww)
*/
void quat_2_euler_ogl (const quat& q, float& yaw, float& pitch, float& roll)
{
	float sqw = q.w*q.w;
	float sqx = q.x*q.x;
	float sqy = q.y*q.y;
	float sqz = q.z*q.z;
	pitch = asinf(2.0f * (q.y*q.z + q.w*q.x)); // rotation about x-axis
	roll = atan2f(2.0f * (q.w*q.y - q.x*q.z),(-sqx - sqy + sqz + sqw)); // rotation about y-axis
	yaw = atan2f(2.0f * (q.w*q.z - q.x*q.y),(-sqx + sqy - sqz + sqw)); // rotation about z-axis
}

/// convert quaternion to euler(in radians)
/// order = (roll(z)->pitch(x)->yaw(y)) in d3d coordinate system
/// yaw(y-axis), pitch(x-axis), roll(z-axis)
/*
RotationMatrix = 
Ry * Rx * Rz = 
[ c2c3+s2s1s3  -c2s3+s2s1c3  s2c1]
[        c1s3          c1c3   -s1]
[-s2c3+c2s1s3   s2s3+c2s1c3  c2c1]

where,
c1 = cos(theta_x), c2 = cos(theta_y), c3 = cos(theta_z)
s1 = sin(theta_x), s2 = sin(theta_y), s3 = sin(theta_z).

quaternion matrix = 
[1-2xx-2zz   2(xy-wz)   2(xz+wy)]
[ 2(xy+wz)  1-2xx-2zz   2(yz-wx)]
[ 2(xz-wy)   2(yz+wx)  1-2xx-2yy]

x-axis-rotation = asin(s1) = asin(-2(yz-wx))
y-axis-rotation = atan(s2c1, c2c1) = atan(2(xz+wy), 1-2xx-2yy) = atan(2(xz+wy), xx+yy+zz+ww-2xx-2yy)
				= atan(2(xz+wy), -xx-yy+zz+ww)
z-axis-rotation = atan(c1s3, c1c3) = atan(2(xy+wz), 1-2xx-2zz) = atan(2(xy+wz), xx+yy+zz+ww-2xx-2zz)
				= atan(2(xy+wz), -xx+yy-zz+ww)
*/
void quat_2_euler_d3d (const quat& q, float& yaw, float& pitch, float& roll)
{
	float sqw = q.w*q.w;
	float sqx = q.x*q.x;
	float sqy = q.y*q.y;
	float sqz = q.z*q.z;
	pitch = asinf(2.0f * (q.w*q.x - q.y*q.z)); // rotation about x-axis
	yaw = atan2f(2.0f * (q.x*q.z + q.w*q.y),(-sqx - sqy + sqz + sqw)); // rotation about y-axis
	roll = atan2f(2.0f * (q.x*q.y + q.w*q.z),(-sqx + sqy - sqz + sqw)); // rotation about z-axis
}

/*
    Given an axis and angle, compute quaternion.
 */
quat & axis_to_quat(quat& q, const vec3& a, const float phi)
{
    vec3 tmp(a.x, a.y, a.z);

    normalize(tmp);
	float s = sinf(phi/2.0f);
    q.x = s * tmp.x;
    q.y = s * tmp.y;
    q.z = s * tmp.z;
    q.w = cosf(phi/2.0f);
    return q;
}


quat & add_quats(quat& p, const quat& q1, const quat& q2)
{
    quat t1, t2;

    t1 = q1;
    t1.x *= q2.w;
    t1.y *= q2.w;
    t1.z *= q2.w;

    t2 = q2;
    t2.x *= q1.w;
    t2.y *= q1.w;
    t2.z *= q1.w;

    p.x = (q2.y * q1.z) - (q2.z * q1.y) + t1.x + t2.x;
    p.y = (q2.z * q1.x) - (q2.x * q1.z) + t1.y + t2.y;
    p.z = (q2.x * q1.y) - (q2.y * q1.x) + t1.z + t2.z;
    p.w = q1.w * q2.w - (q1.x * q2.x + q1.y * q2.y + q1.z * q2.z);

    return p;
}

/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */
float tb_project_to_sphere(float r, float x, float y)
{
    float d, t, z;

    d = sqrtf(x*x + y*y);
    if (d < r * 0.70710678118654752440) {    /* Inside sphere */
        z = sqrtf(r*r - d*d);
    } else {           /* On hyperbola */
        t = float(r / 1.41421356237309504880);
        z = t*t / d;
    }
    return z;
}

/*
 * Ok, simulate a track-ball.  Project the points onto the virtual
 * trackball, then figure out the axis of rotation, which is the cross
 * product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
 * Note:  This is a deformed trackball-- is a trackball in the center,
 * but is deformed into a hyperbolic sheet of rotation away from the
 * center.  This particular function was chosen after trying out
 * several variations.
 *
 * It is assumed that the arguments to this routine are in the range
 * (-1.0 ... 1.0)
 */
quat & trackball(quat& q, vec2& pt1, vec2& pt2, float trackballsize)
{
    vec3 a; // Axis of rotation
    float phi;  // how much to rotate about axis
    vec3 d;
    float t;

    if (pt1.x == pt2.x && pt1.y == pt2.y) 
    {
        // Zero rotation
        q = quat_id;
        return q;
    }

    // First, figure out z-coordinates for projection of P1 and P2 to
    // deformed sphere
    vec3 p1(pt1.x,pt1.y,tb_project_to_sphere(trackballsize,pt1.x,pt1.y));
    vec3 p2(pt2.x,pt2.y,tb_project_to_sphere(trackballsize,pt2.x,pt2.y));

    //  Now, we want the cross product of P1 and P2
    a.cross(p2,p1);

    //  Figure out how much to rotate around that axis.
    d.x = p1.x - p2.x;
    d.y = p1.y - p2.y;
    d.z = p1.z - p2.z;
    t = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z) / (trackballsize);

    // Avoid problems with out-of-control values...

    if (t > 1.0f)
        t = 1.0f;
    if (t < -1.0f) 
        t = -1.0f;
    phi = 2.0f * float(asin(t));
    axis_to_quat(q,a,phi);
    return q;
}



mat4 & transpose(mat4& B, const mat4& A)
{
    B.a00 = A.a00;
    B.a01 = A.a10;
    B.a02 = A.a20;
    B.a03 = A.a30;
    B.a10 = A.a01;
    B.a11 = A.a11;
    B.a12 = A.a21;
    B.a13 = A.a31;
    B.a20 = A.a02;
    B.a21 = A.a12;
    B.a22 = A.a22;
    B.a23 = A.a32;
    B.a30 = A.a03;
    B.a31 = A.a13;
    B.a32 = A.a23;
    B.a33 = A.a33;
    return B;
}

mat3 & transpose(mat3& B, const mat3& A)
{
    B.a00 = A.a00;
    B.a01 = A.a10;
    B.a02 = A.a20;
    B.a10 = A.a01;
    B.a11 = A.a11;
    B.a12 = A.a21;
    B.a20 = A.a02;
    B.a21 = A.a12;
    B.a22 = A.a22;
    return B;
}

mat3 mat3::transpose()
{
	mat3 m(*this);
	return ::transpose(m);
}

mat4 & invert(mat4& B, const mat4& A)
{
    float det,oodet;

    B.a00 =  det3x3(A.a11, A.a21, A.a31, A.a12, A.a22, A.a32, A.a13, A.a23, A.a33);
    B.a10 = -det3x3(A.a10, A.a20, A.a30, A.a12, A.a22, A.a32, A.a13, A.a23, A.a33);
    B.a20 =  det3x3(A.a10, A.a20, A.a30, A.a11, A.a21, A.a31, A.a13, A.a23, A.a33);
    B.a30 = -det3x3(A.a10, A.a20, A.a30, A.a11, A.a21, A.a31, A.a12, A.a22, A.a32);

    B.a01 = -det3x3(A.a01, A.a21, A.a31, A.a02, A.a22, A.a32, A.a03, A.a23, A.a33);
    B.a11 =  det3x3(A.a00, A.a20, A.a30, A.a02, A.a22, A.a32, A.a03, A.a23, A.a33);
    B.a21 = -det3x3(A.a00, A.a20, A.a30, A.a01, A.a21, A.a31, A.a03, A.a23, A.a33);
    B.a31 =  det3x3(A.a00, A.a20, A.a30, A.a01, A.a21, A.a31, A.a02, A.a22, A.a32);

    B.a02 =  det3x3(A.a01, A.a11, A.a31, A.a02, A.a12, A.a32, A.a03, A.a13, A.a33);
    B.a12 = -det3x3(A.a00, A.a10, A.a30, A.a02, A.a12, A.a32, A.a03, A.a13, A.a33);
    B.a22 =  det3x3(A.a00, A.a10, A.a30, A.a01, A.a11, A.a31, A.a03, A.a13, A.a33);
    B.a32 = -det3x3(A.a00, A.a10, A.a30, A.a01, A.a11, A.a31, A.a02, A.a12, A.a32);

    B.a03 = -det3x3(A.a01, A.a11, A.a21, A.a02, A.a12, A.a22, A.a03, A.a13, A.a23);
    B.a13 =  det3x3(A.a00, A.a10, A.a20, A.a02, A.a12, A.a22, A.a03, A.a13, A.a23);
    B.a23 = -det3x3(A.a00, A.a10, A.a20, A.a01, A.a11, A.a21, A.a03, A.a13, A.a23);
    B.a33 =  det3x3(A.a00, A.a10, A.a20, A.a01, A.a11, A.a21, A.a02, A.a12, A.a22);

    det = (A.a00 * B.a00) + (A.a01 * B.a10) + (A.a02 * B.a20) + (A.a03 * B.a30);

    oodet = 1.0f / det;

    B.a00 *= oodet;
    B.a10 *= oodet;
    B.a20 *= oodet;
    B.a30 *= oodet;

    B.a01 *= oodet;
    B.a11 *= oodet;
    B.a21 *= oodet;
    B.a31 *= oodet;

    B.a02 *= oodet;
    B.a12 *= oodet;
    B.a22 *= oodet;
    B.a32 *= oodet;

    B.a03 *= oodet;
    B.a13 *= oodet;
    B.a23 *= oodet;
    B.a33 *= oodet;

    return B;
}

// double version
mat4 & invertd (mat4& B, const mat4& A)
{
    double det,oodet;

	double b[4][4];

    b[0][0] =  det3x3d(A.a11, A.a21, A.a31, A.a12, A.a22, A.a32, A.a13, A.a23, A.a33);
    b[1][0] = -det3x3d(A.a10, A.a20, A.a30, A.a12, A.a22, A.a32, A.a13, A.a23, A.a33);
    b[2][0] =  det3x3d(A.a10, A.a20, A.a30, A.a11, A.a21, A.a31, A.a13, A.a23, A.a33);
    b[3][0] = -det3x3d(A.a10, A.a20, A.a30, A.a11, A.a21, A.a31, A.a12, A.a22, A.a32);

    b[0][1] = -det3x3d(A.a01, A.a21, A.a31, A.a02, A.a22, A.a32, A.a03, A.a23, A.a33);
    b[1][1] =  det3x3d(A.a00, A.a20, A.a30, A.a02, A.a22, A.a32, A.a03, A.a23, A.a33);
    b[2][1] = -det3x3d(A.a00, A.a20, A.a30, A.a01, A.a21, A.a31, A.a03, A.a23, A.a33);
    b[3][1] =  det3x3d(A.a00, A.a20, A.a30, A.a01, A.a21, A.a31, A.a02, A.a22, A.a32);

    b[0][2] =  det3x3d(A.a01, A.a11, A.a31, A.a02, A.a12, A.a32, A.a03, A.a13, A.a33);
    b[1][2] = -det3x3d(A.a00, A.a10, A.a30, A.a02, A.a12, A.a32, A.a03, A.a13, A.a33);
    b[2][2] =  det3x3d(A.a00, A.a10, A.a30, A.a01, A.a11, A.a31, A.a03, A.a13, A.a33);
    b[3][2] = -det3x3d(A.a00, A.a10, A.a30, A.a01, A.a11, A.a31, A.a02, A.a12, A.a32);

    b[0][3] = -det3x3d(A.a01, A.a11, A.a21, A.a02, A.a12, A.a22, A.a03, A.a13, A.a23);
    b[1][3] =  det3x3d(A.a00, A.a10, A.a20, A.a02, A.a12, A.a22, A.a03, A.a13, A.a23);
    b[2][3] = -det3x3d(A.a00, A.a10, A.a20, A.a01, A.a11, A.a21, A.a03, A.a13, A.a23);
    b[3][3] =  det3x3d(A.a00, A.a10, A.a20, A.a01, A.a11, A.a21, A.a02, A.a12, A.a22);

    det = (b[0][0] * A.a00) + (b[1][0] * A.a01) + (b[2][0] * A.a02) + (b[3][0] * A.a03);

    oodet = 1.0 / det;

	B.a00 = static_cast<float>(b[0][0] * oodet);
	B.a10 = static_cast<float>(b[1][0] * oodet);
	B.a20 = static_cast<float>(b[2][0] * oodet);
	B.a30 = static_cast<float>(b[3][0] * oodet);

	B.a01 = static_cast<float>(b[0][1] * oodet);
	B.a11 = static_cast<float>(b[1][1] * oodet);
	B.a21 = static_cast<float>(b[2][1] * oodet);
	B.a31 = static_cast<float>(b[3][1] * oodet);

	B.a02 = static_cast<float>(b[0][2] * oodet);
	B.a12 = static_cast<float>(b[1][2] * oodet);
	B.a22 = static_cast<float>(b[2][2] * oodet);
	B.a32 = static_cast<float>(b[3][2] * oodet);

	B.a03 = static_cast<float>(b[0][3] * oodet);
	B.a13 = static_cast<float>(b[1][3] * oodet);
	B.a23 = static_cast<float>(b[2][3] * oodet);
	B.a33 = static_cast<float>(b[3][3] * oodet);

    return B;
}

mat4 & invert_rot_trans(mat4& B, const mat4& A)
{
    B.a00 = A.a00;
    B.a10 = A.a01;
    B.a20 = A.a02;
    B.a30 = A.a30;
    B.a01 = A.a10;
    B.a11 = A.a11;
    B.a21 = A.a12;
    B.a31 = A.a31;
    B.a02 = A.a20;
    B.a12 = A.a21;
    B.a22 = A.a22;
    B.a32 = A.a32;
    B.a03 = - (A.a00 * A.a03 + A.a10 * A.a13 + A.a20 * A.a23);
    B.a13 = - (A.a01 * A.a03 + A.a11 * A.a13 + A.a21 * A.a23);
    B.a23 = - (A.a02 * A.a03 + A.a12 * A.a13 + A.a22 * A.a23);
    B.a33 = A.a33;
    return B;
}

mat3 & invert(mat3& B, const mat3& A)
{
    float det,oodet;

    B.a00 =  (A.a11 * A.a22 - A.a21 * A.a12);
    B.a10 = -(A.a10 * A.a22 - A.a20 * A.a12);
    B.a20 =  (A.a10 * A.a21 - A.a20 * A.a11);
    B.a01 = -(A.a01 * A.a22 - A.a21 * A.a02);
    B.a11 =  (A.a00 * A.a22 - A.a20 * A.a02);
    B.a21 = -(A.a00 * A.a21 - A.a20 * A.a01);
    B.a02 =  (A.a01 * A.a12 - A.a11 * A.a02);
    B.a12 = -(A.a00 * A.a12 - A.a10 * A.a02);
    B.a22 =  (A.a00 * A.a11 - A.a10 * A.a01);

    det = (A.a00 * B.a00) + (A.a01 * B.a10) + (A.a02 * B.a20);
    
    oodet = 1.0f / det;

    B.a00 *= oodet; B.a01 *= oodet; B.a02 *= oodet;
    B.a10 *= oodet; B.a11 *= oodet; B.a12 *= oodet;
    B.a20 *= oodet; B.a21 *= oodet; B.a22 *= oodet;
    return B;
}

// by zho
// get [eye, center, up] vectors from view matrix
void look_at_inv (const mat4& M, vec3& eye, vec3& center, vec3& up)
{
	mat4 inv(M);
	//invert(inv, M);
	inv = inv.inverse_affine();
	
	vec4 y(0, 1, 0, 1), z(0, 0, -1, 1), o(0, 0, 0, 1);
	vec4 temp;

	// set eye vector. from origin (0, 0, 0)
	mult(temp, inv, o);
	eye = temp;

	// set center vector.
	mult(temp, inv, z);
	center = vec3(temp);

	// set up vector. up = y' vector - eye
	mult(temp, inv, y); // get the world position of the (0, 1, 0). It's not the vector that we want
	up = vec3(temp) - eye;
	normalize(up);
}

// by zho
quat mat4::get_rotation (void) const 
{
	mat3 m;
	get_rot(m);
	quat q;
	q.from_matrix(m);
	return q;
}

// make orthogonal and look at
// like zz_camera::look_at_ortho()
mat4& look_at_ortho (mat4& m, const vec3& eye, const vec3& center, const vec3& up)
{
	// first make orthogonal up vector
	vec3 new_up, right, direction(center - eye);
	cross(right, direction, up);
	cross(new_up, right, direction);
	return look_at(m, eye, center, new_up);
}


// eye : camera origin -> (0, 0, 0)
// center : on the camera's negative axis -> (0, 0, -1)
// up : up vector -> simplly (0, 0, 1)
mat4 & look_at(mat4& M, const vec3& eye, const vec3& center, const vec3& up)
{
    vec3 x, y, z;

    // make rotation matrix

    // Z vector
    z.x = eye.x - center.x;
    z.y = eye.y - center.y;
    z.z = eye.z - center.z;
    normalize(z);

    // Y vector
    y.x = up.x;
    y.y = up.y;
    y.z = up.z;
	
    // X vector = Y cross Z
    cross(x,y,z);

    // Recompute Y = Z cross X
    cross(y,z,x);

    // cross product gives area of parallelogram, which is < 1.0 for
    // non-perpendicular unit-length vectors; so normalize x, y here
    normalize(x);
    normalize(y);

    M.a00 = x.x; M.a01 = x.y; M.a02 = x.z; M.a03 = -x.x * eye.x - x.y * eye.y - x.z*eye.z;
    M.a10 = y.x; M.a11 = y.y; M.a12 = y.z; M.a13 = -y.x * eye.x - y.y * eye.y - y.z*eye.z;
    M.a20 = z.x; M.a21 = z.y; M.a22 = z.z; M.a23 = -z.x * eye.x - z.y * eye.y - z.z*eye.z;
    M.a30 = .0f; M.a31 = .0f; M.a32 = .0f; M.a33 = 1.0f;
    return M;
}

// [ref] D3DXMatrixPerspectiveOffCenterRH()
mat4 & frustum_d3d (mat4& M, const float l, const float r, const float b, 
               const float t, const float n, const float f)
{
	M.a00 = 2.0f*n/(r-l);
	M.a01 = 0.0f;
	M.a02 = (l+r)/(r-l);
	M.a03 = 0.0f;

	M.a10 = 0.0f;
	M.a11 = 2.0f*n/(t-b);
	M.a12 = (t+b)/(t-b);
	M.a13 = 0.0f;

	M.a20 = 0.0f;
	M.a21 = 0.0f;
	M.a22 = f/(n-f);
	M.a23 = n*f/(n-f);

	M.a30 = 0.0f;
	M.a31 = 0.0f;
	M.a32 = -1.0f;
	M.a33 = 0.0f;

/* from D3DXMatrixPerspectiveOffCenterRH()
2*zn/(r-l)   0            0                0
0            2*zn*(t-b)   0                0
(l+r)/(r-l)  (t+b)/(t-b)  zf/(zn-zf)      -1
0            0            zn*zf/(zn-zf)    0
*/
    return M;
}

// [ref] D3DXMatrixPerspectiveFovRH()
// caution fovy use angle, rather than D3D's radian.
mat4 & perspective_d3d (mat4& M, const float fovy, const float aspect, const float n, const float f)
{

/*
w       0       0                0
0       h       0                0
0       0       zf/(zn-zf)      -1
0       0       zn*zf/(zn-zf)    0
where:
h is the view space height. It is calculated from 
h = cot(fovY/2);

w is the view space width. It is calculated from
w = h / Aspect.

*/

	float h, w;
	float temp = fovy * ZZ_TO_RAD * .5f;
	h = cosf(temp) / sinf(temp);

	w = h / aspect;
	
	M.a00 = w;
	M.a01 = 0.0f;
	M.a02 = 0.0f;
	M.a03 = 0.0f;

	M.a10 = 0.0f;
	M.a11 = h;
	M.a12 = 0.0f;
	M.a13 = 0.0f;

	M.a20 = 0.0f;
	M.a21 = 0.0f;
	M.a22 = f / (n - f);
	M.a23 = n * f / (n - f);

	M.a30 = 0.0f;
	M.a31 = 0.0f;
	M.a32 = -1.0f;
	M.a33 = 0.0f;

	////D3D DOC BUG? w = h / aspect;
	//w = h * aspect;
	//
	//M.a00 = 2.f * n / w;
	//M.a01 = 0.0f;
	//M.a02 = 0.0f;
	//M.a03 = 0.0f;

	//M.a10 = 0.0f;
	//M.a11 = 2.f * n / h;
	//M.a12 = 0.0f;
	//M.a13 = 0.0f;

	//M.a20 = 0.0f;
	//M.a21 = 0.0f;
	//M.a22 = f / (n - f);
	//M.a23 = n * f / (n - f);

	//M.a30 = 0.0f;
	//M.a31 = 0.0f;
	//M.a32 = -1.0f;
	//M.a33 = 0.0f;

	return M;
}

// [ref] D3DXMatrixOrthoRH
mat4 & orthogonal_d3d (mat4 & M, const float w, const float h, const float zn, const float zf)
{
/*
2/w  0    0           0
0    2/h  0           0
0    0    1/(zn-zf)   0
0    0    zn/(zn-zf)  1
*/
	M.a00 = 2.0f / w;
	M.a01 = 0;
	M.a02 = 0;
	M.a03 = 0;

	M.a10 = 0;
	M.a11 = 2.0f / h;
	M.a12 = 0;
	M.a13 = 0;

	M.a20 = 0;
	M.a21 = 0;
	M.a22 = 1.0f / (zn - zf);
	M.a23 = zn * M.a22;

	M.a30 = 0;
	M.a31 = 0;
	M.a32 = 0;
	M.a33 = 1.0f;

	return M;
}

mat4 & frustum(mat4 & M, const float l, const float r, const float b, 
	 const float t, const float n, const float f)
{
#ifdef ZZ_ZRANGE_OGL
	return frustum_ogl(M, l, r, b, t, n, f);
#else
	return frustum_d3d(M, l, r, b, t, n, f);
#endif // ZZ_ZRANGE_OGL
}

// fovy : field of view (degree)
// aspect : view space width divided by height
// n : near plane distance from camera
// f : far plane distance from camera
mat4 & perspective(mat4 & M, const float fovy, const float aspect, const float n, const float f)
{
#ifdef ZZ_ZRANGE_OGL
	return perspective_ogl(M, fovy, aspect, n, f);
#else
	return perspective_d3d(M, fovy, aspect, n, f);
#endif // ZZ_ZRANGE_OGL
}

mat4 & orthogonal(mat4 & M, const float w, const float h, const float zn, const float zf)
{
#ifdef ZZ_ZRANGE_OGL
	return orthogonal_ogl(M, w, h, zn, zf);
#else
	return orthogonal_d3d(M, w, h, zn, zf);
#endif // ZZ_ZRANGE_OGL
}

/*
Forward declarations
*/
quat qlerp (const quat &q1,const quat &q2,float t);
quat qinterpolate (int mode,const quat &q1,const quat &q2,const quat &a,const quat &b,float t);
quat qslerp (const quat &q1,const quat &q2,float t);
quat qsquad (const quat &q1,const quat &q2,const quat &a,const quat &b,float t);
quat qsquad (const quat &q1,const quat &q2,const quat &a,float t);
quat qspline (const quat &q1,const quat &q2,const quat &q3);
quat qexp (const quat &q);
quat qlog (const quat &q);

/*
Logarithm of a quat, given as:
qlog(q) = v*a where q = [cos(a),v*sin(a)]
*/
quat qlog(const quat &q)
{
	float a = static_cast<float>(acos(q.w));
	float sina = static_cast<float>(sin(a));
	quat ret;
	ret.w = 0;
	if (sina > 0)
	{
		ret.x = a*q.x/sina;
		ret.y = a*q.y/sina;
		ret.z = a*q.z/sina;
	}
	else
	{
		ret.x=ret.y=ret.z=0;
	}
	return ret;
}

/*
e^quat given as:
qexp(v*a) = [cos(a),vsin(a)]
*/
quat qexp (const quat &q)
{
	float a = static_cast<float>(sqrt(q.x*q.x + q.y*q.y + q.z*q.z));
	float sina = static_cast<float>(sin(a));
	float cosa = static_cast<float>(cos(a));
	quat ret;

	ret.w = cosa;
	if(a > 0)
	{
		ret.x = sina * q.x / a;
		ret.y = sina * q.y / a;
		ret.z = sina * q.z / a;
	}
	else
	{
		ret.x = ret.y = ret.z = 0;
	}

	return ret;
}


// from [Ken Shoemake]
/* Spherical linear interpolation of unit quaternions.  Takes qu0 to qu1
 * as t goes from 0 to 1. */
//quat qslerp_from_Ken_Shoemake (const quat& q0, const quat& q1, float t)
//{
//	quat qut;
//	quat qu0 = q0, qu1 = q1;
//	float omega, cosOmega, sinOmega, qu0Part, qu1Part;
//	
//	cosOmega = qu0.x*qu1.x + qu0.y*qu1.y + qu0.z*qu1.z + qu0.w*qu1.w;
//
//	if ((1.0 + cosOmega) > ZZ_EPS) {
//		/* Usual case */
//		if ((1.0 - cosOmega) > ZZ_EPS) {
//			/* Usual case */
//			omega = acos(cosOmega);
//			sinOmega = sin(omega);
//			qu0Part = sin((1.0 - t)*omega) / sinOmega;
//			qu1Part = sin(t*omega) / sinOmega;
//		}
//		else { /* Ends very close */
//			qu0Part = 1.0 - t;
//			qu1Part = t;
//		}
//	}
//	else { /* Ends nearly opposite */
//		qu1.x = -qu0.y; qu1.y = qu0.x; qu1.z = -qu0.w; qu1.w = qu0.z;
//		qu0Part = sin((0.5 - t) * ZZ_PI);
//		qu1Part = sin(t * ZZ_PI);
//	}
//	qut = qu0*qu0Part + qu1*qu1Part;
//
//	//qut = Qt_Add(Qt_Scale(qu0, qu0Part), Qt_Scale(qu1, qu1Part));
//	return (qut);
//}


// get squad(:t, q1, q2, a, b) after setup (a, b)
quat qsquad_spline (const quat& q0, const quat& q1, const quat& q2, const quat& q3, float t)
{
	quat ret;
	quat a, b;
	quat q0_new, q2_new, q3_new;

	// from D3D document [D3DXQuaternionSquadSetup]
	//q0 = |Q0 + Q1| < |Q0 - Q1| ? -Q0 : Q0
	//q2 = |Q1 + Q2| < |Q1 - Q2| ? -Q2 : Q2
	//q3 = |Q2 + Q3| < |Q2 - Q3| ? -Q3 : Q3
	float plus, minus;	
	quat q;

	q = q0 + q1;
	plus = q.magnitude();
	q = q0 - q1;
	minus = q.magnitude();
	q0_new = (plus < minus) ? -q0 : q0;
	
	q = q1 + q2;
	plus = q.magnitude();
	q = q1 - q2;
	minus = q.magnitude();
	q2_new = (plus < minus) ? -q2 : q2;
	
	q = q2_new + q3;
	plus = q.magnitude();
	q = q2_new - q3;
	minus = q.magnitude();
	q3_new = (plus < minus) ? -q3 : q3;

	a = qspline(q0_new, q1, q2_new);
	b = qspline(q1, q2_new, q3_new);

	return qsquad(q1, q2_new, a, b, t);
}

// 2003-07-09	zho		added
// 
// get the sliding vector on the ground
// 
// v = resulting sliding vector
// p0/1/2 = three point on the plane
// return = magnitude of the sliding vector.
//          to ignore sliding vector,
//          if it is less than the threshold
/*
p1
 | \
 |  \
 p0--p2
*/
float get_ground_sliding_vector (vec3& v, const vec3& p0, const vec3& p1, const vec3& p2)
{
	vec3 n; // normal vector
	vec3 p01 = p1 - p0; // vector from p0 to p1
	vec3 p02 = p2 - p0; // vector from p0 to p2

	// n = p02 x p01
	cross(n, p02, p01);
	n.normalize();

	// v = -z + (z dot n) n
	// z = (0, 0, 1)
	n *= n.z;
	v.set(n.x, n.y, n.z - 1.0f); // sliding vector
	return v.norm();
}

//float get_fov (const mat4& m)
//{
//#ifdef ZZ_ZRANGE_OGL
//	ZZ_LOG("algebra: no get_fov() function defined\n");
//	return 1.0f;
//#else
//	//near_plane = m.a23 / m.a22;
//	//h = cosf(temp) / sinf(temp);
//	////float temp = fovy * ZZ_TO_RAD * .5f;
//	////m.a11 = 2.f * n / h;
//	////h = 2.0f * n / m.a11;
//	//h = 2.0f * (m.a23 / m.a22) / m.a11;
//	//h = cot(temp);
//	//h = 1 / tan(temp);
//	//tan(temp) = 1 / h;
//	//temp = atan(1 / h);
//	return atan(m.a11 * m.a22 * .5f / m.a23) * 2.0f / ZZ_TO_RAD;
//#endif
//}
//
//void set_fov (mat4& m, float fovy)
//{
//#ifdef ZZ_RANGE_OGL
//	ZZ_LOG("algebra: no get_fov() function defined\n");
//#else
//	float aspect = m.a11 / m.a00;
//	float temp = fovy * ZZ_TO_RAD * .5f;
//	float new_h = cosf(temp) / sinf(temp);
//	//D3D DOC BUG? w = h / aspect;
//	float new_w = new_h * aspect;
//	// calc	
//	M.a00 = 2.f * n / w;
//	M.a11 = 2.f * n / h;
//#endif
//}

//M.a00 = 2.f * n / w;
//M.a11 = 2.f * n / h;
//M.a22 = f / (n - f);
//M.a23 = n * f / (n - f);
