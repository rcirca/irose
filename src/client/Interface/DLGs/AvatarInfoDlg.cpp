#include "stdafx.h"
#include "avatarinfodlg.h"
#include "tgamectrl/tguage.h"
#include "tgamectrl/resourcemgr.h"

#include "../it_mgr.h"
#include "../CToolTipMgr.h"
#include "../CTDrawImpl.h"
#include "../IO_ImageRes.h"
#include "../../Object.h"
#include "../../System/CGame.h"
#include "../../JCommandState.h"

#include "../icon/ciconitem.h"
#include "../../gamecommon/item.h"
#include "../../gamedata/event/cteventitem.h"

#include "../Misc/GameUtil.h"
#include "tgamectrl/tstatic.h"
#include "CHelpMgr.h"
#include "CMinimapDLG.h"
#include "Network\CNetwork.h"



///장착된 원거리무기에 적합한 소모탄을 장착하지 않았을경우 대신 아이콘 표시할 아이템 번호
const int c_iRepresentBulletItemNo[MAX_SHOT_TYPE] = { 301, 321, 341 };

CAvatarInfoDlg::CAvatarInfoDlg( int iType )
{
	SetDialogType( iType );
	POINT ptOffset = { 186, 36 };

	m_Slot.SetHeight( 40 );
	m_Slot.SetWidth( 40 );
	m_Slot.SetParent( iType );
	m_Slot.SetOffset( ptOffset );

	m_pRepresentBullet = new CItem;
}

CAvatarInfoDlg::~CAvatarInfoDlg(void)
{
	m_Slot.DetachIcon();
	SAFE_DELETE( m_pRepresentBullet );
}

bool CAvatarInfoDlg::Create ( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{
		RefreshDlg();
		return true;
	}
	return false;
}

void CAvatarInfoDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	m_Slot.MoveWindow( m_sPosition );
}

void CAvatarInfoDlg::Draw()
{
	if( !IsVision() ) return;

	CTDialog::Draw();

#ifdef _NEWUI
#else
	m_Slot.Draw();
#endif


	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

#ifdef _NEWUI
	// 이름	
	RECT rt;
	SetRect( &rt, 28, 6, 150, 25 );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, &rt, D3DCOLOR_XRGB( 255, 255, 255 ), DT_TOP|DT_LEFT, "%s", g_pAVATAR->Get_NAME() );

	// Level, job, weight, exp, zuly.
	const int money_buffer_size = 64;
	char money_buffer[ money_buffer_size ];
	CGameUtil::ConvertMoney2String( g_pAVATAR->Get_MONEY(), money_buffer, money_buffer_size );	

	CWinCtrl * ctrl = NULL;
	ctrl = Find("Info");
	if( ctrl )	
	{        
		((CTStatic*)ctrl)->SetString( CStr::Printf(	"■LEVEL  %d    ■%s    ■WEIGHT  %d / %d    ■EXP  %d / %d    ■ZULY  %s",
													g_pAVATAR->Get_LEVEL(),													//LEVEL
													CStringManager::GetSingleton().GetJobName( g_pAVATAR->Get_JOB() ),		//JOB
													g_pAVATAR->GetCur_WEIGHT(), g_pAVATAR->GetCur_MaxWEIGHT(),					//WEIGHT
													g_pAVATAR->Get_EXP(), g_pAVATAR->Get_NeedEXP(g_pAVATAR->Get_LEVEL()),	//EXP
													money_buffer															//MONEY
													) 
									);
	}
	

#else
	// 이름	
	RECT rt;
	SetRect( &rt, 15, 8, 150, 25 );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, &rt, D3DCOLOR_XRGB( 0, 255, 42 ), DT_CENTER|DT_VCENTER, "%s", g_pAVATAR->Get_NAME() );

	// Level
	SetRect( &rt, 180, 8, 230, 25 );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, &rt, D3DCOLOR_XRGB( 255, 255, 255 ), DT_CENTER|DT_VCENTER, "%d", g_pAVATAR->Get_LEVEL() );
	
	
#endif
	

}

void CAvatarInfoDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );

#ifdef _NEWUI
#else
	m_Slot.Update( ptMouse );
#endif

	int   iPer		= 0;	
	int   iValue	= 0;
	int	  iMax		= 1;
	char*	pszBuf;
	CTGuage* pGuage = (CTGuage*)Find( IID_GUAGE_HP );
	if( pGuage && g_pAVATAR )
	{
		int iValue = g_pAVATAR->Get_HP();
		if ( iValue < 0 ) 	iValue = 0;

		iMax = g_pAVATAR->Get_MaxHP();
		if( iMax != 0 )
		{
			iPer = (int)((float) iValue * 1000  / iMax);
			pGuage->SetValue( iPer );
			pszBuf = CStr::Printf( "%d/%d", iValue, iMax);
			pGuage->SetText( pszBuf );

			// HP%
			CWinCtrl * ctrl = Find("HP_PERCENT");
			if( ctrl )
			{
				((CTStatic*)ctrl)->SetString( CStr::Printf("%d%%", iPer/10) );
			}
		}
	}


	pGuage = (CTGuage*)Find( IID_GUAGE_MP );
	if( pGuage )
	{
		int iValue = g_pAVATAR->Get_MP();
		if ( iValue < 0 ) 	iValue = 0;

		iMax = g_pAVATAR->Get_MaxMP();
		if( iMax != 0 )
		{
			iPer = (int)((float)(iValue *1000) / iMax);
			pGuage->SetValue( iPer );
			pszBuf = CStr::Printf( "%d/%d", iValue, iMax);
			pGuage->SetText( pszBuf );

			// MP%
			CWinCtrl * ctrl = Find("MP_PERCENT");
			if( ctrl )
			{
				((CTStatic*)ctrl)->SetString( CStr::Printf("%d%%", iPer/10) );
			}
		}
	}

	__int64 i64Per = 0;
	pGuage = (CTGuage*)Find( IID_GUAGE_EXP );
	if( pGuage )
	{
		__int64 i64Value = g_pAVATAR->Get_EXP();
		if ( i64Value < 0 ) 	i64Value = 0;

		__int64 i64Max = g_pAVATAR->Get_NeedEXP(g_pAVATAR->Get_LEVEL());
		if( i64Max != 0 )
		{	

			i64Per = i64Value * 1000 / i64Max;
			pGuage->SetValue( (int)i64Per );

#ifdef _NEWUI			
			// 홍근 : 폰트가 스케일이 먹는다.
			pGuage->SetText( "" );
#else			
			pszBuf = CStr::Printf( "%.2f%%", i64Value * 100 / (float)i64Max );
			pGuage->SetText( pszBuf );
#endif
			
		}
	}

#ifdef _NEWUI
	//exp 열개로 나누어 뿌림
	if( GetIsMaxView() )
	{
		int i=0;
		CTGuage * pGuageLast = NULL;
		for( i=0; i< 10; i++)
		{
			pGuage = (CTGuage*)Find( CStr::Printf( "EXP%02d", i ) );
			if( pGuage )
			{
				if( (i64Per/100) >= i )
				{
					pGuage->Show();
					pGuage->SetValue( 1000 );
					pGuageLast = pGuage;
				}
				else
				{
					pGuage->Hide();
				}
			}
		}
		if( pGuageLast )
		{
			pGuageLast->SetValue( i64Per%100 * 10 );
		}
	}
#endif
	

	///이하 툴팁 표시
	CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog();
	if( pDlg && pDlg != this )
			return ;


	CWinCtrl* pCtrl = Find( IID_BTN_SELFTARGET );
	if( pCtrl )
	{
		if( pCtrl->IsInside( ptMouse.x, ptMouse.y ) )
		{
#ifdef _NEWUI
#else
			std::string strToolTip(TT_SELFTARGETING);
			CToolTipMgr::GetInstance().RegToolTip( ptMouse.x, ptMouse.y, strToolTip.c_str() );
#endif			
		}
	}
}

unsigned CAvatarInfoDlg::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			{
				switch( iProcID )
				{
				case IID_BTN_SELFTARGET:
#ifdef _NEWUI
					{
						CWinCtrl * pCtrl = Find( "WindowMode" );
						if(pCtrl)
						{
							pCtrl->Hide();
						}
						pCtrl = Find( "FullMode" );
						if(pCtrl)
						{
							pCtrl->Show();
						}
						SetMinView();
					}
#else
					g_UserInputSystem.SetTargetSelf();
#endif
					
					break;
				case IID_BTN_MENU:
#ifdef _NEWUI
					{
						CWinCtrl * pCtrl = Find( "WindowMode" );
						if(pCtrl)
						{
							pCtrl->Show();
						}
						pCtrl = Find( "FullMode" );
						if(pCtrl)
						{
							pCtrl->Hide();
						}
						SetMaxView();
					}
#else
					g_itMGR.OpenDialog( DLG_TYPE_MENU );
#endif					
					break;
				case IID_BTN_DIALOG2ICON:
					
					break;
				case IID_BTN_SCREENSHOT:
					//CGame::GetInstance().ScreenCAPTURE();
					g_pNet->Send_cli_SCREEN_SHOT_TIME();
					break;
				default:
					break;
				}
				break;
			}
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CAvatarInfoDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	if( pObj && strcmp( pObj->toString(), "CTEventItem" ) == 0 )
	{
		CTEventItem* pEvent = (CTEventItem*)pObj;
		int iIndex = pEvent->GetIndex();
		
		switch( pEvent->GetID() )
		{
		case CTEventItem::EID_ADD_ITEM:																			/// 인벤토리에 아이템이 추가/장착 되었을때
			{
				CItem* pAddItem = pEvent->GetItem();
				if( iIndex == EQUIP_IDX_WEAPON_R )																/// 오른손 무기가 장착되었을경우
				{
					m_Slot.DetachIcon();
					tagITEM& AddItem = pAddItem->GetItem();
					if( AddItem.GetShotTYPE() == MAX_SHOT_TYPE )												/// 소모탄을 사용하는 무기가 아닐경우
					{
						m_Slot.AttachIcon( pAddItem->CreateItemIcon() );
					}
					else///소모탄이 장착되어 있다면 그것을 그리고 아니라면 대표이미지를 그린다.
					{
						CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
						CItem* pBullet = pItemSlot->GetItem( INVENTORY_SHOT_ITEM0 + AddItem.GetShotTYPE() );
						if( pBullet )/// 해당 소모탄이 장착되어 있을경우
						{
							m_Slot.AttachIcon( pBullet->CreateItemIcon() );
						}
						else		/// 해당 소모탄이 장착되어 있지 않을경우 : 대표 이미지
						{
							tagITEM TempBullet;
							TempBullet.Clear();
							TempBullet.m_cType = ITEM_TYPE_NATURAL;
							TempBullet.m_nItemNo = c_iRepresentBulletItemNo[ AddItem.GetShotTYPE() ];
							m_pRepresentBullet->SetIndex( AddItem.GetShotTYPE() );
							m_pRepresentBullet->SetItem( TempBullet );

							m_Slot.AttachIcon( m_pRepresentBullet->CreateItemIcon() );
						}
					}
				}
				else if( iIndex >= INVENTORY_SHOT_ITEM0 && iIndex < INVENTORY_SHOT_ITEM0 + MAX_SHOT_TYPE )		/// 소모탄이 장착되었을 경우
				{
					CIcon* pPrevIcon = m_Slot.GetIcon();
					if( pPrevIcon )
					{
						CIconItem* pPrevItemIcon = (CIconItem*)pPrevIcon;
						switch( pPrevItemIcon->GetIndex() )
						{
						case SHOT_TYPE_ARROW:
						case SHOT_TYPE_BULLET:
						case SHOT_TYPE_THROW:
							if( pPrevItemIcon->GetIndex() == iIndex - INVENTORY_SHOT_ITEM0 )
							{
								m_Slot.DetachIcon();
								m_Slot.AttachIcon( pAddItem->CreateItemIcon() );
							}
							break;						
						case EQUIP_IDX_WEAPON_R:
							{
								tagITEM& PrevItem = pPrevItemIcon->GetItem();
								if( PrevItem.GetShotTYPE() == iIndex - INVENTORY_SHOT_ITEM0 )
								{
									m_Slot.DetachIcon();
									m_Slot.AttachIcon( pAddItem->CreateItemIcon() );
								}
								break;
							}
						default:
							break;
						}
					}
				}	
				break;
			}
		case CTEventItem::EID_DEL_ITEM:/// 인벤토리에서  아이템을 버리거나 탈착하였을때
			{
				if( iIndex == EQUIP_IDX_WEAPON_R )/// 오른손 무기의 경우
				{
					m_Slot.DetachIcon();
				}
				else if( iIndex >= INVENTORY_SHOT_ITEM0 && iIndex < INVENTORY_SHOT_ITEM0 + MAX_SHOT_TYPE )///소모탄일경우
				{
					CIcon* pIcon = m_Slot.GetIcon();
					if( pIcon )
					{
						CIconItem* pItemIcon = (CIconItem*)pIcon;
						
						
						if( pItemIcon->GetIndex() == EQUIP_IDX_WEAPON_R )					///장착된무기가 있을경우
						{
							tagITEM& EquipItem = pItemIcon->GetItem();
							if( EquipItem.GetShotTYPE() == iIndex - INVENTORY_SHOT_ITEM0 )	/// 장착되어 있는 무기의 소모탄일경우
							{
								tagITEM TempBullet;
								TempBullet.Clear();
								TempBullet.m_cType = ITEM_TYPE_NATURAL;
								TempBullet.m_nItemNo = c_iRepresentBulletItemNo[ EquipItem.GetShotTYPE() ];

								m_pRepresentBullet->SetIndex( EquipItem.GetShotTYPE() );
								m_pRepresentBullet->SetItem( TempBullet );
								
								m_Slot.DetachIcon();///기존 아이콘을 빼고
								m_Slot.AttachIcon( m_pRepresentBullet->CreateItemIcon() );	///대표 Item의 Icon을 Attach한다.
								
							}
						}
						else if( pItemIcon->GetIndex() == iIndex )						  	/// 소모탄이 탈착되어 지는 경우 한번더 체크
						{
							CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
							CItem* pEquipItem = pItemSlot->GetItem( EQUIP_IDX_WEAPON_R );
							m_Slot.DetachIcon();
							if( pEquipItem )
							{
								tagITEM& EquipItem = pEquipItem->GetItem();
								if( EquipItem.GetShotTYPE() == MAX_SHOT_TYPE )
								{
									m_Slot.AttachIcon( pEquipItem->CreateItemIcon() );
								}
								else
								{
									tagITEM TempBullet;
									TempBullet.Clear();
									TempBullet.m_cType = ITEM_TYPE_NATURAL;
									TempBullet.m_nItemNo = c_iRepresentBulletItemNo[ EquipItem.GetShotTYPE() ];

									m_pRepresentBullet->SetIndex( EquipItem.GetShotTYPE() );
									m_pRepresentBullet->SetItem( TempBullet );
									
									m_Slot.AttachIcon( m_pRepresentBullet->CreateItemIcon() );
								}
							}
						}
					}
				}	
				else
				{
					///나머지는 무시
				}
				break;
			}
		default:
			break;
		}
	}
	else
	{
		assert( 0 && "CTEvent is NULL or Invalid Type @CAvatarInfoDlg::Update" );
	}
}


void CAvatarInfoDlg::RefreshDlg()
{
#ifdef _NEWUI
	CTGuage* pGuage = (CTGuage*)Find( IID_GUAGE_EXP );
	if( pGuage )
	{			
		pGuage->SetSizeFit(true);
		pGuage->SetWidth( getScreenWidth() - pGuage->GetPosition().x );
	}

	CWinCtrl * ctrl = Find( "EXP" );
	if( ctrl )
	{					
		ctrl->Hide();

		//exp 열개 위치 세팅
		int i=0;		
		CTGuage * pGuage = NULL;
		float fPosUnit = (float)(getScreenWidth() - ctrl->GetPosition().x)/10.0f;
		int iPosGap = -1;		

		for( i=0; i< 10; i++)
		{
			pGuage = (CTGuage*)Find( CStr::Printf( "EXP%02d", i ) );
			if( pGuage )
			{
				pGuage->SetWidth( fPosUnit + iPosGap );
				pGuage->SetPositionX( ctrl->GetPosition().x + fPosUnit * i );
				pGuage->SetSizeFit( true );
			}
		}		
	}

	ctrl = Find("Exp_BG");
	if( ctrl )
	{
		ctrl->SetScaleWidth(  200  );	//충분히 크게.
	}		
	ctrl = Find("Info_bg");
	if( ctrl )
	{
		ctrl->SetScaleWidth( 200 );		//충분히 크게.		
	}

	
	POINT pt;
	pt.y = 0;

	ctrl = Find("SOUND");
	if( ctrl )
	{		
		pt.x = getScreenWidth()-93;		
		ctrl->SetOffset( pt );
	}
	ctrl = Find("CHAT");
	if( ctrl )
	{
		pt.x = getScreenWidth()-70;
		ctrl->SetOffset( pt );
	}
	ctrl = Find("TIP");
	if( ctrl )
	{
		pt.x = getScreenWidth()-47;
		ctrl->SetOffset( pt );
	}
	ctrl = Find("CTRL");
	if( ctrl )
	{
		pt.x = getScreenWidth()-24;
		ctrl->SetOffset( pt );
	}

	CTStatic * pStatic = (CTStatic *)Find("HP_PERCENT");
	if( pStatic )
	{
		pStatic->SetFont( FONT_NORMAL_OUTLINE );
	}
	pStatic = (CTStatic *)Find("MP_PERCENT");
	if( pStatic )
	{
		pStatic->SetFont( FONT_NORMAL_OUTLINE );
	}

#endif
}

void CAvatarInfoDlg::Show()
{
	CTDialog::Show();
	
	CWinCtrl * ctrl = Find("FullMode");
	if( ctrl )
	{
		ctrl->Hide();
	}	


	SetMiniMapPos();


}

void CAvatarInfoDlg::Hide()
{
	CTDialog::Hide();
}

void CAvatarInfoDlg::SetMaxView( bool bIsMax )
{

#ifdef _NEWUI
#else
	return;
#endif

	CWinCtrl * ctrl = NULL;

	ctrl = Find("Info_bg");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}
	ctrl = Find("Exp_BG");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}
	ctrl = Find("CTRL");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}
	ctrl = Find("TIP");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}

	ctrl = Find("CHAT");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}
	ctrl = Find("SOUND");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}
	ctrl = Find("Main");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}
	ctrl = Find("HP");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}
	ctrl = Find("MP");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}

	ctrl = Find("HP_PERCENT");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}
	ctrl = Find("MP_PERCENT");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}
	ctrl = Find("Info");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}


	int i=0;		
	CTGuage * pGuage = NULL;
	for( i=0; i< 10; i++)
	{		
		pGuage = (CTGuage*)Find( CStr::Printf( "EXP%02d", i ) );
		if( pGuage )
		{	
			if(bIsMax)	{	pGuage->Show();	}	else {	pGuage->Hide();	}
		}
	}	


	ctrl = Find("Caption");
	if( ctrl )
	{	
		if(bIsMax)
		{
			this->SetWidth( 1600 );
		}	
		else
		{
			this->SetWidth( ctrl->GetWidth() );			
		}	
	}




	g_HelpMgr.SetShowTip( bIsMax );

	m_bIsMaxView = bIsMax;

	SetMiniMapPos();

	
}

void CAvatarInfoDlg::SetMinView()
{
	SetMaxView(false);
	
}

bool CAvatarInfoDlg::GetIsMaxView()
{
	return m_bIsMaxView;
}

void CAvatarInfoDlg::SetInterfacePos_After()
{
	SetMiniMapPos();	
}

void CAvatarInfoDlg::SetMiniMapPos()
{

#ifdef _NEWUI
#else
	return;
#endif

	//SET MINIMAP POSY
	CMinimapDLG * pDLG = (CMinimapDLG*)g_itMGR.FindDlg( DLG_TYPE_MINIMAP );
	if( pDLG )
	{
		if( GetIsMaxView() )
		{		
			CWinCtrl * ctrl = Find( "EXP" );
			if( ctrl )
			{					
				pDLG->SetAvataInfoPos( ctrl->GetPosition().y + 7 );
			}			
		}
		else
		{			            
			pDLG->SetAvataInfoPos( 0 );
		}		
	}
}