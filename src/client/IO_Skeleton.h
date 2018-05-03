/*
	$Header: /Client/IO_Skeleton.h 8     03-11-27 3:05p Jeddli $
*/
#ifndef	__IO_SKEL_H
#define	__IO_SKEL_H
typedef unsigned int HNODE;
//-------------------------------------------------------------------------------------------------

#define	DEFAULT_UNIT_TYPE(I)		g_DATA.GetDefaultITEM (I, 1)
#define	DEFAULT_HAIR(I)				g_DATA.GetDefaultITEM (I, 2)
#define	DEFAULT_FACE(I)				g_DATA.GetDefaultITEM (I, 3)
#define	DEFAULT_HELMET(I)			g_DATA.GetDefaultITEM (I, 4)
#define	DEFAULT_ARMOR(I)			g_DATA.GetDefaultITEM (I, 5)
#define	DEFAULT_GAUNTLET(I)			g_DATA.GetDefaultITEM (I, 6)
#define	DEFAULT_BOOTS(I)			g_DATA.GetDefaultITEM (I, 7)
#define	DEFAULT_WEAPON_L(I)			g_DATA.GetDefaultITEM (I, 8)
#define DEFAULT_WEAPON_R(I)			g_DATA.GetDefaultITEM (I, 9)

/// skeleton list
class CSKELETON : public CFileLIST<HNODE> {
private :
	short	**m_ppDATA;

	bool	Load_FILE (tagFileDATA<HNODE> *pHNODE);
	void	Free_FILE (tagFileDATA<HNODE> *pHNODE);

public  :
	CSKELETON () : CFileLIST<HNODE>("SKEL")		{	;	}
	~CSKELETON ();

	t_HASHKEY Add_SKELETON (char *szFileName);

	short Get_DATA(short nRow, short nCol)		{	return	m_ppDATA[ nRow ][ nCol ];	}
	bool  Load (char *szSTBFile);
	void  Free ();
} ;

//-------------------------------------------------------------------------------------------------
#endif