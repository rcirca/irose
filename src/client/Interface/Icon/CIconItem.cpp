#include "stdafx.h"
#include "ciconitem.h"
#include "../CTDrawImpl.h"
#include "../interfacetype.h"
#include "../dlgs/subclass/cslot.h"
#include "../it_mgr.h"
#include "../TypeResource.h"
#include "../IO_ImageRes.h"
#include "../slotcontainer/CItemSlot.h"
#include "../../GameCommon/Item.h"
#include "../../Common/IO_STB.h"
#include "../../Common/CItem.h"
#include "../../System/CGame.h"
#include "../command/uicommand.h"
#include "../../GameProc/UseItemDelay.h"
#include "../../GameCommon/ReloadProcess.h"
#include "../IO_Event.h"
#include "../Country.h"


#include "tgamectrl/resourcemgr.h"
#include "tgamectrl/tcommand.h"

HNODE CIconItem::m_hSocketTexture = NULL;
int	  CIconItem::m_iSocketTextureRefCount = 0;

CIconItem::CIconItem(void)
{
	m_bDeleteItemData = false;
	m_pItem = NULL;
	++m_iSocketTextureRefCount;
}

CIconItem::CIconItem( CItem*	pItem ) : CIcon()
{
	m_bDeleteItemData   = false;
	assert( pItem );
	m_iWidth			= 40;
	m_iHeight			= 40;
	m_iModuleID			= IMAGE_RES_ITEM;
	m_pItem				= pItem;
	if( m_hSocketTexture == NULL )
		m_hSocketTexture = loadTexture( "soket.dds","3ddata\\control\\res\\soket.dds", 1, 0 );
	
	++m_iSocketTextureRefCount;
}

CIconItem::CIconItem( tagITEM* pItemData ) : CIcon()
{
	m_iWidth			= 40;
	m_iHeight			= 40;
	m_iModuleID			= IMAGE_RES_ITEM;
	m_pItem				= new CItem( pItemData );
	if( m_hSocketTexture == NULL )
		m_hSocketTexture = loadTexture( "soket.dds","3ddata\\control\\res\\soket.dds", 1, 0 );
	
	++m_iSocketTextureRefCount;
	m_bDeleteItemData = true;
}

CIconItem::~CIconItem(void)
{
	--m_iSocketTextureRefCount;
	if( m_iSocketTextureRefCount <= 0 && m_hSocketTexture )
	{
		unloadTexture( m_hSocketTexture );
		m_hSocketTexture = NULL;
	}
	if( m_bDeleteItemData )
	{
		SAFE_DELETE( m_pItem );
	}
}

void CIconItem::Draw()
{
	assert( m_pItem );

	if( m_pItem == NULL )
		return;

	tagITEM& Item			= m_pItem->GetItem();

	if( Item.IsEmpty() ) 
	{
		assert( 0 && "Item is Empty @CIconItem::Draw()" );
		return;
	}

	m_iIconGraphicID	= ITEM_ICON_NO( Item.GetTYPE(), Item.GetItemNO() );



	if( Item.GetTYPE() != ITEM_TYPE_USE )
	{
		CIcon::Draw();
	}
	else
	{
		//2005. 6. 30 박 지호 
		//새로운 Delay Type이 허용한 국가들이라면 설정... 
		if( CCountry::GetSingleton().IsUseItemDelayNewVersion() )
		{
			//아이템 딜레이 타입을 가져온다. 
			int iItem	   = Item.GetItemNO();
			int iDelayType = USEITME_DELAYTIME_TYPE( iItem );
		
			float srcTick = 0.0f;
			float desTick = 0.0f;

			//그룹 타입이 있다면...
			if(iDelayType)
			{
				srcTick = g_CurUseItemDelayTick.GetUseItemDelay( iDelayType ); 
				desTick = g_UseItemDelay.GetUseItemDelay( iDelayType ); 
			}
			//단독으로 구동된다면...
			else
			{
				srcTick = (float)((USEITME_DELAYTIME_TICK( iItem )) * 1000);
				desTick = g_SoloUseItemDelayTick.GetUseItemDelay( iItem );
			}

			//만약 딜레이 랜더링 중이라면...
			if(desTick )	
			{
				float fProcessRate =  (float(desTick) / srcTick);

				CReloadProcess* pReloadProcess = g_itMGR.GetReloadProcess();
				pReloadProcess->Draw( (int)m_ptPosition.x, (int)m_ptPosition.y ,IMAGE_RES_ITEM,m_iIconGraphicID, fProcessRate );			
			}
			else
				CIcon::Draw();
			
		}
		
		else
		{
			if( USEITME_STATUS_STB( Item.GetItemNO() ) )
			{
				switch( USEITME_STATUS_STB( Item.GetItemNO() ) )
				{
					case 1:
					case 2:
					case 3:
						{
							if( float fUseItemDelay = (float)g_UseItemDelay.GetUseItemDelay( USE_ITEM_HP ) )
							{
								float fProcessRate =  fUseItemDelay / DEFAULT_HP_ITEM_DELAY;

								CReloadProcess* pReloadProcess = g_itMGR.GetReloadProcess();
								pReloadProcess->Draw( (int)m_ptPosition.x, (int)m_ptPosition.y ,																						IMAGE_RES_ITEM, m_iIconGraphicID, fProcessRate );		
							}
							else
								CIcon::Draw();
						}
						break;

					case 4:
					case 5:
					case 6:
						{
							if( float fUseItemDelay = (float)g_UseItemDelay.GetUseItemDelay( USE_ITEM_MP ) )
							{
								float fProcessRate =  fUseItemDelay / DEFAULT_MP_ITEM_DELAY;

								CReloadProcess* pReloadProcess = g_itMGR.GetReloadProcess();
								pReloadProcess->Draw((int)m_ptPosition.x, (int)m_ptPosition.y, IMAGE_RES_ITEM								,											m_iIconGraphicID, fProcessRate );		
							}
							else
								CIcon::Draw();
							
						}
						break;

					default:
						{
							if( float fUseItemDelay = (float)g_UseItemDelay.GetUseItemDelay( USE_ITEM_OTHERS ) )
							{
								float fProcessRate =  fUseItemDelay / DEFAULT_OTHER_ITEM_DELAY;

								CReloadProcess* pReloadProcess = g_itMGR.GetReloadProcess();
								pReloadProcess->Draw( (int)m_ptPosition.x, (int)m_ptPosition.y, IMAGE_RES_ITEM, m_iIconGraphicID, fProcessRate );		
							}
							else
								CIcon::Draw();
						}
						break;
					}
				}

			else
			{
				/// 스크롤 스킬일경우
				if( ITEM_TYPE( Item.GetTYPE(), Item.GetItemNO()  ) == USE_ITEM_SKILL_DOING )
				{
					if( float fUseItemDelay = (float)g_UseItemDelay.GetUseItemDelay( USE_ITEM_SCROLL ) )
					{
						float fProcessRate =  fUseItemDelay / DEFAULT_USE_SCROLL_DELAY;

						CReloadProcess* pReloadProcess = g_itMGR.GetReloadProcess();
						pReloadProcess->Draw( (int)m_ptPosition.x, (int)m_ptPosition.y, IMAGE_RES_ITEM, m_iIconGraphicID, fProcessRate );		
					}
					else
					{
						CIcon::Draw();
					}
				}
				else///OTHER
				{
					if( float fUseItemDelay = (float)g_UseItemDelay.GetUseItemDelay( USE_ITEM_OTHERS ) )
					{
						float fProcessRate =  fUseItemDelay / DEFAULT_OTHER_ITEM_DELAY;

						CReloadProcess* pReloadProcess = g_itMGR.GetReloadProcess();
						pReloadProcess->Draw( (int)m_ptPosition.x, (int)m_ptPosition.y, IMAGE_RES_ITEM, m_iIconGraphicID, fProcessRate );		
					}
					else
					{
						CIcon::Draw();
					}
				}
			}
		}
		
	}

	if( Item.HasSocket() && m_hSocketTexture )
	{
		if( Item.GetGemNO() > 300 )
		{
			assert( Item.GetGemNO() <= g_TblGEMITEM.m_nDataCnt );
			if( Item.GetGemNO() <= g_TblGEMITEM.m_nDataCnt )
				g_DrawImpl.Draw( m_ptPosition.x, m_ptPosition.y, IMAGE_RES_SOCKETJAM_ICON, GEMITEM_MARK_IMAGE( Item.GetGemNO() ) );
		}
		else 
		{
			::drawSprite( m_hSocketTexture, 
					NULL,
					NULL,
					&D3DXVECTOR3( 0, 0, 0 ),								
					D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
		}
	}
	
	if( m_pSlot	&& m_pSlot->GetParent() == DLG_TYPE_STORE )///상점에서는 아이템의 개수표시를 안한다.
		return;

	if( Item.IsEnableDupCNT() && GetQuantity())/// Quantity Item일경우
	{
		int iImageIndex =  CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"ID_BLACK_PANEL");
		char szCount[32]= {0};
		itoa( GetQuantity(), szCount, 10 );
		HNODE hFont = g_GameDATA.m_hFONT[ FONT_NORMAL ];
			
		SIZE size = getFontTextExtent (  hFont, szCount );
		int iWidth = (int)size.cx + 4;		
		g_DrawImpl.Draw( (int)m_ptPosition.x , (int)m_ptPosition.y + 2, iWidth , IMAGE_RES_UI, iImageIndex ,D3DCOLOR_ARGB(128,255,255,255));
		drawFontf( hFont, true, 2,2 ,D3DCOLOR_ARGB( 255,255,255,255 ), szCount );
	}
	
	
	
	
}

void CIconItem::ExecuteCommand()
{
	assert( m_pItem );
	if( !m_pItem->IsEnable() )
		return;

	if( m_pCommand )
		m_pCommand->Exec( m_pItem );
	else
		m_pItem->Excute();
}

bool CIconItem::IsEnable()
{
	assert( m_pItem );
	return m_pItem->IsEnable();
}

void CIconItem::Update( POINT ptMouse )
{
	CIcon::Update( ptMouse );
	if( m_pItem )
	{
		tagITEM& Item = m_pItem->GetItem();
		if( Item.HasLife() )
		{
			if( Item.GetLife() < 50 && IsEnable() )
				m_dwIconDrawColor = D3DCOLOR_ARGB( 255, 255,128,128 );
		}
	}
	/*
	if( IsInside( ptMouse ) && m_pParentDlg )
	{
		g_itMGR.DrawItemINFO( (short)ptMouse.x, (short)ptMouse.y, m_sItem, m_pParentDlg->GetDialogType(), 0 );
	}
	*/
}

int CIconItem::GetIndex()
{
	assert( m_pItem );
	return m_pItem->GetIndex();
}

bool CIconItem::IsEmpty()
{
	assert( m_pItem );
	return m_pItem->GetItem().IsEmpty();
}

tagITEM& CIconItem::GetItem()
{
	assert( m_pItem );

	return m_pItem->GetItem();
}
int	CIconItem::GetQuantity()
{
	assert( m_pItem );
	return m_pItem->GetQuantity();
}
int CIconItem::GetItemNo()
{
	assert( m_pItem );
	return m_pItem->GetItemNo();
}

CIcon* CIconItem::Clone()
{
	assert( m_pItem );
	CIcon* pIcon = m_pItem->CreateItemIcon();
	pIcon->SetSlot( GetSlot() );
	return pIcon;
}

void CIconItem::Clear()
{
	assert( m_pItem );
//	m_pItem->DeleteItemIcon( this );
}

void CIconItem::GetToolTip( CInfo& strToolTip,  DWORD dwDialogType, DWORD dwType )
{
	assert( m_pItem );
	
	DWORD dwInfoType = dwType;

	switch( dwDialogType )
	{
	case DLG_TYPE_MSGBOX:
	case DLG_TYPE_EXCHANGE:
	case DLG_TYPE_DEAL:
		dwInfoType |= INFO_STATUS_DETAIL;
		break;
	case DLG_TYPE_STORE:
		dwInfoType |= INFO_ADD_PRICE_SELL;
		dwInfoType |= INFO_STATUS_DETAIL;
		break;
	case DLG_TYPE_ITEM:
		if( CGame::GetInstance().GetRepairMode() == CGame::REPAIR_NPC )
			dwInfoType |= INFO_ADD_PRICE_REPAIR;
		else if( g_itMGR.GetState() == IT_MGR::STATE_APPRAISAL )
			dwInfoType |= INFO_ADD_PRICE_APPRAISAL;
		else if( g_itMGR.IsDlgOpened( DLG_TYPE_STORE ) )
			dwInfoType |= INFO_ADD_PRICE;
		else if( g_itMGR.IsDlgOpened( DLG_TYPE_BANK ) )
			dwInfoType |= INFO_ADD_PRICE_STORAGE_FEE;
		break;
	case DLG_TYPE_AVATARSTORE:
		dwInfoType |= INFO_STATUS_DETAIL;
		break;
	case DLG_TYPE_PRIVATESTORE:
		dwInfoType |= INFO_ADD_PRICE_PRIVATESTORE;
		dwInfoType |= INFO_STATUS_DETAIL;
		break;
	default:
		break;
	}
	
	if( GetAsyncKeyState( VK_RBUTTON ) < 0 )
		dwInfoType |= INFO_STATUS_DETAIL;

	m_pItem->GetToolTip( strToolTip, dwDialogType, dwInfoType );
}

CItem* CIconItem::GetCItem()
{
	return m_pItem;
}
const char*	CIconItem::GetName()
{
	assert( m_pItem );
	if( m_pItem )
		return m_pItem->GetName();

	return CIcon::GetName();	
}

unsigned CIconItem::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( uiMsg == WM_LBUTTONDOWN && (wParam & MK_CONTROL) )
	{
		if( m_pItem )
		{
			CTCmdAddItem2WishList* pCmd = new CTCmdAddItem2WishList( m_pItem->GetItem() );
			g_itMGR.OpenMsgBox(CStr::Printf("%s %s", m_pItem->GetName() ,STR_QUERY_REGIST_WISHLIST),
				CMsgBox::BT_OK | CMsgBox::BT_CANCEL ,true, 0, pCmd, 0 );

			return uiMsg;
		}
	}
	return 0;
}