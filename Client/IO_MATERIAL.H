/*
	$Header: /Client/IO_MATERIAL.H 15    04-07-25 3:19p Zho $
*/
#ifndef	__IO_MATERIAL_H
#define	__IO_MATERIAL_H
typedef unsigned int HNODE;
//-------------------------------------------------------------------------------------------------

/// Material
struct tagMATERIAL {
	CStrVAR	m_RealFileName;
	HNODE	m_hMAT;
	HNODE	m_hShader;
	short	m_nAlpha;
	short	m_nTwoSide;

	/// 货肺 眠啊等 加己甸
	short	m_nAlphaTest;
	short	m_nAlphaRef;
	short	m_nZTest;
	short	m_nZWrite;
	short	m_nBlendType;
	short	m_nSpecular;
	float		m_fAlphaValue;
	short	m_nGlowType;
	float		m_fGlowColor[3];
	
	tagMATERIAL ()	{
		m_hMAT=NULL,m_hShader=NULL,m_nAlpha=0,m_nTwoSide=0;
		m_nAlphaTest=1,m_nAlphaRef=0x80,m_nZTest=1,m_nZWrite=1,m_nBlendType=0,m_nSpecular=0;
		m_fAlphaValue = 1.0f;
		m_nGlowType = 0;
		m_fGlowColor[0] = m_fGlowColor[1] = m_fGlowColor[2] = 1.0f;
	}
} ;

/// Material list
class CMatLIST : public CFileLIST<tagMATERIAL*> {
private :
	CStrVAR		m_NameTemplate;
	bool	Load_FILE (tagFileDATA<tagMATERIAL*> *pData);
	void	Free_FILE (tagFileDATA<tagMATERIAL*> *pData);

public  :
	CMatLIST () : CFileLIST<tagMATERIAL*>("MAT")	{	;	}
	CMatLIST (char *szNameTemplate);
	~CMatLIST ();

	bool Load (char *szSTBFile, char *szNameTemplate = NAME_MATERIAL);
	void Free ();

	t_HASHKEY	Add_MATERIAL (
		char *szFileName,
		short nIsSkin, short nIsAlpha, short nIs2Side,
		bool bTerrainMAT=false, char *szMatNameUseLightMap=NULL,
		short nAlphaTest=1, short nAlphaRef=0x80, short nZTest=1,
		short nZWrite=1, short nBlendType=0, short nSpecular=0,
		float fAlphaValue = 1.0f,
		short nGlowType = 0,
		float * pfGlowColor = 0
		);

	HNODE	IDX_HNODE(short		nIndex);
	HNODE	KEY_HNODE(t_HASHKEY	HashKEY);

	void Mem_FREE (tagFileDATA<tagMATERIAL*> *pData);
} ;
extern CMatLIST	g_MatFILE;

//-------------------------------------------------------------------------------------------------
#endif