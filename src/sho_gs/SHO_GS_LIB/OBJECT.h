/**
 * \ingroup SHO_GS
 * \file	Object.h
 * \brief	서버에 생성된 전체 객체 관리
 */
#ifndef __CObject_H
#define __CObject_H
#include "CCharDATA.h"
#include "CObjITEM.h"
#include "CObjEVENT.h"
#include "GS_USER.h"
//-------------------------------------------------------------------------------------------------

/**
 * \ingroup SHO_GS_LIB
 * \class	CObjMNG
 * \author	wookSang.Jo
 * \brief	서버에 존제하는 모든 객체를 관리하는 클래스
 */
class CObjMNG : public CSyncIdxARRAY < CGameOBJ* > {
public :
    CObjMNG (int iMaxGameObject);
    ~CObjMNG ();

    CGameOBJ *Get_OBJECT ( WORD wIndex );
    CObjCHAR *Get_CharOBJ( WORD wIndex, bool bCheckHP  );
    CObjITEM *Get_ItemOBJ( WORD wIndex );
	classUSER*Get_UserOBJ( DWORD dwIndex );

	CGameOBJ *Get_GameOBJ( WORD wIndex, t_ObjTAG ObjTYPE );

    CGameOBJ *Get_ClientOBJECT( WORD wIndex )					{	return Get_OBJECT(wIndex);				}
	CObjCHAR *Get_ClientCharOBJ( WORD wIndex, bool bCheckHP  )	{	return Get_CharOBJ(wIndex,bCheckHP);	}
	int		  Get_ClientObjectIndex( int iServerObject )		{	return iServerObject;					}
} ;
extern CObjMNG  *g_pObjMGR;

#include "GS_ObjPOOL.h"

//-------------------------------------------------------------------------------------------------
#endif
