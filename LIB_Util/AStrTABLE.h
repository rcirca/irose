#ifndef	__AStringTable_H
#define	__AStringTable_H
//-------------------------------------------------------------------------------------------------

struct tagStrTblDATA {
	long	m_lFilePOS;
	short	m_nStrLEN;
	char   *m_pStr;

	void Set(char *szStr, short nLen)
	{
		if ( m_pStr ) {
			delete[] m_pStr;
			m_pStr = NULL;
			m_nStrLEN = 0;
		}

		if ( szStr ) {
			m_pStr = new char[ nLen+1 ];
			strncpy( m_pStr, szStr, nLen );
			m_pStr[ nLen ] = 0;
		}
	}
	tagStrTblDATA () 
	{
		m_pStr = NULL;
		m_lFilePOS = 0;
		m_nStrLEN = 0;
	}

	~tagStrTblDATA ()
	{
		if ( m_pStr ) {
			delete[] m_pStr;
			m_pStr = NULL;
		}
	}
} ;

class AStringTable {
private:
	int				m_iRowCNT;
	int				m_iColCNT;
	tagStrTblDATA **m_ppFilePOS;
	FILE		   *m_FP;
	char		   *m_pTmpStr;
	short			m_nTmpStrLen;

public :
	AStringTable ()
	{
		m_FP = NULL;
		m_ppFilePOS = NULL;
		m_nTmpStrLen = 1024;
		m_pTmpStr = new char[ m_nTmpStrLen ];
	}
	~AStringTable ()
	{
		this->Clear ();
		this->Close ();
	}
	
	bool Create (WORD wDefColCNT, WORD wDefRowCNT)
	{
		this->Clear ();

		m_iColCNT = wDefColCNT;
		m_iRowCNT = wDefRowCNT;

		m_ppFilePOS = new tagStrTblDATA* [ m_iRowCNT ];
		for (int iR=0; iR<m_iRowCNT; iR++)
			m_ppFilePOS[ iR ] = new tagStrTblDATA[ m_iColCNT ];
	}

	bool AppendRow ()
	{
		tagStrTblDATA **ppFilePOS;
		ppFilePOS = new tagStrTblDATA* [ 1+m_iRowCNT ];

		for (int iR=0; iR<m_iRowCNT; iR++)
			ppFilePOS[iR] = m_ppFilePOS[iR];

		ppFilePOS[m_iRowCNT] = new tagStrTblDATA[ m_iColCNT ];
		m_iRowCNT ++;

		delete[] m_ppFilePOS;
		m_ppFilePOS = ppFilePOS;

		return true;
	}
	//WORD wColIDX, char *szStr
	int GetEmptyROW ()
	{

	}

	bool AppendColumn ()
	{
		tagStrTblDATA *pFilePOS;

		int iR,iC;
		for (iR=0; iR<m_iRowCNT; iR++) {
			pFilePOS = new tagStrTblDATA[ 1+m_iColCNT ];

			for (iC=0; iC<m_iColCNT; iC++)
				pFilePOS[iC] = m_ppFilePOS[iR][iC];

			delete[] m_ppFilePOS[iR];
			m_ppFilePOS[iR] = pFilePOS;
		}
		m_iColCNT++;

		return true;
	}

	void Clear ()
	{
		if ( m_ppFilePOS ) {
			for (int iR=0; iR<m_iRowCNT; iR++) 
				delete[] m_ppFilePOS[iR];

			delete[] m_ppFilePOS;
		}
		m_iRowCNT = m_iColCNT = 0;
	}

	bool Open (char *szFileName)
	{
		if ( m_FP )
			fclose(m_FP);

		m_FP = fopen( szFileName, "rb" );
		if ( NULL == m_FP )
			return false;
		fread( &m_iColCNT,	sizeof(int),	1,	m_FP );
		fread( &m_iRowCNT,	sizeof(int),	1,	m_FP );

		m_ppFilePOS = new tagStrTblDATA* [ m_iRowCNT ];
		int iR, iC;
		for (iR=0; iR<m_iRowCNT; iR++) {
			m_ppFilePOS[ iR ] = new tagStrTblDATA[ m_iColCNT ];
			for (iC=0; iC<m_iColCNT; iC++) {
				fread( &m_ppFilePOS[iR][iC].m_lFilePOS,	sizeof(long),	1,	m_FP ); 
				fread( &m_ppFilePOS[iR][iC].m_nStrLEN,	sizeof(char),	1,	m_FP ); 
			}
		}

		return true;
	}

	void  Close ()
	{
		if ( m_FP ) {
			fclose( m_FP );
			m_FP = NULL;
		}
	}

	bool SetString (WORD wColIDX, WORD wRowIDX, char *szString)
	{
		if ( wRowIDX >= m_iRowCNT || wColIDX >= m_iColCNT )
			return false;
		m_ppFilePOS[ wRowIDX ][ wColIDX ].Set( szString, strlen(szString) );
		return true;
	}

	short GetLength(WORD wColIDX, WORD wRowIDX)
	{
		if ( wRowIDX >= m_iRowCNT || wColIDX >= m_iColCNT )
			return -1;
		return m_ppFilePOS[ wRowIDX ][ wColIDX ].m_nStrLEN;
	}

	char *ReadString(WORD wColIDX, WORD wRowIDX)
	{
		if ( wRowIDX >= m_iRowCNT || wColIDX >= m_iColCNT )
			return NULL;
		if ( 0 == m_ppFilePOS[ wRowIDX ][ wColIDX ].m_lFilePOS ||
			 0 == m_ppFilePOS[ wRowIDX ][ wColIDX ].m_nStrLEN )
			return NULL;

		if ( m_ppFilePOS[ wRowIDX ][ wColIDX ].m_nStrLEN >= m_nTmpStrLen ) {
			delete[] m_pTmpStr;
			m_nTmpStrLen = m_ppFilePOS[ wRowIDX ][ wColIDX ].m_nStrLEN;
			m_pTmpStr = new char[ m_nTmpStrLen+1 ];
		}
		fseek( m_FP, m_ppFilePOS[ wRowIDX ][ wColIDX ].m_lFilePOS, SEEK_SET ); 
		fread( m_pTmpStr, sizeof(char), m_ppFilePOS[ wRowIDX ][ wColIDX ].m_nStrLEN,	m_FP );
	}

	bool Load (char *szFileName)
	{
		if ( !this->Open(szFileName) )
			return false;

		for (int iR=0; iR<m_iRowCNT; iR++) {
			for (int iC=0; iC<m_iColCNT; iC++) {
				fseek( m_FP, m_ppFilePOS[ iR ][ iC ].m_lFilePOS, SEEK_SET ); 
				fread( m_pTmpStr, sizeof(char), m_ppFilePOS[ iR ][ iC ].m_nStrLEN,	m_FP );
				m_pTmpStr[ m_ppFilePOS[ iR ][ iC ].m_nStrLEN ] = 0;
				m_ppFilePOS[ iR ][ iC ].Set( m_pTmpStr, m_ppFilePOS[ iR ][ iC ].m_nStrLEN ); 
			}
		}

		this->Close ();
	}

	bool Save (char *szFileName)
	{
		FILE *fp;
		fp = fopen( szFileName, "wb" );
		if ( NULL == fp )
			return false;
		
		fwrite( &m_iColCNT,	sizeof(int),	1,	fp );
		fwrite( &m_iRowCNT,	sizeof(int),	1,	fp );

		long lBackupPos = ftell( fp );
		fseek( fp, ( sizeof(long)+sizeof(short) ) * m_iColCNT * m_iRowCNT, SEEK_CUR );

		int iR, iC;
		for (iR=0; iR<m_iRowCNT; iR++) {
			for (iC=0; iC<m_iColCNT; iC++) {
				if ( m_ppFilePOS[ iR ][ iC ].m_nStrLEN ) {
					m_ppFilePOS[ iR ][ iC ].m_lFilePOS = ftell( fp );
					fwrite( m_ppFilePOS[ iR ][ iC ].m_pStr,	sizeof(char),	m_ppFilePOS[ iR ][ iC ].m_nStrLEN,	fp ); 
				}
			}
		}

		fseek( fp, lBackupPos, SEEK_SET );

		for (iR=0; iR<m_iRowCNT; iR++) {
			for (iC=0; iC<m_iColCNT; iC++) {
				fwrite( &m_ppFilePOS[iR][iC].m_lFilePOS,	sizeof(long),	1,	fp ); 
				fwrite( &m_ppFilePOS[iR][iC].m_nStrLEN,		sizeof(short),	1,	fp ); 
			}
		}

		fclose( fp );
		return true;
	}
} ;

//-------------------------------------------------------------------------------------------------
#endif