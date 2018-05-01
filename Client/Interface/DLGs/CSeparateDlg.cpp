#include "stdafx.h"
#include ".\cseparatedlg.h"
#include "../CDragItem.h"
#include "../icon/ciconitem.h"
#include "../it_mgr.h"
#include "../command/uicommand.h"
#include "../../common/cinventory.h"
#include "../../gamedata/event/cteventseparate.h"
#include "../../gamedata/cseparate.h"
#include "../../gamecommon/item.h"
#include "../../object.h"
#include "../TypeResource.h"


CSeparateDlg::CSeparateDlg(void)
{
	CSlot Slot;
	POINT pt	=	{ 169, 172 };
	int iType	=	DLG_TYPE_SEPARATE;

	for( int i = 0; i < 4; ++i )
	{
		Slot.SetOffset( pt );		
		Slot.SetParent( iType );

		m_OutputItemSlots.push_back( Slot );
		pt.y += 46;
	}
	
	///제조할 아이템이 들어갈 Slot
	m_pDragItem = new CDragItem;
	CTCommand* pCmd = new CTCmdTakeOutItemFromSeparateDlg;

	m_pDragItem->AddTarget( iType, NULL );
	m_pDragItem->AddTarget( CDragItem::TARGET_ALL, pCmd );


	pt.x = 169;
	pt.y = 99;
	m_MaterialItemSlot.SetOffset( pt );
	m_MaterialItemSlot.SetParent( iType );
	m_MaterialItemSlot.SetDragAvailable();
	m_MaterialItemSlot.SetDragItem( m_pDragItem );

	SetDialogType( iType );
}

CSeparateDlg::~CSeparateDlg(void)
{
	SAFE_DELETE( m_pDragItem );
}

void CSeparateDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj && strcmp( pObj->toString() ,"Separate" ) == 0 );
	if( pObj == NULL || strcmp( pObj->toString() ,"Separate" ) ) return;

	CTEventSeparate* pEvent = (CTEventSeparate*)pObj;
	switch( pEvent->GetID() )
	{
	case CTEventSeparate::EID_REMOVE_MATERIAL_ITEM:
		m_MaterialItemSlot.DetachIcon();
		break;
	case CTEventSeparate::EID_SET_MATERIAL_ITEM:
		{
			CItem* pItem = pEvent->GetItem();
			assert( pItem );
			if( pItem  )
				m_MaterialItemSlot.AttachIcon( pItem->CreateItemIcon() );
		
			break;
		}
	case CTEventSeparate::EID_REMOVE_OUTPUT_ITEM:
		{
			int iIndex = pEvent->GetIndex();
			assert( iIndex >= 0 && iIndex < (int)m_OutputItemSlots.size() );
			if( iIndex  >= 0 && iIndex < (int)m_OutputItemSlots.size() )
				m_OutputItemSlots[iIndex].DetachIcon();	
			break;
		}
	case CTEventSeparate::EID_SET_OUTPUT_ITEM:
		{
			int iIndex = pEvent->GetIndex();
			assert( iIndex >= 0 && iIndex < (int)m_OutputItemSlots.size() );
			if( iIndex  >= 0 && iIndex < (int)m_OutputItemSlots.size() )
			{
				CItem* pItem = pEvent->GetItem();
				assert( pItem && m_OutputItemSlots[iIndex].GetIcon() == NULL );
				if( pItem && m_OutputItemSlots[iIndex].GetIcon() == NULL )
					m_OutputItemSlots[iIndex].AttachIcon( pItem->CreateItemIcon() );
			}
			break;
		}
	default:
		assert( 0 && "Invalid Separate Event Type" );
		break;
	}
}

unsigned CSeparateDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		if( m_MaterialItemSlot.Process( uiMsg, wParam , lParam ) )
			return uiMsg;

		if( uiMsg == WM_LBUTTONUP )
		{
			switch( uiProcID )
			{
			case IID_BTN_START:
				{

					CSeparate& Separate = CSeparate::GetInstance();
					CIcon* pIcon = m_MaterialItemSlot.GetIcon();
					if( pIcon == NULL )///재료가 없다면 Disable
					{
						g_itMGR.OpenMsgBox(STR_SEPARATE_EMPTY_TARGETITEM);
						break;
					}

					int iCount = 0;
					std::vector< CSlot >::iterator iterOutput;
					for( iterOutput = m_OutputItemSlots.begin(); iterOutput != m_OutputItemSlots.end(); ++iterOutput )
					{
						if( iterOutput->GetIcon() )
							++iCount;
					}

					if( iCount <= 0 )
					{
						g_itMGR.OpenMsgBox( STR_CANT_SEPARATE );
						break;
					}

					switch( Separate.GetType() )
					{
					case CSeparate::TYPE_SKILL:
						{
							if( Separate.GetRequireMp() > g_pAVATAR->Get_MP() )
							{
								g_itMGR.OpenMsgBox(STR_NOT_ENOUGH_MANA);
								return uiMsg;
							}
							break;
						}
					case CSeparate::TYPE_NPC:
						{
							if( Separate.GetRequireMoney() > g_pAVATAR->Get_MONEY() )
							{
								g_itMGR.OpenMsgBox(STR_NOT_ENOUGH_MONEY);
								return uiMsg;
							}
							break;
						}
					default:
						break;
					}

					CIconItem* pItem = (CIconItem*)pIcon;
					if( !HasEnoughInventoryEmptySlot( pItem,m_OutputItemSlots ) )
					{
						g_itMGR.OpenMsgBox(STR_NOTENOUGH_EMPTY_INVENTORYSLOT);
						break;
					}

					CSeparate::GetInstance().Send_CRAFT_BREAKUP_REQ();
					break;
				}
			case IID_BTN_CLOSE:
				{
					Hide();
					CSeparate::GetInstance().RemoveItem();
					break;
				}
			default:
				break;
			}
		}
		return uiMsg;
	}
	return 0;
}

void CSeparateDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );

	m_MaterialItemSlot.Update( ptMouse );
	std::vector< CSlot >::iterator iter;
	for( iter = m_OutputItemSlots.begin(); iter != m_OutputItemSlots.end(); ++iter )
		iter->Update( ptMouse );


	if( CSeparate::GetInstance().GetType() == CSeparate::TYPE_NPC )
	{
		WORD wNpcSvrIdx = CSeparate::GetInstance().GetNpcSvrIdx();
		CObjCHAR* pNpc = g_pObjMGR->Get_ClientCharOBJ( wNpcSvrIdx, false );
		if( !( pNpc && g_pAVATAR->IsInRANGE( pNpc, AVT_CLICK_EVENT_RANGE )) )
		{
			Hide();
			CSeparate::GetInstance().RemoveItem();
		}
	}
}

void CSeparateDlg::Show()
{
	if( IsVision() ) return;
	CTDialog::Show();

	switch( CSeparate::GetInstance().GetType() )
	{
	case CSeparate::TYPE_SKILL:
		HideChild( IID_TEXT_COST );
		break;
	default:
		break;
	}
}

//*-------------------------------------------------------------------/
/// 분리/분해후 아이템이 들어갈 자리가 충분히 있는가?
//*-------------------------------------------------------------------/
bool CSeparateDlg::HasEnoughInventoryEmptySlot( CIconItem* pItemIcon, std::vector<CSlot>& OutputItems )
{
	assert( pItemIcon );
	if( pItemIcon == NULL ) return false;

	///일단 모든 타입의 빈 슬롯을 구한다.
	short nEmptySlotCount[ MAX_INV_TYPE ];

	CInventory* pInven = g_pAVATAR->GetInventory();
	assert( pInven );

	for( int i = INV_WEAPON; i < MAX_INV_TYPE; ++i )
		nEmptySlotCount[i] = pInven->GetEmptyInvenSlotCount( (t_InvTYPE)i );


	/// 분리/분해할 아이템의 자리를 빈슬롯으로 계산한다( 개수가 없거나 개수가 있지만 1개일경우)
	tagITEM& Item = pItemIcon->GetItem();
	if( (Item.IsEnableDupCNT() && Item.GetQuantity() == 1 ) || !Item.IsEnableDupCNT() )
		nEmptySlotCount[ CInventory::GetInvPageTYPE( Item ) ]++;


	CIcon*		pIcon = NULL;
	CIconItem*	pOutputItemIcon = NULL;
	std::vector<CSlot>::iterator iter;
	for( iter = OutputItems.begin(); iter != OutputItems.end(); ++iter )
	{
		if( pIcon = iter->GetIcon() )
		{
			pOutputItemIcon = ( CIconItem* )pIcon;
			nEmptySlotCount[ CInventory::GetInvPageTYPE( pOutputItemIcon->GetItem() ) ]--;
			if( nEmptySlotCount[ CInventory::GetInvPageTYPE( pOutputItemIcon->GetItem() ) ] <= 0 )
				return false;
		}
	}
	return true;
}

void CSeparateDlg::Draw()
{
	if( !IsVision() ) return;

	CTDialog::Draw();
	m_MaterialItemSlot.Draw();
	CIcon* pIcon = NULL;
	RECT rcDraw;

	if( pIcon = m_MaterialItemSlot.GetIcon() )
	{
		SetRect( &rcDraw, -130, 21, -17, 39 );
		drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_CENTER | DT_VCENTER, pIcon->GetName() );

		///필요 MP or 줄리
		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
		::setTransformSprite( mat );
		
		SetRect( &rcDraw, 176,379,176+34,379+15 );
		CSeparate& Separate = CSeparate::GetInstance();
		switch( Separate.GetType() )
		{
		case CSeparate::TYPE_SKILL:
			drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_RIGHT,"%d", Separate.GetRequireMp() );
			break;
		case CSeparate::TYPE_NPC:
			drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_RIGHT,"%d", Separate.GetRequireMoney() );
			break;
		default:
			break;

		}
	}

	SetRect( &rcDraw, -140, 15, -55, 33 );
	std::vector< CSlot >::iterator iter;
	for( iter = m_OutputItemSlots.begin(); iter != m_OutputItemSlots.end(); ++iter )
	{
		iter->Draw();
		if( pIcon = iter->GetIcon() )
			drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_CENTER | DT_VCENTER , pIcon->GetName() );

	}
}

void CSeparateDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	m_MaterialItemSlot.MoveWindow( m_sPosition );
	std::vector< CSlot >::iterator iter;
	for( iter = m_OutputItemSlots.begin(); iter != m_OutputItemSlots.end(); ++iter )
		iter->MoveWindow( m_sPosition );
}