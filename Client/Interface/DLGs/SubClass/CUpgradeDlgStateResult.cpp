#include "stdafx.h"
#include ".\cupgradedlgstateresult.h"
#include "../CUpgradeDlg.h"
#include "Net_prototype.h"
#include "CGuage.h"
#include "../../../GameData/CUpgrade.h"
#include "../../../GameCommon/Item.h"
#include "../../../Network/CNetwork.h"
#include "../../../Game.h"
#include "../../command/uicommand.h"
#include "../../it_mgr.h"
#include "../../io_imageres.h"
#include "../../TypeResource.h"
#include "../../../Object.h"
#include "../../../Game_Func.h"
#include "ResourceMgr.h"
CUpgradeDlgStateResult::CUpgradeDlgStateResult( CUpgradeDlg* pParent )
{
	m_pParent = pParent;

	m_iRedGuageImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"ID_GUAGE_MAKE_RED" );
	m_iGreenGuageImageID  = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"ID_GUAGE_MAKE_GREEN" );
	int iModuleID	    = IMAGE_RES_UI;
	int iGuageWidth		= 88;

	POINT	ptOffset	= { 27,187 + 47 * 3 };

	m_pResultGuage = new CGuage(m_iRedGuageImageID, 0, iModuleID , iGuageWidth );
	m_pResultGuage->SetOffset( ptOffset );
	m_pResultGuage->SetAutoIncrement( 50, 100 );
}

CUpgradeDlgStateResult::~CUpgradeDlgStateResult(void)
{
	SAFE_DELETE( m_pResultGuage );
}
void CUpgradeDlgStateResult::Leave()
{
	m_bWaitUserinput = false;
	BYTE btResult = CUpgrade::GetInstance().GetResult();
	switch( btResult )
	{
	case CRAFT_UPGRADE_SUCCESS:
		CUpgrade::GetInstance().RemoveTargetItem();
		CUpgrade::GetInstance().ApplyResultItemSet();
		//g_itMGR.OpenMsgBox(STR_CRAFT_UPGRADE_SUCCESS);///해당 아이템을 보여주자.
		break;
	case CRAFT_UPGRADE_FAILED:
		CUpgrade::GetInstance().RemoveTargetItem();
		CUpgrade::GetInstance().ApplyResultItemSet();
		//g_itMGR.OpenMsgBox(STR_CRAFT_UPGRADE_FAILED);
		break;
	case CRAFT_UPGRADE_INVALID_MAT:
		g_itMGR.OpenMsgBox( STR_CRAFT_UPGRADE_INVALID_MAT);
		break;
	default:
		break;
	}
}

void CUpgradeDlgStateResult::Enter()
{
	m_bWaitUserinput = false;
	BYTE btResult = CUpgrade::GetInstance().GetResult();

	m_pResultGuage->SetValue(0);
	m_pResultGuage->SetGuageImageID(m_iRedGuageImageID);


	switch( btResult )
	{
	case CRAFT_UPGRADE_SUCCESS:
		{
			int iSuccess = CUpgrade::GetInstance().GetResultSuccessProb() / 10;
			if( iSuccess <= 30 ) iSuccess = 31;
			if( iSuccess >= 100 ) iSuccess = 100;

			m_iVirtualSuccessPoint = iSuccess - ( rand() % 30 );
			m_pResultGuage->SetAutoIncrementMaxValue( iSuccess  );
			break;
		}
	case CRAFT_UPGRADE_FAILED:
		{
			m_iVirtualSuccessPoint = rand() % 50 + 30;

			int iSuccess = m_iVirtualSuccessPoint - ( rand() % 20 );

			m_pResultGuage->SetAutoIncrementMaxValue( iSuccess  );

			break;
		}
	case CRAFT_UPGRADE_INVALID_MAT:
		m_pParent->ChangeState( CUpgradeDlg::STATE_NORMAL );
		break;
	default:
		break;
	}
	m_dwPrevTime = g_GameDATA.GetGameTime();
}

void CUpgradeDlgStateResult::Update( POINT ptMouse )
{
	if( m_bWaitUserinput ) return;
	DWORD dwCurrTime = g_GameDATA.GetGameTime();

	CUpgrade& Upgrade = CUpgrade::GetInstance();
	if( m_pResultGuage->Update( ptMouse, m_dwPrevTime, dwCurrTime ) == CGuage::UPDATE_END )
	{
		m_bWaitUserinput = true;

		BYTE btResult = CUpgrade::GetInstance().GetResult();
		switch( btResult )
		{
		case CRAFT_UPGRADE_SUCCESS:
			{
				CTCmdChangeStateUpgradeDlg* pCmd = new CTCmdChangeStateUpgradeDlg(CUpgradeDlg::STATE_NORMAL );
				g_itMGR.OpenMsgBox(STR_CRAFT_UPGRADE_SUCCESS, CMsgBox::BT_OK , true, 0 , pCmd );///해당 아이템을 보여주자.

				int itemtype = 0;
				int itemno   = 0;
				if( CItemFragment* pTargetItem = CUpgrade::GetInstance().GetTargetItem() )
				{
					itemtype = pTargetItem->GetItem().GetTYPE();
					itemtype = pTargetItem->GetItem().GetItemNO();
				}

				g_pNet->Send_cli_ITEM_RESULT_REPORT( REPORT_ITEM_UPGRADE_SUCCESS, itemtype, itemno );
				break;
			}
		case CRAFT_UPGRADE_FAILED:
			{
				CTCmdChangeStateUpgradeDlg* pCmd = new CTCmdChangeStateUpgradeDlg(CUpgradeDlg::STATE_NORMAL );
				g_itMGR.OpenMsgBox(STR_CRAFT_UPGRADE_FAILED, CMsgBox::BT_OK , true, 0 , pCmd );

				int itemtype = 0;
				int itemno   = 0;
				if( CItemFragment* pTargetItem = CUpgrade::GetInstance().GetTargetItem() )
				{
					itemtype = pTargetItem->GetItem().GetTYPE();
					itemtype = pTargetItem->GetItem().GetItemNO();
				}
				g_pNet->Send_cli_ITEM_RESULT_REPORT( REPORT_ITEM_UPGRADE_FAILED, itemtype, itemno );
				break;
			}
		default:
			m_pParent->ChangeState( CUpgradeDlg::STATE_NORMAL );
			break;
		}
	}
		

	if( m_pResultGuage->GetValue() >=  m_iVirtualSuccessPoint )
		m_pResultGuage->SetGuageImageID(m_iGreenGuageImageID);

	m_dwPrevTime = dwCurrTime;
}

void CUpgradeDlgStateResult::Draw()
{
	m_pResultGuage->Draw();
}
void CUpgradeDlgStateResult::MoveWindow( POINT ptPosition )
{
	m_pResultGuage->MoveWindow( ptPosition );
}