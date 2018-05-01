/**
 * @file zz_algebra.h
 * @brief algebra classes and functions
 * @author Jiho Choi(zho@popsmail.com)
 * @version 2.0
 * @date 12-jun-2002
 *
 * $Header: /engine/include/zz_algebra.h 6     04-09-01 2:59p Zho $
 * $History: zz_algebra.h $
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-09-01   Time: 2:59p
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-05-19   Time: 8:11p
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-04-17   Time: 2:56p
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-06   Time: 1:23p
 * Updated in $/engine/include
 * Added double version.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-07   Time: 9:44p
 * Updated in $/engine/include
 * Small optimization in get_angle()
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:05p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:53p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:47p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:19p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 19  *****************
 * User: Zho          Date: 03-11-26   Time: 5:18p
 * Updated in $/znzin11/engine/include
 * added uivec2 class
 *
 */

// Copyright (C) 1999, 2000 NVIDIA Corporation
// Copyright (C) 2000, Jason Shankel
// modified by Jiho Choi

#ifndef __ZZ_ALGEBRA_H__
#define __ZZ_ALGEBRA_H__

#include <math.h>
#include <memory.h> // for memcpy

#define ZZ_HALF_PI            float(3.14159265358979323846264338327950288419716939937510582 * 0.5)
#define ZZ_PI			      float(3.14159265358979323846264338327950288419716939937510582)
#define ZZ_TWO_PI			  float(3.14159265358979323846264338327950288419716939937510582 * 2.0)
#define ZZ_OO_PI			  1.0f / ZZ_PI
#define ZZ_OO_TWO_PI	      1.0f / ZZ_TWO_PI
#define ZZ_OO_255   	      1.0f / 255.f
#define ZZ_OO_128   	      1.0f / 128.f
#define ZZ_TO_RAD             ZZ_PI / 180.f
#define ZZ_TO_DEG             float(180) / ZZ_PI
#define ZZ_EPS		          float(10e-6)
#define ZZ_DOUBLE_EPS	      float(10e-6) * 2.0f
#define ZZ_BIG_EPS            float(10e-6)
#define ZZ_SMALL_EPS          float(10e-2)

#define ZZ_SCURVE(_x_)		(float(3.0f*(_x_)*(_x_) - 2.0f*(_x_)*(_x_)*(_x_)))
#define ZZ_SINUSOID(_x_)	(float(sin(ZZ_PI*((_x_) - .5f))))

#define ZZ_ZRANGE_D3D        // use z-depth [0, 1] in Direct3D, now being used in frustum() and perspective()
#undef ZZ_ZRANGE_OGL         // use z-depth [-1, 1] in OpenGL, now being used in frustum() and perspective()

#define FABS(_x_)  (((_x_) >= 0) ? (_x_) : -1.0f*(_x_))

//--------------------------------------------------------------------------------
// vector 2 component
//--------------------------------------------------------------------------------
struct vec2
{
    union {
        struct {
            float x,y;          // standard names for components
        };
        float vec_array[2];     // array access
    };

	vec2() { }
    vec2(float x, float y) : x(x), y(y) { }
    vec2(const float* xy) : x(xy[0]), y(xy[1]) { }
	vec2(const vec2& u) : x(u.x), y(u.y) { }

	bool operator==(const vec2 & u)
	{
		return (u.x == x && u.y == y) ? true : false;
	}

	vec2& operator*=(const float & lambda)
	{
		x*= lambda;
		y*= lambda;
		return *this;
	}

	// following 2 lines by zho
	vec2& operator-=(const float f) { x-= f; y-= f; return *this; }
	vec2& operator+=(const float f) { x+= f; y+= f; return *this; }

	vec2& operator-=(const vec2 & u)
	{
		x-= u.x;
		y-= u.y;
		return *this;
	}

	vec2& operator+=(const vec2 & u)
	{
		x+= u.x;
		y+= u.y;
		return *this;
	}

	float& operator[](int i)
	{
		return vec_array[i];
	}

	const float operator[](int i) const
	{
		return vec_array[i];
	}

	// by zho
	void set (float x, float y)
	{
		this->x = x;
		this->y = y;
	}

    //bool operator==(const vec2 & u);
    //vec2 & operator*=(const float & lambda);
    //vec2 & operator-=(const vec2 & u);
    //vec2 & operator+=(const vec2 & u);
    //float & operator[](int i);
    //const float operator[](int i) const;
};

//--------------------------------------------------------------------------------
// vec2
//--------------------------------------------------------------------------------
inline const vec2 operator+(const vec2& u, const vec2& v)
{
	return vec2(u.x + v.x, u.y + v.y);
}

inline const vec2 operator-(const vec2& u, const vec2& v)
{
    return vec2(u.x - v.x, u.y - v.y);
}

inline const vec2 operator*(const float s, const vec2& u)
{
	return vec2(s * u.x, s * u.y);
}

inline const vec2 operator/(const vec2& u, const float s)
{
	return vec2(u.x / s, u.y / s);
}

inline const vec2 operator*(const vec2&u, const vec2&v)
{
	return vec2(u.x * v.x, u.y * v.y);
}

//-----------------------------------------------------------------------------------------------------------------
// unsigned integer vector 2
//-----------------------------------------------------------------------------------------------------------------
struct uivec2
{
	union {
        struct {
            unsigned int x,y;
        };
        unsigned int uint_array[2];     // array access
    };

	uivec2() { }
    uivec2(unsigned int x, unsigned int y) : x(x), y(y) { }
    uivec2(const unsigned int* xyz) : x(xyz[0]), y(xyz[1]) { }
	uivec2(const uivec2& u) : x(u.x), y(u.y) { }

	unsigned int & operator[](int i)
	{
		return uint_array[i];
	}

	const unsigned int operator[](int i) const
	{
		return uint_array[i];
	}
};


//--------------------------------------------------------------------------------
// vector 3 component
//--------------------------------------------------------------------------------
struct vec4;
struct vec3
{
	union {
        struct {
            float x, y, z;
        };
        float vec_array[3];
    };
	vec3 () { }
    vec3 (float x, float y, float z) : x(x), y(y), z(z) { }
	vec3 (const float* xyz) : x(xyz[0]), y(xyz[1]), z(xyz[2]) { }
	vec3 (const vec3& u) : x(u.x), y(u.y), z(u.z) { }
	vec3 (const vec4& u);

	bool operator==(const vec3& u) const
	{
		return (u.x == x && u.y == y && u.z == z) ? true : false;
	}

	bool operator!=(const vec3& rhs) const
	{
		return !(*this == rhs );
	}

	// added by zho
	// vector-vector multiplication. It means .* operation in MATLAB
	vec3& operator*=(const vec3 & scale)
	{
		x *= scale.x;
		y *= scale.y;
		z *= scale.z;
		return *this;
	}

	vec3& operator*=(const float & lambda)
	{
		x*= lambda;
		y*= lambda;
		z*= lambda;
		return *this;
	}

	// following 2 lines by zho
	vec3& operator-=(const float f) { x-= f; y-= f; z-= f; return *this; }
	vec3& operator+=(const float f) { x+= f; y+= f; z+= f; return *this; }

	vec3 operator - () const
	{
		return vec3(-x, -y, -z);
	}

	vec3& operator-=(const vec3 & u)
	{
		x-= u.x;
		y-= u.y;
		z-= u.z;
		return *this;
	}

	vec3& operator+=(const vec3 & u)
	{
		x+= u.x;
		y+= u.y;
		z+= u.z;
		return *this;
	}

	float normalize()
	{
		float norm = sqrtf(x * x + y * y + z * z);
		if (norm > ZZ_EPS)
			norm = 1.0f / norm;
		else
			norm = .0f;
		x *= norm;
		y *= norm;
		z *= norm;
		return norm;
	}

	float sq_norm() const
	{
		return x * x + y * y + z * z;
	}

	float norm() const
	{
		return sqrtf(sq_norm());
	}

	float & operator[](int i)
	{
		return vec_array[i];
	}

	const float operator[](int i) const
	{
		return vec_array[i];
	}

	// computes the cross product ( v cross w) and stores the result in u
	// i.e.     u = v cross w
	vec3& cross(const vec3 & v, const vec3 & w)
	{
		x = v.y*w.z - v.z*w.y;
		y = v.z*w.x - v.x*w.z;
		z = v.x*w.y - v.y*w.x;
		return *this;
	}

	// by zho
	float distance (const vec3& to) const
	{
		vec3 from_to(to.x - this->x, to.y - this->y, to.z - this->z);
		return float(sqrt(from_to.x*from_to.x + from_to.y*from_to.y + from_to.z*from_to.z));
	}

	// by zho
	float distance_square (const vec3& to) const
	{
		vec3 from_to(to.x - this->x, to.y - this->y, to.z - this->z);
		return (from_to.x*from_to.x + from_to.y*from_to.y + from_to.z*from_to.z);
	}

	// by zho
	void set (float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

 //   bool operator==(const vec3& u) const;
 //   bool operator!=(const vec3& rhs) const;
	//vec3& operator*=(const float & lambda);
 //   vec3 operator - () const;
 //   vec3 & operator-=(const vec3 & u);
 //   vec3 & operator+=(const vec3 & u);
	//float normalize();
	//float sq_norm() const;
	//float norm() const;
 //   float & operator[](int i);
 //   const float operator[](int i) const;
	//vec3 & cross(const vec3 & v, const vec3 & w);
};

inline const vec3 operator+(const vec3& u, const vec3& v)
{
	return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline const vec3 operator-(const vec3& u, const vec3& v)
{
    return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline const vec3 operator+(const float s, const vec3& v)
{
	return vec3(s + v.x, s + v.y, s + v.z);
}

inline const vec3 operator-(const float s, const vec3& v)
{
	return vec3(s - v.x, s - v.y, s - v.z);
}

inline const vec3 operator^(const vec3& u, const vec3& v)
{
    return vec3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

inline const vec3 operator*(const float s, const vec3& u)
{
	return vec3(s * u.x, s * u.y, s * u.z);
}

inline const vec3 operator/(const vec3& u, const float s)
{
	return vec3(u.x / s, u.y / s, u.z / s);
}

inline const vec3 operator*(const vec3& u, const vec3& v)
{
	return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

// by zho
inline const vec3 operator/(const vec3& u, const vec3& v)
{
	return vec3(u.x / v.x, u.y / v.y, u.z / v.z);
}

//--------------------------------------------------------------------------------
// vector 4 component
//--------------------------------------------------------------------------------
struct vec4
{
	union {
        struct {
            float x,y,z,w;
        };
        struct {
            float r,g,b,a;
        };
		float vec_array[4];     // array access
    };
	vec4() { }
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
    vec4(const float* xyzw) : x(xyzw[0]), y(xyzw[1]), z(xyzw[2]), w(xyzw[3]) { }
	vec4(const vec3& u) : x(u.x), y(u.y), z(u.z), w(0.0f) { }
	vec4(const vec4& u) : x(u.x), y(u.y), z(u.z), w(u.w) { }

	bool operator==(const vec4 & u) const
	{
		return (u.x == x && u.y == y && u.z == z && u.w == w) ? true : false;
	}

	bool operator!=(const vec4& rhs) const
	{
		return !(*this == rhs );
	}

	vec4& operator*=(const float & lambda)
	{
		x*= lambda;
		y*= lambda;
		z*= lambda;
		w*= lambda;
		return *this;
	}

	vec4& operator-=(const vec4 & u)
	{
		x-= u.x;
		y-= u.y;
		z-= u.z;
		w-= u.w;
		return *this;
	}

	vec4& operator+=(const vec4 & u)
	{
		x+= u.x;
		y+= u.y;
		z+= u.z;
		w+= u.w;
		return *this;
	}

	vec4 operator - () const
	{
		return vec4(-x, -y, -z, -w);
	}

	float & operator[](int i)
	{
		return vec_array[i];
	}

	const float operator[](int i) const
	{
		return vec_array[i];
	}

	// by zho
	void set (float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

    //bool operator==(const vec4 & u) const;
    //bool operator!=(const vec4& rhs) const;
    //vec4& operator*=(const float & lambda);
    //vec4 & operator-=(const vec4 & u);
    //vec4 & operator+=(const vec4 & u);
    //vec4 operator - () const;
    //float& operator[](int i);
    //const float operator[](int i) const;
};

inline const vec4 operator+(const vec4& u, const vec4& v)
{
	return vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}

inline const vec4 operator-(const vec4& u, const vec4& v)
{
	return vec4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
}

inline const vec4 operator+(const float s, const vec4& u)
{
	return vec4(s + u.x, s + u.y, s + u.z, s + u.w);
}

inline const vec4 operator-(const float s, const vec4& u)
{
	return vec4(s - u.x, s - u.y, s - u.z, s - u.w);
}

inline const vec4 operator*(const float s, const vec4& u)
{
	return vec4(s * u.x, s * u.y, s * u.z, s * u.w);
}

inline const vec4 operator/(const vec4& u, const vec4& v)
{
	return vec4(u.x / v.x, u.y / v.y, u.z / v.z, u.w / v.w);
}

// by zho
inline const vec4 operator/(const vec4& u, const float s)
{
	return vec4(u.x / s, u.y / s, u.z / s, u.w / s);
}

inline const vec4 operator*(const vec4& u, const vec4& v)
{
	return vec4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w);
}

struct ivec3
{
	union {
        struct {
            int x,y,z;
        };
        int int_array[3];     // array access
    };

	ivec3() { }
    ivec3(int x, int y, int z) : x(x), y(y), z(z) { }
    ivec3(const int* xyz) : x(xyz[0]), y(xyz[1]), z(xyz[2]) { }
	ivec3(const ivec3& u) : x(u.x), y(u.y), z(u.z) { }
	
	int& operator[](int i)
	{
		return int_array[i];
	}

	const int operator[](int i) const
	{
		return int_array[i];
	}
};

struct usvec3
{
	union {
        struct {
            unsigned short x,y,z;
        };
        unsigned short ushort_array[3];     // array access
    };
	usvec3() { }
    usvec3(unsigned short x, unsigned short y, unsigned short z) : x(x), y(y), z(z) { }
    usvec3(const unsigned short* xyz) : x(xyz[0]), y(xyz[1]), z(xyz[2]) { }
	usvec3(const usvec3& u) : x(u.x), y(u.y), z(u.z) { }
	unsigned short & operator[](int i)
	{
		return ushort_array[i];
	}
	const unsigned short operator[](int i) const
	{
		return ushort_array[i];
	}
};

struct uivec3
{
	union {
        struct {
            unsigned int x,y,z;
        };
        unsigned int uint_array[3];     // array access
    };

	uivec3() { }
    uivec3(unsigned int x, unsigned int y, unsigned int z) : x(x), y(y), z(z) { }
    uivec3(const unsigned int* xyz) : x(xyz[0]), y(xyz[1]), z(xyz[2]) { }
	uivec3(const uivec3& u) : x(u.x), y(u.y), z(u.z) { }

	unsigned int & operator[](int i)
	{
		return uint_array[i];
	}

	const unsigned int operator[](int i) const
	{
		return uint_array[i];
	}
};

struct ivec4
{
	union {
        struct {
            int x,y,z,w;
        };
        int int_array[4];     // array access
    };

	ivec4() { }
    ivec4(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) { }
    ivec4(const int* xyzw) : x(xyzw[0]), y(xyzw[1]), z(xyzw[2]), w(xyzw[3]) { }
	ivec4(const ivec4& u) : x(u.x), y(u.y), z(u.z), w(u.w) { }

	int & operator[](int i)
	{
		return int_array[i];
	}

	const int operator[](int i) const
	{
		return int_array[i];
	}
};

struct uivec4
{
	union {
        struct {
            unsigned int x,y,z,w;
        };
        unsigned int uint_array[3];     // array access
    };

	uivec4() { }
    uivec4(unsigned int x, unsigned int y, unsigned int z, unsigned int w) :
		x(x), y(y), z(z), w(w) { }
    uivec4(const int* xyzw) : x(xyzw[0]), y(xyzw[1]), z(xyzw[2]), w(xyzw[3]) { }
	uivec4(const uivec4& u) : x(u.x), y(u.y), z(u.z), w(u.w) { }

	unsigned int & operator[](int i)
	{
		return uint_array[i];
	}

	const unsigned int operator[](int i) const
	{
		return uint_array[i];
	}
};


struct usvec4
{
	union {
        struct {
            unsigned short x,y,z,w;
        };
        unsigned short ushort_array[3];     // array access
    };

	usvec4() { }
    usvec4(unsigned short x, unsigned short y, unsigned short z, unsigned short w) :
		x(x), y(y), z(z), w(w) { }
    usvec4(const short * xyzw) : x(xyzw[0]), y(xyzw[1]), z(xyzw[2]), w(xyzw[3]) { }
	usvec4(const usvec4& u) : x(u.x), y(u.y), z(u.z), w(u.w) { }

	unsigned short & operator[](int i)
	{
		return ushort_array[i];
	}

	const unsigned short operator[](int i) const
	{
		return ushort_array[i];
	}
};

//--------------------------------------------------------------------------------
// mat3
//--------------------------------------------------------------------------------
/*
    for all the matrices...a<x><y> indicates the element at row x, col y

    For example:
    a01 <-> row 0, col 1 
*/
// quaternion
struct quat; 
struct mat3
{
	union {
        struct {
            float a00, a10, a20;        // standard names for components
            float a01, a11, a21;        // standard names for components
            float a02, a12, a22;        // standard names for components
        };
		struct {
			float _11, _21, _31;
			float _12, _22, _32;
			float _13, _23, _33;
		};
        float mat_array[9];     // array access
    };
    
	mat3()
	{
	}

	mat3(const float* array)
	{
		memcpy(mat_array, array, sizeof(float) * 9);
	}

	mat3(const mat3 & M)
	{
		memcpy(mat_array, M.mat_array, sizeof(float) * 9);
	}

	mat3(const quat& q);
	
	mat3& operator=(const quat& q);

	const vec3 col(const int i) const
	{
		return vec3(&mat_array[i * 3]);
	}

	const vec3 operator[](int i) const
	{
		return vec3(mat_array[i], mat_array[i + 3], mat_array[i + 6]);
	}

	
	const float& operator()(const int& i, const int& j) const
	{
		return mat_array[ j * 3 + i ];
	}

	float& operator()(const int& i, const int& j)
	{
		return  mat_array[ j * 3 + i ];
	}

	void set_row(int i, const vec3 & v)
	{
		mat_array[i] = v.x;
		mat_array[i + 3] = v.y;
		mat_array[i + 6] = v.z;
	}

	void set_col(int i, const vec3 & v)
	{
		mat_array[i * 3] = v.x;
		mat_array[i * 3 + 1] = v.y;
		mat_array[i * 3 + 2] = v.z;
	}

    void set_rot(const float & theta_radian, const vec3 & v);
    void set_rot(const vec3 & u, const vec3 & v);

	mat3& mult_scale (const vec3& scale);
	mat3 transpose ();
};

inline const vec3 operator*(const mat3& M, const vec3& v)
{
	vec3 u;
    u.x = M.a00 * v.x + M.a01 * v.y + M.a02 * v.z;
    u.y = M.a10 * v.x + M.a11 * v.y + M.a12 * v.z;
    u.z = M.a20 * v.x + M.a21 * v.y + M.a22 * v.z;
    return u;
}

inline mat3 operator*(const mat3& A, const mat3& B)
{
	mat3 C;
	
	C.a00 = A.a00 * B.a00 + A.a01 * B.a10 + A.a02 * B.a20;
    C.a10 = A.a10 * B.a00 + A.a11 * B.a10 + A.a12 * B.a20;
    C.a20 = A.a20 * B.a00 + A.a21 * B.a10 + A.a22 * B.a20;
    C.a01 = A.a00 * B.a01 + A.a01 * B.a11 + A.a02 * B.a21;
    C.a11 = A.a10 * B.a01 + A.a11 * B.a11 + A.a12 * B.a21;
    C.a21 = A.a20 * B.a01 + A.a21 * B.a11 + A.a22 * B.a21;
    C.a02 = A.a00 * B.a02 + A.a01 * B.a12 + A.a02 * B.a22;
    C.a12 = A.a10 * B.a02 + A.a11 * B.a12 + A.a12 * B.a22;
    C.a22 = A.a20 * B.a02 + A.a21 * B.a12 + A.a22 * B.a22;

	return C;
}

inline vec3 & cross(vec3 & u, const vec3 & v, const vec3 & w)
{
    u.x = v.y*w.z - v.z*w.y;
    u.y = v.z*w.x - v.x*w.z;
    u.z = v.x*w.y - v.y*w.x;
    return u;
}

inline float dot(const vec3& v, const vec3& w)
{
	float u;
    u = v.x*w.x + v.y*w.y + v.z*w.z;
    return u;
}

inline float dot(const vec4& v, const vec4& w)
{
	float u;
    u = v.x*w.x + v.y*w.y + v.z*w.z + v.w*w.w;
    return u;
}

inline float & dot(float& u, const vec3& v, const vec3& w)
{
    u = v.x*w.x + v.y*w.y + v.z*w.z;
    return u;
}

inline float & dot(float& u, const vec4& v, const vec4& w)
{
    u = v.x*w.x + v.y*w.y + v.z*w.z + v.w*w.w;
    return u;
}

// by zho
inline const float & get_angle(float& angle_radian, const vec3& from, const vec3& to)
{
	float dot_product;
	dot(dot_product, from, to);
	
	const float EPSILON = 0.0000001f;
	const float EPSILON_POSITIVE = 1.0f - EPSILON;
	const float EPSILON_NEGATIVE = -1.0f + EPSILON;

	if (dot_product > EPSILON_POSITIVE) {
		angle_radian = 0;
	}
	else if (dot_product < EPSILON_NEGATIVE) {
		angle_radian = ZZ_PI;
	}
	else {
		angle_radian = acosf(dot_product);
	}
	return angle_radian;
}

//--------------------------------------------------------------------------------
// mat4
//--------------------------------------------------------------------------------
struct mat4
{
	union {
        struct {
            float a00, a10, a20, a30;   // standard names for components
            float a01, a11, a21, a31;   // standard names for components
            float a02, a12, a22, a32;   // standard names for components
            float a03, a13, a23, a33;   // standard names for components
        };
        struct {
            float _11, _21, _31, _41;   // standard names for components
            float _12, _22, _32, _42;   // standard names for components
            float _13, _23, _33, _43;   // standard names for components
            float _14, _24, _34, _44;   // standard names for components
        };
        union {
            struct {
                float b00, b10, b20, p; // standard names for components
                float b01, b11, b21, q; // standard names for components
                float b02, b12, b22, r; // standard names for components
                float x, y, z, w;       // standard names for components
            };
        };
        float mat_array[16];     // array access
    };

	mat4()
	{
	}

	mat4(const float* array)
	{
		memcpy(mat_array, array, sizeof(float) * 16);
	}

	mat4(const mat4& M)
	{
		memcpy(mat_array, M.mat_array, sizeof(float) * 16);
	}

    mat4( const float& f0,  const float& f1,  const float& f2,  const float& f3,
  		  const float& f4,  const float& f5,  const float& f6,  const float& f7,
  		  const float& f8,  const float& f9,  const float& f10, const float& f11,
  		  const float& f12, const float& f13, const float& f14, const float& f15 )
  		  : a00( f0 ), a10( f4 ), a20( f8 ), a30( f12),
            a01( f1 ), a11( f5 ), a21( f9 ), a31( f13),
            a02( f2 ), a12( f6 ), a22( f10), a32( f14),
            a03( f3 ), a13( f7 ), a23( f11), a33( f15)
	{
	}

	mat4( const vec3& pos, const quat& rot );

	mat4(const mat3& M);

	const vec4 col(const int i) const
	{
		return vec4(&mat_array[i * 4]);
	}

	const vec4 operator[](const int& i) const
	{
		return vec4(mat_array[i], mat_array[i + 4], mat_array[i + 8], mat_array[i + 12]);
	}

	const float& operator()(const int& i, const int& j) const
	{
		return mat_array[ j * 4 + i ];
	}

	float& operator()(const int& i, const int& j)
	{
		return  mat_array[ j * 4 + i ];
	}

	void set_col(int i, const vec4 & v)
	{
		mat_array[i] = v.x;
		mat_array[i + 1] = v.y;
		mat_array[i + 2] = v.z;
		mat_array[i + 3] = v.w;
	}

	void set_row(int i, const vec4 & v)
	{
		mat_array[i] = v.x;
		mat_array[i + 4] = v.y;
		mat_array[i + 8] = v.z;
		mat_array[i + 12] = v.w;
	}

	mat3 & get_rot(mat3& M) const
	{
		// assign the 3x3 rotation block
		M.a00 = a00; M.a10 = a10; M.a20 = a20;
		M.a01 = a01; M.a11 = a11; M.a21 = a21;
		M.a02 = a02; M.a12 = a12; M.a22 = a22;
		return M;
	}

	
	void set_rot(const mat3& M)
	{
		// copy the 3x3 rotation block
		a00 = M.a00; a10 = M.a10; a20 = M.a20;
		a01 = M.a01; a11 = M.a11; a21 = M.a21;
		a02 = M.a02; a12 = M.a12; a22 = M.a22;
	}

	void set_translation(const vec3& t)
	{
		a03 = t.x;
		a13 = t.y;
		a23 = t.z;
	}

	vec3 & get_translation(vec3& t) const
	{
		t.x = a03;
		t.y = a13;
		t.z = a23;
		return t;
	}

	vec3 get_position (void) const
	{
		return vec3(a03, a13, a23);
	}

	quat get_rotation (void) const;

    quat & get_rot(quat & q) const;
    void set_rot(const quat & q);
    void set_rot(const float & theta_radian, const vec3 & v);
    void set_rot(const vec3 & u, const vec3 & v);

	void set(const vec3& pos, const quat& rot);

    mat4 operator*(const mat4&) const;
	void set_identity (void);
	mat4 inverse();
	mat4 inverse_affine(); // fast inverse. no scale applied

	mat4& mult_scale (const vec3& scale);
};

inline vec3::vec3 (const vec4& u) : x(u.x), y(u.y), z(u.z) { }

//--------------------------------------------------------------------------------
// quat
//--------------------------------------------------------------------------------
struct quat {
public:
	quat(float x = 0, float y = 0, float z = 0, float w = 1) : x(x), y(y), z(z), w(w)
	{
	}

	quat(const quat& quat)
	{
		x = quat.x;
		y = quat.y;
		z = quat.z;
		w = quat.w;
	}

	quat(const vec3& axis, float angle)
	{
		float len = axis.norm();
		if (len) {
			float invLen = 1 / len;
			float angle2 = angle / 2;
			float scale = sinf(angle2) * invLen;
			x = scale * axis[0];
			y = scale * axis[1];
			z = scale * axis[2];
			w = cosf(angle2);
		}
	}

	quat(const mat3& rot);
	
	void from_matrix(const mat3& mat);
	
	quat& operator=(const quat& q);
	quat& operator=(const mat3& mat);

	const quat inverse();
	void normalize();

	// rewrite this to compute directly
	void set_rot(const vec3& u, const vec3& v);

	void to_matrix(mat3& mat) const;
	quat& operator*=(const quat& quat);
	static const quat identity;
	float& operator[](int i) { return comp[i]; }
	float magnitude () { return (x*x + y*y + z*z + w*w); }
	const float operator[](int i) const { return comp[i]; }
	union {
		struct {
			float x, y, z, w;
		};
		float comp[4];
	};
};

// quaternion
quat & normalize(quat & p);

inline const quat quat::inverse()
{
	return quat(- x, - y, - z, w);
}

inline void quat::normalize()
{
	float len = sqrtf(x * x + y * y + z * z + w * w);
	if (len > 0) {
		float invLen = 1 / len;
		x *= invLen;
		y *= invLen;
		z *= invLen;
		w *= invLen;
	}
}

inline const quat operator*(const quat& p, const quat& q)
{
	return quat(
		p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y,
		p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z,
		p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x,
		p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z
	);
}

inline quat& quat::operator*=(const quat& quat)
{
	*this = *this * quat;
	return *this;
}

inline quat & conj(quat & p)
{
    p.x = -p.x;
    p.y = -p.y;
    p.z = -p.z;
    return p;
}

inline quat & conj(quat& p, const quat& q)
{
    p.x = -q.x;
    p.y = -q.y;
    p.z = -q.z;
    p.w = q.w;
    return p;
}

quat & add_quats(quat & p, const quat & q1, const quat & q2);
quat & axis_to_quat(quat & q, const vec3 & a, const float phi);
mat3 & quat_2_mat(mat3 &M, const quat &q );
quat & mat_2_quat(quat &q, const mat3 &M);
quat & mat_2_quat(quat &q, const mat4 &M);
quat & euler_2_quat_d3d (quat &q, const float yaw, const float pitch, const float roll); // by zho
quat & euler_2_quat_ogl (quat &q, const float yaw, const float pitch, const float roll); // by zho
void quat_2_euler_d3d (const quat& q, float& yaw, float& pitch, float& roll); // by zho
void quat_2_euler_ogl (const quat& q, float& yaw, float& pitch, float& roll); // by zho

const quat operator*(const quat& p, const quat& q);

quat operator * (const quat& q,float v);
quat operator *(float v,const quat &q);
quat operator /(const quat &q,float v);
quat operator -(const quat &q);
quat operator -(const quat &q1,const quat &q2);
quat operator +(const quat &q1,const quat &q2);
quat qlog (const quat &q);
quat qexp (const quat &q);
quat qlerp (const quat &q1,const quat &q2,float t);
quat qslerp (const quat &q1,const quat &q2,float t);
quat qslerp_noinvert (const quat &q1,const quat &q2,float t);
quat qsquad (const quat &q1,const quat &q2,const quat &a,const quat &b,float t);
quat qspline (const quat &qnm1,const quat &qn,const quat &qnp1);
quat qsquad_spline (const quat& q0, const quat& q1, const quat& q2, const quat& q3, float t);

//--------------------------------------------------------------------------------
// etc
//--------------------------------------------------------------------------------

// constant algebraic values
const float array16_id[] =        { 1.0f, .0f, .0f, .0f,
                                     .0f,1.0f, .0f, .0f,
                                     .0f, .0f,1.0f, .0f,
                                     .0f, .0f, .0f,1.0f};

const float array16_null[] =      { .0f, .0f, .0f, .0f,
                                    .0f, .0f, .0f, .0f,
                                    .0f, .0f, .0f, .0f,
                                    .0f, .0f, .0f, .0f};

const float array16_scale_bias[] = { .5f, .0f, .0f, .0f,
                                     .0f, .5f, .0f, .0f,
                                     .0f, .0f, .5f, .0f,
                                     .5f, .5f, .5f,1.0f};

const float array9_id[] =         { 1.0f, .0f, .0f,
                                     .0f,1.0f, .0f,
                                     .0f, .0f,1.0f};


const vec2      vec2_null(.0f,.0f);
const vec4      vec4_one(1.0f,1.0f,1.0f,1.0f);
const vec3      vec3_one(1.0f,1.0f,1.0f);
const vec3      vec3_null(.0f,.0f,.0f);
const vec3      vec3_x(1.0f,.0f,.0f);
const vec3      vec3_y(.0f,1.0f,.0f);
const vec3      vec3_z(.0f,.0f,1.0f);
const vec3      vec3_neg_x(-1.0f,.0f,.0f);
const vec3      vec3_neg_y(.0f,-1.0f,.0f);
const vec3      vec3_neg_z(.0f,.0f,-1.0f);
const vec4      vec4_null(.0f,.0f,.0f,.0f);
const vec4      vec4_x(1.0f,.0f,.0f,.0f);
const vec4      vec4_y(.0f,1.0f,.0f,.0f);
const vec4      vec4_z(.0f,.0f,1.0f,.0f);
const vec4      vec4_w(.0f,.0f,.0f,1.0f);
const quat      quat_id(.0f,.0f,.0f,1.0f);
const mat4      mat4_id(array16_id);
const mat3      mat3_id(array9_id);
const mat4      mat4_null(array16_null);
const mat4      mat4_scale_bias(array16_scale_bias);

const ivec3     ivec3_null(0, 0, 0);
const ivec4     ivec4_null(0, 0, 0, 0);
const uivec3    uivec3_null(0, 0, 0);
const uivec4    uivec4_null(0, 0, 0, 0);

// Computes u = v * lambda + u
extern vec3 & madd(vec3 & u, const vec3 & v, const float & lambda);
// Computes u = v * lambda
extern vec3 & mult(vec3 & u, const vec3 & v, const float & lambda);
// Computes u = v * w
extern vec3 & mult(vec3 & u, const vec3 & v, const vec3 & w);
// Computes u = v + w
extern vec3 & add(vec3 & u, const vec3 & v, const vec3 & w);
// Computes u = v - w
extern vec3 & sub(vec3 & u, const vec3 & v, const vec3 & w);

// Computes u = u * s
extern  vec3 & scale(vec3 & u, const float s);
extern  vec4 & scale(vec4 & u, const float s);

// Computes u = M * v
extern  vec3 & mult(vec3 & u, const mat3 & M, const vec3 & v);
extern  vec3 & mult(vec3 & u, const mat4 & M, const vec3 & v);
extern  vec4 & mult(vec4 & u, const mat4 & M, const vec4 & v);

// Computes u = M(3x3) * v and devides by w
extern  vec3 & mult_pos(vec3 & u, const mat4 & M, const vec3 & v);
// Computes u = M(3x3) * v
extern  vec3 & mult_dir(vec3 & u, const mat4 & M, const vec3 & v);


// Computes A += B
extern mat4 & add(mat4 & A, const mat4 & B);
extern mat3 & add(mat3 & A, const mat3 & B);

// Computes C = A * B
extern mat4 & mult(mat4 & C, const mat4 & A, const mat4 & B);
extern mat3 & mult(mat3 & C, const mat3 & A, const mat3 & B);
extern mat4 & multd(mat4& C, const mat4& A, const mat4& B); // double precision version
// Computes B = Transpose(A)
//       T
//  B = A
extern mat3 & transpose(mat3 & B, const mat3 & A);
extern mat4 & transpose(mat4 & B, const mat4 & A);
extern mat3 & transpose(mat3 & B);
extern mat4 & transpose(mat4 & B);

// Computes B = inverse(A)
//       -1
//  B = A
extern mat4 & invert(mat4 & B, const mat4 & A);
extern mat3 & invert(mat3 & B, const mat3 & A);
extern mat4 & invertd (mat4& B, const mat4& A); // double precision version

// Computes B = inverse(A)
//                                       T  T
//                   (R t)             (R -R t)
// assuming that A = (0 1) so that B = (0    1)
//  B = A
mat4 & invert_rot_trans(mat4 & B, const mat4 & A);

// fast inverse with no scale
inline mat4 mat4::inverse_affine()
{
	mat4 inv_mat;
	return invert_rot_trans(inv_mat, *this);
}

void look_at_inv (const mat4& M, vec3& eye, vec3& center, vec3& up);

// make orthogonal up vector and do look_at()
mat4 & look_at_ortho (mat4& m, const vec3& eye, const vec3& center, const vec3& up);

mat4 & look_at(mat4 & M, const vec3 & eye, const vec3 & center, const vec3 & up);

mat4 & frustum(mat4 & M, const float l, const float r, const float b, 
	 const float t, const float n, const float f);

mat4 & perspective(mat4 & M, const float fovy, const float aspect, const float n, const float f);

mat4 & orthogonal(mat4 & M, const float w, const float h, const float zn, const float zf);

quat & trackball(quat & q, vec2 & pt1, vec2 & pt2, float trackballsize);

// surface properties
mat3 & tangent_basis(mat3 & basis,const vec3 & v0,const vec3 & v1,const vec3 & v2,const vec2 & t0,const vec2 & t1,const vec2 & t2, const vec3 & n);

// linear interpolation
inline float lerp(float t, float a, float b)
{ return a * (1.0f - t) + t * b; }

inline vec3 & lerp(vec3 & w, const float & t, const vec3 & u, const vec3 & v)
{ w.x = lerp(t, u.x, v.x); w.y = lerp(t, u.y, v.y); w.z = lerp(t, u.z, v.z); return w; }

// normalizes a vector and return a reference of itself
extern  vec3 & normalize(vec3 & u);
extern vec4 & normalize(vec4 & u);

extern vec3 & cross(vec3 & u, const vec3 & v, const vec3 & w);

extern float catmull_rom (float p0, float p1, float p2, float p3, float t);

extern vec3 catmull_rom (const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float t);

inline float catmull_rom (float p0, float p1, float p2, float p3, float t)
{
	float ret;
	ret = (-.5f*t*t*t + t*t - .5f*t)*p0 +
		(1.5f*t*t*t - 2.5f*t*t + 1.0f)*p1 +
		(-1.5f*t*t*t + 2.0f*t*t + 0.5f*t)*p2 +
		(.5f*t*t*t - .5f*t*t)*p3;
	return ret;
}

inline vec3 catmull_rom (const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float t)
{
	vec3 ret;

	ret = (-.5f*t*t*t + t*t - .5f*t)*p0 +
		(1.5f*t*t*t - 2.5f*t*t + 1.0f)*p1 +
		(-1.5f*t*t*t + 2.0f*t*t + 0.5f*t)*p2 +
		(.5f*t*t*t - .5f*t*t)*p3;
	return ret;
}

float get_ground_sliding_vector (vec3& v, const vec3& p0, const vec3& p1, const vec3& p2);

/*
from [shankel, GPG 1]
Spherical linear interpolation between two quats
*/
inline quat qslerp (const quat &q1,const quat &q2,float t)
{
	quat q3;
	float dot;
	dot = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;

	/*
	dot = cos(theta)
	if (dot < 0), q1 and q2 are more than 90 degrees apart,
	so we can invert one to reduce spinning
	*/
	if (dot < 0)
	{
		dot = -dot;
		q3 = -1*q2;
	}
	else
	{
		q3 = q2;
	}

	
	if (dot < 0.95f)
	{
		float angle = static_cast<float>(acos(dot));
		float sina,sinat,sinaomt;
		sina = static_cast<float>(sin(angle));
		sinat = static_cast<float>(sin(angle*t));
		sinaomt = static_cast<float>(sin(angle*(1-t)));
		return (q1*sinaomt+q3*sinat)/sina;
	}
	/*
	if the angle is small, use linear interpolation
	*/
	else
	{
		return qlerp(q1,q3,t);
	}
}

/*
This version of slerp, used by squad, does not check for theta > 90.
*/

inline quat qslerp_noinvert(const quat &q1,const quat &q2,float t)
{
	float dot = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;

	if (dot > -0.95f && dot < 0.95f)
	{
		float angle = static_cast<float>(acos(dot));
		float sina,sinat,sinaomt;
		sina = static_cast<float>(sin(angle));
		sinat = static_cast<float>(sin(angle*t));
		sinaomt = static_cast<float>(sin(angle*(1-t)));
		return (q1*sinaomt+q2*sinat)/sina;
	}
	/*
	if the angle is small, use linear interpolation
	*/
	else
	{
		return qlerp(q1,q2,t);
	}
}


/*
Spherical cubic interpolation
*/
inline quat qsquad(const quat &q1,const quat &q2,const quat &a,const quat &b,float t)
{
	quat c,d;
	c = qslerp(q1,q2,t);
	d = qslerp(a,b,t);
	return qslerp(c,d,2.0f*t*(1.0f-t));

	//c = qslerp_noinvert(q1,q2,t);
	//d = qslerp_noinvert(a,b,t);
	//return qslerp_noinvert(c,d,2*t*(1-t));
}

/*
Given 3 quats, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
*/

inline quat qspline(const quat &qnm1,const quat &qn,const quat &qnp1)
{
	quat qni;
	
	qni.x = -qn.x;
	qni.y = -qn.y;
	qni.z = -qn.z;
	qni.w = qn.w;

	return qn*qexp((qlog(qni*qnm1)+qlog(qni*qnp1))/-4);
}

inline vec3 & madd(vec3 & u, const vec3& v, const float& lambda)
{
    u.x += v.x*lambda;
    u.y += v.y*lambda;
    u.z += v.z*lambda;
    return u;
}

inline vec3 & mult(vec3 & u, const vec3& v, const float& lambda)
{
    u.x = v.x*lambda;
    u.y = v.y*lambda;
    u.z = v.z*lambda;
    return u;
}

inline vec3 & mult(vec3 & u, const vec3& v, const vec3& w)
{
    u.x = v.x*w.x;
    u.y = v.y*w.y;
    u.z = v.z*w.z;
    return u;
}

inline vec3 & sub(vec3 & u, const vec3& v, const vec3& w)
{
    u.x = v.x - w.x;
    u.y = v.y - w.y;
    u.z = v.z - w.z;
    return u;
}

inline vec3 & add(vec3 & u, const vec3& v, const vec3& w)
{
    u.x = v.x + w.x;
    u.y = v.y + w.y;
    u.z = v.z + w.z;
    return u;
}

inline vec3 & scale(vec3& u, const float s)
{
    u.x *= s;
    u.y *= s;
    u.z *= s;
    return u;
}

inline vec4 & scale(vec4& u, const float s)
{
    u.x *= s;
    u.y *= s;
    u.z *= s;
    u.w *= s;
    return u;
}

inline vec3 & mult(vec3& u, const mat3& M, const vec3& v)
{
    u.x = M.a00 * v.x + M.a01 * v.y + M.a02 * v.z;
    u.y = M.a10 * v.x + M.a11 * v.y + M.a12 * v.z;
    u.z = M.a20 * v.x + M.a21 * v.y + M.a22 * v.z;
    return u;
}

inline vec4 & mult(vec4& u, const mat4& M, const vec4& v)
{
    u.x = M.a00 * v.x + M.a01 * v.y + M.a02 * v.z + M.a03 * v.w;
    u.y = M.a10 * v.x + M.a11 * v.y + M.a12 * v.z + M.a13 * v.w;
    u.z = M.a20 * v.x + M.a21 * v.y + M.a22 * v.z + M.a23 * v.w;
    u.w = M.a30 * v.x + M.a31 * v.y + M.a32 * v.z + M.a33 * v.w;
    return u;
}

inline vec3 & mult(vec3 & u, const mat4 & M, const vec3 & v)
{
	float w;
	u.x = M.a00 * v.x + M.a01 * v.y + M.a02 * v.z + M.a03;
    u.y = M.a10 * v.x + M.a11 * v.y + M.a12 * v.z + M.a13;
    u.z = M.a20 * v.x + M.a21 * v.y + M.a22 * v.z + M.a23;
	w = M.a30 * v.x + M.a31 * v.y + M.a32 * v.z + M.a33;
    u = u / w;
    return u;
}

inline const vec4 operator*(const mat4& M, const vec4& v)
{
	vec4 u;
    u.x = M.a00 * v.x + M.a01 * v.y + M.a02 * v.z + M.a03 * v.w;
    u.y = M.a10 * v.x + M.a11 * v.y + M.a12 * v.z + M.a13 * v.w;
    u.z = M.a20 * v.x + M.a21 * v.y + M.a22 * v.z + M.a23 * v.w;
    u.w = M.a30 * v.x + M.a31 * v.y + M.a32 * v.z + M.a33 * v.w;
    return u;
}

inline vec3 & mult_pos(vec3& u, const mat4& M, const vec3& v)
{
    float oow = 1.0f / (v.x * M.a30 + v.y * M.a31 + v.z * M.a32 + M.a33);
    u.x = (M.a00 * v.x + M.a01 * v.y + M.a02 * v.z + M.a03) * oow;
    u.y = (M.a10 * v.x + M.a11 * v.y + M.a12 * v.z + M.a13) * oow;
    u.z = (M.a20 * v.x + M.a21 * v.y + M.a22 * v.z + M.a23) * oow;
    return u;
}

inline vec3 & mult_dir(vec3& u, const mat4& M, const vec3& v)
{
    u.x = M.a00 * v.x + M.a01 * v.y + M.a02 * v.z;
    u.y = M.a10 * v.x + M.a11 * v.y + M.a12 * v.z;
    u.z = M.a20 * v.x + M.a21 * v.y + M.a22 * v.z;
    return u;
}

inline mat4 & add(mat4& A, const mat4& B)
{
    A.a00 += B.a00;
    A.a10 += B.a10;
    A.a20 += B.a20;
    A.a30 += B.a30;
    A.a01 += B.a01;
    A.a11 += B.a11;
    A.a21 += B.a21;
    A.a31 += B.a31;
    A.a02 += B.a02;
    A.a12 += B.a12;
    A.a22 += B.a22;
    A.a32 += B.a32;
    A.a03 += B.a03;
    A.a13 += B.a13;
    A.a23 += B.a23;
    A.a33 += B.a33;
    return A;
}

inline mat3 & add(mat3& A, const mat3& B)
{
    A.a00 += B.a00;
    A.a10 += B.a10;
    A.a20 += B.a20;
    A.a01 += B.a01;
    A.a11 += B.a11;
    A.a21 += B.a21;
    A.a02 += B.a02;
    A.a12 += B.a12;
    A.a22 += B.a22;
    return A;
}

// C = A * B

// C.a00 C.a01 C.a02 C.a03   A.a00 A.a01 A.a02 A.a03   B.a00 B.a01 B.a02 B.a03
//                                                                            
// C.a10 C.a11 C.a12 C.a13   A.a10 A.a11 A.a12 A.a13   B.a10 B.a11 B.a12 B.a13
//                                                                         
// C.a20 C.a21 C.a22 C.a23   A.a20 A.a21 A.a22 A.a23   B.a20 B.a21 B.a22 B.a23  
//                                                                            
// C.a30 C.a31 C.a32 C.a33 = A.a30 A.a31 A.a32 A.a33 * B.a30 B.a31 B.a32 B.a33

inline mat4 & mult(mat4& C, const mat4& A, const mat4& B)
{
    C.a00 = A.a00 * B.a00 + A.a01 * B.a10 + A.a02 * B.a20 + A.a03 * B.a30;
    C.a10 = A.a10 * B.a00 + A.a11 * B.a10 + A.a12 * B.a20 + A.a13 * B.a30;
    C.a20 = A.a20 * B.a00 + A.a21 * B.a10 + A.a22 * B.a20 + A.a23 * B.a30;
    C.a30 = A.a30 * B.a00 + A.a31 * B.a10 + A.a32 * B.a20 + A.a33 * B.a30;
    C.a01 = A.a00 * B.a01 + A.a01 * B.a11 + A.a02 * B.a21 + A.a03 * B.a31;
    C.a11 = A.a10 * B.a01 + A.a11 * B.a11 + A.a12 * B.a21 + A.a13 * B.a31;
    C.a21 = A.a20 * B.a01 + A.a21 * B.a11 + A.a22 * B.a21 + A.a23 * B.a31;
    C.a31 = A.a30 * B.a01 + A.a31 * B.a11 + A.a32 * B.a21 + A.a33 * B.a31;
    C.a02 = A.a00 * B.a02 + A.a01 * B.a12 + A.a02 * B.a22 + A.a03 * B.a32;
    C.a12 = A.a10 * B.a02 + A.a11 * B.a12 + A.a12 * B.a22 + A.a13 * B.a32;
    C.a22 = A.a20 * B.a02 + A.a21 * B.a12 + A.a22 * B.a22 + A.a23 * B.a32;
    C.a32 = A.a30 * B.a02 + A.a31 * B.a12 + A.a32 * B.a22 + A.a33 * B.a32;
    C.a03 = A.a00 * B.a03 + A.a01 * B.a13 + A.a02 * B.a23 + A.a03 * B.a33;
    C.a13 = A.a10 * B.a03 + A.a11 * B.a13 + A.a12 * B.a23 + A.a13 * B.a33;
    C.a23 = A.a20 * B.a03 + A.a21 * B.a13 + A.a22 * B.a23 + A.a23 * B.a33;
    C.a33 = A.a30 * B.a03 + A.a31 * B.a13 + A.a32 * B.a23 + A.a33 * B.a33;
	return C;
}

inline mat4 & multd(mat4& C, const mat4& A, const mat4& B)
{
    C.a00 = (float)((double)A.a00 * (double)B.a00 + (double)A.a01 * (double)B.a10 + (double)A.a02 * (double)B.a20 + (double)A.a03 * (double)B.a30);
    C.a10 = (float)((double)A.a10 * (double)B.a00 + (double)A.a11 * (double)B.a10 + (double)A.a12 * (double)B.a20 + (double)A.a13 * (double)B.a30);
    C.a20 = (float)((double)A.a20 * (double)B.a00 + (double)A.a21 * (double)B.a10 + (double)A.a22 * (double)B.a20 + (double)A.a23 * (double)B.a30);
    C.a30 = (float)((double)A.a30 * (double)B.a00 + (double)A.a31 * (double)B.a10 + (double)A.a32 * (double)B.a20 + (double)A.a33 * (double)B.a30);
    C.a01 = (float)((double)A.a00 * (double)B.a01 + (double)A.a01 * (double)B.a11 + (double)A.a02 * (double)B.a21 + (double)A.a03 * (double)B.a31);
    C.a11 = (float)((double)A.a10 * (double)B.a01 + (double)A.a11 * (double)B.a11 + (double)A.a12 * (double)B.a21 + (double)A.a13 * (double)B.a31);
    C.a21 = (float)((double)A.a20 * (double)B.a01 + (double)A.a21 * (double)B.a11 + (double)A.a22 * (double)B.a21 + (double)A.a23 * (double)B.a31);
    C.a31 = (float)((double)A.a30 * (double)B.a01 + (double)A.a31 * (double)B.a11 + (double)A.a32 * (double)B.a21 + (double)A.a33 * (double)B.a31);
    C.a02 = (float)((double)A.a00 * (double)B.a02 + (double)A.a01 * (double)B.a12 + (double)A.a02 * (double)B.a22 + (double)A.a03 * (double)B.a32);
    C.a12 = (float)((double)A.a10 * (double)B.a02 + (double)A.a11 * (double)B.a12 + (double)A.a12 * (double)B.a22 + (double)A.a13 * (double)B.a32);
    C.a22 = (float)((double)A.a20 * (double)B.a02 + (double)A.a21 * (double)B.a12 + (double)A.a22 * (double)B.a22 + (double)A.a23 * (double)B.a32);
    C.a32 = (float)((double)A.a30 * (double)B.a02 + (double)A.a31 * (double)B.a12 + (double)A.a32 * (double)B.a22 + (double)A.a33 * (double)B.a32);
    C.a03 = (float)((double)A.a00 * (double)B.a03 + (double)A.a01 * (double)B.a13 + (double)A.a02 * (double)B.a23 + (double)A.a03 * (double)B.a33);
    C.a13 = (float)((double)A.a10 * (double)B.a03 + (double)A.a11 * (double)B.a13 + (double)A.a12 * (double)B.a23 + (double)A.a13 * (double)B.a33);
    C.a23 = (float)((double)A.a20 * (double)B.a03 + (double)A.a21 * (double)B.a13 + (double)A.a22 * (double)B.a23 + (double)A.a23 * (double)B.a33);
    C.a33 = (float)((double)A.a30 * (double)B.a03 + (double)A.a31 * (double)B.a13 + (double)A.a32 * (double)B.a23 + (double)A.a33 * (double)B.a33);
	return C;
}

// C = A * B

// C.a00 C.a01 C.a02   A.a00 A.a01 A.a02   B.a00 B.a01 B.a02
//                                                          
// C.a10 C.a11 C.a12   A.a10 A.a11 A.a12   B.a10 B.a11 B.a12
//                                                          
// C.a20 C.a21 C.a22 = A.a20 A.a21 A.a22 * B.a20 B.a21 B.a22

inline mat3 & mult(mat3& C, const mat3& A, const mat3& B)
{
    C.a00 = A.a00 * B.a00 + A.a01 * B.a10 + A.a02 * B.a20;
    C.a10 = A.a10 * B.a00 + A.a11 * B.a10 + A.a12 * B.a20;
    C.a20 = A.a20 * B.a00 + A.a21 * B.a10 + A.a22 * B.a20;
    C.a01 = A.a00 * B.a01 + A.a01 * B.a11 + A.a02 * B.a21;
    C.a11 = A.a10 * B.a01 + A.a11 * B.a11 + A.a12 * B.a21;
    C.a21 = A.a20 * B.a01 + A.a21 * B.a11 + A.a22 * B.a21;
    C.a02 = A.a00 * B.a02 + A.a01 * B.a12 + A.a02 * B.a22;
    C.a12 = A.a10 * B.a02 + A.a11 * B.a12 + A.a12 * B.a22;
    C.a22 = A.a20 * B.a02 + A.a21 * B.a12 + A.a22 * B.a22;
    return C;
}


inline mat3 & transpose(mat3& A)
{
    float tmp;
    tmp = A.a01;
    A.a01 = A.a10;
    A.a10 = tmp;

    tmp = A.a02;
    A.a02 = A.a20;
    A.a20 = tmp;

    tmp = A.a12;
    A.a12 = A.a21;
    A.a21 = tmp;
    return A;
}

inline mat4 & transpose(mat4& A)
{
    float tmp;
    tmp = A.a01;
    A.a01 = A.a10;
    A.a10 = tmp;

    tmp = A.a02;
    A.a02 = A.a20;
    A.a20 = tmp;

    tmp = A.a03;
    A.a03 = A.a30;
    A.a30 = tmp;

    tmp = A.a12;
    A.a12 = A.a21;
    A.a21 = tmp;

    tmp = A.a13;
    A.a13 = A.a31;
    A.a31 = tmp;

    tmp = A.a23;
    A.a23 = A.a32;
    A.a32 = tmp;
    return A;
}


/*
    calculate the determinent of a 2x2 matrix in the from

    | a1 a2 |
    | b1 b2 |

*/
inline float det2x2(float a1, float a2, float b1, float b2)
{
    return a1 * b2 - b1 * a2;
}

inline double det2x2d(double a1, double a2, double b1, double b2)
{
    return a1 * b2 - b1 * a2;
}

/*
    calculate the determinent of a 3x3 matrix in the from

    | a1 a2 a3 |
    | b1 b2 b3 |
    | c1 c2 c3 |

*/
inline float det3x3(float a1, float a2, float a3, 
                         float b1, float b2, float b3, 
                         float c1, float c2, float c3)
{
    return a1 * det2x2(b2, b3, c2, c3) - b1 * det2x2(a2, a3, c2, c3) + c1 * det2x2(a2, a3, b2, b3);
}

inline double det3x3d(double a1, double a2, double a3, 
                         double b1, double b2, double b3, 
                         double c1, double c2, double c3)
{
    return a1 * det2x2d(b2, b3, c2, c3) - b1 * det2x2d(a2, a3, c2, c3) + c1 * det2x2d(a2, a3, b2, b3);
}


inline float det(const mat3& A)
{
    return det3x3(A.a00, A.a01, A.a02, 
                 A.a10, A.a11, A.a12, 
                 A.a20, A.a21, A.a22);
}


inline vec3 & normalize(vec3& u)
{
    float norm = sqrtf(u.x * u.x + u.y * u.y + u.z * u.z);
    if (norm > ZZ_EPS)
        norm = 1.0f / norm;
    else
        norm = .0f;
    return scale(u,norm); 
}

inline vec4 & normalize(vec4& u)
{
    float norm = sqrtf(u.x * u.x + u.y * u.y + u.z * u.z + u.w * u.w);
    if (norm > ZZ_EPS)
        norm = 1.0f / norm;
    else
        norm = .0f;
    return scale(u,norm); 
}

inline quat & normalize(quat & p)
{
    float norm = sqrtf(p.x * p.x + p.y * p.y + p.z * p.z + p.w * p.w);
    if (norm > ZZ_EPS)
        norm = 1.0f / norm;
    else
        norm = .0f;
    p.x *= norm;
    p.y *= norm;
    p.z *= norm;
    p.w *= norm;
    return p; 
}

mat4 & frustum_d3d (mat4& M, const float l, const float r, const float b, 
               const float t, const float n, const float f);

inline mat4 & frustum_ogl (mat4& M, const float l, const float r, const float b, 
               const float t, const float n, const float f)
{
    M.a00 = (2.0f*n) / (r-l);
    M.a10 = 0.0;
    M.a20 = 0.0;
    M.a30 = 0.0;

    M.a01 = 0.0;
    M.a11 = (2.0f*n) / (t-b);
    M.a21 = 0.0;
    M.a31 = 0.0;

    M.a02 = (r+l) / (r-l);
    M.a12 = (t+b) / (t-b);
    M.a22 = -(f+n) / (f-n);
    M.a32 = -1.0f;

    M.a03 = 0.0;
    M.a13 = 0.0;
    M.a23 = -(2.0f*f*n) / (f-n);
    M.a33 = 0.0;
    return M;
}

inline mat4 & perspective_ogl (mat4& M, const float fovy, const float aspect, const float n, const float f)
{
    float xmin, xmax, ymin, ymax;

    ymax = n * tanf(fovy * ZZ_TO_RAD * .5f);
    ymin = -ymax;

    xmin = ymin * aspect;
    xmax = ymax * aspect;

    return frustum_d3d(M, xmin, xmax, ymin, ymax, n, f);
}


inline quat operator * (const quat& q,float v)
{
	quat ret;
	ret.x = q.x*v;
	ret.y = q.y*v;
	ret.z = q.z*v;
	ret.w = q.w*v;
	return ret;
}

inline quat operator *(float v,const quat &q)
{
	return q*v;
}

inline quat operator /(const quat &q,float v)
{
	quat ret;
	ret.x = q.x/v;
	ret.y = q.y/v;
	ret.z = q.z/v;
	ret.w = q.w/v;
	return ret;
}

// get negative of quaternion 
// by zho
inline quat operator -(const quat &q)
{
	quat ret;
	ret.x = -q.x;
	ret.y = -q.y;
	ret.z = -q.z;
	ret.w = -q.w;
	return ret;
}

inline quat operator -(const quat &q1,const quat &q2)
{
	quat ret;
	ret.x = q1.x-q2.x;
	ret.y = q1.y-q2.y;
	ret.z = q1.z-q2.z;
	ret.w = q1.w-q2.w;
	return ret;
}

inline quat operator +(const quat &q1,const quat &q2)
{
	quat ret;
	ret.x = q1.x+q2.x;
	ret.y = q1.y+q2.y;
	ret.z = q1.z+q2.z;
	ret.w = q1.w+q2.w;
	return ret;
}


/*
Linear interpolation between two quats
*/
inline quat qlerp (const quat &q1,const quat &q2,float t)
{
	quat ret;
	ret = q1 + t*(q2-q1);
	/*
		return value must be normalized
	*/
	return normalize(ret);
}

#endif // __ZZ_ALGEBRA_H__