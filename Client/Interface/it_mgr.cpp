
#include "StdAfx.h"
#include "..\\Object.h"
#include "it_mgr.h"
#include "IO_ImageRes.h"
//#include "LookUpImageIndex.h"
#include "InterfaceType.h"
#include "CTDrawImpl.h"
#include "CHelpMgr.h"
#include "CDragNDropMgr.h"
#include "CToolTipMgr.h"
#include "CTDrawImpl.h"

#include "itstate/CITStateNormal.h"
#include "itstate/CITStateRepair.h"
#include "itstate/CITStateAppraisal.h"
#include "itstate/CITStateDead.h"
#include "itstate/CITStateWaitDisconnect.h"
#include "itstate/CITStateNpcDialog.h"
#include "itstate/CITStateRideCart.h"

#include "Icon/CIconDialog.h"

#include "cursor/ccursor.h"
#include "Command/CTCmdHotExec.h"
#include "Command/CTCmdNumberInput.h"
#include "Command/CTCmdOpenDlg.h"
#include "Command/UICommand.h"

#include "Dlgs/CBankWindowDlg.h"
#include "Dlgs/PartyOptionDlg.h"
#include "Dlgs/InputNameDlg.h"
#include "Dlgs/DeliveryStoreDlg.h"
#include "Dlgs/Skilltreedlg.h"
#include "Dlgs/EventDialog.h"
#include "Dlgs/SelectEventDlg.h"
#include "Dlgs/NotifyButtonDlg.h"
#include "Dlgs/NotifyDlg.h"
#include "Dlgs/CMsgBoxSpecial.h"
#include "Dlgs/CChatFilterDlg.h"
#include "Dlgs/ClanRegistNotice.h"
#include "Dlgs/ClanOrganizeDlg.h"
#include "Dlgs/CClanDlg.h"
#include "Dlgs/CUpgradeDlg.h"
#include "Dlgs/CDialogDlg.h"
#include "Dlgs/CMemoViewDlg.h"
#include "Dlgs/CChatRoomDlg.h"
#include "Dlgs/CAddFriendDlg.h"
#include "Dlgs/CMemoDlg.h"
#include "Dlgs/CPrivateChatDlg.h"
#include "Dlgs/CCommDlg.h"

#include "Dlgs/CAvatarStoreDlg.h"
#include "Dlgs/CQuestDlg.h"
#include "Dlgs/CPrivateStoreDlg.h"
#include "Dlgs/CSystemMsgDlg.h"
#include "Dlgs/CNumberInputDlg.h"
#include "Dlgs/COptionDlg.h"
#include "Dlgs/CGoodsDlg.h"
#include "Dlgs/CItemDlg.h"
#include "Dlgs/CTargetMenu.h"
#include "Dlgs/CPartyDlg.h"
#include "Dlgs/CPartyMember.h"
#include "Dlgs/CBankDlg.h"
#include "Dlgs/CMenuDlg.h"
#include "Dlgs/QuickToolBAR.h"
#include "Dlgs/ChattingDLG.h"
#include "Dlgs/MakeDLG.h"
#include "Dlgs/CStoreDLG.h"
#include "Dlgs/DealDLG.h"
#include "Dlgs/CharacterDLG.h"
#include "Dlgs/CMinimapDLG.h"
#include "Dlgs/TargetObjectDLG.h"
#include "Dlgs/CRestartDLG.h"
#include "Dlgs/CSystemDLG.h"
#include "Dlgs/CSkillDLG.h"
#include "Dlgs/ExchangeDLG.h"
#include "Dlgs/HelpDlg.h"
#include "Dlgs/AvatarInfoDlg.h"
#include "Dlgs/CPatDlg.h"
#include "Dlgs/CPrivateChatDlg.h"
#include "Dlgs/CSeparateDlg.h"

#include "cursor/CCursor.h"

#include "../util/CCheat.h"
#include "../IO_Terrain.h"
#include "../Network/CNetwork.h"
#include "../JCommandState.h"	
#include "../System/CGame.h"
#include "../gamedata/cstore.h"
#include "../gamedata/cdealdata.h"
#include "../GameData/CExchange.h"
#include "../GameData/CParty.h"
#include "../GameData/CSeparate.h"
#include "../GameData/CUpgrade.h"
#include "../GameData/CClan.h"

#include "TGameCtrl.h"
#include "TDialog.h"
#include "TListBox.h"
#include "WinCtrl.h"
#include "ResourceMgr.h"
#include "TCaption.h"
#include "TScrollBar.h"
#include "TControlMgr.h"
#include "TEditBox.h"
#include "TCommand.h"
#include "TButton.h"
#include "../IO_Event.h"

#include "../GameCommon/ReloadProcess.h"
#include "../SFX/ISFX.h"
#include "../SFX/SFXManager.h"
#include "../SFX/SFXFont.h"

#include "../Common/IO_QUEST.H"
#include "controls/EffectString.h"
#include <algorithm>


const D3DCOLOR c_dwChatColorAll		= D3DCOLOR_ARGB( 255,255,255,255 );
const D3DCOLOR c_dwChatColorShout	= D3DCOLOR_ARGB( 255,189,250,255 );
const D3DCOLOR c_dwChatColorParty	= D3DCOLOR_ARGB( 255,255,237,140 );
const D3DCOLOR c_dwChatColorWhisper	= D3DCOLOR_ARGB( 255,201,255,144 );
const D3DCOLOR c_dwChatColorNotice	= D3DCOLOR_ARGB( 255,255,188,172 );
const D3DCOLOR c_dwChatColorSystem	= D3DCOLOR_ARGB( 255,255,224,229 );
const D3DCOLOR c_dwChatColorError   = D3DCOLOR_ARGB( 255,255,0,0 );
const D3DCOLOR c_dwChatColorQuest   = D3DCOLOR_ARGB( 255,151, 221, 241 );
const D3DCOLOR c_dwChatColorClan	= D3DCOLOR_ARGB( 255, 255, 228, 122 );
const D3DCOLOR c_dwChatColorQuestReward = D3DCOLOR_ARGB( 255, 224, 162, 209 );


IT_MGR		g_itMGR;
STBDATA		g_TblHELP; 


IT_MGR::IT_MGR()
{
	m_pDialogIconFromMenu = NULL;


	m_pStates[ STATE_NORMAL ]			= new CITStateNormal;
	m_pStates[ STATE_REPAIR ]			= new CITStateRepair;
	m_pStates[ STATE_APPRAISAL ]		= new CITStateAppraisal;
	m_pStates[ STATE_DEAD ]				= new CITStateDead;
	m_pStates[ STATE_WAITDISCONNECT ]	= new CITStateWaitDisconnect;
	m_pStates[ STATE_NPCDIALOG ]		= new CITStateNpcDialog;
	m_pStates[ STATE_RIDECART ]			= new CITStateRideCart;

	m_pCurrState = m_pStates[ STATE_NORMAL ];
	m_pNotifyButtonDlg = NULL;	
}

IT_MGR::~IT_MGR()
{
	SAFE_DELETE( m_pDialogIconFromMenu );

	for( int i= 0 ; i  < STATE_MAX; ++i )
		SAFE_DELETE( m_pStates[ i ] );
	
	m_pCurrState = 0;
	SAFE_DELETE( m_pNotifyButtonDlg );
}

void IT_MGR::RefreshDlg()
{	
	list_dlgs_itor  itorDlgs;
	CTDialog*		pDlg = NULL;
	for( itorDlgs = m_Dlgs.begin(); itorDlgs != m_Dlgs.end(); ++itorDlgs )
	{	
		pDlg = *itorDlgs;
		pDlg->RefreshDlg();
	}	
}

bool IT_MGR::Init( )
{
	InitDLG();

	m_pNotifyButtonDlg = new CNotifyButtonDlg;


	g_TblHELP.Load2("3DDATA\\STB\\HELP.STB",false, false);


	m_listSavedDialog.push_back(DLG_TYPE_ITEM);
	m_listSavedDialog.push_back(DLG_TYPE_CHAR);
	m_listSavedDialog.push_back(DLG_TYPE_SYSTEM);
	m_listSavedDialog.push_back(DLG_TYPE_QUEST);
	m_listSavedDialog.push_back(DLG_TYPE_SKILL);
	m_listSavedDialog.push_back(DLG_TYPE_OPTION);
	m_listSavedDialog.push_back(DLG_TYPE_HELP);

	//// add Observers to Observables


	CTDialog* pDlg = NULL;

	pDlg = FindDlg( DLG_TYPE_UPGRADE );
	if( pDlg )
	{
		CUpgradeDlg* pUpgradeDlg = (CUpgradeDlg*) pDlg;
		CUpgrade::GetInstance().AddObserver( pUpgradeDlg );
	}


	pDlg = FindDlg( DLG_TYPE_SEPARATE );
	if( pDlg )
	{
		CSeparateDlg* pSeparateDlg = (CSeparateDlg*) pDlg;
		CSeparate::GetInstance().AddObserver( pSeparateDlg );
	}

	pDlg = FindDlg( DLG_TYPE_STORE );
	if( pDlg )
	{
		CStoreDLG* pStoreDlg = ( CStoreDLG* )pDlg;
		CStore::GetInstance().AddObserver( pStoreDlg );
	}
	else
	{
		assert( 0 && "Not Found StoreDlg @IT_MGR::Init");
	}


	pDlg = FindDlg( DLG_TYPE_DEAL );
	if( pDlg )
	{
		CDealDLG* pDealDlg = ( CDealDLG* ) pDlg;
		CDealData::GetInstance().AddObserver( pDealDlg );
	}
	else
	{
		assert( 0 && "Not Found DealDlg @IT_MGR::Init");
	}


	pDlg = FindDlg( DLG_TYPE_BANK );
	if( pDlg )
	{
		CBankDlg* pBankDlg = ( CBankDlg* )pDlg;
		CBank::GetInstance().AddObserver( pBankDlg );
	}
	else
	{
		assert( 0 && "Not Found BankDlg @IT_MGR::Init" );
	}

	pDlg = FindDlg( DLG_TYPE_EXCHANGE );
	if( pDlg )
	{
		CExchangeDLG* pExchangeDlg = ( CExchangeDLG*)pDlg;
		CExchange::GetInstance().AddObserver( pExchangeDlg );
	}
	else
	{
		assert( 0 && "Not Found ExchangeDlg @IT-MGR::Init" );
	}

	pDlg = FindDlg( DLG_TYPE_PARTY );
	if( pDlg )
	{
		CPartyDlg* pPartyDlg = (CPartyDlg*)pDlg;
		CParty::GetInstance().AddObserver( pPartyDlg );
	}

	if( pDlg = FindDlg( DLG_TYPE_PARTYOPTION ) )
	{
		CPartyOptionDlg* p = (CPartyOptionDlg*)pDlg;
		CParty::GetInstance().AddObserver( p );
	}

	pDlg = FindDlg( DLG_TYPE_CLAN );
	if( pDlg )
	{
		CClanDlg* pClanDlg = (CClanDlg*)pDlg;
		CClan::GetInstance().AddObserver( pClanDlg );
		
		CClanSkillSlot* pClanSkillSlot = CClan::GetInstance().GetClanSkillSlot();
		pClanSkillSlot->AddObserver( pClanDlg );
	}

	/*m_pReloadProcess = new CReloadProcess();
	if( m_pReloadProcess->LoadReloadTexture() == false )
	{
		assert( 0 && "ReloadProcess loading failed" );
	}*/

	return true;
}

void IT_MGR::Free()
{
	///인터페이스 관련 Profile에 저장될 Data를 g_ClientStorage에 넣고 Save한다.
	CTDialog* pDlg = NULL;
	list_dlgs_itor iter;


	for( iter = m_Dlgs.begin(); iter != m_Dlgs.end(); ++iter )
	{
		pDlg = *iter;
		g_ClientStorage.SetSavedDialogPos( pDlg->GetDialogType(), pDlg->GetPosition());


	/*	if( pDlg->GetDialogType() == DLG_TYPE_CHAT )
		{
			CChatDLG* pChatDlg = (CChatDLG*)pDlg;
			g_ClientStorage.SetChatDlgType( pChatDlg->GetListType() );
		}*/

		/////Quick Type
		if( pDlg->GetDialogType() == DLG_TYPE_QUICKBAR )
		{
			CQuickBAR* pQuickBarDlg = (CQuickBAR*)pDlg;
			g_ClientStorage.SetQuickBarDlgType( pQuickBarDlg->GetType());
		}
#ifdef _NEW_UI
		/////Quick Type 2nd SkillBar
		if( pDlg->GetDialogType() == DLG_TYPE_QUICKBAR_EXT )
		{
			CQuickBAR* pQuickBarDlg = (CQuickBAR*)pDlg;
			g_ClientStorage.SetQuickBarExtDlgType( pQuickBarDlg->GetType());
		}
#endif
	}
	g_TblHELP.Free();
	FreeDLG();
	g_ClientStorage.Save();
}


UINT IT_MGR::MsgProc(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if( m_pNotifyButtonDlg->Process( uiMsg, wParam ,lParam ) )
		return uiMsg;

	return m_pCurrState->Process( uiMsg, wParam ,lParam );
}


// *----------------------------------------------------------------------------------* //



void IT_MGR::Update()
{
	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

	ProcCommand();

	if( m_pCurrState->Draw() )
		DrawDLGs( ptMouse );


	m_pCurrState->Update( ptMouse );
	//g_DragNDrop.DrawDRAG_Item();	
	CDragNDropMgr::GetInstance().Draw( ptMouse );
	CCursor::GetInstance().Draw( ptMouse );
}

void IT_MGR::InitDLG()
{	
	CBankWindowDlg* pBankWindowDlg = new CBankWindowDlg;
	pBankWindowDlg->Create( "DlgBankWindow" );
	AppendDlg( DLG_TYPE_BANKWINDOW, pBankWindowDlg, pBankWindowDlg->GetControlID() );

	CPartyOptionDlg* pPartyOptionDlg = new CPartyOptionDlg;
	pPartyOptionDlg->Create("DlgPartyOption" );
	AppendDlg( DLG_TYPE_PARTYOPTION, pPartyOptionDlg, pPartyOptionDlg->GetControlID() );

	CInputNameDlg* pInputNameDlg = new CInputNameDlg;
	pInputNameDlg->Create( "DlgInputName" );
	AppendDlg( DLG_TYPE_INPUTNAME, pInputNameDlg, pInputNameDlg->GetControlID() );

	CDeliveryStoreDlg* pDeliveryStoreDlg = new CDeliveryStoreDlg;
	pDeliveryStoreDlg->Create("DeliveryStore");
	AppendDlg( DLG_TYPE_DELIVERYSTORE, pDeliveryStoreDlg, pDeliveryStoreDlg->GetControlID() );

	CSkillTreeDlg* pSkilltreedlg = new CSkillTreeDlg;
	pSkilltreedlg->Create( "dlgskilltree" );
	AppendDlg( DLG_TYPE_SKILLTREE, pSkilltreedlg, pSkilltreedlg->GetControlID() );

//	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
//	pFileSystem->OpenFile( "3DData\\Control\\xml\\dlgnotify.xml", OPEN_READ_BIN );
//	pFileSystem->ReadToMemory();
	CNotifyDlg* pNotifyDlg = new CNotifyDlg;
//	pNotifyDlg->Create( (const char*)pFileSystem->GetData() );
	pNotifyDlg->Create( "dlgnotify" );
	AppendDlg( DLG_TYPE_NOTIFY, pNotifyDlg , pNotifyDlg->GetControlID());
//	pFileSystem->CloseFile();

	CEventDialog* pEventDlg = new CEventDialog;
	pEventDlg->Create( "DlgDialogEvent" );
	AppendDlg( DLG_TYPE_EVENTDIALOG, pEventDlg, pEventDlg->GetControlID() );
	///루나행성의 필드 워프창같은 특정 Event를 선택하는데 사용되는 창
	CSelectEventDlg* pSelectEventDlg = new CSelectEventDlg;
	pSelectEventDlg->Create( "DlgSelectEvent" );
	AppendDlg( DLG_TYPE_SELECTEVENT, pSelectEventDlg, pSelectEventDlg->GetControlID() );


	CMsgBoxSpecial* pSpecialMsgBox = new CMsgBoxSpecial;
	pSpecialMsgBox->Create( "MsgBox" );
	AppendDlg( DLG_TYPE_MSGBOX_SPECIAL, pSpecialMsgBox, pSpecialMsgBox->GetControlID() );

	//DLG_TYPE_MSGBOX_SPECIAL
	CChatFilterDlg* pFilterDlg = new CChatFilterDlg;
	pFilterDlg->Create("DlgChatFilter" );
	AppendDlg( DLG_TYPE_CHATFILTER, pFilterDlg, pFilterDlg->GetControlID() );

	CClanRegistNotice* pClanRegistNotice = new CClanRegistNotice;
	pClanRegistNotice->Create("DlgClanRegNotice");
	AppendDlg( DLG_TYPE_CLAN_NOTICE, pClanRegistNotice, pClanRegistNotice->GetControlID() );
	

	CClanOrganizeDlg* pClanOrganizeDlg = new CClanOrganizeDlg;
	pClanOrganizeDlg->Create( "DlgOrganizeClan" );
	AppendDlg( DLG_TYPE_CLAN_ORGANIZE, pClanOrganizeDlg, pClanOrganizeDlg->GetControlID() );

	/*CClanRegistNotice* pClanRegistNotice = new CClanRegistNotice;
	pClanRegistNotice->Create( "DlgClan*/

	CClanDlg* pClanDlg = new CClanDlg;
	pClanDlg->Create( "DlgClan" );
	AppendDlg( DLG_TYPE_CLAN,pClanDlg, pClanDlg->GetControlID() );

	CUpgradeDlg* pUpgradeDlg = new CUpgradeDlg( DLG_TYPE_UPGRADE );
	pUpgradeDlg->Create( "DlgUpgrade");
	AppendDlg( DLG_TYPE_UPGRADE, pUpgradeDlg, pUpgradeDlg->GetControlID() );


	CDialogDlg*	pDialogDlg = new CDialogDlg;
	pDialogDlg->Create("DlgDialog");
	AppendDlg( DLG_TYPE_DIALOG, pDialogDlg, pDialogDlg->GetControlID() );

	CSeparateDlg* pSeparateDlg = new CSeparateDlg;
	pSeparateDlg->Create( "DlgSeparate" );
	AppendDlg( DLG_TYPE_SEPARATE, pSeparateDlg, pSeparateDlg->GetControlID() );

	CMemoViewDlg* pMemoViewDlg = new CMemoViewDlg;
	pMemoViewDlg->Create( "DlgMemoView" );
	AppendDlg( DLG_TYPE_MEMOVIEW, pMemoViewDlg, pMemoViewDlg->GetControlID() );

	CChatRoomDlg*	pChatroomDlg = new CChatRoomDlg( DLG_TYPE_CHATROOM );
	pChatroomDlg->Create( "DlgChatRoom" );
	AppendDlg( DLG_TYPE_CHATROOM, pChatroomDlg, pChatroomDlg->GetControlID() );

	//CPrivateChatDlg* pPrivateChatDlg = new CPrivateChatDlg( DLG_TYPE_PRIVATECHAT );
	//pPrivateChatDlg->Create( "DlgPrivateChat" );
	//AppendDlg( DLG_TYPE_PRIVATECHAT, pPrivateChatDlg, pPrivateChatDlg->GetControlID () );

	CMemoDlg*	pMemoDlg = new CMemoDlg( DLG_TYPE_MEMO );
	pMemoDlg->Create( "DlgMemo" );
	AppendDlg( DLG_TYPE_MEMO, pMemoDlg, pMemoDlg->GetControlID() );

	CAddFriendDlg*	pAddFriendDlg = new CAddFriendDlg( DLG_TYPE_ADDFRIEND );
	pAddFriendDlg->Create("DlgAddFriend");
	AppendDlg( DLG_TYPE_ADDFRIEND, pAddFriendDlg, pAddFriendDlg->GetControlID() );

	CCommDlg*	pCommDlg = new CCommDlg( DLG_TYPE_COMMUNITY );
	pCommDlg->Create("DlgComm" );
	AppendDlg( DLG_TYPE_COMMUNITY, pCommDlg, pCommDlg->GetControlID() );

	CAvatarStoreDlg* pAvatarStoreDlg = new CAvatarStoreDlg( DLG_TYPE_AVATARSTORE );
	pAvatarStoreDlg->Create("DlgAvatarStore");
	AppendDlg( DLG_TYPE_AVATARSTORE, pAvatarStoreDlg, pAvatarStoreDlg->GetControlID() );

	CGoodsDlg* pGoodsDlg = new CGoodsDlg( DLG_TYPE_GOODS );
	pGoodsDlg->Create("DlgGoods" );
	AppendDlg( DLG_TYPE_GOODS, pGoodsDlg, pGoodsDlg->GetControlID() );

	CPrivateStoreDlg* pPrivateStoreDlg = new CPrivateStoreDlg( DLG_TYPE_PRIVATESTORE );
	pPrivateStoreDlg->Create( "DlgPrivateStore" );
	AppendDlg( DLG_TYPE_PRIVATESTORE, pPrivateStoreDlg, pPrivateStoreDlg->GetControlID() );

	CItemDlg*	pItemDlg = new CItemDlg( DLG_TYPE_ITEM );
	pItemDlg->Create( "DlgItem" );
	AppendDlg( DLG_TYPE_ITEM, pItemDlg, pItemDlg->GetControlID() );

	CRestartDLG* pRestartDLG = new CRestartDLG;
	pRestartDLG->Create("DlgRestart");
	AppendDlg( DLG_TYPE_RESTART, pRestartDLG , pRestartDLG->GetControlID() );

	// 기본창 
	CMenuDlg* pMenuDlg = new CMenuDlg;
	pMenuDlg->Create( "DlgMenu" );
	AppendDlg( DLG_TYPE_MENU, pMenuDlg, pMenuDlg->GetControlID() );


	// 시스템 다이알로그
	CSystemDLG* pSystemDLG = new CSystemDLG;
	pSystemDLG->Create("DlgSystem");
	AppendDlg( DLG_TYPE_SYSTEM, pSystemDLG, pSystemDLG->GetControlID() );

	
	
	// 미니맵 다이알로그
	CMinimapDLG* pMinimapDLG = new CMinimapDLG;
	pMinimapDLG->Create("DlgMinimap");
	AppendDlg( DLG_TYPE_MINIMAP, pMinimapDLG, pSystemDLG->GetControlID() );
	
	
	//인벤토리 
	//CInventoryDLG* pAvtInvDLG = new CInventoryDLG( DLG_TYPE_INVENTORY );
	//pAvtInvDLG->Create("DlgInven");
	//AppendDlg( DLG_TYPE_INVENTORY, pAvtInvDLG, pAvtInvDLG->GetControlID() );

	

	//상점 
	CStoreDLG* pStoreDLG = new CStoreDLG( DLG_TYPE_STORE );
	pStoreDLG->Create("DlgStore");
	AppendDlg( DLG_TYPE_STORE, pStoreDLG , pStoreDLG->GetControlID() );

	//Deal
	CDealDLG* pDealDLG = new CDealDLG( DLG_TYPE_DEAL );
	pDealDLG->Create("DlgDeal");
	AppendDlg( DLG_TYPE_DEAL, pDealDLG, pDealDLG->GetControlID() );


	//캐릭터창 

	CCharacterDLG* pCharDLG = new CCharacterDLG( DLG_TYPE_CHAR );
	pCharDLG->Create("DlgAvata");
	AppendDlg( DLG_TYPE_CHAR, pCharDLG, pCharDLG->GetControlID() );

	
	// 타겟 오브젝트 다이알로그
	/*CTargetObjectDLG* pTargetObjectDLG = new CTargetObjectDLG;
	pTargetObjectDLG->Create("DlgTarget");
	AppendDlg( DLG_TYPE_TARGET_OBJECT, pTargetObjectDLG, pTargetObjectDLG->GetControlID() );*/


	//퀵바 
	CQuickBAR* pQuickBAR = new CQuickBAR( DLG_TYPE_QUICKBAR );
	pQuickBAR->Create("DlgQuickBar");
	AppendDlg( DLG_TYPE_QUICKBAR, pQuickBAR, pQuickBAR->GetControlID() );


	//Ext QuickBar
#ifdef _NEW_UI 
	
	pQuickBAR = new CQuickBAR( DLG_TYPE_QUICKBAR_EXT );
	pQuickBAR->Create("DlgQuickBar");
	pQuickBAR->SetQuickBarType( QUICKBAR_TYPE_EXTENSION );
	AppendDlg( DLG_TYPE_QUICKBAR_EXT, pQuickBAR, pQuickBAR->GetControlID() );
#endif

	CQuestDlg* pQuestDlg = new CQuestDlg;
	pQuestDlg->Create("DlgQuest");
	AppendDlg( DLG_TYPE_QUEST, pQuestDlg, pQuestDlg->GetControlID() );

	//스킬창 
	CSkillDLG* pSkillDLG = new CSkillDLG( DLG_TYPE_SKILL );
	pSkillDLG->Create("DlgSkill");
	AppendDlg( DLG_TYPE_SKILL, pSkillDLG,pSkillDLG->GetControlID() );


	CNumberInputDlg* pNInputDlg = new CNumberInputDlg;
	pNInputDlg->Create("DlgNInput");
	AppendDlg( DLG_TYPE_N_INPUT, pNInputDlg, pNInputDlg->GetControlID() );


	CSystemMsgDlg* pSystemMsgDlg = new CSystemMsgDlg;
	AppendDlg( DLG_TYPE_SYSTEMMSG, pSystemMsgDlg, pSystemMsgDlg->GetControlID() );


	///옵션 설정 창
	// <Option Dialog>
	///*
	COptionDlg* pOptionDlg = new COptionDlg;
	pOptionDlg->Create("DlgOption");
	AppendDlg( DLG_TYPE_OPTION, pOptionDlg, pOptionDlg->GetControlID() );
	//*/
	// </Option Dialog>


	///아이템 교환 창
	CExchangeDLG* pExchangeDlg = new CExchangeDLG( DLG_TYPE_EXCHANGE );
	pExchangeDlg->Create( "DlgExchange");
	AppendDlg( DLG_TYPE_EXCHANGE, pExchangeDlg, pExchangeDlg->GetControlID() );


	///파티창
	CPartyDlg* pPartyDlg = new CPartyDlg;
	pPartyDlg->Create("DlgParty");
	AppendDlg( DLG_TYPE_PARTY, pPartyDlg, pPartyDlg->GetControlID() );



	CMakeDLG*	pMakeDlg = new CMakeDLG( DLG_TYPE_MAKE );
	pMakeDlg->Create("DlgMake");
	AppendDlg( DLG_TYPE_MAKE, pMakeDlg, pMakeDlg->GetControlID() );


	///REMARK TEST CODE
	CBankDlg* pBankDlg = new CBankDlg( DLG_TYPE_BANK );
	pBankDlg->Create("DlgBank");
	AppendDlg( DLG_TYPE_BANK, pBankDlg, pBankDlg->GetControlID() );

	
	CHelpDlg*	pHelpDlg = new CHelpDlg;
	pHelpDlg->Create( "DlgHelp" );
	AppendDlg( DLG_TYPE_HELP, pHelpDlg, pHelpDlg->GetControlID() );


	CAvatarInfoDlg* pInfoDlg = new CAvatarInfoDlg( DLG_TYPE_INFO );
	pInfoDlg->Create("DlgInfo");
	AppendDlg( DLG_TYPE_INFO, pInfoDlg, pInfoDlg->GetControlID() );



	//CPatDlg* pPatDlg = new CPatDlg( DLG_TYPE_PAT );
	//pPatDlg->Create("DlgPat");
	//AppendDlg( DLG_TYPE_PAT, pPatDlg, pPatDlg->GetControlID());

	// 채팅창 
	CChatDLG* pChatDLG = new CChatDLG;
	pChatDLG->Create("DlgChat");
	AppendDlg( DLG_TYPE_CHAT, pChatDLG, pChatDLG->GetControlID() );

	MoveDlg2ListEnd( pChatDLG );

	InitInterfacePos();

	if( g_ClientStorage.HasSavedDialogPos() )
	{		
		SetInterfacePosBySavedData();
	}

#ifdef _NEWUI
	SetInterfacePos_After();
#endif
	

	return;
}

void IT_MGR::SetInterfacePos_After()
{
	CTDialog* pDlg = NULL;
	list_dlgs_itor iter;
	for( iter = m_Dlgs.begin(); iter != m_Dlgs.end(); ++iter )
	{
		pDlg = *iter;
        pDlg->SetInterfacePos_After();		
	}
}

void IT_MGR::FreeDLG()
{
	CTDialog* pDlg = NULL;
	if( g_pAVATAR )
	{
		if( pDlg = FindDlg( DLG_TYPE_ITEM ) )
		{
			CItemDlg* pItemDlg = (CItemDlg*)pDlg;
			std::list<S_InventoryData> list;
			pItemDlg->GetVirtualInventory( list );
			///CClientDB::GetInstance().SetInventoryData( g_pAVATAR->Get_NAME(), list );
			g_ClientStorage.SetInventoryData( g_pAVATAR->Get_NAME(), list );
		}
	}

	list_dlgs_itor iterDlg;

	for( iterDlg = m_Dlgs.begin(); iterDlg != m_Dlgs.end(); )
	{
		pDlg = *iterDlg;
		delete pDlg;
		iterDlg = m_Dlgs.erase( iterDlg );
	}

	std::list< CIconDialog* >::iterator iterIcon;
	for( iterIcon = m_Icons.begin(); iterIcon != m_Icons.end(); )
	{
		delete *iterIcon;
		iterIcon = m_Icons.erase( iterIcon );
	}
//	SAFE_DELETE( m_pDrawImpl );


}


void IT_MGR::DrawDLGs( POINT ptMouse )
{
	g_HelpMgr.UpdateFrame();

	/// Control들이 내부적으로 처리하는 변수 초기화 
	/// Update시 MouseOver처리 변수 초기화
	CWinCtrl::SetProcessMouseOver( false );
	CWinCtrl::SetProcessMouseOverCtrl( NULL );
	CTDialog::SetProcessMouseOverDialog( NULL );
	///
	CTDialog*		pDlg = NULL;


	for_each( m_Dlgs.rbegin(), m_Dlgs.rend(), bind2nd( mem_fun( &CTDialog::Update ), ptMouse ));
	for_each( m_Icons.rbegin(), m_Icons.rend(), bind2nd( mem_fun( &CIcon::Update), ptMouse ));
	m_pNotifyButtonDlg->Update( ptMouse );

	for_each( m_Icons.begin(), m_Icons.end(), mem_fun( &CIcon::Draw ));
	for_each( m_Dlgs.begin(), m_Dlgs.end(), mem_fun( &CTDialog::Draw ));
	m_pNotifyButtonDlg->Draw();


	if( !IsMouseOnInterface() )
	{
		CInfo& 	MouseInfo = CCursor::GetInstance().GetMouseInfo();
		ptMouse.y -= 15;


		if( MouseInfo.GetHeight() )
		{
			MouseInfo.SetPosition( ptMouse );
			CToolTipMgr::GetInstance().RegistInfo( MouseInfo );
		}
	}

	g_pAVATAR->m_EndurancePack.Draw();

	CToolTipMgr::GetInstance().Draw();


//	m_Indicate.Update( ptMouse );
//	m_Indicate.Draw();
}


CChatDLG*	IT_MGR::GetChatDLG()
{
	CChatDLG* pDLG = (CChatDLG*)FindDlg( DLG_TYPE_CHAT );
	return pDLG;
}


CDialogDlg*  IT_MGR::GetQueryDLG()
{
	CDialogDlg* pDLG = (CDialogDlg*)FindDlg( DLG_TYPE_DIALOG );
	return pDLG;
}



CQuickBAR*	IT_MGR::GetQuickBAR()
{
	CQuickBAR*	pDLG = (CQuickBAR*)FindDlg( DLG_TYPE_QUICKBAR );
	return pDLG;
}

#ifdef _NEW_UI // 2nd SkillBar
CQuickBAR*	IT_MGR::GetQuickBAR_EXT()
{
	CQuickBAR*	pDLG = (CQuickBAR*)FindDlg( DLG_TYPE_QUICKBAR_EXT );
	return pDLG;
}
#endif

/// 미니맵 다이알로그를 얻어온다.
CMinimapDLG* IT_MGR::GetMinimapDLG()
{
	CMinimapDLG* pDLG = (CMinimapDLG*)FindDlg( DLG_TYPE_MINIMAP );
	return pDLG;
}



/// 거래창을 얻어온다.
CExchangeDLG*	IT_MGR::GetExchangeDLG()
{
	CExchangeDLG* pDLG = (CExchangeDLG*)FindDlg( DLG_TYPE_EXCHANGE );
	return pDLG;
}


///
/// Open, close dlgs..
///
bool IT_MGR::OpenMakeDlg( int iSkillSlotIdx )
{
	if( !IsDlgOpened( DLG_TYPE_MAKE ) )///중복되서 여는것을 방지한다.
	{
		CManufacture::GetInstance().SetSkillSlotIndex( iSkillSlotIdx );

		OpenDialog( DLG_TYPE_MAKE, false);
		OpenDialog( DLG_TYPE_ITEM, false );
	}
	else
	{
		OpenMsgBox(STR_USING_MAKESKILL);
		return false;
	}
	return false;
}
void IT_MGR::OpenDialogBySkill( int iSkillSlotIdx , int iTargetDialogID )
{
	switch( iTargetDialogID )
	{
	case DLG_TYPE_MAKE:
		OpenMakeDlg( iSkillSlotIdx );
		break;
	case DLG_TYPE_SEPARATE:
		{
			CSeparate::GetInstance().SetSkillSlotIdx( iSkillSlotIdx );
			g_itMGR.OpenDialog( DLG_TYPE_ITEM , false );
			g_itMGR.OpenDialog( DLG_TYPE_SEPARATE, false );
			break;
		}
	case DLG_TYPE_UPGRADE:
		{
			CUpgrade::GetInstance().SetSkillSlotIdx( iSkillSlotIdx );
			g_itMGR.OpenDialog( DLG_TYPE_ITEM, false );
			g_itMGR.OpenDialog( DLG_TYPE_UPGRADE, false );
			break;
		}
	default:
		break;
	}
}

bool IT_MGR::OpenQueryDLG(short nSX,short nSY,char* szQuery,short nMaxCharLen,int iEventID,void (*fpCloseProc)(int iEventID), int iEventOwner, int iEventDlgType )
{
	CDialogDlg* pDlg = NULL;

	switch( iEventDlgType )
	{
		/// NPC 대화창
		case EVENT_DLG_NPC:
			{
				/// 팻탑승시는 열지 않는다.
				if( g_pAVATAR->GetPetMode() > 0 )
					return true;

				pDlg = (CDialogDlg*)FindDlg( DLG_TYPE_DIALOG );
				if( !pDlg && pDlg->IsInValidShow() )
					return false;

				pDlg->SetScript( szQuery );
				CObjCHAR *pTarget = (CObjCHAR*)g_pAVATAR->Get_TARGET();
				if( pTarget )
					pDlg->SetTargetNpcClientObjectIndex( pTarget->Get_INDEX() );

				OpenDialog( DLG_TYPE_DIALOG , false );	
			}

			break;
		/// Warp DLG
		case EVENT_DLG_WARP:
			{
				pDlg = (CDialogDlg*)FindDlg( DLG_TYPE_SELECTEVENT );
				if( !pDlg && pDlg->IsInValidShow() )
					return false;

				CSelectEventDlg* pEventDlg = (CSelectEventDlg*)pDlg;
				pEventDlg->SetTitle( szQuery );
				pEventDlg->SetTargetClientObjectIndex( iEventOwner );
				OpenDialog( DLG_TYPE_SELECTEVENT, false );
			}
			break;
		/// Event Object Dlg
		case EVENT_DLG_EVENT_OBJECT:
			{
				pDlg = (CDialogDlg*)FindDlg( DLG_TYPE_EVENTDIALOG );
				if( !pDlg && pDlg->IsInValidShow() )
					return false;

				CEventDialog* pEventDlg = (CEventDialog*)pDlg;
				pEventDlg->SetScript( szQuery );
				pEventDlg->SetTargetNpcClientObjectIndex( iEventOwner );
				OpenDialog( DLG_TYPE_EVENTDIALOG, false );
			}
			break;
	}	

	return true;
}

bool IT_MGR::QueryDLG_AppendExam(char* szExam,int iEventID,void (*fpExamEvent)(int iEventID), int iType )
{
	switch( iType )
	{
	case QUERYTYPE_ITEM:
		{
			if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_EVENTDIALOG ) )
			{
				CEventDialog* pEventDlg = (CEventDialog*)pDlg;
				pEventDlg->AddAnswerExample( szExam, iEventID, fpExamEvent );
			}
		}
		break;
	case QUERYTYPE_NPC:
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DIALOG );
			if( pDlg )
			{
				CDialogDlg* pDialogDlg = (CDialogDlg*)pDlg;
				pDialogDlg->AddAnswerExample( szExam , iEventID, fpExamEvent );
				return true;
			}
		}
		break;
	case QUERYTYPE_SELECT:
		{
			if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_SELECTEVENT ) )
			{
				CSelectEventDlg* pSelectDlg = (CSelectEventDlg*)pDlg;
				pSelectDlg->AddEvent( szExam, iEventID, fpExamEvent );
			}
		}
		break;
	default:
		break;
	}

	return false;
}

void IT_MGR::CloseQueryDlg( )
{
	///Query관련 다 닫아버린다.
	CloseDialog( DLG_TYPE_DIALOG );
	CloseDialog( DLG_TYPE_SELECTEVENT );
	CloseDialog( DLG_TYPE_EVENTDIALOG );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief CHotIconSlot 정보를 바탕으로 QuickDlg 의 슬롯들을 업데이트
//----------------------------------------------------------------------------------------------------

void IT_MGR::UpdateQuickSlot()
{
	CQuickBAR* pDLG = NULL;
	pDLG = GetQuickBAR();
	if(pDLG) 
	{
		pDLG->UpdateHotIconSlot();
	}
#ifdef _NEW_UI // 2nd SkillBar
	pDLG = GetQuickBAR_EXT();
	if(pDLG) 
	{
		pDLG->UpdateHotIconSlot();
	}
#endif
}


short IT_MGR::GetItemInventoryTAB(tagITEM sItem)
{
	short nType = sItem.GetTYPE();
	
	switch(nType)
	{
	case ITEM_TYPE_FACE_ITEM:
	case ITEM_TYPE_HELMET:
	case ITEM_TYPE_ARMOR:
	case ITEM_TYPE_GAUNTLET:
	case ITEM_TYPE_BOOTS:
	case ITEM_TYPE_KNAPSACK:
	case ITEM_TYPE_WEAPON:
	case ITEM_TYPE_SUBWPN:
	case ITEM_TYPE_JEWEL:
		return 0;
	case ITEM_TYPE_USE:
		return 1;
	case ITEM_TYPE_ETC:
	case ITEM_TYPE_NATURAL:
	case ITEM_TYPE_QUEST:
		return 2;
	}
	return 0;
}

void IT_MGR::Close_store()
{
	// 상점 닫기 
	CStoreDLG* pStore = (CStoreDLG*)FindDlg( DLG_TYPE_STORE );
	if( pStore )
	{
		pStore->SetTab(0);
		pStore->Hide();
	}
	
	// 거래창 닫기 
	CDealDLG* pDeal = (CDealDLG*)FindDlg( DLG_TYPE_DEAL );
	
	if( pDeal )
		pDeal->Hide();

	///숫자입력기가 떠 있다면 닫는다.
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_N_INPUT );
	if( pDlg && pDlg->IsVision() )
		pDlg->Hide();

}

void IT_MGR::InsertDlg(short nDlgType, CTDialog* pDlg,  int iNextID , int iID )
{
	list_dlgs_itor iter;
	for( iter = m_Dlgs.begin(); iter != m_Dlgs.end(); ++iter )
	{
		if( iNextID == (*iter)->GetControlID() )
		{
			pDlg->SetDialogType( nDlgType );
			pDlg->SetControlID( iID );
			m_Dlgs.insert( iter, pDlg );///해당 Dialog앞으로 넣는다.
			return;
		}
	}
	AppendDlg( nDlgType, pDlg, iID );
}

void IT_MGR::AppendDlg( short nDlgType, CTDialog* pDlg ,int iID )
{
	pDlg->SetDialogType( nDlgType );
	pDlg->SetControlID( iID );
	if( pDlg->IsModal() )
		m_Dlgs.push_back( pDlg );
	else
	{
		list_dlgs_itor iter;
		for( iter = m_Dlgs.begin(); iter != m_Dlgs.end(); ++iter )
		{
			CTDialog* pDlg = *iter;
			if( pDlg->IsVision() && pDlg->IsModal() )
				break;
		}
		m_Dlgs.insert( iter, pDlg );
	}
}

CTDialog* IT_MGR::FindDlg( DWORD dwDlgType )
{
	/// REMARK : NEW UI LIB ->
	list_dlgs_itor  itorDlgs;
	CTDialog*		pDlg = NULL;
	for( itorDlgs = m_Dlgs.begin(); itorDlgs != m_Dlgs.end(); ++itorDlgs )
	{
		///REMARK : For Debuging
		pDlg = *itorDlgs;
		if( pDlg->GetDialogType() == dwDlgType )
			return pDlg;
	}
	///<-
	return NULL;
}

int  IT_MGR::IsDlgOpened( int iDlgID )
{
	CTDialog* pDlg = FindDlg( (short)iDlgID );
	if( pDlg && pDlg->IsVision() )
		return 1;

	return 0;
}

///CMsgBox의 경우 같은 Type으로 여러개가 있을수 있다.
void IT_MGR::MoveDlg2ListEnd( CTDialog* pDlg )
{
	list_dlgs_itor  itorDlgs;
	for( itorDlgs = m_Dlgs.begin(); itorDlgs != m_Dlgs.end(); ++itorDlgs )
	{
		///REMARK : For Debuging
		if( *itorDlgs == pDlg )
		{
			m_Dlgs.erase( itorDlgs );
			m_Dlgs.push_back( pDlg );
			return;
		}
	}
	///<-
}
void IT_MGR::MoveDlgIcon2ListEnd( CIconDialog* pIcon )
{
	std::list<CIconDialog*>::iterator iter;
	for( iter = m_Icons.begin(); iter != m_Icons.end(); ++iter )
	{
		if( *iter == pIcon )
		{
			m_Icons.erase( iter );
			m_Icons.push_back( pIcon );
			return;
		}
	}
}
//
//void IT_MGR::AppendChatMsg( const char* pMsg,D3DCOLOR dwCol )
//{
//	CTDialog* pDlg = FindDlg( DLG_TYPE_CHAT );
//	if( pDlg == NULL )
//	{
//		LogString(LOG_DEBUG,"Not Found ChatDlg/IT_MGR::AppendChatMsg\n");
//		return;
//	}
//	CChatDLG* pChatDlg = ( CChatDLG* ) pDlg;
//	pChatDlg->AppendMsg( pMsg, dwCol, 0);
//}

void IT_MGR::AppendChatMsg( const char* pszMsg, int iType ,DWORD forceapply_color)
{
	CTDialog* pDlg = FindDlg( DLG_TYPE_CHAT );
	if( pDlg == NULL )
	{
		LogString(LOG_DEBUG,"Not Found ChatDlg/IT_MGR::AppendChatMsg\n");
		return;
	}
	
	CChatDLG* pChatDlg = ( CChatDLG* ) pDlg;
	DWORD dwColor = c_dwChatColorAll;

	int iFilterType = CChatDLG::FILTER_NORMAL;

	switch( iType )
	{
	case CHAT_TYPE_SHOUT:
		if( forceapply_color )
			dwColor = forceapply_color;
		else
			dwColor = c_dwChatColorShout;

		iFilterType = CChatDLG::FILTER_NORMAL;
		break;
	case CHAT_TYPE_PARTY:
		if( forceapply_color )
			dwColor = forceapply_color;
		else
			dwColor = c_dwChatColorParty;

		iFilterType = CChatDLG::FILTER_PARTY;
		break;
	case CHAT_TYPE_WHISPER:
		if( forceapply_color )
			dwColor = forceapply_color;
		else
			dwColor = c_dwChatColorWhisper;

		iFilterType = CChatDLG::FILTER_WHISPER;
		break;
	case CHAT_TYPE_NOTICE:
		if( forceapply_color )
			dwColor = forceapply_color;
		else
			dwColor = c_dwChatColorNotice;

		iFilterType = CChatDLG::FILTER_SYSTEM;
#ifdef FRAROSE
		pChatDlg->AppendMsg2( pszMsg, dwColor, iFilterType );
		return;
#endif
		break;
	case CHAT_TYPE_SYSTEM:
		if( forceapply_color )
			dwColor = forceapply_color;
		else
			dwColor = c_dwChatColorSystem;

		iFilterType = CChatDLG::FILTER_SYSTEM;
#ifdef FRAROSE
		pChatDlg->AppendMsg2( pszMsg, dwColor, iFilterType );
		return;
#endif
		break;
	case CHAT_TYPE_QUEST:
		if( forceapply_color )
			dwColor = forceapply_color;
		else
			dwColor = c_dwChatColorQuest;
		
		iFilterType = CChatDLG::FILTER_SYSTEM;
#ifdef FRAROSE
		pChatDlg->AppendMsg2( pszMsg, dwColor, iFilterType );
		return;
#endif
		break;
	case CHAT_TYPE_QUESTREWARD:
		if( forceapply_color )
			dwColor = forceapply_color;
		else
			dwColor = c_dwChatColorQuestReward;

		iFilterType = CChatDLG::FILTER_SYSTEM;
#ifdef FRAROSE
		pChatDlg->AppendMsg2( pszMsg, dwColor, iFilterType );
		return;
#endif
		break;
	case CHAT_TYPE_TRADE:
		if( forceapply_color )
			dwColor = forceapply_color;
		else
			dwColor = c_dwChatColorAll;
		iFilterType = CChatDLG::FILTER_TRADE;
		break;
	case CHAT_TYPE_CLAN:
		if( forceapply_color )
			dwColor = forceapply_color;
		else
			dwColor = c_dwChatColorClan;
		iFilterType = CChatDLG::FILTER_CLAN;
		break;
	case CHAT_TYPE_ALLIED:
		if( forceapply_color )
			dwColor = forceapply_color;
		else
			dwColor = c_dwChatColorClan;
		iFilterType = CChatDLG::FILTER_ALLIED;
		break;
	case CHAT_TYPE_FRIEND:
		if( forceapply_color )
			dwColor = forceapply_color;
		else
			dwColor = c_dwChatColorWhisper;

		iFilterType = CChatDLG::FILTER_SYSTEM;
		break;
	case CHAT_TYPE_ALL:
	default:
		if( forceapply_color )
			dwColor = forceapply_color;
		else
			dwColor = c_dwChatColorAll;
		iFilterType = CChatDLG::FILTER_NORMAL;
		break;
	}

	pChatDlg->AppendMsg( pszMsg, dwColor, iFilterType );
}

//bool IT_MGR::PacketProc( t_PACKET * pRecvPacket )
//{
//	if( pRecvPacket == NULL) return false;
//
//	//switch( pRecvPacket->m_HEADER.m_wType )
//	//{
//	/////파티가 결성되거나 조인되었을경우 서버에서 내려온다.
//	/////파티창을 Show하는곳은 항상 이곳이다.
//	//case GSV_PARTY_MEMBER:
//	//	{
//	//		break;
//	//	}
//	//default:
//	//	return false;
//	//}
//	return true;
//}

/// Invoker Dialog ID = iDlgID
void IT_MGR::ShowNumberInputDlg( CTCmdNumberInput* pCmd, CTObject* pCmdParam ,__int64 iMaxNumber )
{
	CTDialog* pDlg = NULL;
	pDlg = FindDlg( DLG_TYPE_N_INPUT );
	if( pDlg )
	{
		CNumberInputDlg* pNInputDlg = ( CNumberInputDlg*) pDlg;
		
		pNInputDlg->SetMaxNumber( iMaxNumber );
		pNInputDlg->SetCommand( pCmd, pCmdParam );
		
		pDlg->Show();
		MoveDlg2ListEnd( pDlg );
	}
}

bool IT_MGR::AddTCommand( int iDlgID, CTCommand* pCmd )
{
	if( pCmd == NULL )
		return false;

	//즉시 실행 커맨드
	if( iDlgID == 0 )
	{
		pCmd->Exec(NULL);

#ifndef __SRVDISCONNECTRELOGIN
		delete pCmd;
#endif
	
	}
	else if( iDlgID == DLG_TYPE_MAX )///IT_MGR고 CommandQ를 가지게 수정된다. Q에 넣고Update에서 실행시키자
	{
		m_CommandQ.Push_Back( pCmd );
	}
	else
	{
		CTDialog* pDlg = FindDlg( iDlgID );
		if( pDlg == NULL )
		{
			delete pCmd;
			return false;
		}
		pDlg->Push_Back_Cmd( pCmd );
	}
	return true;
}

bool IT_MGR::OpenInputNameDlg( CTCmdInputName* pCmd )
{
	if( IsDlgOpened( DLG_TYPE_INPUTNAME ) ) return false;
	if( CTDialog* pDlg = FindDlg( DLG_TYPE_INPUTNAME ) )
	{
		CInputNameDlg* p = (CInputNameDlg*)pDlg;
		p->SetCommand( pCmd );
		OpenDialog( DLG_TYPE_INPUTNAME );
		return true;
	}
	return false;
}

void IT_MGR::OpenMsgBox( const char* szMsg, short int iButtonType, bool bModal, unsigned int iInvokerDlgID , CTCommand* pCmdOk, CTCommand* pCmdCancel, int iMsgType )
{
	CreateMsgBoxData Data;
	Data.bModal			= bModal;
	Data.iButtonType	= iButtonType;
	Data.iInvokerDlgID  = iInvokerDlgID;
	Data.iMsgType		= iMsgType;
	Data.pCmdCancel     = pCmdCancel;
	Data.pCmdOk			= pCmdOk;
	Data.strMsg			= szMsg;
	CTCmdCreateMsgBox* pCmd = new CTCmdCreateMsgBox( Data );
	AddTCommand( DLG_TYPE_MAX, pCmd );
}

void IT_MGR::OpenMsgBox_CartRide( const char* szMsg, short int iButtonType, bool bModal, unsigned int iInvokerDlgID , CTCommand* pCmdOk, CTCommand* pCmdCancel, int iMsgType,
								 WORD parm1, WORD parm2 )
{
	CreateMsgBoxData Data;
	Data.bModal			= bModal;
	Data.iButtonType	= iButtonType;
	Data.iInvokerDlgID  = iInvokerDlgID;
	Data.iMsgType		= iMsgType;
	Data.pCmdCancel     = pCmdCancel;
	Data.pCmdOk			= pCmdOk;
	Data.strMsg			= szMsg;

	Data.parm1			= parm1;
	Data.parm2			= parm2;

	CTCmdCreateMsgBox* pCmd = new CTCmdCreateMsgBox( Data );
	AddTCommand( DLG_TYPE_MAX, pCmd );
}

void IT_MGR::OpenMsgBox2(CreateMsgBoxData& Data)
{
	CTCmdCreateMsgBox* pCmd = new CTCmdCreateMsgBox( Data );
	AddTCommand( DLG_TYPE_MAX, pCmd );
}

void IT_MGR::InitInterfacePos()
{
	POINT ptNew = { 0 , 0 };
	int	  iDefaultX, iDefaultY, iAdjustX = 0, iAdjustY = 0;
	CTDialog* pDlg = NULL;
	int iTempHeight = 0;

	list_dlgs_itor iter;
	for( iter = m_Dlgs.begin(); iter != m_Dlgs.end(); ++iter )
	{
		pDlg = *iter;
		iDefaultX = pDlg->GetDefaultPosX();
		iDefaultY = pDlg->GetDefaultPosY();
		iAdjustX  = pDlg->GetDefaultAdjustPosX();
		iAdjustY  = pDlg->GetDefaultAdjustPosY();

		switch( iDefaultX )
		{
		case CTDialog::TDXP_CENTER:
			ptNew.x = g_pCApp->GetWIDTH() / 2 - pDlg->GetWidth() / 2;
			break;
		case CTDialog::TDXP_RIGHT:
			ptNew.x = g_pCApp->GetWIDTH() - pDlg->GetWidth();
			break;
		case CTDialog::TDXP_LEFT:
		default:
			ptNew.x = 0;
			break;
		}


		switch( iDefaultY )
		{
		case CTDialog::TDYP_CENTER:
			ptNew.y = g_pCApp->GetHEIGHT() / 2 - pDlg->GetHeight() / 2;
			break;
		case CTDialog::TDYP_BOTTOM:
			ptNew.y = g_pCApp->GetHEIGHT() - pDlg->GetHeight();
			break;
		case CTDialog::TDYP_TOP:
		default:
			ptNew.y = 0;
			break;
		}

		ptNew.x += iAdjustX;
		ptNew.y += iAdjustY;
		pDlg->MoveWindow( ptNew );
		if( pDlg->IsDefaultVisible() && !pDlg->IsVision())
		{
			pDlg->Show();

#ifdef _NEWUI
			pDlg->RefreshDlg();		//해상도 변경시에 정해진 위치로 이동하게 만드는 함수.
#endif
			
		}
	}

	if( m_pNotifyButtonDlg )
	{
		m_pNotifyButtonDlg->UpdatePosition();
	}
}

void IT_MGR::CloseDialogAllExceptDefaultView()
{
	list_dlgs_itor iter;
	for( iter = m_Dlgs.begin(); iter != m_Dlgs.end(); ++iter )
	{
		if( !(*iter)->IsDefaultVisible() )
		{
			CloseDialog( (*iter)->GetDialogType() );
			DelDialogIcon( (*iter)->GetDialogType() );///아이콘화 되어 있는 것도 다 지우자.
		}
	}
}

void IT_MGR::SetInterfacePosBySavedData()
{
	POINT ptNew = { 0 , 0 };
	list_dlgs_itor iter;
	CTDialog* pDlg = NULL;

	for( iter = m_Dlgs.begin(); iter != m_Dlgs.end(); ++iter )
	{
		pDlg = *iter;

		if( pDlg->GetCaption() )
		{
			if( pDlg->GetCaption() )///Caption이 있고 타겟창이 아닐경우에만 Save된 Position을 적용한다.
			{
				ptNew.x = g_ClientStorage.GetSavedDialogPosX( pDlg->GetDialogType());
				ptNew.y = g_ClientStorage.GetSavedDialogPosY( pDlg->GetDialogType());
				pDlg->MoveWindow( ptNew );
			}
		}
		
		///Chat List Type Set
		//if( pDlg->GetDialogType() == DLG_TYPE_CHAT )
		//{
		//	CChatDLG* pChatDlg = (CChatDLG*)pDlg;
		//	pChatDlg->ChangeListType( g_ClientStorage.GetChatDlgType() );
		//	if( g_ClientStorage.GetChatDlgType() == CChatDLG::LIST_TYPE_BIG )
		//	{
		//		///REMARK TEMP CODE:Big일경우 ChangeListType에서 강제로 -143을 해준다.이를 임시적으로 막는 코드
		//		///
		//		POINT pt = pChatDlg->GetPosition();
		//		pt.y += 143;
		//		pChatDlg->MoveWindow( pt );
		//	}
		//}

		///Quick Type
		if( pDlg->GetDialogType() == DLG_TYPE_QUICKBAR )
		{
			CQuickBAR* pQuickBarDlg = (CQuickBAR*)pDlg;
			pQuickBarDlg->ChangeType( g_ClientStorage.GetQuickBarDlgType() ,true);
		}
#ifdef _NEW_UI // 2nd SkillBar
		if( pDlg->GetDialogType() == DLG_TYPE_QUICKBAR_EXT )
		{
			CQuickBAR* pQuickBarDlg = (CQuickBAR*)pDlg;
			pQuickBarDlg->ChangeType( g_ClientStorage.GetQuickBarExtDlgType() ,true);
		}
#endif
	}
}

void IT_MGR::ServerDisconnected()
{
	assert( m_pCurrState );
	if( m_pCurrState->GetID() == STATE_NORMAL )
	{
// 05. 10. 27 - 김주현
// __SRVDISCONNECTRELOGIN(서버디스컨넥트가 되었을 경우)에 로그인창으로 이동할것인가
// 아니면... 프로그램을 강종할것인가..

#ifdef __SRVDISCONNECTRELOGIN
		CTCommand* pCmd = new CTCmdReLogin; // 이건 재로그인
#else
		CTCommand* pCmd = new CTCmdExit; // 이건 강종
#endif
		OpenMsgBox("Server Disconnected",CMsgBox::BT_OK, true, 0, pCmd ,NULL);
	}
	else if( m_pCurrState->GetID() == STATE_WAITDISCONNECT )
	{
		if( g_itMGR.GetWaitDisconnectType() == 0 )
			{
			#ifdef __SRVDISCONNECTRELOGIN
				CGame::GetInstance().ChangeState( CGame::GS_RELOGIN );
			#else
				g_pCApp->SetExitGame();
			#endif
			}
	
	}
	else
	{
		g_pCApp->SetExitGame();
	}
}

void IT_MGR::OpenDialog( int iDlgType, bool bToggle, int iPosX , int iPosY )
{
	CTDialog* pDlg = NULL;
	if( pDlg = FindDlg( iDlgType ))
	{
		if( iPosX >= 0 && iPosY >= 0 )
		{
			POINT pt = { iPosX, iPosY };
			pDlg->MoveWindow( pt );
		}
	
		if( bToggle )
		{
			if( pDlg->IsVision())
			{
				pDlg->Hide();
			}
			else
			{
				MoveDlg2ListEnd( pDlg );
				DelDialogIcon( iDlgType );
				pDlg->Show();
			}
		}
		else
		{
			if( !pDlg->IsVision() )
			{
				MoveDlg2ListEnd( pDlg );
				DelDialogIcon( iDlgType );
				pDlg->Show();
			}
		}
	}
}

void IT_MGR::CloseDialog( int iDlgType )
{
	CTDialog* pDlg = NULL;
	if( pDlg = FindDlg( iDlgType ))
	{
		if( pDlg->IsVision() )
			pDlg->Hide();
	}
}


bool IT_MGR::IsMouseOnInterface()
{
	if( CTDialog::GetProcessMouseOverDialog() || CDragNDropMgr::GetInstance().IsDraging() )
		return true;
	return false;
}

void IT_MGR::ProcessEscKeyDown()
{
	std::list< int >::iterator	iter;
	std::list< int >			listDialogSet;

	for( iter = m_listSavedDialog.begin(); iter != m_listSavedDialog.end(); ++iter )
	{
		if( IsDlgOpened( *iter ) )
			listDialogSet.push_back( *iter );	
	}
	
	if( listDialogSet.empty() )
	{
		if( !m_StackDialogSet.empty() )
		{
			std::list< int > DialogSet = m_StackDialogSet.back();

			for( iter = DialogSet.begin(); iter != DialogSet.end();++iter )
				OpenDialog( *iter, false );

			m_StackDialogSet.pop_back();
		}
	}
	else
	{
		m_StackDialogSet.push_back( listDialogSet );
		for( iter = listDialogSet.begin(); iter != listDialogSet.end(); ++iter )
			CloseDialog( *iter );

	}
}


void IT_MGR::ProcCommand()
{
	CTCommand* pCmd = m_CommandQ.GetCommand();
	if( pCmd )
	{
		if( pCmd->Exec( NULL ) )
			delete pCmd;
		else
		{
			if( pCmd->IsWaitExecSucceed() )
				m_CommandQ.Push_Front( pCmd );
			else
				m_CommandQ.Push_Back( pCmd );
		}
	}
}

CMsgBox* IT_MGR::FindMsgBox( int iMsgType )
{
	list_dlgs_itor  itorDlgs;
	CTDialog*		pDlg    = NULL;
	CMsgBox*		pMsgBox = NULL;
	for( itorDlgs = m_Dlgs.begin(); itorDlgs != m_Dlgs.end(); ++itorDlgs )
	{
		pDlg = *itorDlgs;
		if( pDlg->GetDialogType() == DLG_TYPE_MSGBOX )
		{
			pMsgBox = (CMsgBox*)pDlg;
			if( pMsgBox->GetMsgType() == iMsgType )
			{
				return pMsgBox;
			}
		}
	}
	return NULL;

}

CPrivateChatDlg* IT_MGR::GetPrivateChatDlg( DWORD dwUserTag )
{
	list_dlgs_itor iter;
	CTDialog* pDlg;
	for( iter = m_Dlgs.begin() ; iter != m_Dlgs.end() ; ++iter )
	{
		pDlg = *iter;
		if( pDlg->GetDialogType() == DLG_TYPE_PRIVATECHAT && pDlg->GetControlID() == dwUserTag )
			return (CPrivateChatDlg*)pDlg;
	}
	return NULL;
}

void IT_MGR::OpenPrivateChatDlg( DWORD dwUserTag , BYTE btStatus, const char* pszName )
{
	CTCmdOpenPrivateChatDlg Cmd( dwUserTag, btStatus, pszName );
	Cmd.Exec( NULL );
}

//void IT_MGR::BeginRepairItem()
//{
//	OpenDialog( DLG_TYPE_ITEM );
//	///커서 바꿈.. 항상 바꾼다.
//	//CGame::GetInstance().BeginRepairMode();
//}

///같은 TYPE의 메세지 박스가 떠있는지 체크한다.
bool IT_MGR::IsOpenedMsgBox( int iType )
{
	CMsgBox* pMsgBox = NULL;
	list_dlgs_itor iter;
	for( iter = m_Dlgs.begin(); iter != m_Dlgs.end(); ++iter )
	{
		if( (*iter)->GetDialogType() == DLG_TYPE_MSGBOX )
		{
			pMsgBox = (CMsgBox*)*iter;
			if( pMsgBox->GetMsgType() == iType )
				return true;
		}
	}
	return false;
}

void IT_MGR::AddDialogIcon( int iDialogType )
{
	std::list< CIconDialog* >::iterator iterIcon;
	for( iterIcon = m_Icons.begin(); iterIcon != m_Icons.end(); ++iterIcon )
	{
		if( (*iterIcon)->GetDialogType() == iDialogType )
		{
			return;
		}
	}

	CIconDialog* pIcon = CIconDialog::CreateIcon( iDialogType , true);
	if( pIcon )
	{
		std::map< int, POINT >::iterator iterPtIcons;
		iterPtIcons = m_ptIcons.find( iDialogType );
		if( iterPtIcons == m_ptIcons.end() )
		{
			CTDialog* pDlg = FindDlg( iDialogType );
			pIcon->SetPosition( pDlg->GetPosition() );
			m_ptIcons.insert( std::map< int, POINT >::value_type( iDialogType, pDlg->GetPosition() ) );
		}
		else
		{
			pIcon->SetPosition( iterPtIcons->second );
		}
		m_Icons.push_back( pIcon );
		CloseDialog( iDialogType );
	}
}

void IT_MGR::DelDialogIcon( int iDialogType )
{
	std::list< CIconDialog* >::iterator iterIcon;
	for( iterIcon = m_Icons.begin(); iterIcon != m_Icons.end(); ++iterIcon )
	{
		if( (*iterIcon)->GetDialogType() == iDialogType )
		{
			delete *iterIcon;
			m_Icons.erase( iterIcon );
			return;
		}
	}
}

CIconDialog* IT_MGR::FindDialogIcon( int iDialogType )
{
	std::list< CIconDialog* >::iterator iterIcon;
	for( iterIcon = m_Icons.begin(); iterIcon != m_Icons.end(); ++iterIcon )
	{
		if( (*iterIcon)->GetDialogType() == iDialogType )
			return *iterIcon;
	}
	return NULL;
}
void	IT_MGR::SetDialogIconFromMenu( CIconDialog* pIcon )
{
	m_pDialogIconFromMenu = pIcon;
}
void	IT_MGR::DelDialogiconFromMenu()
{
	SAFE_DELETE( m_pDialogIconFromMenu );
}

void	IT_MGR::SetDialogIconPosition( int iDialogType, POINT pt )
{
	std::map< int, POINT >::iterator iterPtIcons;
	iterPtIcons = m_ptIcons.find( iDialogType );
	if( iterPtIcons == m_ptIcons.end() )
	{
		m_ptIcons.insert( std::map< int, POINT >::value_type( iDialogType, pt) );
	}
	else
	{
		iterPtIcons->second = pt;
	}

	if( CIconDialog* pIcon = FindDialogIcon( iDialogType ) )
		pIcon->SetPosition( pt );
}

void IT_MGR::BeginRepair()
{
	//ChangeState( STATE_REPAIR );
	CTCommand* pCmd = new CTCmdChangeStateInterface( STATE_REPAIR );
	AddTCommand( DLG_TYPE_MAX, pCmd );
}

void IT_MGR::BeginAppraisal()
{
	//ChangeState( STATE_APPRAISAL );

	CTCommand* pCmd = new CTCmdChangeStateInterface( STATE_APPRAISAL );
	AddTCommand( DLG_TYPE_MAX, pCmd );

	g_itMGR.OpenDialog( DLG_TYPE_ITEM, false );
}

void IT_MGR::EndRepair()
{
	ChangeState( STATE_NORMAL );
}

void IT_MGR::EndAppraisal()
{
	ChangeState( STATE_NORMAL );
}

void IT_MGR::ChangeState( int iIndex )
{
	assert( iIndex >= 0 && iIndex < STATE_MAX );
	if( iIndex >= 0 && iIndex < STATE_MAX )
	{
		if( m_pCurrState->GetID() != iIndex )
		{
			m_pCurrState->Leave();
			m_pCurrState = m_pStates[iIndex];
			m_pCurrState->Enter();
		}
	}
}
int IT_MGR::GetState()
{
	return m_pCurrState->GetID();
}

void IT_MGR::SetWaitDisconnectTime( int iSec )
{
	m_iWaitDisconnectTime = iSec;
}

int IT_MGR::GetWaitDisconnectTime()
{
	return m_iWaitDisconnectTime;
}

void IT_MGR::SetWaitDisconnectType( int iType )
{
	m_iWaitDisconnectType = iType;
}

int	 IT_MGR::GetWaitDisconnectType()
{
	return m_iWaitDisconnectType;
}
void IT_MGR::ClearNotifyButtons()
{
	assert( m_pNotifyButtonDlg );
	if( m_pNotifyButtonDlg )
		m_pNotifyButtonDlg->Clear();
}
void IT_MGR::AddNotifybutton( int iEventIndex )
{
	int ID = iEventIndex;
	int iWidth  = 36; 
	int iHeight = 36;

	int iNormalGraphicID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI13_BTN_EVENTNOTIFY_NORMAL" );
	int iOverGraphicID   = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI13_BTN_EVENTNOTIFY_OVER" );
	int iDownGraphicID   = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI13_BTN_EVENTNOTIFY_DOWN" );
	int iBlinkGraphicID  = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI13_BTN_EVENTNOTIFY_BLINK" );
	int iModuleID		 = IMAGE_RES_UI;
	
	CTButton* pBtn = new CTButton;
	pBtn->Create( 0, 0 , iWidth, iHeight, iNormalGraphicID, iOverGraphicID, iDownGraphicID ,iModuleID );
	pBtn->SetControlID( ID );
	pBtn->SetBlinkGid( iBlinkGraphicID );
	pBtn->SetBlink( 1, 500 );

	assert( m_pNotifyButtonDlg );
	if( m_pNotifyButtonDlg )
		m_pNotifyButtonDlg->AddButton( pBtn );
}

void IT_MGR::ShowQuestStartMessage( int index )
{
	g_itMGR.AppendChatMsg( 
						CStringManager::GetSingleton().GetQuestDesc( index ) ,
						IT_MGR::CHAT_TYPE_QUEST 
					);

	//if( ISFX* p = CSFXManager::GetSingleton().FindSFXWithType( SFX_FONT ))
	//{
	//	char* startmsg = (char*)QUEST_DESC( index );
	//	if( startmsg == NULL ) return;
	//	if( startmsg[0] == '\0' ) return;

	//	CSFXFont* sfx_font =(CSFXFont*)p;
	//	sfx_font->RemoveEffectStringsByType( CSFXFont::TYPE_QUEST_NAME );
	//	sfx_font->RemoveEffectStringsByType( CSFXFont::TYPE_QUEST_STARTMSG );

	//	POINT draw_position;
	//	int   draw_width;

	//	///Zone Name
	//	SIZE  size = getFontTextExtent( g_GameDATA.m_hFONT[ FONT_OUTLINE_18_BOLD ], QUEST_NAME( index ) );
	//	draw_width		= size.cx;
	//	draw_position.x = g_pCApp->GetWIDTH() / 2 - size.cx / 2;
	//	draw_position.y = 150;///일단 고정


	//	CEffectString* child = new CEffectString;
	//	child->SetType( CSFXFont::TYPE_QUEST_NAME );
	//	child->SetString( FONT_OUTLINE_18_BOLD, (char*)QUEST_NAME( index ), draw_position, draw_width, 5 * 1000 );
	//	sfx_font->AddEffectString( child );


	//	///Zone Description
	//	draw_width      = 400;
	//	draw_position.x = g_pCApp->GetWIDTH() / 2 - 400 / 2;
	//	draw_position.y = 200;
	//	child = new CEffectString;
	//	child->SetType( CSFXFont::TYPE_QUEST_STARTMSG );
	//	child->SetString( FONT_OUTLINE_14_BOLD, startmsg, draw_position, draw_width, 5 * 1000 );
	//	sfx_font->AddEffectString( child );
	//}
}

//*-------------------------------------------------------------------------------------------/
/// 현재 포커스를 가지고 있는 에디트박스의 포인터(CTEditBox)를 리턴한다.
//*-------------------------------------------------------------------------------------------/
CTEditBox*	IT_MGR::GetFocusEditBox()
{
	return CTEditBox::s_pFocusEdit;
}

void IT_MGR::PostMsg2Dlg( int dialog_id, unsigned msg_id, unsigned param1, unsigned param2 )
{
	CTDialog* pDlg = FindDlg( dialog_id );
	assert( pDlg );
	if( pDlg )
		pDlg->PostMsg( msg_id, param1, param2 );
}

CReloadProcess* IT_MGR::GetReloadProcess()	
{ 
	return m_pReloadProcess; 
}

CTDrawImpl*	IT_MGR::GetDrawImplPtr()	
{ 
	return &g_DrawImpl; 
}
