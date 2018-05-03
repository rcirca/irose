
#include "stdAFX.h"
#include "IO_Mesh.h"
#include "Common\\IO_Motion.h"
#include "IO_Material.h"
#include "IO_Morpher.h"


tagMORPHER::tagMORPHER ()
{
	m_HashMESH = NULL;
	m_HashANI = NULL;
	m_HashMAT = NULL;
}
tagMORPHER::~tagMORPHER ()
{
	if ( m_HashMESH ) {
		g_MeshFILE.Del_FILE( m_HashMESH );
	}
	if ( m_HashANI ) {
		g_MotionFILE.Del_FILE( m_HashANI );
	}
	if ( m_HashMAT ) {
		g_MatFILE.Del_FILE( m_HashMAT );
	}
}

//-------------------------------------------------------------------------------------------------
CMorpherDATA::CMorpherDATA ()
{
}
CMorpherDATA::~CMorpherDATA ()
{
	this->Free ();
}

//-------------------------------------------------------------------------------------------------
bool CMorpherDATA::Load (char *szSTBFile)
{
	CGameSTB fSTB;

	if ( fSTB.Open (szSTBFile) ) {
		short nAlpha, n2Side;

		m_nDataCNT = fSTB.GetRowCount ();
		m_pMorpher = new tagMORPHER[ m_nDataCNT ];

		for (short nY=1; nY<m_nDataCNT; nY++) {
			m_pMorpher[ nY ].m_HashMESH = g_MeshFILE.Add_FILE	( fSTB.GetString( 1, nY ) );	// mesh file
			m_pMorpher[ nY ].m_HashANI  = g_MotionFILE.Add_FILE	( fSTB.GetString( 2, nY ) );	// ani file

							   nAlpha		= fSTB.GetInteger (4, nY );	// alpha
							   n2Side		= fSTB.GetInteger (5, nY );	// 2side
			m_pMorpher[ nY ].m_nAlphaTest	= fSTB.GetInteger (6, nY );	// alpha test
			m_pMorpher[ nY ].m_nZTest		= fSTB.GetInteger (7, nY );	// z test
			m_pMorpher[ nY ].m_nZWrite		= fSTB.GetInteger (8, nY );	// z write

			m_pMorpher[ nY ].m_nSrcBlend	= fSTB.GetInteger (9, nY );
			m_pMorpher[ nY ].m_nDestBlend	= fSTB.GetInteger (10, nY );
			m_pMorpher[ nY ].m_nBlendOP		= fSTB.GetInteger (11, nY );

			m_pMorpher[ nY ].m_HashMAT  = g_MatFILE.Add_MATERIAL (
					fSTB.GetString ( 3, nY ),	// char *szFileName, 
					0,							// short nIsSkin, 
					nAlpha,						// short nIsAlpha, 
					n2Side);					// short nIs2Side );
		}

		fSTB.Close ();
		return true;
	}

	return false;

}
void CMorpherDATA::Free ()
{
	SAFE_DELETE_ARRAY( m_pMorpher );
}

//-------------------------------------------------------------------------------------------------
