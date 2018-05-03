/** 
 * @file zz_misc.h
 * @brief miscellaneous functions.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-feb-2002
 *
 * $Header: /engine/include/zz_misc.h 2     04-02-28 5:44p Zho $
 * $History: zz_misc.h $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-28   Time: 5:44p
 * Updated in $/engine/include
 * riva tnt2 enabled.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
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
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef	__ZZ_MISC_H__
#define __ZZ_MISC_H__

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif

#ifndef __ZZ_TYPE_H__
#include "zz_type.h"
#endif

int random_number(int imin, int imax);
float random_number(float fmin, float fmax);

inline vec3 random_number (vec3 vmin, vec3 vmax)
{
  float x = random_number(vmin.x, vmax.x);
  float y = random_number(vmin.y, vmax.y);
  float z = random_number(vmin.z, vmax.z);
  return(vec3(x,y,z));
}

inline vec4 random_number (vec4 vmin, vec4 vmax)
{
  float x = random_number(vmin.x, vmax.x);
  float y = random_number(vmin.y, vmax.y);
  float z = random_number(vmin.z, vmax.z);
  float w = random_number(vmin.w, vmax.w);
  return(vec4(x, y, z, w));
}


//////////////////////////////////////////////////////////////////////////////
///
/// Fast log2 ( Get smallest n for 2^n > X, Get biggest n for 2^n < X
///
//////////////////////////////////////////////////////////////////////////////
// All of these functions assume sizeof(unsigned) == 4 bytes.

// Find the smallest bit mask greater than or equal to n.
// This can also be stated as finding the smallest non-negative integer x
// such that pow(2,x) > n and returning pow(2,x)-1.

inline uint32 bitmask ( uint32 n )
{
	if ( n == 0 )
		return 0;

	uint32 mask = 0xffffffff;

	uint32 t = 0xffff;
	if ( t >= n )
		mask = t;

	t = mask >> 8;
	if ( t >= n )
		mask = t;

	t = mask >> 4;
	if ( t >= n )
		mask = t;

	t = mask >> 2;
	if ( t >= n )
		mask = t;

	t = mask >> 1;
	if ( t >= n )
		mask = t;

	return mask;
}

// Find the largest non-negative integer x such that pow(2,x) <= n.
// The exception is n=0, which returns 0.
inline uint32 log2le ( uint32 n )
{
	uint32 t, log2;

	if ( n >= 0x10000 )
	{
		log2 = 16;
		t = 0x1000000;
	}
	else
	{
		log2 = 0;
		t = 0x100;
	}

	if ( n >= t )
	{
		log2 += 8;
		t <<= 4;
	}
	else
	{
		t >>= 4;
	}

	if ( n >= t )
	{
		log2 += 4;
		t <<= 2;
	}
	else
	{
		t >>= 2;
	}

	if ( n >= t )
	{
		log2 += 2;
		t <<= 1;
	}
	else
	{
		t >>= 1;
	}

	if ( n >= t )
	{
		log2 += 1;
	}

	return log2;
}

// Find the smallest non-negative integer x such that pow(2,x) >= n.
inline uint32 log2ge ( uint32 n )
{
	if ( n > 0x80000000 )
		return 32;

	uint32 t, log2;

	if ( n > 0x8000 )
	{
		log2 = 16;
		t = 0x800000;
	}
	else
	{
		log2 = 0;
		t = 0x80;
	}

	if ( n > t )
	{
		log2 += 8;
		t <<= 4;
	}
	else
	{
		t >>= 4;
	}

	if ( n > t )
	{
		log2 += 4;
		t <<= 2;
	}
	else
	{
		t >>= 2;
	}

	if ( n > t )
	{
		log2 += 2;
		t <<= 1;
	}
	else
	{
		t >>= 1;
	}

	if ( n > t )
	{
		log2 += 1;
	}

	return log2;
}

#endif // __ZZ_MISC_H__
