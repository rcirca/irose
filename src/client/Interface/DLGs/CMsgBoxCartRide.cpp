#include "stdafx.h"
#include ".\cmsgboxcartride.h"
#include "CObjCART.h"
#include "OBJECT.h"

CMsgBox_CartRide::CMsgBox_CartRide()
{
	m_wOwner = 0;
	m_wGuest = 0;
}

void CMsgBox_CartRide::Update(POINT ptMouse)
{
	CMsgBox::Update( ptMouse );

	//Find Index Owner.
	int iOwnerObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_wOwner );
	CObjAVT* pOwnerAVT = g_pObjMGR->Get_CharAVT( iOwnerObjClientIndex, true );

	//Find Index Guest.
	int iGuestObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_wGuest );
	CObjAVT* pGuestAVT = g_pObjMGR->Get_CharAVT( iGuestObjClientIndex, true );

	if( pOwnerAVT && pGuestAVT )
	{

#ifdef _DEBUG
		CStr::DebugString( "Dist %f \n", pOwnerAVT->Get_DISTANCE(pGuestAVT) );
#endif	
		
		if( pOwnerAVT->Get_DISTANCE(pGuestAVT) > SKILL_SCOPE(26) )	//cart ride skill Index = 26.
		{
			SendCommand( m_iInvokerDlgID, m_pCmdCancel );
			m_pCmdCancel = NULL;
			ClearCommand();
			Hide();
		}                
	}
}