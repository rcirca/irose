
#include "LIB_gsMAIN.h"
#include "CCharDATA.h"


//-------------------------------------------------------------------------------------------------
CCharDATA::CCharDATA ()
{
	m_nAniCNT = 0;
	m_ppAniFILE = NULL;
}
CCharDATA::~CCharDATA ()
{
	SAFE_DELETE_ARRAY( m_ppAniFILE );
}

//-------------------------------------------------------------------------------------------------
bool CCharDATA::Load_MOBorNPC (FILE *fp, t_HASHKEY *pAniKEY, short nAniCNT)
{
	char cIsValid;

	fread (&cIsValid,	sizeof(char), 1, fp);
	if ( cIsValid == 0 )
		return true;

	short nI, nIndex;

	// read skel index
	fread (&nIndex, sizeof(short), 1, fp);

	char *pStr = CStr::ReadString (fp);

#ifdef	_DEBUG
	m_Name.Set( pStr );
#endif

	// read data index
	fread (&nIndex, sizeof(short),			1, fp);
	fseek ( fp,		nIndex*sizeof(short),	SEEK_CUR);

	// read ani index counter
	short nAniIDX;

	fread (&m_nAniCNT, sizeof(short), 1, fp);
	assert( m_nAniCNT > 0 );

	m_ppAniFILE = new tagMOTION*[ MAX_MOB_ANI ];
	::ZeroMemory( m_ppAniFILE,	sizeof( tagMOTION* ) * MAX_MOB_ANI );

	for (nI=0; nI<m_nAniCNT; nI++) {
		// nAniIDX == 0 인 정지가 안들어 온다..
		fread (&nAniIDX, sizeof(short), 1, fp);
		fread (&nIndex,  sizeof(short), 1, fp);

		if ( nAniIDX < 0 ) {
			// 사용하지 않는다...
			continue;
		}

		assert( nAniIDX < MAX_MOB_ANI );
		assert( nIndex >= 0 );
		assert( nIndex < nAniCNT );

		m_ppAniFILE[ nAniIDX ] = g_MotionFILE.KEY_GetMOTION( pAniKEY[ nIndex ] );

		// 여기서 뻑나면...
		// Load_MOBorNPC함수에서 g_MotionFILE.Add_FILE함수 호출시 nAniIDX값을 이용해서
		// 모션 파일 이름을 찾자~~~~
		if ( 0 == m_ppAniFILE[ nAniIDX ] ) {
			int i=00;
			assert( m_ppAniFILE[ nAniIDX ] );
		}
	}
	m_nAniCNT = MAX_MOB_ANI;

	// read bone effect counter
	fread (&nIndex, sizeof(short),			1, fp);
	fseek (fp,		nIndex*sizeof(short)*2,	SEEK_CUR);

	return true;
}


//-------------------------------------------------------------------------------------------------

IMPLEMENT_INSTANCE( CCharDatLIST );

CCharDatLIST::CCharDatLIST ()
{
	m_pMODELS = NULL;
	m_nModelCNT = NULL;
}
CCharDatLIST::~CCharDatLIST ()
{
	this->Free ();
}

void CCharDatLIST::Free ()
{
	SAFE_DELETE_ARRAY( m_pMODELS );
	m_nModelCNT = 0;
}

//-------------------------------------------------------------------------------------------------
bool CCharDatLIST::Load_MOBorNPC (char *szBaseDIR, char *szFileName)
{
	char *pFullPath;
	FILE *fp;

	if ( szBaseDIR ) {
		pFullPath = CStr::Printf ("%s%s", szBaseDIR, szFileName);
	} else
		pFullPath = szFileName;

	fp = fopen( pFullPath, "rb" );
	if ( fp == NULL ) {
		return false;
	}

	t_HASHKEY *pAniKEY;
	char *pStr;
	short nI, nFileCNT, nAniCNT;

	// skel file ...
	fread (&nFileCNT, sizeof(short), 1, fp);
	for (nI=0; nI<nFileCNT; nI++) {
		pStr = CStr::ReadString (fp);
	}

	// motion file ...
	fread (&nAniCNT, sizeof(short), 1, fp);
	assert( nAniCNT > 0 );
	pAniKEY = new t_HASHKEY[ nAniCNT ];
	for (nI=0; nI<nAniCNT; nI++) {
		pStr = CStr::ReadString (fp);
		if ( szBaseDIR ) {
			pFullPath = CStr::Printf ("%s%s", szBaseDIR, pStr);
		} else
			pFullPath = pStr;
/*
		if ( nI == 331 ) {
			int i=0;
		}
*/
		if ( !CUtil::Is_FileExist( pFullPath ) ) {
			::MessageBox( NULL, pFullPath, "모션 파일 없음", MB_OK );
		}
		pAniKEY[ nI ] = g_MotionFILE.Add_FILE ( pFullPath );
		assert( pAniKEY[ nI ] );
	}

	// bone effect file ...
	fread (&nFileCNT, sizeof(short), 1, fp);
	for (nI=0; nI<nFileCNT; nI++) {
		pStr = CStr::ReadString (fp);
	}

	fread (&m_nModelCNT, sizeof(short), 1, fp);
	assert( m_nModelCNT > 0 );

	m_pMODELS = new CCharDATA[ m_nModelCNT ];
	for (nI=0; nI<m_nModelCNT; nI++) {
		// LogString( 0xffff, CStr::Printf(">> load model %d \n", nI ) );

		m_pMODELS[ nI ].Load_MOBorNPC (fp, pAniKEY, nAniCNT);

		// LogString( 0xffff, CStr::Printf("<< load model %d \n", nI ) );
	}

	SAFE_DELETE_ARRAY( pAniKEY  );

	fclose (fp);

	return true;

}

//-------------------------------------------------------------------------------------------------
