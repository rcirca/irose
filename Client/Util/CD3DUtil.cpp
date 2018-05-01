/*
	$Header: /Client/Util/CD3DUtil.cpp 4     03-12-15 10:22a Jeddli $
*/
#include "stdAFX.h"
#include "CD3DUtil.h"

//-------------------------------------------------------------------------------------------------
bool CD3DUtil::Init ()
{
	return true;
}
void CD3DUtil::Free ()
{
}

//-----------------------------------------------------------------------------
// Name: IntersectTriangle()
// Desc: Given a ray origin (orig) and direction (dir), and three vertices of
//       of a triangle, this function returns TRUE and the interpolated texture
//       coordinates if the ray intersects the triangle
//-----------------------------------------------------------------------------
bool CD3DUtil::IntersectTriangle( const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, 
						D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                        FLOAT* t, FLOAT* u, FLOAT* v )
{
    // Find vectors for two edges sharing vert0
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    D3DXVECTOR3 tvec;
    if( det > 0 ) {
        tvec = orig - v0;
    } else {
        tvec = v0 - orig;
        det = -det;
    }

    if( det < 0.0001f )
        return false;

    // Calculate U parameter and test bounds
    *u = D3DXVec3Dot( &tvec, &pvec );
    if( *u < 0.0f || *u > det )
        return false;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    *v = D3DXVec3Dot( &dir, &qvec );
    if( *v < 0.0f || *u + *v > det )
        return false;

    // Calculate t, scale parameters, ray intersects triangle
    *t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

    return true;
}

//-------------------------------------------------------------------------------------------------
