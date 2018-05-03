/*
	$Header: /Client/util/CD3DUtil.h 10    04-02-14 5:29p Zho $
*/
#ifndef	__CD3DUTIL_H
#define	__CD3DUTIL_H
//-------------------------------------------------------------------------------------------------
///
/// Sprite와 거리관련 Wrapper Class 
///
class CD3DUtil {
public :	
	
public  :
	static bool		Init ();
	static void		Free ();
	
	static bool		IntersectTriangle( const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2, FLOAT* t, FLOAT* u, FLOAT* v );


	/** 
	 * 두 점사이의 거리를 구함.
	 * @param From 시작점
	 * @param To 끝점
	 * @see distance_square
	 */
	static float	distance (D3DVECTOR &From, D3DVECTOR &To)
	{
		D3DXVECTOR3 from_to( To.x - From.x, To.y - From.y, To.z - From.z);
		return float( sqrt(from_to.x*from_to.x + from_to.y*from_to.y + from_to.z*from_to.z) );
	}

	/*!
		두 점사이의 거리의 제곱을 구함
		\param From 시작점
		\param To 끝점
		\see distance
	*/
	static float	distance_square (D3DVECTOR &From, D3DVECTOR &To)
	{
		D3DXVECTOR3 from_to( To.x - From.x, To.y - From.y, To.z - From.z);
		return (from_to.x*from_to.x + from_to.y*from_to.y + from_to.z*from_to.z);
	}

	/*!
		두 점사이의 거리를 구함
		\param iX1 시작점 X좌표
		\param iY1 시작점 Y좌표
		\param iX2 끝점 X좌표
		\param iY2 끝점 Y좌표
	*/
	static int		distance (int iX1, int iY1, int iX2, int iY2) 
	{
		// 오차 3.5%
		iX1 = abs( iX2-iX1 );
		iY1 = abs( iY2-iY1 );

		int mn = min(iX1, iY1);
		return (iX1+iY1-(mn>>1)-(mn>>2)+(mn>>4));
	}

	/*!
		벡터의 거리를 구함
		\param iX X좌표
		\param iY Y좌표
	*/
	static int length (int iX, int iY)
	{
		iX = (iX < 0) ? -iX : iX;
		iY = (iY < 0) ? -iY : iY;
		int mn = min(iX, iY);
		return (iX+iY-(mn>>1)-(mn>>2)+(mn>>4));
	}
} ;

//-------------------------------------------------------------------------------------------------
#endif
