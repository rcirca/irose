#ifndef	__COBJMORPH_H
#define	__COBJMORPH_H
#include "CGameOBJ.h"
//-------------------------------------------------------------------------------------------------

///
/// 모퍼 오브젝트
///

class CObjMORPH : public CGameOBJ
{
private:
	HNODE					m_hMorpher;

public :
	short					m_nMorphIDX;

public:
	CObjMORPH ();
	virtual ~CObjMORPH ();



	//////////////////////////////////////////////////////////////////////////////////////////
	/// < Inherited from GameObj

	/*override*/int					Get_TYPE ()		{	return OBJ_MORPH;	}
	
	/*override*/virtual bool		IsInViewFrustum(){ return inViewfrustum( m_hMorpher )?true:false; }
	/*override*/bool				IsIntersect (float &fCurDistance);
	/*override*/void				InsertToScene (void);
	/*override*/void				RemoveFromScene (bool bIncludeEFFECT=true);
	
	/// < End
	//////////////////////////////////////////////////////////////////////////////////////////



	bool					Create (int iMorphID, D3DVECTOR &Position);

	void					Scale (D3DVECTOR &Scale);
	void					Rotate (D3DXQUATERNION &Rotate);
} ;

//-------------------------------------------------------------------------------------------------
#endif