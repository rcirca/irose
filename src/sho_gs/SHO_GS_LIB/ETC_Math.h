/*
	$Header: /7HeartsOnline/Server/SHO_GS/Sho_gs_lib/ETC_Math.h 2     04-04-14 10:17a Icarus $
*/
#ifndef	__ETC_MATH_H
#define	__ETC_MATH_H
#pragma warning (disable:4201)
//-------------------------------------------------------------------------------------------------

float	distance (tPOINTF &From, tPOINTF &To);
float	distance_square (tPOINTF &From, tPOINTF &To);
int		distance (int iX1, int iY1, int iX2, int iY2);


//-----------------------------------------------
// quat
//-----------------------------------------------
class quat {
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
/*
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
*/
	union {
		struct {
			float x, y, z, w;
		};
		float comp[4];
	};
};

// quaternion
/*
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
*/

void  quat_2_euler_ogl (const quat& q, float& yaw, float& pitch, float& roll); // by zho
float quaternionToModelDirection ( float fQuatW, float fQuatX, float fQuatY, float fQuatZ );

//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)
#endif