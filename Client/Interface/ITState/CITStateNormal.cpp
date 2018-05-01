#include "stdafx.h"
#include ".\citstatenormal.h"
#include "../it_mgr.h"
#include "../CDragNDropMgr.h"
#include "../Dlgs/ChattingDLG.h"
#include "../Dlgs/CMinimapDLG.h"

#include "../Icon/CIconDialog.h"
#include "../../System/CGame.h"

#include "TEditBox.h"
#include "TControlMgr.h"

CITStateNormal::CITStateNormal(void)
{
	m_iID = IT_MGR::STATE_NORMAL;
}

CITStateNormal::~CITStateNormal(void)
{

}

void CITStateNormal::Enter()
{

}

unsigned CITStateNormal::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	UINT uiRet = 0;

	/// Ctrl들의 내부처리를 위한 Setting
	if( uiMsg == WM_LBUTTONUP )
		CWinCtrl::SetMouseExclusiveCtrl( NULL );


	list_dlgs_ritor ritorDlgs;
	CTDialog*		pDlg = NULL;
	int				iProcessDialogType = 0;

	/// 일반적인 다이얼로그 처리
	for( ritorDlgs = g_itMGR.m_Dlgs.rbegin(); ritorDlgs != g_itMGR.m_Dlgs.rend(); ++ritorDlgs )
	{
		pDlg = *ritorDlgs;
		if( pDlg->Process( uiMsg, wParam,lParam ) )
		{
			if( uiMsg == WM_LBUTTONDOWN )
				g_itMGR.MoveDlg2ListEnd( pDlg );///iterator가 파괴될수 있다 항상 loop를 벗어날것

			uiRet = uiMsg;
			iProcessDialogType = pDlg->GetDialogType();
			break;
		}

		///모달 다이얼로그일 경우는 다음 다이얼로그를 처리할필요가 없다.
		if( pDlg->IsVision() && pDlg->IsModal() )
		{
			DWORD dwDialgType = pDlg->GetDialogType();		
			return 1;
		} 
	}
	

	/// 아이콘화된 다이얼로그 처리
	std::list< CIconDialog* >::reverse_iterator riterIcons;
	for( riterIcons = g_itMGR.m_Icons.rbegin(); riterIcons != g_itMGR.m_Icons.rend(); ++riterIcons )
	{
		if( (*riterIcons)->Process( uiMsg, wParam, lParam ) )
		{
			if( uiMsg == WM_LBUTTONDOWN )
				g_itMGR.MoveDlgIcon2ListEnd( *riterIcons );

			uiRet = uiMsg;
			break;
		}
	}
	/// 메뉴가 오픈된 상태에서 다른곳을 클릭시 메뉴를 닫는다.
	if( uiMsg == WM_LBUTTONDOWN )
	{
		if( iProcessDialogType != DLG_TYPE_MENU && iProcessDialogType != DLG_TYPE_NOTIFY )
		{
#ifdef _NEWUI
#else
			g_itMGR.CloseDialog( DLG_TYPE_MENU );
#endif
			
		}
	}

	// 단축키 처리 
	switch( it_GetKeyboardInputType() )
	{
	case CTControlMgr::INPUTTYPE_AUTOENTER:
		{
			if( !ProcessHotKeyAutoEnter( uiMsg, wParam, lParam ) )
				uiRet = 0;
		}
		break;
	case CTControlMgr::INPUTTYPE_NORMAL:
		{
			if( !ProcessHotKeyNormalInput( uiMsg, wParam, lParam ) )
				uiRet = 0;


		}
		break;
	default:
		break;
	}

	switch(uiMsg)
	{
	case WM_KEYDOWN:
		// quick 슬롯처리 
		uiRet = uiMsg;
		switch( wParam)
		{
		case 91:///Window Key
			//g_itMGR.OpenDialog( DLG_TYPE_MENU );
			/// 윈도우 시작메뉴가 시작되면서 화면전환이 되버린다. 화면이 떠 있을경우에는 
			/// 후킹해서 막아버릴까? 일단 보류
			break;
		
		case VK_ESCAPE:
			{
				g_itMGR.ProcessEscKeyDown();
				break;
			}
		default:
			uiRet = 0;
			break;
		}
		break;
	case WM_LBUTTONUP:
		// 드래그앤드롭 처리 		
//		g_DragNDrop.DropToWindow();
		CDragNDropMgr::GetInstance().DragEnd( iProcessDialogType );
		g_itMGR.DelDialogiconFromMenu();
		break;
	}
	
	///교환중일경우 이동을 막는다.
	if( (pDlg = g_itMGR.FindDlg( DLG_TYPE_EXCHANGE )) && pDlg->IsVision() )
		return uiMsg;

	return uiRet;
}

//*-----------------------------------------------------------------------------------------//
/// @brief 채팅을 한번 할때마다 Enter 를 입력 해야 하는 방식
//*-----------------------------------------------------------------------------------------//
bool CITStateNormal::ProcessHotKeyNormalInput( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( CTEditBox::s_pFocusEdit == NULL )///입력 포커스가 없는경우
	{
		switch(uiMsg)
		{
		case WM_KEYDOWN:
			{
				unsigned int oemScan = int( lParam & (0xff << 16) ) >> 16; 
				UINT vk = MapVirtualKey( oemScan, 1 );

				switch( vk )
				{
					//Dagnarus
				/*case 0x41:
					g_itMGR.OpenDialog( DLG_TYPE_CHAR);
					return true;
				case 0x43:
					g_itMGR.OpenDialog( DLG_TYPE_COMMUNITY );
					return true;
				case 0x48:
					g_itMGR.OpenDialog( DLG_TYPE_HELP );
					return true;
				case 0x49:
				case 0x56:
					g_itMGR.OpenDialog( DLG_TYPE_ITEM );
					return true;
					//Dagnarus
				/*case 0x53:
					g_itMGR.OpenDialog( DLG_TYPE_SKILL );
					return true;
				case 0x51:
					g_itMGR.OpenDialog( DLG_TYPE_QUEST );
					return true;
				case 0x4d:
					{
						CMinimapDLG* pDlg = ( CMinimapDLG*)g_itMGR.FindDlg( DLG_TYPE_MINIMAP );
						pDlg->ToggleShowMinimap();
						return true;
					}
				case 0x4c:
					{
						CMinimapDLG* pDlg = ( CMinimapDLG*)g_itMGR.FindDlg( DLG_TYPE_MINIMAP );
						pDlg->ToggleZoomMinimap();
						return true;
					}
				case 0x4e:
					{
						g_itMGR.OpenDialog( DLG_TYPE_CLAN );
						return true;
					}
			/*	case 0x58:
					g_itMGR.OpenDialog( DLG_TYPE_SYSTEM );
					return true;
				case 0x4f:
					g_itMGR.OpenDialog( DLG_TYPE_OPTION );
					return true;*/
				default:
					break;
				}
			}
			return false;
		default:
			break;
			
		}
	}
	return ProcessHotKeyAutoEnter( uiMsg, wParam, lParam );
}
//*-----------------------------------------------------------------------------------------//
/// @brief 항상 입력창에 포커스가 위치하여 엔터를 칠 필요가 없을때 HotKey처리
/// @warning NormalInput일경우에도 AutoEnter()를 처리하고 있다.
//*-----------------------------------------------------------------------------------------//
bool CITStateNormal::ProcessHotKeyAutoEnter( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uiMsg)
	{
	case WM_SYSKEYDOWN:
		{
			unsigned int oemScan = int( lParam & (0xff << 16) ) >> 16; 
			UINT vk = MapVirtualKey( oemScan, 1 );

			switch(vk)
			{
			case 0x41: // 'a'
				//캐릭터창을 연다 
				g_itMGR.OpenDialog( DLG_TYPE_CHAR);
				return true;
			case 0x43:// 'c'
				g_itMGR.OpenDialog( DLG_TYPE_COMMUNITY );
				return true;
			case 0x48://h
				g_itMGR.OpenDialog( DLG_TYPE_HELP );
				return true;
			case 0x49: // 'i'
			case 0x56: // 'v' /// 2004 / 1 / 26 / Navy /추가( SYSTEM + I가 한손으로 누르기 힘들다는 의견으로 )				
				//인벤토리를 연다 
				g_itMGR.OpenDialog( DLG_TYPE_ITEM );
				return true;
			///스킬창
			case 0x53: // 's'
				g_itMGR.OpenDialog( DLG_TYPE_SKILL );
				return true;
			/// 퀘스트창
			case 0x51: // 'q'
				g_itMGR.OpenDialog( DLG_TYPE_QUEST );
				return true;
			///미니맵 보이기 / 숨기기
			case 0x4d: // 'm'	
				{
					CMinimapDLG* pDlg = ( CMinimapDLG*)g_itMGR.FindDlg( DLG_TYPE_MINIMAP );
					pDlg->ToggleShowMinimap();
					return true;
				}
			///미니맵 확대 / 축소
			case 0x4c: // 'l'
				{
					CMinimapDLG* pDlg = ( CMinimapDLG*)g_itMGR.FindDlg( DLG_TYPE_MINIMAP );
					pDlg->ToggleZoomMinimap();
					return true;
				}
			case 0x4e: //n
				{
					g_itMGR.OpenDialog( DLG_TYPE_CLAN );
					return true;
				}
			case 0x58: //'x'
				g_itMGR.OpenDialog( DLG_TYPE_SYSTEM );
				return true;
			case 0x4f: //'o'
				g_itMGR.OpenDialog( DLG_TYPE_OPTION );
				return true;
			default:
				break;
			}
		}
		return false;
		break;
	default:
		break;
	}
	return true;
}
