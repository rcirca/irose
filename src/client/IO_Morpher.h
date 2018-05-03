#ifndef	__IO_MORPHER_H
#define	__IO_MORPHER_H
//-------------------------------------------------------------------------------------------------

struct tagMORPHER {
	tagMORPHER ();
	~tagMORPHER ();

	t_HASHKEY	m_HashMESH;
	t_HASHKEY	m_HashANI;
	t_HASHKEY	m_HashMAT;

	short		m_nAlphaTest;
	short		m_nZTest;
	short		m_nZWrite;

	short		m_nSrcBlend;
	short		m_nDestBlend;
	short		m_nBlendOP;
} ;

class CMorpherDATA
{
private:
	tagMORPHER	*m_pMorpher;
	short		 m_nDataCNT;

public :
	CMorpherDATA ();
	~CMorpherDATA ();

	bool Load (char *szSTBFile);
	void Free (void);

	tagMORPHER* Get(short nIndex) 
	{
		if ( nIndex > 0 && nIndex < m_nDataCNT )
			return &m_pMorpher[ nIndex ];
		return NULL;
	}
} ;

//-------------------------------------------------------------------------------------------------
#endif
