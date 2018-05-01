#include "stdafx.h"
#include ".\cclandlg.h"
#include "../../Game.h"
#include "../CTDrawImpl.h"
#include "../IO_ImageRes.h"
#include "../it_mgr.h"
#include "../TypeResource.h"
#include "../../GameData/Event/CTEventClan.h"
#include "../../GameData/CClan.h"
#include "subclass/ClanMemberItem.h"
#include "subclass/CClanSkillListItem.h"
#include "../icon/ciconskillclan.h"
#include "../GameCommon/StringManager.h"
#include "../Network/CNetwork.h"
#include "../JCommandState.h"
#include "../Command/UICommand.h"
#include "../../IO_Terrain.h"
#include "../ClanMarkTransfer.h"
#include "../../Country.h"


#include "TButton.h"
#include "TabbedPane.h"
#include "ZListBox.h"
#include "JContainer.h"
#include "ResourceMgr.h"
#include "../CClanMarkView.h"
CClanDlg::CClanDlg(void)
{
	m_iTab = TAB_INFO;
}

CClanDlg::~CClanDlg(void)
{

}

void CClanDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();


	D3DVECTOR vCharPos = { m_sPosition.x + 238,  m_sPosition.y + 102,  0 };


	switch( m_iTab )
	{
	case TAB_INFO:		
		CClanMarkView::Draw( g_pAVATAR, vCharPos );
		DrawInfomation();	
		m_ClanMarkPreView.Draw( vCharPos );
		break;
	case TAB_MEMBER:	
		CClanMarkView::Draw( g_pAVATAR, vCharPos );		
		DrawMembers();		
		break;
	case TAB_SKILL:		
		if( !CCountry::GetSingleton().IsApplyNewVersion() )
		{
			CClanMarkView::Draw( g_pAVATAR, vCharPos );		
			DrawSkills();
		}
		break;
	case TAB_NOTICE:	
		CClanMarkView::Draw( g_pAVATAR, vCharPos );		
		DrawNotice();		
		break;
	default:
		break;
	}

	//vCharPos = { m_sPosition.x + 238,  m_sPosition.y + 102,  0 };
	//CClanMarkView::Draw( g_pAVATAR, vCharPos );

	///내가 클랜에 가입중이라면
	//g_DrawImpl.Draw( (int)m_sPosition.x + 238, (int)m_sPosition.y + 102, IMAGE_RES_CLANBACK,   CClan::GetInstance().GetMarkBack() );
	//g_DrawImpl.Draw( (int)m_sPosition.x + 238, (int)m_sPosition.y + 102, IMAGE_RES_CLANCENTER, CClan::GetInstance().GetMarkCenter() );

}
unsigned CClanDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ))
	{
		switch( uiMsg )
		{
		case WM_LBUTTONDOWN:
			OnLButtonDown( uiProcID, wParam , lParam );
			break;
		case WM_LBUTTONUP:
			OnLButtonUp( uiProcID, wParam, lParam );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CClanDlg::OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{

	case IID_BTN_TAB_INFO:
		m_iTab = TAB_INFO;
		break;
	case IID_BTN_TAB_MEMBER:
		m_iTab = TAB_MEMBER;
		break;
	case IID_BTN_TAB_SKILL:
		m_iTab = TAB_SKILL;
		break;
	case IID_BTN_TAB_NOTICE:
		m_iTab = TAB_NOTICE;
		break;
	default:
		break;
	}
}

void CClanDlg::OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_PREVIEW:
		{
			m_ClanMarkPreView.Free();
			///Mark.bmp를 로드해서 CClanMarkUserDefined에 넣는다.
			///CClanMarkTransfer::RegisterMarkToServer()와 비슷한 체크루틴이 필요하다.
			HANDLE hBmpFile = CreateFile( CClanMarkUserDefined::NewClanMarkFileName.c_str(), GENERIC_READ,FILE_SHARE_READ, NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL );
			if( hBmpFile == INVALID_HANDLE_VALUE )
			{
				//DWORD dwErr = GetLastError();
				//_RPTF1( _CRT_ASSERT,"CreateFile Error : Bmp File - Invalid_handle_value(%d)", dwErr );
				g_itMGR.OpenMsgBox( CStr::Printf("%s %s",STR_CLANMARK_FILE_NOTFOUND , STR_CLANMARK_HELP_HOMEPAGE) );
				return;
			}

			if( false == CClanMarkTransfer::GetSingleton().Check_BmpFile( hBmpFile, CClanMarkUserDefined::ClanMarkSize ) )
			{
				//g_itMGR.OpenMsgBox( CStr::Printf("%s %s",STR_CLANMARK_INVALID_FORMAT , STR_CLANMARK_HELP_HOMEPAGE) );			
				CloseHandle( hBmpFile );
				break;
			}

			CloseHandle( hBmpFile );
			HNODE hNode = loadTexture( CClanMarkUserDefined::NewClanMarkFileName.c_str(), CClanMarkUserDefined::NewClanMarkFileName.c_str(), 1,  0 );
			m_ClanMarkPreView.SetNode( hNode );
		}
		break;
	case IID_BTN_REGIST_CLANMARK:
		if( CClan::GetInstance().GetClanNo() )
		{
			if(  CClan::GetInstance().GetClass() >= CClan::CLAN_MASTER )
			{
				//CClanMarkTransfer::GetSingleton().RegisterMarkToServer( g_pAVATAR->GetClanID(), CClanMarkUserDefined::NewClanMarkFileName.c_str() );
				CTCommand* pCmdOk = new CTCmdRegisterClanMark( g_pAVATAR->GetClanID(), CClanMarkUserDefined::NewClanMarkFileName.c_str() );
				g_itMGR.OpenMsgBox( CStr::Printf("%s %s",STR_QUERY_CHANGE_CLANMARK,STR_CLANMARK_UPDATE_ERROR), CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmdOk, 0 );
			}
			else
				g_itMGR.OpenMsgBox( STR_CLAN_NO_RIGHT );
		}
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;
	case IID_BTN_ICONIZE:
		g_itMGR.AddDialogIcon( 	GetDialogType() );
		break;
	case IID_BTN_REGIST_NOTICE:
		{
			CClan& Clan = CClan::GetInstance();
			if( Clan.GetClanNo() )
			{
				///클랜 마스터일경우에만
				if( CClan::GetInstance().GetClass() >= CClan::CLAN_MASTER )
					g_itMGR.OpenDialog( DLG_TYPE_CLAN_NOTICE );
				else
					g_itMGR.OpenMsgBox( STR_CLAN_NO_RIGHT );
			}
		}
		break;
	case IID_BTN_DELETE_NOTICE:
		///클랜 마스터일경우에만
		break;
	case IID_BTN_WITHDRAWAL:
		{
			if( g_pTerrain->IsAgitZone() )
			{
				g_itMGR.OpenMsgBox( STR_CANT_SKILL_IN_AGIT );
				break;
			}

			CClan& Clan = CClan::GetInstance();
			if( Clan.GetClanNo() && !g_pTerrain->IsPVPZone() )
			{
				if( CClan::GetInstance().GetClass() < CClan::CLAN_MASTER )
				{
					///클랜 마스터가 아닐경우에만
					g_pNet->Send_cli_CLAN_COMMAND( GCMD_QUIT , NULL );
					CClan::GetInstance().Clear();
					g_pAVATAR->ResetClan();
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CLAN_CANT_QUIT_CLANMASTER );
				}
			}
		}
		break;
	case IID_BTN_ENTRUST:
		{
			CClan& Clan = CClan::GetInstance();
			if( Clan.GetClanNo() && !g_pTerrain->IsPVPZone() )
			{
				if( CClan::GetInstance().GetClass() >= CClan::CLAN_MASTER )
				{
					CClanMemberItem* pMember = GetSelectedMember();
					if( pMember != NULL && strcmpi( pMember->GetName(), g_pAVATAR->Get_NAME() )) 
					{
						if( pMember->GetClass() >= CClan::CLAN_COMMANDER )
						{
							CTCmdClanCommand* pCmd = new CTCmdClanCommand( GCMD_LEADER, pMember->GetName() );

							g_itMGR.OpenMsgBox( CStr::Printf( STR_CLAN_QUERY_ENTRUST, pMember->GetName() ), 
								CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd );

						}
						else
						{
							g_itMGR.OpenMsgBox( STR_CLAN_CANT_ENTRUST_TO_LOW_CLASS );
						}
					}
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CLAN_NO_RIGHT );
				}
			}
		}
		break;
	case IID_BTN_BAN:
		{
			if( g_pTerrain->IsAgitZone() )
			{
				g_itMGR.OpenMsgBox( STR_CANT_SKILL_IN_AGIT );
				break;
			}
			CClan& Clan = CClan::GetInstance();
			if( Clan.GetClanNo() && !g_pTerrain->IsPVPZone() )
			{
				if( CClan::GetInstance().GetClass() >= CClan::CLAN_MASTER )
				{
					CClanMemberItem* pMember = GetSelectedMember();
					if( pMember != NULL &&  strcmpi( pMember->GetName(), g_pAVATAR->Get_NAME() ))
					{
						CTCmdClanCommand* pCmd = new CTCmdClanCommand( GCMD_REMOVE, pMember->GetName() );
						g_itMGR.OpenMsgBox( CStr::Printf( STR_CLAN_QUERY_BAN, pMember->GetName() ), 
							CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd );
					}
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CLAN_NO_RIGHT );
				}
			}
		}
		break;
	case IID_BTN_CLASS_UP:
		{
			CClanMemberItem* pMember = GetSelectedMember();
			if( pMember != NULL &&  strcmpi( pMember->GetName(), g_pAVATAR->Get_NAME() ) && !g_pTerrain->IsPVPZone() )
			{
				if( CClan::GetInstance().GetClass() >= CClan::CLAN_MASTER )
				{
					if( CClan::GetInstance().IsValidClassUp( pMember->GetClass() ) )
					{
						CTCmdClanCommand* pCmd = new CTCmdClanCommand( GCMD_PROMOTE, pMember->GetName() );
						g_itMGR.OpenMsgBox( CStr::Printf( STR_CLAN_QUERY_PROMOTE, pMember->GetName() ), 
							CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd );
					}
					else
					{
						g_itMGR.OpenMsgBox( 
							CStr::Printf( STR_CLAN_CANT_PROMOTE, CStringManager::GetSingleton().GetClanClass( pMember->GetClass() + 1 ) )
						);
					}
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CLAN_NO_RIGHT );
				}
			}
		}
		break;
	case IID_BTN_CLASS_DOWN:
		{
			CClanMemberItem* pMember = GetSelectedMember();
			if( pMember &&  strcmpi( pMember->GetName(), g_pAVATAR->Get_NAME() ) && !g_pTerrain->IsPVPZone() )
			{
				if( CClan::GetInstance().GetClass() >= CClan::CLAN_MASTER )
				{
					if( pMember->GetClass() >= 0 )
					{
						CTCmdClanCommand* pCmd = new CTCmdClanCommand( GCMD_DEMOTE, pMember->GetName() );
						g_itMGR.OpenMsgBox( CStr::Printf( STR_CLAN_QUERY_DEMOTE, pMember->GetName() ), 
							CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd );
					}
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CLAN_NO_RIGHT );
				}
			}
		}
		break;
	case IID_BTN_REQJOIN:
		{

			CClan& Clan = CClan::GetInstance();
			
			if( Clan.GetClanNo() && !g_pTerrain->IsPVPZone() )
			{
				if( Clan.GetClass() >= CClan::CLAN_SUB_MASTER )
				{
					CObjAVT* pAvt = g_pObjMGR->Get_ClientCharAVT( 
						g_pObjMGR->Get_ServerObjectIndex(g_UserInputSystem.GetCurrentTarget()), 
						false 
					);

					if( pAvt )
					{
						if( Clan.IsValidJoinMember() )
							g_pNet->Send_cli_CLAN_COMMAND( GCMD_INVITE , pAvt->Get_NAME() );
						else
							g_itMGR.OpenMsgBox( STR_CLAN_CANT_MORE_JOINMEMBER );
					}
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CLAN_RESULT_CLAN_JOIN_NO_RIGHT );
				}
			}
		}
		break;
	default:
		break;
	}
}

void CClanDlg::DrawInfomation()
{
	CClan& Clan = CClan::GetInstance();
	
	if( Clan.GetClanNo() )
	{
		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
		::setTransformSprite( mat );
	//	DebugBreak();

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15, 73, g_dwBLACK, STR_CLAN_NAME);
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 88, 73, g_dwWHITE, Clan.GetName());

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15, 94, g_dwBLACK, STR_CLAN_LEV);
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 88, 94, g_dwWHITE, "%d" ,Clan.GetLevel() );

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15,115, g_dwBLACK, STR_CLAN_POINT);
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 88,115, g_dwWHITE, "%d", Clan.GetPoint());		

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15, 135, g_dwBLACK, STR_CLAN_SLOGAN);

		RECT rcDraw = { 88, 135, 210, 180 };
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE, DT_WORDBREAK,Clan.GetSlogan() );		

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15, 187, g_dwBLACK, STR_CLAN_MONEY );
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 88, 187, g_dwWHITE,"%d", Clan.GetMoney());




		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15, 208, g_dwBLACK, STR_CLAN_MEMBER_COUNT);
		SetRect( &rcDraw, 88, 208, 210, 230 );
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE, DT_CENTER, "%d / %d", Clan.GetMemberCount(), Clan.GetMemberMaxCount() );		


		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15, 229, g_dwBLACK, STR_CLANMARK_REGISTER_TIME);
		if( Clan.GetClanNo() && Clan.GetClass() >= CClan::CLAN_MASTER && g_pAVATAR->GetClanMarkBack() == 0 )
		{
			SYSTEMTIME clanmark_regtime = Clan.GetClanMarkRegTime();
			::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 88, 229, g_dwWHITE, "%d/%d/%d %d:%02d", 
				clanmark_regtime.wYear,clanmark_regtime.wMonth, clanmark_regtime.wDay, clanmark_regtime.wHour, clanmark_regtime.wMinute );
		}


		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15,248, g_dwBLACK, STR_CLAN_ALLYED);
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 88,248, g_dwWHITE, Clan.GetAllyName(0) );
//		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 71,229, g_dwWHITE, Clan.GetAllyName(1) );
//		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 71,250, g_dwWHITE, Clan.GetAllyName(2) );



		DWORD dwColor = g_dwWHITE;
		if( CClan::GetInstance().GetClass() <= CClan::CLAN_PENALTY )
			dwColor = g_dwRED;

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 84,288, dwColor, CStringManager::GetSingleton().GetClanClass( CClan::GetInstance().GetClass() ) );///자신의 직위
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 84,309, g_dwWHITE, "%d",Clan.GetMyClanPoint() );


		g_DrawImpl.Draw(  m_sPosition.x + 229, m_sPosition.y + 283, IMAGE_RES_UI, m_iClassMarkImageNo[ Clan.GetClass() ] );
	}

}

void CClanDlg::DrawMembers()
{

	if( !m_strSelectedMemberView.empty() )
	{
		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x + 20, (float)m_sPosition.y + m_iHeight - 28,0.0f);
		::setTransformSprite( mat );

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 0,0 , g_dwWHITE, m_strSelectedMemberView.c_str() );
	}
}

void CClanDlg::DrawSkills()
{

}

void CClanDlg::DrawNotice()
{
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	RECT rcDraw = { 30, 75, 190, 310 };

	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwBLACK, DT_WORDBREAK,CClan::GetInstance().GetNotice() );
}

void CClanDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );

	if( pObservable == NULL || pObj == NULL ) return;

	if( strcmp( pObj->toString(), "CTEventClan") == 0 )
	{

		CTEventClan* pEvent = (CTEventClan*)pObj;

		switch( pEvent->GetID() )
		{
		case CTEventClan::EID_SET_JOB:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*) pCtrl;
						CClanMemberItem* pItem	= NULL;
						for( int i = 0 ; i < pList->GetSize(); ++i )
						{
							if( pCtrl = pList->GetItem( i ) )
							{
								CClanMemberItem* pItem = (CClanMemberItem*)pCtrl;
								if( strcmpi( pItem->GetName(), pEvent->GetName() ) == 0 )
								{
									pItem->SetJob( pEvent->GetJob() );
									break;
								}
							}
						}
					}
				}			
			}
			break;
		case CTEventClan::EID_SET_LEVEL:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*) pCtrl;
						CClanMemberItem* pItem	= NULL;
						for( int i = 0 ; i < pList->GetSize(); ++i )
						{
							if( pCtrl = pList->GetItem( i ) )
							{
								CClanMemberItem* pItem = (CClanMemberItem*)pCtrl;
								if( strcmpi( pItem->GetName(), pEvent->GetName() ) == 0 )
								{
									pItem->SetLevel( pEvent->GetLevel() );
									break;
								}
							}
						}
					}
				}			
			}
			break;
		case CTEventClan::EID_ADD_MEMBER:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*) pCtrl;
						CClanMemberItem* pItem = new CClanMemberItem( pEvent->GetName(),  pEvent->GetClass() , pEvent->GetPoint(), pEvent->GetChannelNo() ,pEvent->GetLevel(), pEvent->GetJob());
						pList->Add( pItem );

						if( pList->GetSize() > 1 )
							SortMemberlistByClass();
					}
				}			
			}
			break;
		case CTEventClan::EID_REMOVE_MEMBER:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*) pCtrl;
						CClanMemberItem* pItem	= NULL;
						for( int i = 0 ; i < pList->GetSize(); ++i )
						{
							if( pCtrl = pList->GetItem( i ) )
							{
								CClanMemberItem* pItem = (CClanMemberItem*)pCtrl;
								if( strcmpi( pItem->GetName(), pEvent->GetName() ) == 0 )
								{
									pList->DelItem( i );
									break;
								}
							}
						}
					}
				}			
			}
			break;
		case CTEventClan::EID_CHANGE_CLASS:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*) pCtrl;
						CClanMemberItem* pItem	= NULL;
						for( int i = 0 ; i < pList->GetSize(); ++i )
						{
							if( pCtrl = pList->GetItem( i ) )
							{
								CClanMemberItem* pItem = (CClanMemberItem*)pCtrl;
								if( strcmpi( pItem->GetName(), pEvent->GetName() ) == 0 )
								{
									pItem->SetClass( pEvent->GetClass() );
									SortMemberlistByClass();
									break;
								}
							}
						}
					}
				}			
			}
			break;
		case CTEventClan::EID_CHANGE_CONNECT_STATUS:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*) pCtrl;
						CClanMemberItem* pItem	= NULL;
						for( int i = 0 ; i < pList->GetSize(); ++i )
						{
							if( pCtrl = pList->GetItem( i ) )
							{
								CClanMemberItem* pItem = (CClanMemberItem*)pCtrl;
								if( strcmpi( pItem->GetName(), pEvent->GetName() ) == 0 )
								{
									pItem->SetConnectStatus( pEvent->GetChannelNo() );
									break;
								}
							}
						}
					}
				}			
			}
			break;
		default:
			break;
		}
	}
	else if( strcmp( pObj->toString(), "ETClanSkill") == 0 )
	{
		CTEventClanSkill* pEvent = (CTEventClanSkill*)pObj;
		switch( pEvent->GetID() )
		{
		case CTEventClanSkill::EID_ADD_SKILL:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_SKILL, IID_ZLIST_SKILL );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*)pCtrl;
						
						CIconSkillClan* pIcon		= new CIconSkillClan( pEvent->GetIndex() );
						CClanSkillListItem* pItem	= new CClanSkillListItem;
						pItem->Show();
						pItem->SetIcon( pIcon );
						pList->Add( pItem );
					}
				}			
			}
			break;
		case CTEventClanSkill::EID_DEL_SKILL:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_SKILL, IID_ZLIST_SKILL );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*)pCtrl;
						RemoveClanSkillListItem( pList, pEvent->GetIndex() );
					}
				}			
			}
			break;
		default:
			break;
		}
	}
}

void CClanDlg::RemoveClanSkillListItem( CZListBox* pListBox, int iSkillSlotIndex )
{
	CClanSkillListItem*	pItem	= NULL;
	CWinCtrl* pCtrl				= NULL;
	CIconSkillClan* pSkillIcon	= NULL;
	for( int i = 0 ; i < pListBox->GetSize(); ++i )
	{
		pCtrl = pListBox->GetItem( i );
		assert( pCtrl );
		if( pCtrl )
		{
			pItem = (CClanSkillListItem*)pCtrl;
			pSkillIcon = pItem->GetIcon();
			assert( pSkillIcon );
			if( pSkillIcon )
			{
				if( pSkillIcon->GetSkillSlotFromIcon() == iSkillSlotIndex )
				{
					pListBox->DelItem( i );
					break;
				}
			}
		}
	}
}
CWinCtrl* CClanDlg::FindCtrlInTabbedPane( CTabbedPane* pPane, int iTabID, int iCtrlID )
{
	CWinCtrl* pCtrl = NULL;
	CJContainer* pContainer = pPane->GetTabContainer( iTabID );
	assert( pContainer );
	if( pContainer )
		return pContainer->Find( iCtrlID );

	return NULL;
}

CClanMemberItem* CClanDlg::GetSelectedMember()
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
		{
			CZListBox* pList = (CZListBox*) pCtrl;
			int iSelectedIndex = pList->GetSelectedItemIndex();
			if( iSelectedIndex >= 0 )
			{
				if( pCtrl = pList->GetItem( iSelectedIndex ) )
				{
					return (CClanMemberItem*)pCtrl;
				}
			}
		}
	}			
	return NULL;
}

void CClanDlg::SortMemberlistByClass()
{
	std::multimap< int , CClanMemberItem* > TempMap;
	std::multimap< int , CClanMemberItem* >::reverse_iterator riterMap;

	CClanMemberItem* pItem = NULL;
	CClanMemberItem* pNewItem = NULL;
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
		{
			CZListBox* pList = (CZListBox*) pCtrl;
			for( int i = 0 ; i < pList->GetSize(); ++i )
			{

				pItem = (CClanMemberItem*)pList->GetItem( i );
				pNewItem = new CClanMemberItem( pItem->GetName(), pItem->GetClass() , pItem->GetClanPoint(), pItem->GetChannelNo(), pItem->GetLevel(), pItem->GetJob() );

				TempMap.insert( make_pair( pItem->GetClass() , pNewItem ) );
			}

			pList->Clear();
			for( riterMap = TempMap.rbegin(); riterMap != TempMap.rend(); ++riterMap )
				pList->Add( riterMap->second );

			TempMap.clear();
		}
	}
}

bool CClanDlg::Create( const char* IDD )
{
	bool bRet = CTDialog::Create( IDD );
	if( bRet )
	{
		m_iClassMarkImageNo[0] = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "CLAN01_MARK_JUNIOR" );
		m_iClassMarkImageNo[1] = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "CLAN01_MARK_JUNIOR" );
		m_iClassMarkImageNo[2] = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "CLAN01_MARK_SENIOR" );
		m_iClassMarkImageNo[3] = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "CLAN01_MARK_LANDER" );
		m_iClassMarkImageNo[4] = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "CLAN01_MARK_COMMANDER" );
		m_iClassMarkImageNo[5] = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "CLAN01_MARK_SUBMASTER" );
		m_iClassMarkImageNo[6] = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "CLAN01_MARK_MASTER" );
		
		CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
		if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
		{
			CTabbedPane* pPane = (CTabbedPane*)pCtrl;

			CTButton* pButton = NULL;
			pCtrl = FindCtrlInTabbedPane( pPane, TAB_INFO, IID_BTN_PREVIEW );
			assert( pCtrl && pCtrl->GetControlType() == CTRL_BUTTON );
			if( pCtrl && pCtrl->GetControlType() == CTRL_BUTTON )
			{
				pButton = (CTButton*)pCtrl;
				pButton->SetText( STR_PREVIEW );
			}

			pCtrl = pCtrl = FindCtrlInTabbedPane( pPane, TAB_INFO, IID_BTN_REGIST_CLANMARK );
			assert( pCtrl && pCtrl->GetControlType() == CTRL_BUTTON );
			if( pCtrl && pCtrl->GetControlType() == CTRL_BUTTON )
			{
				pButton = (CTButton*)pCtrl;
				pButton->SetText( STR_REGIST_CLANMARK );
			}
		}
	}
	return bRet;
}

void CClanDlg::SetSelectedMember( int iClanPoint )
{
	m_strSelectedMemberView.clear();
	m_strSelectedMemberView.append( STR_CLAN_MEMBER_CONTRIB );
	m_strSelectedMemberView.append( CStr::Printf(" : %d", iClanPoint ) );
}

void CClanDlg::Hide()
{
	CTDialog::Hide();
	m_ClanMarkPreView.Free();
}

void CClanDlg::Show()
{
	CTDialog::Show();
	///내가 마스터 이상이고 등록된 마크 이미지를 사용한다면
	SYSTEMTIME clanmark_regtime = CClan::GetInstance().GetClanMarkRegTime();
	if( g_pAVATAR && g_pAVATAR->GetClanPos() >= CClan::CLAN_MASTER && g_pAVATAR->GetClanMarkBack() == 0 && clanmark_regtime.wYear == 0 )
		g_pNet->Send_cli_CLANMARK_REG_TIME();

}