/*
	$Header: /Client/Util/CshoSOCKET.h 5     04-06-29 12:10p Navy $
*/
#ifndef	__CSHO_SOCK_H
#define	__CSHO_SOCK_H
#include "CClientSOCKET.h"
#include "Net_Prototype.h"
//-------------------------------------------------------------------------------------------------

class CshoClientSOCK : public CClientSOCKET
{
private:
	HANDLE	m_hEncSOCK;
	int		m_iSendSEQ;

	WORD	mF_ESP (t_PACKETHEADER *pPacket);
	WORD	mF_DRH (t_PACKETHEADER *pPacket);
	short	mF_DRB (t_PACKETHEADER *pPacket);

public :
	CshoClientSOCK ();
	virtual ~CshoClientSOCK ();

	void mF_Init(DWORD dwInit);
	void Set_NetSTATUS (BYTE btStatus);
	void OnAccepted(int *pSendSEQ);
	bool WndPROC ( WPARAM wParam, LPARAM lParam );
} ;

//-------------------------------------------------------------------------------------------------
#endif
