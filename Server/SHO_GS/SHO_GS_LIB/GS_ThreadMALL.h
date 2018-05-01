/**
 * \ingroup SHO_GS
 * \file	GS_ThreadMALL.h
 * \brief	홈페이지 쇼핑몰에서 구입한 아이템이 들어있는 디비와 연결,통신
 */
#ifndef	__GS_THREAD_MALL_H
#define	__GS_THREAD_MALL_H
#include "CSqlTHREAD.h"
//-------------------------------------------------------------------------------------------------

/**
 * \ingroup SHO_GS_LIB
 * \class	GS_CThreadMALL
 * \author	wookSang.Jo
 * \brief	쇼핑몰 디비와 연결되는 쓰래드 클래스
 *			디비연결및 처리 클래스인 CSqlTHREAD를 상속 받음
 */
class GS_CThreadMALL : public CSqlTHREAD
{
private:
	bool		m_bWaiting;
	CStrVAR		m_QuerySTR;

	bool Send_gsv_MALL_ITEM_REPLAY(classUSER *pUSER, BYTE btType, BYTE btData=0);

	bool Give_MallITEM ( tagQueryDATA *pSqlPACKET, BYTE btMallInvIDX, short nDupCnt );
	bool Bring_MallITEM ( tagQueryDATA *pSqlPACKET, BYTE btMallInvIDX, short nDupCnt );
	bool List_MallITEM( int iScoketIDX, char *szAccount );

	bool Run_SqlPACKET( tagQueryDATA *pSqlPACKET );
	void Execute ();

public :
	GS_CThreadMALL ();
	virtual ~GS_CThreadMALL ();

	bool Add_SqlPacketWithACCOUNT (classUSER *pUSER, t_PACKET *pPacket);
	bool Add_SqlPacketWithAVATAR  (classUSER *pUSER, t_PACKET *pPacket);
} ;
extern GS_CThreadMALL *g_pThreadMALL;

//-------------------------------------------------------------------------------------------------
#endif