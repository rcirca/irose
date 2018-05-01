#include "stdafx.h"
#include "CSystemDLG.h"
#include "../../Network/CNetwork.h"
#include "../../System/CGame.h"
#include "../../GameData/CParty.h"
#include "../Command/CTCmdHotExec.h"
#include "../it_mgr.h"

CSystemDLG::CSystemDLG()
{

}

CSystemDLG::~CSystemDLG()
{

}


unsigned int CSystemDLG::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if(!IsVision()) return 0;	

	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			if(On_LButtonUP( iProcID  ))
				return uiMsg;
			break;
		}
		return uiMsg;
	}
	return 0;
}


bool CSystemDLG::On_LButtonUP( unsigned iProcID)
{
	switch( iProcID )
	{
		case IID_BTN_EXIT:
			{
				Hide();

				if( g_GameDATA.GetGameTime() - g_pAVATAR->GetLastBattleTime() <= 10 * 1000 )
				{
					g_itMGR.AppendChatMsg( STR_CANT_EXITGAME, IT_MGR::CHAT_TYPE_SYSTEM );
				}
				else
				{
					g_itMGR.SetWaitDisconnectType( 0 );
					g_pNet->Send_cli_LOGOUT_REQ();
				}
			}
			break;
		case IID_BTN_CONTINUE:
		case IID_BTN_CLOSE:
			Hide();
			break;
		case IID_BTN_GOTO_SELECTAVT:
			Hide();
			if( g_GameDATA.GetGameTime() - g_pAVATAR->GetLastBattleTime() <= 10 * 1000 )
			{
				g_itMGR.AppendChatMsg( STR_CANT_EXITGAME, IT_MGR::CHAT_TYPE_SYSTEM );
			}
			else
			{
				g_itMGR.SetWaitDisconnectType( 1 );
				g_pNet->Send_cli_CHAR_CHANGE();
			}
			break;
		default:
			break;
	}
	return true;
}


