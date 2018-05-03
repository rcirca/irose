/*
	$Header: /Client/IO_MESH.H 8     03-11-27 3:05p Jeddli $
*/
#ifndef	__IO_MESH_H
#define	__IO_MESH_H
typedef unsigned int HNODE;
//-------------------------------------------------------------------------------------------------

/// mesh list
class CMeshLIST : public CFileLIST<HNODE> {
private :
	bool	Load_FILE (tagFileDATA<HNODE> *pHNODE);
	void	Free_FILE (tagFileDATA<HNODE> *pHNODE);

public  :
	CMeshLIST () : CFileLIST<HNODE>("MESH")	{	;	}
	~CMeshLIST ()							{	CFileLIST< HNODE >::Free ();			}

	HNODE	IDX_HNODE(short		nIndex)		{	return this->Get_DATAUseIDX(nIndex);	}
	HNODE	KEY_HNODE(t_HASHKEY	HashKEY);//	{	return this->Get_DATAUseKEY(HashKEY);	}
} ;
extern CMeshLIST g_MeshFILE;

//-------------------------------------------------------------------------------------------------
#endif
