/*
	$Header: /7HeartsOnline/Server/SHO_GS/Sho_gs_lib/CObjEVENT.h 11    05-09-21 4:29p Icarus $
	$History: CObjEVENT.h $
 * 
 * *****************  Version 11  *****************
 * User: Icarus       Date: 05-09-21   Time: 4:29p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib
 * 
 * *****************  Version 10  *****************
 * User: Icarus       Date: 05-08-25   Time: 1:23p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib
 * 
 * *****************  Version 9  *****************
 * User: Icarus       Date: 04-10-07   Time: 8:00p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib
 * 
 * *****************  Version 8  *****************
 * User: Icarus       Date: 04-06-03   Time: 3:40p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib
 * 
 * *****************  Version 7  *****************
 * User: Icarus       Date: 04-05-27   Time: 8:31p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib
 * 
 * *****************  Version 6  *****************
 * User: Icarus       Date: 04-05-03   Time: 6:46p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib
 * 
 * *****************  Version 5  *****************
 * User: Icarus       Date: 04-04-17   Time: 3:36p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib
 * 
 * *****************  Version 4  *****************
 * User: Icarus       Date: 04-04-01   Time: 5:43p
 * Updated in $/7HeartsOnline/Server/SHO_GS/SHO_GS_LIB
 * 
 * *****************  Version 3  *****************
 * User: Icarus       Date: 04-03-31   Time: 3:10p
 * Updated in $/7HeartsOnline/Server/SHO_GS/SHO_GS_LIB
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-03-30   Time: 2:24p
 * Updated in $/7HeartsOnline/Server/SHO_GS/SHO_GS_LIB
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:20a
 * Created in $/7HeartsOnline/Server/Sho_gs/Sho_gs_lib
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:47a
 * Created in $/SevenHearts/Server/Sho_gs/Sho_gs_lib
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:35a
 * Created in $/SHO/Server/Sho_gs/Sho_gs_lib
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:26a
 * Created in $/SevenHearts/Server/SHO_GS_LIB/Server/Sho_gs/Sho_gs_lib
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-03-24   Time: 9:43p
 * Updated in $/SevenHearts/Server/SHO_GS_LIB
*/
#ifndef	__COBJEVENT_H
#define	__COBJEVENT_H
#include "CGameOBJ.h"
//-------------------------------------------------------------------------------------------------
/**
 * \ingroup SHO_GS_LIB
 * \class	CObjEVENT
 * \author	wookSang.Jo
 * \brief	이벤트 객체 관리및 처리하는 클래스
 *			CGameOBJ클래스를 상속받음
 */
class CObjEVENT : public CObjVAR, public CGameOBJ
{
private:
	char		m_cMapX;
	char		m_cMapY;
	WORD		m_wMapEventID;
//	short		m_nEventSTATUS;
	
	t_HASHKEY	m_HashID;

	bool Send_gsv_SET_EVENT_STATUS ();

public :
	CObjEVENT ();
	virtual ~CObjEVENT ();

	// virtual function inherit from CGameOBJ -----------------------------------------------------
    t_ObjTAG	Get_TYPE ()		{	return OBJ_CNST;	}
    bool Make_gsv_ADD_OBJECT( classPACKET *pCPacket );

	// virtual function inherit from CObjVAR -----------------------------------------------------
	short VGetCur_ZoneNO();
	float VGetCur_XPOS()		{	return	this->m_PosCUR.x;		}
	float VGetCur_YPOS()		{	return	this->m_PosCUR.y;		}	

	int	  VGetCur_ZoneTIME()	{	return	GetCur_ZoneTIME();		}

	void  Set_ObjVAR(BYTE btVarIDX, short nValue )
	{
		// 여기서 설정 ? 아님 서버에서 받아서 설정 ???
		if ( 0 == btVarIDX ) {
			short nBefore = this->Get_ObjVAR( 0 );
			CObjVAR::Set_ObjVAR( 0, nValue );
			if ( nBefore != this->Get_ObjVAR( 0 ) ) {
				// 값이 바뀌었다.
				this->Send_gsv_SET_EVENT_STATUS ();
			}
		} else
			CObjVAR::Set_ObjVAR( btVarIDX, nValue );
	}

	t_HASHKEY	Get_ID ()		{	return m_HashID;	}

	bool Init (CZoneTHREAD *pZONE, char cMapX, char cMapY, float fWorldX, float fWorldY, WORD wMapEventID);
	int	 Proc (void);
} ;

//-------------------------------------------------------------------------------------------------
#endif
