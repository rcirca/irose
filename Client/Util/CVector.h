#ifndef	__CVECTOR_H
#define	__CVECTOR_H
#include <math.h>
//-------------------------------------------------------------------------------------------------

class CVec2 {
public :
	float	x, y;

	float	Length ()		{	return (float)::sqrt(x*x + y*y);					}
	void	Normalize ()	{	float fL=Length();	if (fL!=0.0f)	x/=fL,y/=fL;	}

	bool   operator==(const CVec2 & v1)
	{
		return (v1.x == x && v1.y == y) ? true : false;
	}

	CVec2& operator*=(const float & fScale)
	{
		x*= fScale;
		y*= fScale;
		return *this;
	}

	CVec2& operator-=(const CVec2 & v1)
	{
		x-= v1.x;
		y-= v1.y;
		return *this;
	}

	CVec2& operator+=(const CVec2 & v1)
	{
		x+= v1.x;
		y+= v1.y;
		return *this;
	}
} ;
inline const CVec2 operator+(const CVec2& v1, const CVec2& v2)
{
	CVec2 Vec;
	Vec.x = v1.x + v2.x;
	Vec.y = v1.y + v2.y;
	return Vec;
}

inline const CVec2 operator-(const CVec2& v1, const CVec2& v2)
{
	CVec2 Vec;
	Vec.x = v1.x - v2.x;
	Vec.y = v1.y - v2.y;
	return Vec;
}

inline const CVec2 operator*(const float fScale, const CVec2& v1)
{
	CVec2 Vec;
	Vec.x = fScale * v1.x;
	Vec.y = fScale * v1.y;
	return Vec;
}
inline const CVec2 operator*(const CVec2& v1, const float fScale)
{
	CVec2 Vec;
	Vec.x = v1.x * fScale;
	Vec.y = v1.y * fScale;
	return Vec;
}

inline const CVec2 operator/(const CVec2& v1, const float fScale)
{
	CVec2 Vec;
	Vec.x = v1.x / fScale;
	Vec.y = v1.y / fScale;
	return Vec;
}

inline const CVec2 operator*(const CVec2&v1, const CVec2&v2)
{
	CVec2 Vec;
	Vec.x = v1.x * v2.x;
	Vec.y = v1.y * v2.y;
	return Vec;
}

//-------------------------------------------------------------------------------------------------
#endif