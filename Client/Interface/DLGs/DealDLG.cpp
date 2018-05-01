
#include "StdAfx.h"
#include "DealDLG.h"
#include "CMsgBox.h"
#include "../it_mgr.h"
#include "../TypeResource.h"
#include "../InterfaceType.h"
#include "../CTDrawImpl.h"
#include "../IO_ImageRes.h"
#include "../CDragItem.h"
#include "../icon/ciconitem.h"

#include "../../System/CGame.h"
#include "../../gamecommon/iconcommand.h"
#include "../../gamecommon/item.h"
#include "../../gamedata/event/cteventdeal.h"
#include "../../gamedata/cstore.h"
#include "../../misc/gameutil.h"


#include "../../../TGameCtrl/ResourceMgr.h"


#define DS_CAPTION_SX	31
#define DS_CAPTION_SY	11

#define DS_TEXT_1X		29
#define DS_TEXT_1Y		46
#define DS_TEXT_2X		140
#define DS_TEXT_2Y		46
#define DS_TEXT_WIDTH	96
#define DS_TEXT_HEIGHT	17

#define DS_B_SX			24				/// 구입 슬롯 시작위치
#define DS_B_SY			64
#define DS_S_SX			139				/// 판매 슬롯 시작위치
#define DS_S_SY			64

#define DS_WIDTH		33				/// 슬롯 가로 크기
#define DS_HEIGHT		33				/// 슬롯 세로 크기

#define DS_COST_BX		48				/// 구입총액
#define DS_COST_BY		165			
#define DS_COST_SX		163				/// 판매총액
#define DS_COST_SY		165			

#define DS_DIGIT_WIDTH	73
#define DS_DIGIT_HEIGHT	17



CDealDLG::CDealDLG( int iType )
{	
	m_pCmdRemoveItemFromBuyList		= NULL;
	m_pCmdRemoveItemFromSellList	= NULL;

	m_pBuyDragItem  = new CDragItem;
	m_pSellDragItem = new CDragItem;

	CTCommand* pCmd = NULL;

	pCmd = new CTCmdRemoveItemInBuyList;
	m_pBuyDragItem->AddTarget( iType, NULL ); 
	m_pBuyDragItem->AddTarget( CDragItem::TARGET_ALL, pCmd );

	pCmd = new CTCmdRemoveItemInSellList;
	m_pSellDragItem->AddTarget( iType, NULL );
	m_pSellDragItem->AddTarget( CDragItem::TARGET_ALL, pCmd );

	POINT ptOffset;

	for( int iSlot = 0; iSlot < TOTAL_DEAL_INVENTORY; ++iSlot )
	{
		m_Slots[DEAL_SELL ][iSlot].SetWidth( 40 );
		m_Slots[DEAL_SELL ][iSlot].SetHeight( 40 );

		ptOffset.x = 10  + ( iSlot % 5 ) * ( m_Slots[DEAL_SELL ][iSlot].GetWidth() + 1 );
		ptOffset.y = 183  + ( iSlot / 5 ) * ( m_Slots[DEAL_SELL ][iSlot].GetHeight() + 1 );
	
		m_Slots[DEAL_SELL ][iSlot].SetOffset( ptOffset );
		m_Slots[DEAL_SELL ][iSlot].SetParent( iType );
		m_Slots[DEAL_SELL ][iSlot].SetDragAvailable();
		m_Slots[DEAL_SELL ][iSlot].SetDragItem( m_pSellDragItem );
	}

	for( int iSlot = 0; iSlot < TOTAL_DEAL_INVENTORY; ++iSlot )
	{
		m_Slots[DEAL_BUY][iSlot].SetWidth( 40 );
		m_Slots[DEAL_BUY][iSlot].SetHeight( 40 );
		ptOffset.x = 10 + ( iSlot % 5 ) * ( m_Slots[DEAL_BUY][iSlot].GetWidth() + 1 ) ;
		ptOffset.y = 52 + ( iSlot / 5 ) * ( m_Slots[DEAL_BUY][iSlot].GetHeight() + 1 );
		m_Slots[DEAL_BUY][iSlot].SetOffset( ptOffset );
		m_Slots[DEAL_BUY][iSlot].SetParent( iType );
		m_Slots[DEAL_BUY][iSlot].SetDragAvailable();
		m_Slots[DEAL_BUY][iSlot].SetDragItem( m_pBuyDragItem );
	}
}

CDealDLG::~CDealDLG()
{
	SAFE_DELETE( m_pCmdRemoveItemFromBuyList );
	SAFE_DELETE( m_pCmdRemoveItemFromSellList );
	SAFE_DELETE( m_pBuyDragItem );
	SAFE_DELETE( m_pSellDragItem );
}

void CDealDLG::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	if( pObj && strcmp( pObj->toString(), "CTEventDeal") == 0 )
	{

		CTEventDeal*	pEvent			= ( CTEventDeal* ) pObj;
		CItemFragment*	pFragmentItem	= pEvent->GetItem();

		CIcon*	pIcon = NULL;

		switch( pEvent->GetID() )
		{
		case CTEventDeal::EID_ADD_BUYITEM:
			{
				if( pFragmentItem == NULL )
				{
					assert( pFragmentItem && "Event has not Item @CTEventDeal::EID_ADD_SELLITEM" );				
					break;
				}

				for( int iSlot = 0 ; iSlot < TOTAL_DEAL_INVENTORY; ++iSlot )
				{
					if( m_Slots[DEAL_BUY][iSlot].GetIcon() == NULL )
					{
						pIcon = pFragmentItem->CreateItemIcon();
						pIcon->SetCommand( m_pCmdRemoveItemFromBuyList );
						m_Slots[DEAL_BUY][iSlot].AttachIcon( pIcon );
						return;
					}
				}
				assert( 0 && "Has not Empty Slot @CDealDLG::Update@EID_ADD_BUYITEM" );
				break;
			}
		case CTEventDeal::EID_ADD_SELLITEM:
			{
				if( pFragmentItem == NULL )
				{
					assert( pFragmentItem && "Event has not Item @CTEventDeal::EID_ADD_SELLITEM" );				
					break;
				}

				for( int iSlot = 0 ; iSlot < TOTAL_DEAL_INVENTORY; ++iSlot )
				{
					if( m_Slots[DEAL_SELL][iSlot].GetIcon() == NULL )
					{
						pIcon = pFragmentItem->CreateItemIcon();
						pIcon->SetCommand( m_pCmdRemoveItemFromSellList );
						m_Slots[DEAL_SELL][iSlot].AttachIcon( pIcon );
						return;
					}
				}
				assert( 0 && "Has not Empty Slot @CDealDLG::Update::EID_ADD_SELLITEM" );
				break;
			}
		case CTEventDeal::EID_REMOVE_BUYITEM:
			{
				if( pFragmentItem == NULL )
				{
					assert( pFragmentItem && "Event has not Item @CTEventDeal::EID_REMOVE_BUYITEM" );				
					break;
				}

				for( int iSlot = 0 ; iSlot < TOTAL_DEAL_INVENTORY ; ++iSlot )
				{
					if( pIcon = m_Slots[DEAL_BUY][iSlot].GetIcon() )
					{
						if( ((CIconItem*)pIcon)->GetCItem() == (CItem*)pFragmentItem )
						{
							m_Slots[DEAL_BUY][iSlot].DetachIcon();
							return;
						}
					}
				}
				assert( 0 && "Trade List에 없는 아이템을 지우려고 합니다 @CDealDlg::Update@EID_REMOVE_BUYITEM" );			
				break;
			}
		case CTEventDeal::EID_REMOVE_SELLITEM:
			{
				if( pFragmentItem == NULL )
				{
					assert( pFragmentItem && "Event has not Item @CTEventDeal::EID_REMOVE_SELLITEM" );				
					break;
				}

				for( int iSlot = 0; iSlot < TOTAL_DEAL_INVENTORY  ; ++iSlot )
				{
					if( pIcon = m_Slots[DEAL_SELL][iSlot].GetIcon() )
					{
						if( ((CIconItem*)pIcon)->GetCItem() == (CItem*)pFragmentItem )
						{
							m_Slots[DEAL_SELL][iSlot].DetachIcon();
							return;
						}
					}
				}
				assert( 0 && "Trade List에 없는 아이템을 지우려고 합니다 @CDealDlg::Update@EID_REMOVE_BUYITEM" );			
			

				break;
			}
		case CTEventDeal::EID_CLEAR:
			{
				for( int i = 0; i < MAX_DEAL_SELLBUY; ++i )
					for( int iSlot = 0; iSlot < TOTAL_DEAL_INVENTORY; ++iSlot )
						m_Slots[i][iSlot].DetachIcon();
			
				break;
			}
		default:
			assert( 0 && "Event Type is Invalid @CDealDlg::Update" );
			break;
		}
	}
	else
	{
		assert( 0 && "Event is NULL or Invalid Type @CDealDlg::Update" );
	}
}

void CDealDLG::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	for( int i = 0; i < MAX_DEAL_SELLBUY; ++i )
		for( int iSlot = 0; iSlot < TOTAL_DEAL_INVENTORY; ++iSlot )
			m_Slots[i][iSlot].MoveWindow( m_sPosition );
}

void CDealDLG::Update(POINT ptMouse)
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );
	for( int i = 0; i < MAX_DEAL_SELLBUY; ++i )
		for( int iSlot = 0; iSlot < TOTAL_DEAL_INVENTORY; ++iSlot )
			m_Slots[i][iSlot].Update( ptMouse );
}

void CDealDLG::Draw()
{
	if(!IsVision()) return ;
	CTDialog::Draw();

	for( int i = 0; i < MAX_DEAL_SELLBUY; ++i )
		for( int iSlot = 0; iSlot < TOTAL_DEAL_INVENTORY; ++iSlot )
			m_Slots[i][iSlot].Draw();


	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );
	
	__int64	Price;
	//총구입가 
	Price = CDealData::GetInstance().GetTotal_BUY();

	const int money_buffer_size = 64;
	char money_buffer[money_buffer_size];
	CGameUtil::ConvertMoney2String( Price, money_buffer, money_buffer_size );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 39, 139, g_dwWHITE, "%s", money_buffer );
	
	//총판매가 
	Price = CDealData::GetInstance().GetTotal_SELL();
	CGameUtil::ConvertMoney2String( Price, money_buffer, money_buffer_size );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 39, 272, g_dwWHITE, "%s", money_buffer ); 


	if( CStore::GetInstance().IsUnionStore() )
		g_DrawImpl.Draw( m_sPosition.x + 12, m_sPosition.y + 135, IMAGE_RES_STATE_ICON, UNION_MARK( CStore::GetInstance().GetUnionNo() ) );
}

void CDealDLG::Hide()
{
	CTDialog::Hide();
	CDealData::GetInstance().ClearTradeList();
}

unsigned int CDealDLG::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if(!IsVision()) return 0;

	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		for( int i = 0; i < MAX_DEAL_SELLBUY; ++i )
		{
			for( int iSlot = 0; iSlot < TOTAL_DEAL_INVENTORY; ++iSlot )
			{
				if( m_Slots[i][iSlot].Process( uiMsg, wParam, lParam ) )
					return uiMsg;
			}
		}

		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			On_LButtonUP( iProcID, wParam, lParam );
			break;
		default:
			break;
		}
		return uiMsg;

	}
	return 0;
}


bool CDealDLG::On_LButtonUP( unsigned iProcID, WPARAM wParam, LPARAM lParam )
{
	switch(iProcID)
	{
	case DEAL_BTN_CLOSE:
		g_itMGR.Close_store();
		return true;
	case DEAL_BTN_OK:
		CDealData::GetInstance().SendTradeReq();
		return true;
	default:
		break;
	}

	return false;
}

