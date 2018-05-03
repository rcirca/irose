#include "stdafx.h"
#include "ctooltipmgr.h"
#include "cdragndropmgr.h"
#include "interfacetype.h"
#include "typeresource.h"

#include "Dlgs/CQuestDlg.h"
#include "Dlgs/CTargetMenu.h"
#include "Dlgs/CPartyDlg.h"
#include "Dlgs/CharacterDlg.h"
#include "Dlgs/QuickToolBar.h"
#include "Dlgs/CMinimapDlg.h"
#include "Dlgs/ChattingDlg.h"
#include "Dlgs/MakeDlg.h"
#include "Dlgs/CSkillDlg.h"
#include "it_mgr.h"
#include "../object.h"
#include "../gamecommon/Skill.h"

#include "tgamectrl/SplitHangul.h"
CToolTipMgr::CToolTipMgr(void)
{
	m_bHasDrawInfo = false;
}

CToolTipMgr::~CToolTipMgr(void)
{

}

CToolTipMgr& CToolTipMgr::GetInstance()
{
	static CToolTipMgr s_Instance;
	return s_Instance;
}


void CToolTipMgr::RegToolTip( int x, int y, const char* pszMsg )
{
	m_Info.Clear();
	m_bHasDrawInfo = true;
	POINT pt  = { x + 20, y };
	m_Info.AddString( pszMsg );
	m_Info.SetPosition( pt );
}

void CToolTipMgr::RegUIInfo( const int x, const int y, const DWORD dwDlgType, const int iToolTipID )
{
	m_Info.Clear();
	m_bHasDrawInfo = true;
	POINT pt  = { x + 20, y };

	//std::string TempString = "NULL";
	//CSplitHangul splitHangul;

	switch( dwDlgType )
	{
	case DLG_TYPE_PARTY:
		{
		/*	switch( iToolTipID )
			{
			case CPartyDlg::IID_BTN_ENTRUST:
				m_Info.AddString( TT_PARTY_ENTRUST );
				break;
			case CPartyDlg::IID_BTN_LEAVE:
				m_Info.AddString( TT_PARTY_LEAVE );
				break;
			case CPartyDlg::IID_BTN_BAN:
				m_Info.AddString( TT_PARTY_BAN );
				break;
			default:
				m_bHasDrawInfo = false;
				break;
			}*/
			break;
		}
	case DLG_TYPE_CHAR:
		{
	/*		if( iToolTipID >= CHAR_BUTTON_UP_STR && iToolTipID <= CHAR_BUTTON_UP_SEN )
			{
				char szTemp[255];

				int iNeedPoint = 0;
				switch( iToolTipID )
				{
				case CHAR_BUTTON_UP_STR:
					iNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_STR );
					break;
				case CHAR_BUTTON_UP_DEX:
					iNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_DEX );
					break;
				case CHAR_BUTTON_UP_INT:
					iNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_INT );
					break;
				case CHAR_BUTTON_UP_CON:
					iNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_CON );
					break;
				case CHAR_BUTTON_UP_CHM:	
					iNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_CHARM );
					break;
				case CHAR_BUTTON_UP_SEN:
					iNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_SENSE );
					break;
				default:
					break;
				}
				sprintf( szTemp,"%s %d",STR_REQUIRE_POINT, iNeedPoint );
				m_Info.AddString( szTemp );
			}
			else
			{
				int iTempID = iToolTipID;
				iTempID -= 10;
				if( iTempID >= EQUIP_IDX_FACE_ITEM && iTempID <= MAX_EQUIP_IDX )
				{
					switch( iTempID )
					{
					case EQUIP_IDX_FACE_ITEM:	TempString = TT_SLOT_FACEITEM;break;
					case EQUIP_IDX_HELMET:		TempString = TT_SLOT_HELMET;break;
					case EQUIP_IDX_ARMOR:		TempString = TT_SLOT_ARMOR;break;
					case EQUIP_IDX_KNAPSACK:	TempString = TT_SLOT_KNAPSACK;break;
					case EQUIP_IDX_GAUNTLET:	TempString = TT_SLOT_GAUNTLET;break;
					case EQUIP_IDX_BOOTS:		TempString = TT_SLOT_BOOTS;break;
					case EQUIP_IDX_WEAPON_R:	TempString = TT_SLOT_WEAPON_R;break;
					case EQUIP_IDX_WEAPON_L:	TempString = TT_SLOT_WEAPON_L;break;
					case EQUIP_IDX_NECKLACE:	TempString = TT_SLOT_NECKLACE;break;
					case EQUIP_IDX_RING:		TempString = TT_SLOT_RING;break;
					case EQUIP_IDX_EARRING:		TempString = TT_SLOT_EARING;break;
					default:break;
					}
					m_Info.AddString(TempString.c_str());
				}
			}*/
			break;
		}
	case DLG_TYPE_QUEST:
		{
			if( iToolTipID == CQuestDlg::IID_BTN_ABANDON )
				m_Info.AddString(TT_QUEST_BTN_ABANDON);

			break;
		}
	case DLG_TYPE_SKILL:
		{
		/*	if( iToolTipID == BTN_SKILL_LEVELUP )
				m_Info.AddString(TT_SKILL_BTN_LEVELUP);
			break;*/
		}
	case DLG_TYPE_QUICKBAR:
		{
			//switch( iToolTipID )
			//{
			//case QUICK_BTN_1:
			//case QUICK_BTN_3:
			//	{
			//		m_Info.AddString(TT_QUICKBAR_BTN_NEXT);
			//		break;
			//	}
			//case QUICK_BTN_2:
			//case QUICK_BTN_4:
			//	{
			//		m_Info.AddString(TT_QUICKBAR_BTN_ROTATE);
			//		break;
			//	}
			//default:
			//	m_bHasDrawInfo = false;
			//	break;
			//}
			break;
		}
	//case DLG_TYPE_BASE:
	//	{
	//		switch( iToolTipID )
	//		{
	//		case BASE_BTN_CHAR_OPEN:	TempString = TT_BASE_BTN_CHAR	;break;
	//		case BASE_BTN_INV_OPEN:		TempString = TT_BASE_BTN_INVENTORY		;break;
	//		case BASE_BTN_SKILL_OPEN:	TempString = TT_BASE_BTN_SKILL			;break;
	//		case BASE_BTN_QUEST_OPEN:	TempString = TT_BASE_BTN_QUEST	;break;
	//		case BASE_BTN_COMMUNITY_OPEN:TempString = TT_BASE_BTN_COMMUNITY	;break;
	//		case BASE_BTN_OPTION_OPEN:	TempString = TT_BASE_BTN_OPTION		;break;
	//		case BASE_BTN_SYS_OPEN:		TempString = TT_BASE_BTN_SYSTEM	;break;
	//		default: TempString = "NULL";break;
	//		}
	//		m_Info.AddString(TempString.c_str());
	//		break;
	//	}
	case DLG_TYPE_MINIMAP:
		{
			//CMinimapDLG*	pDlg = g_itMGR.GetMinimapDLG();
			//switch( iToolTipID )
			//{
			//case CMinimapDLG::IID_BTN_SHOW	:
			//	{
			//		if( pDlg->IsShowMinimap())				
			//			TempString = TT_MINIMAP_BTN_SHOW_HIDE ;
			//		else
			//			TempString = TT_MINIMAP_BTN_SHOW_SHOW;

			//		break;
			//	}
			//case CMinimapDLG::IID_BTN_ZOOM   :
			//	{
			//		if( pDlg->IsZoomMinimap())				
			//			TempString = TT_MINIMAP_BTN_ZOOM_MIN; 
			//		else
			//			TempString = TT_MINIMAP_BTN_ZOOM_MAX; 
			//		break;
			//	}
			//default: 
			//	{
			//		TempString = "NULL";
			//		break;
			//	}
			//}
			//m_Info.AddString(TempString.c_str());
			break;
		}
	case DLG_TYPE_CHAT:
		{
			//switch( iToolTipID )
			//{
			//case CChatDLG::IID_BTN_CHANGESIZE:
			//	{
			//		CChatDLG*	pDlg = g_itMGR.GetChatDLG();
			//		if( pDlg->GetListType() == CChatDLG::LIST_TYPE_BIG )
			//			TempString = TT_CHAT_BTN_MINIMIZE;
			//		else
			//			TempString = TT_CHAT_BTN_MAXIMIZE;

			//		m_Info.AddString(TempString.c_str());
			//		break;
			//	}
			//default:
			//	break;
			//}
		}
	default:
		break;
	}

	m_Info.SetPosition( pt );
}

void  CToolTipMgr::Draw()
{
	if( m_bHasDrawInfo )
	{
		if( !CDragNDropMgr::GetInstance().IsDraging() )
			m_Info.Draw();
		m_bHasDrawInfo = false;
	}
}

void  CToolTipMgr::RegistInfo( const CInfo& Info )
{
	m_Info.Clear();
	m_Info = Info;
	m_bHasDrawInfo = true;
}