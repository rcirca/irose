#include "stdafx.h"
#include "CRestartDLG.h"
#include "..\\..\\NetWork\\CNetwork.h"
#include "../../Country.h"
#include "../../IO_Terrain.h"


CRestartDLG::CRestartDLG()
{
	// Construct
}

CRestartDLG::~CRestartDLG()
{
	// Destruct0
}
 
unsigned int CRestartDLG::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			if(On_LButtonUP( iProcID ))
				return uiMsg;
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

bool CRestartDLG::On_LButtonUP( unsigned iProcID )
{
	switch( iProcID )
	{
	case BTN_RESTART_SAVE_POSITION:
	case BTN_RESTART_DUNGEON_GATE:
		g_pNet->Send_cli_REVIVE_REQ( REVIVE_TYPE_SAVE_POS );
		Hide();
		break;
	case BTN_RESTART_CURRENT_FIELD:
		g_pNet->Send_cli_REVIVE_REQ( REVIVE_TYPE_REVIVE_POS );
		Hide();
		break;
	}
	

#ifdef __VIRTUAL_SERVER
	g_pAVATAR->Set_HP( g_pAVATAR->Get_MaxHP() );
#endif

	return true;
}

void CRestartDLG::Show()
{
	CTDialog::Show();
	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{
		CWinCtrl* pCtrl = NULL;
		if( ZONE_IS_UNDERGROUND( g_pTerrain->GetZoneNO() ) )
		{
			if( pCtrl = Find( BTN_RESTART_SAVE_POSITION ) )
				pCtrl->Hide();
			if( pCtrl = Find( BTN_RESTART_DUNGEON_GATE ) )
				pCtrl->Show();
		}
		else
		{
			if( pCtrl = Find( BTN_RESTART_SAVE_POSITION ) )
				pCtrl->Show();
			if( pCtrl = Find( BTN_RESTART_DUNGEON_GATE ) )
				pCtrl->Hide();
		}
	}

	//if( g_pAVATAR )
	//{
	//	if( g_pAVATAR->Get_LEVEL() < 10 || g_GameDATA.m_iPvPState )
	//		SetEnableChild( IID_BTN_RESTART_SAVEDTOWN, false );
	//	else
	//		SetEnableChild( IID_BTN_RESTART_SAVEDTOWN, true );
	//}
}

