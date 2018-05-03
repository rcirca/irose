/*
	$Header: /7HeartsOnline/Server/Sho_gs/Sho_gs_lib/ETC_Math.cpp 1     04-03-25 11:20a Icarus $
*/

#include "stdAFX.h"
#include "ETC_Math.h"


//-------------------------------------------------------------------------------------------------
float	distance (tPOINTF &From, tPOINTF &To)
{
    tPOINTF from_to;
    from_to.x = To.x - From.x;
    from_to.y = To.y - From.y;
    return float( sqrt(from_to.x*from_to.x + from_to.y*from_to.y ) );
}

float	distance_square (tPOINTF &From, tPOINTF &To)
{
    tPOINTF from_to;
    from_to.x = To.x - From.x;
    from_to.y = To.y - From.y;
    return ( from_to.x*from_to.x + from_to.y*from_to.y );
}

int		distance (int iX1, int iY1, int iX2, int iY2)
{
    // ¿ÀÂ÷ 3.5%
    iX1 = abs( iX2-iX1 );
    iY1 = abs( iY2-iY1 );

    int mn = min(iX1, iY1);
    return (iX1+iY1-(mn>>1)-(mn>>2)+(mn>>4));
}

//-------------------------------------------------------------------------------------------------
void quat_2_euler_ogl (const quat& q, float& yaw, float& pitch, float& roll)
{
	float sqw = q.w*q.w;
	float sqx = q.x*q.x;
	float sqy = q.y*q.y;
	float sqz = q.z*q.z;

/*
	pitch = asinf (2.0f * (q.y*q.z + q.w*q.x)); // rotation about x-axis
	roll  = atan2f(2.0f * (q.w*q.y - q.x*q.z),(-sqx - sqy + sqz + sqw)); // rotation about y-axis
	yaw   = atan2f(2.0f * (q.w*q.z - q.x*q.y),(-sqx + sqy - sqz + sqw)); // rotation about z-axis
*/
	pitch = asin  (2.0f * (q.y*q.z + q.w*q.x)); // rotation about x-axis
	roll  = atan2 (2.0f * (q.w*q.y - q.x*q.z),(-sqx - sqy + sqz + sqw)); // rotation about y-axis
	yaw   = atan2 (2.0f * (q.w*q.z - q.x*q.y),(-sqx + sqy - sqz + sqw)); // rotation about z-axis
}

#define ZZ_PI		float(3.14159265358979323846264338327950288419716939937510582)
#define ZZ_TO_DEG	float(180) / ZZ_PI
float quaternionToModelDirection ( float fQuatW, float fQuatX, float fQuatY, float fQuatZ )
{
	quat q(fQuatX, fQuatY, fQuatZ, fQuatW);
	float yaw, pitch, roll;
	quat_2_euler_ogl(q, yaw, pitch, roll);
	return ZZ_TO_DEG * yaw;
}


//-------------------------------------------------------------------------------------------------
