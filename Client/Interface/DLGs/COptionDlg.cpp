#include "stdafx.h"
#include ".\coptiondlg.h"
#include "../../../TGameCtrl/TComboBox.h"
#include "../../../TGameCtrl/TScrollBar.h"
#include "../../Game.h"
#include "../../System/CGame.h"
#include "../../CApplication.h"
#include "../it_mgr.h"
#include "../../Common/IO_STB.h"
#include "../../JCommandState.h"
#include "../Cursor/CCursor.h"
#include "../../Sound/MusicMgr.h"

#include "JContainer.h"
#include "TabbedPane.h"
#include "TComboBox.h"
#include "TScrollBar.h"
#include "TRadioBox.h"
#include "TRadioButton.h"
#include "TCheckBox.h"
#include "TIme2.h"
#include "TControlMgr.h"
#include "TEditBox.h"

#include "../../IO_Terrain.h"
#include "TriggerInfo.h"

COptionDlg::COptionDlg(void)
{
	ZeroMemory(&m_VideoOption, sizeof( t_OptionVideo ));
	m_iTab = IID_TABVIDEO;
	m_pAdptInfo = NULL;
}

COptionDlg::~COptionDlg(void)
{
	if(m_pAdptInfo)
	{
		TI_ReleaseAdptInfo();
		m_pAdptInfo = NULL;
	}
}

bool COptionDlg::Create( const char* szIDD )
{
	char*	pszName;
	if( CTDialog::Create( szIDD ) == false )
		return false;

	m_pAdptInfo = TI_ReadAdptInfoFile ();
	int	iAdptIDX = 0;

	SystemResolution *pResolution = m_pAdptInfo->pAdapter[ iAdptIDX ].pResolution;
	int iResCNT = m_pAdptInfo->pAdapter[ iAdptIDX ].nResolution;

	SupportedScreen Screen;

	for( int i = 0; i < iResCNT; ++i )
	{
		AddSupportedScreen( pResolution[ i ].pixel_width,
					pResolution[ i ].pixel_height,
					pResolution[ i ].pixel_color,
					pResolution[ i ].frequency );
	}
	for( int i = 0; i < 17; )
	{
		AddSupportedAntiAlising(i);
		if(i == 0)
			i = 2;
		else
			i *= 2;

	}

	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		
		///VIDEO
		CJContainer* pContainer = pPane->GetTabContainer( IID_TABVIDEO );
		assert( pContainer );
		pCtrl = pContainer->Find( IID_COMBO_RESOLUTION );
		if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		{
			CTComboBox* pComboBox = (CTComboBox*)pCtrl;
			std::vector<SupportedScreen>::iterator iter;
			for( iter = m_SupportedScreens.begin(); iter != m_SupportedScreens.end(); ++iter )
			{
				pComboBox->AddItem( CStr::Printf("%d*%d*%d", 
					iter->m_iWidth, iter->m_iHeight, iter->m_iDepth ) );
			}
		}

		pCtrl = pContainer->Find( IID_LIST_CAMERA );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			pListBox->SetExtent( 1 );
			for( int i = 0; i < 3; ++i )
			{
				pszName = CAMERA_NAME(i);
				if( pszName )
					pListBox->AppendText( pszName , g_dwWHITE );
			}
		}

		pCtrl = pContainer->Find( IID_LIST_ZNZIN );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			pListBox->SetExtent( 1 );
			for( int i = 0; i < 5; ++i )
				pListBox->AppendText( "1" , g_dwWHITE );
		}

		pCtrl = pContainer->Find( IID_LIST_ANTIALSING );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		{
			CTComboBox* pComboBox = (CTComboBox*)pCtrl;
			pComboBox->AddItem( "0" );
		}


	/*	pCtrl = pContainer->Find( IID_LIST_GAMMA );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			pListBox->SetExtent( 1 );
			for( int i = 0; i < 5; ++i )
				pListBox->AppendText( "1" , g_dwWHITE );
		}*/

		///AUDIO
		pContainer = pPane->GetTabContainer( IID_TABAUDIO );
		assert( pContainer );
		pCtrl = pContainer->Find( IID_LIST_BGM );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			for( int i = 0; i < MAX_BGM_VOLUME ; ++i )
			{
				//ltoa( g_ListBgmVolume[i], szTemp, 10 );
				pListBox->AppendText( "1", g_dwWHITE );
			}
		}


		pCtrl = pContainer->Find( IID_LIST_EFFECTSOUND );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			for( int i = 0; i < MAX_EFFECT_VOLUME; ++i )
			{
				//ltoa( g_ListEffectVolume[i], szTemp, 10 );
				pListBox->AppendText("2", g_dwWHITE );
			}
		}
	}


	return true;
}
void COptionDlg::OnLButtonDown( unsigned iProcID )
{
	switch( iProcID )
	{
	case IID_COMBO_RESOLUTION:
		{
			CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
			if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
			{
				CTabbedPane* pPane = (CTabbedPane*)pCtrl;
				CJContainer* pContainer = pPane->GetTabContainer( IID_TABVIDEO );
				assert( pContainer );

				pCtrl = pContainer->Find( IID_COMBO_RESOLUTION );
				if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
				{
					CTComboBox* pComboBox = (CTComboBox*)pCtrl;
					short nResolutionIndex = pComboBox->GetSelectedItemID();

					pCtrl = pContainer->Find( IID_COMBO_FREQUENCY );
					if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
					{
						CTComboBox* pComboBox = (CTComboBox*)pCtrl;
						std::vector< int >::iterator iter;
						short iIndex = 0;
						short iSelectIndex = 0;
						pComboBox->ClearItem();
						for( iter = m_SupportedScreens[nResolutionIndex].m_Frequencys.begin();
							iter != m_SupportedScreens[nResolutionIndex].m_Frequencys.end();
							++iter, ++iIndex )
						{
							pComboBox->AddItem( CStr::Printf("%dHz",*iter ) );
							if( *iter == m_VideoOption.tResolution.iFrequency )
								iSelectIndex = iIndex;
						}
						pComboBox->SetSelectedItem( iSelectIndex );
					}
				}
			}
			break;
		}
	default:
		break;
	}
}
void COptionDlg::OnLButtonUp( unsigned iProcID )
{
	switch( iProcID )
	{
	case IID_BTN_ICONIZE:
		g_itMGR.AddDialogIcon( 	GetDialogType() );
		break;
	case IID_BTN_INITIALIZE:
		g_itMGR.InitInterfacePos();
		InitOption();
		break;
	case IID_TABBUTTON_VIDEO:
		m_iTab = IID_TABVIDEO;
		break;
	case IID_TABBUTTON_AUDIO:
		m_iTab = IID_TABAUDIO;
		break;
	case IID_TABBUTTON_PLAY:
		m_iTab = IID_TABPLAY;
		break;
	case IID_TABBUTTON_COMMUNITY:
		m_iTab = IID_TABCOMMUNITY;
		break;
	case IID_TABBUTTON_KEYBOARD:
		m_iTab = IID_TABKEYBOARD;
		break;
	case IID_BTN_CONFIRM:
		ChangeOption();
		SaveCurrentOption();
		Hide();
		break;
	//case IID_BTN_CANCEL:
	//	DoCancel();
	//	Hide();
	//	break;
	case IID_BTN_CLOSE:
		DoCancel();
		Hide();
		break;
	default:
		break;
	}
}

unsigned int COptionDlg::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if(!IsVision()) return 0;
	
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONDOWN:
			OnLButtonDown( iProcID );
			break;
		case WM_LBUTTONUP:
			OnLButtonUp( iProcID );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void COptionDlg::Show()
{
	GetCurrentOption();
	CTDialog::Show();
	//SaveCurrentOption();
}
void COptionDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );

	///실시간으로 볼륨 조정
	switch( m_iTab )
	{
	case IID_TABAUDIO:
		{
			CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
			assert( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE );
			if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
			{
				CTabbedPane* pPane = (CTabbedPane*)pCtrl;
				CJContainer* pContainer = pPane->GetTabContainer( IID_TABAUDIO );
				assert( pContainer );
				if( pContainer == NULL ) 
					return;


				pCtrl = pContainer->Find( IID_LIST_BGM );
				assert( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX );
				if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
				{
					CTListBox* pListBox = ( CTListBox* )pCtrl;
					CMusicMgr::GetSingleton().SetVolume( g_ListBgmVolume[ pListBox->GetValue() ] );
				}

				pCtrl = pContainer->Find( IID_LIST_EFFECTSOUND );
				assert( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX );
				if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
				{
					CTListBox* pListBox = ( CTListBox* )pCtrl;
					g_pSoundLIST->SetVolume( g_ListEffectVolume[ pListBox->GetValue() ] );
				}
			}
			break;
		}
	case IID_TABVIDEO:
		{
			/*CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
			assert( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE );
			if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
			{
				CTabbedPane* pPane = (CTabbedPane*)pCtrl;
				CJContainer* pContainer = pPane->GetTabContainer( IID_TABVIDEO );
				assert( pContainer );
				if( pContainer == NULL ) 
					return;


				pCtrl = pContainer->Find( IID_LIST_GAMMA );
				assert( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX );
				if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
				{
					CTListBox* pListBox = ( CTListBox* )pCtrl;
					setGammaValue( c_GammaValues[ pListBox->GetValue() ] );		
				}
			}					*/
			break;
		}
	default:
		break;
	}
}



void COptionDlg::ChangeResolution( t_OptionResolution Resolution )
{

	int iWidth  = Resolution.iWidth;
	int iHeight = Resolution.iHeight;
	int iDepth  = Resolution.iDepth;
	int iFrequency = Resolution.iFrequency;

	///변경된 사항이 없다면 바꾸지 않는다.
	if( iWidth == m_VideoOption.tResolution.iWidth &&
		iHeight == m_VideoOption.tResolution.iHeight &&
		iDepth == m_VideoOption.tResolution.iDepth &&
		iFrequency == m_VideoOption.tResolution.iFrequency
		)
		return;


	if( iWidth != m_VideoOption.tResolution.iWidth ||
		iHeight != m_VideoOption.tResolution.iHeight ||
		iDepth != m_VideoOption.tResolution.iDepth )
	{
		g_pCApp->ResizeWindowByClientSize( iWidth, iHeight, iDepth, true );
		m_VideoOption.tResolution.iWidth  = iWidth;
		m_VideoOption.tResolution.iHeight = iHeight;
		m_VideoOption.tResolution.iDepth  = iDepth;
		g_ClientStorage.SetVideoOption( m_VideoOption );
	}
	
	if( iFrequency != m_VideoOption.tResolution.iFrequency )
	{
		setMonitorRefreshRate( iFrequency );
		//resetScreen();
	}

	resetScreen();
	
	CCursor::GetInstance().ReloadCursor();


//	g_pCApp->SetWIDTH( g_GameDATA.m_nScrWidth );
//	g_pCApp->SetHEIGHT( g_GameDATA.m_nScrHeight );

	SIZE sizeScreen = { g_pCApp->GetWIDTH(), g_pCApp->GetHEIGHT() };
	CTIme::GetInstance().ChangeScreenSize( sizeScreen );
	_RPT2( _CRT_WARN,"Screen Width %d, Height %d\n", g_pCApp->GetWIDTH(), g_pCApp->GetHEIGHT() );
	g_itMGR.InitInterfacePos();
}

void COptionDlg::DoCancel()
{
	CMusicMgr::GetSingleton().SetVolume( g_ClientStorage.GetBgmVolumeByIndex( m_SoundOption.iBgmVolume ) );	
	g_pSoundLIST->SetVolume(g_ClientStorage.GetEffectVolumeByIndex( m_SoundOption.iEffectVolume ) );

}

void COptionDlg::ChangeKeyboardOption()
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		CJContainer* pContainer = pPane->GetTabContainer( IID_TABKEYBOARD );
		assert( pContainer );

		pCtrl = pContainer->Find( IID_RADIOBOX_KEYBOARD );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			UINT uiPressedBtn = pRadioBox->GetPressedButtonID();
			switch( uiPressedBtn )
			{
			case IID_RADIOBTN_ENTERCHATTING:
				{
					m_KeyboardOption.iChattingMode = 0;
					it_SetKeyboardInputType( CTControlMgr::INPUTTYPE_NORMAL );
					break;
				}
			case IID_RADIOBTN_NORMALCHATTING:
				{
					m_KeyboardOption.iChattingMode = 1;
					it_SetKeyboardInputType( CTControlMgr::INPUTTYPE_AUTOENTER );
					///현재 포커스를 가진 EditBox가 없을경우 채팅창 EditBox에 강제로 포커스를 준다.
					CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT );
					if( CTEditBox::s_pFocusEdit == NULL && pDlg != NULL  )
						pDlg->Show();

					break;
				}
			default:
				break;
			}
		}
	}
}
void COptionDlg::ChangeVideoOption()
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		CJContainer* pContainer = pPane->GetTabContainer( IID_TABVIDEO );
		assert( pContainer );

		pCtrl = pContainer->Find( IID_LIST_CAMERA );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			short iID = pListBox->GetValue();
			if( m_VideoOption.iCamera != iID )
			{
				g_ClientStorage.ApplyCameraOption( iID );
				m_VideoOption.iCamera = iID;
			}
		}

		pCtrl = pContainer->Find( IID_LIST_ZNZIN );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			short iID = pListBox->GetValue();
			if( m_VideoOption.iPerformance != iID )
			{
				setDisplayQualityLevel( c_iPeformances[iID] );
				
				//UINT i = g_ClientStorage.GetVideoResolution();
				//
				//int iWidth  = RESOLUTION_WIDTH( i );
				//int iHeight = RESOLUTION_HEIGHT( i );
				//int iDepth  = RESOLUTION_DEPTH( i );

				//setScreen( iWidth, iHeight, iDepth, g_pCApp->IsFullScreenMode() );
				//setBuffer ( iWidth, iHeight, iDepth );
				//resetScreen();

				m_VideoOption.iPerformance = iID;

				//----------------------------------------------------------------------------------------------------				
				/// @brief wide terrain 갱신
				//----------------------------------------------------------------------------------------------------
				/*if( iID < 4 )
					g_pTerrain->SubWideMap( 0 );	*/			
			}
		}


		pCtrl = pContainer->Find( IID_COMBO_RESOLUTION );
		if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		{
			CTComboBox* pComboBox = (CTComboBox*)pCtrl;
			short iID = pComboBox->GetSelectedItemID();

			assert( iID >= 0 && iID < (int)m_SupportedScreens.size() );
			if( iID >= 0 && iID < (int)m_SupportedScreens.size() )
			{
				pCtrl = pContainer->Find( IID_COMBO_FREQUENCY );
				if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
				{
					CTComboBox* pComboBox = (CTComboBox*)pCtrl;
					int iFrequencyIndex = pComboBox->GetSelectedItemID();
					assert( iFrequencyIndex >= 0 && iFrequencyIndex < (int)m_SupportedScreens[iID].m_Frequencys.size() );

					if( iFrequencyIndex >= 0 && iFrequencyIndex < (int)m_SupportedScreens[iID].m_Frequencys.size() )
					{
						t_OptionResolution Resolution;
						Resolution.iWidth      = m_SupportedScreens[ iID ].m_iWidth;
						Resolution.iHeight     = m_SupportedScreens[ iID ].m_iHeight;
						Resolution.iDepth      = m_SupportedScreens[ iID ].m_iDepth;
						Resolution.iFrequency  = m_SupportedScreens[ iID ].m_Frequencys[ iFrequencyIndex];

						ChangeResolution( Resolution );
						
						m_VideoOption.tResolution = Resolution;
					}
				}
			}
		}

		pCtrl = pContainer->Find( IID_RADIOBOX_FULLSCREEN );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			UINT uiPressedBtn = pRadioBox->GetPressedButtonID();
			switch( uiPressedBtn )
			{
			case IID_RADIOBUTTON_FULLSCREEN:
				{
					if( m_VideoOption.iFullScreen == 0 && !g_pCApp->IsFullScreenMode())
					{
						CGame::GetInstance().ChangeScreenMode();
						m_VideoOption.iFullScreen = 1;
					}
					break;
				}
			case IID_RADIOBUTTON_WINDOWMODE:
				{
					if( m_VideoOption.iFullScreen == 1 && g_pCApp->IsFullScreenMode() )
					{
						CGame::GetInstance().ChangeScreenMode();
						m_VideoOption.iFullScreen = 0;
					}
					break;
				}
			default:
				break;
			}
		}

		pCtrl = pContainer->Find( IID_LIST_ANTIALSING );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		{
			CTComboBox* pComboBox = (CTComboBox*)pCtrl;
			//char* cID = pComboBox->GetSelectedItem();
			short iID = atoi( pComboBox->GetSelectedItem() );
			if( m_VideoOption.iAntiAlising != iID )
			{
				setFullSceneAntiAliasing( iID );
				m_VideoOption.iAntiAlising = iID;
				resetScreen();
			}
		}
	}
}
void COptionDlg::ChangeAudioOption()
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		CJContainer* pContainer = pPane->GetTabContainer( IID_TABAUDIO );
		assert( pContainer );

		pCtrl = pContainer->Find( IID_LIST_BGM );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			m_SoundOption.iBgmVolume = pListBox->GetValue();

			CMusicMgr::GetSingleton().SetVolume( g_ListBgmVolume[ pListBox->GetValue() ] );			
		}
		else
		{
			assert( 0 && "Not Found Bgm Volume List" );
		}

		pCtrl = pContainer->Find( IID_LIST_EFFECTSOUND );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			m_SoundOption.iEffectVolume = pListBox->GetValue();
			g_pSoundLIST->SetVolume( g_ListEffectVolume[ pListBox->GetValue() ] );		
		}
		else
		{
			assert( 0 && "Not Found Effect Volume List" );
		}
	}
}

void COptionDlg::ChangePlayOption()
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		CJContainer* pContainer = pPane->GetTabContainer( IID_TABPLAY );
		assert( pContainer );
		pCtrl = pContainer->Find( IID_RADIOBOX_CONTROL );

		assert( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			switch( pRadioBox->GetPressedButtonID() )
			{
			case IID_RADIOBUTTON_DEFAULTCONTROL:
				g_UserInputSystem.ChangeUserInputStyle( DEFAULT_USER_INPUT_STYLE );
				m_PlayOption.uiControlType = 0;
				break;
			case IID_RADIOBUTTON_SEVENHEARTSCONTROL:
				g_UserInputSystem.ChangeUserInputStyle( SEVENHEARTS_USER_INPUT_STYLE );
				m_PlayOption.uiControlType = 1;
				break;
			default:
				break;
			}
		}

		pCtrl = pContainer->Find( IID_CHECKBOX_SHOWPCNAME );
		assert( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX );
		if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
		{
			CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
			if( pCheckBox->IsCheck() )
				m_PlayOption.iShowPcName = 1;
			else
				m_PlayOption.iShowPcName = 0;
		}

		pCtrl = pContainer->Find( IID_CHECKBOX_SHOWNPCNAME );
		assert( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX );
		if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
		{
			CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
			if( pCheckBox->IsCheck() )
				m_PlayOption.iShowNpcName = 1;
			else
				m_PlayOption.iShowNpcName = 0;
		}

	}
}
void COptionDlg::ChangeOption()
{

	switch( m_iTab )
	{
	case IID_TABVIDEO:
		ChangeVideoOption();
		break;
	case IID_TABAUDIO:
		ChangeAudioOption();
		break;
	case IID_TABPLAY:
		ChangePlayOption();
		break;
	case IID_TABCOMMUNITY:
		ChangeCommunityOption();
		break;
	case IID_TABKEYBOARD:
		ChangeKeyboardOption();
		break;
	default:
		break;
	}
}

void COptionDlg::InitOption()
{
	switch( m_iTab )
	{
	case IID_TABVIDEO:
		ChangeVideoOption();
		break;
	case IID_TABAUDIO:
		ChangeAudioOption();
		break;
	case IID_TABPLAY:
		ChangePlayOption();
		break;
	case IID_TABCOMMUNITY:
		ChangeCommunityOption();
		break;
	default:
		break;
	}


	/*CTComboBox* pComboBox = NULL;
	if( pComboBox = GetComboBox( IID_COMBO_RESOLUTION ) )
		pComboBox->SetSelectedItem(RESOLUTION_DEFAULT);

	if( pComboBox = GetComboBox( IID_COMBO_CAMERA ) )
		pComboBox->SetSelectedItem(g_iDefaultCamera);

	CWinCtrl* pCtrl = Find( IID_BGM_SCROLLBAR );
	if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
	{
		CTScrollBar* pScrollBar = ( CTScrollBar*)pCtrl;
		m_SoundOption.iBgmVolume = DEFAULT_BGM_VOLUME;
		pScrollBar->SetValue( m_SoundOption.iBgmVolume );
	}

	pCtrl = Find( IID_EFFECT_SCROLLBAR );
	if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
	{
		CTScrollBar* pScrollBar = ( CTScrollBar*)pCtrl;
		m_SoundOption.iEffectVolume = DEFAULT_EFFECT_VOLUME;
		pScrollBar->SetValue( m_SoundOption.iEffectVolume );
	}*/
}

void COptionDlg::SaveCurrentOption()
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		CJContainer* pContainer = pPane->GetTabContainer( IID_TABVIDEO );
		assert( pContainer );

		pCtrl = pContainer->Find( IID_LIST_BGM );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			m_SoundOption.iBgmVolume = pListBox->GetValue();

			CMusicMgr::GetSingleton().SetVolume( g_ClientStorage.GetBgmVolumeByIndex( m_SoundOption.iBgmVolume ) );			
		}

		pCtrl = pContainer->Find( IID_LIST_EFFECTSOUND );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			m_SoundOption.iEffectVolume = pListBox->GetValue();
			g_pSoundLIST->SetVolume( g_ClientStorage.GetEffectVolumeByIndex( m_SoundOption.iEffectVolume ) );		
		}
	}	

	g_ClientStorage.SetKeyboardOption( m_KeyboardOption );
	g_ClientStorage.SetVideoOption( m_VideoOption );
	g_ClientStorage.SetSoundOption( m_SoundOption );
	g_ClientStorage.SetPlayOption( m_PlayOption );
	g_ClientStorage.SetCommunityOption( m_CommunityOption );
	g_ClientStorage.Save();
}

void COptionDlg::GetCurrentOption()
{
	g_ClientStorage.GetKeyboardOption( m_KeyboardOption );
	g_ClientStorage.GetCommunityOption( m_CommunityOption );
	g_ClientStorage.GetVideoOption( m_VideoOption );
	g_ClientStorage.GetSoundOption( m_SoundOption );
	g_ClientStorage.GetPlayOption( m_PlayOption );

	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;

		///커뮤니티
		CJContainer* pContainer = pPane->GetTabContainer( IID_TABCOMMUNITY );
		assert( pContainer );
		if( pContainer )
		{
			pCtrl = pContainer->Find( IID_RADIOBOX_WHISPER );
			assert( pCtrl );
			if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
			{
				CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
				if( m_CommunityOption.iWhisper )
					pRadioBox->SetPressedButton( IID_RADIOBTN_WHISPER_ON );
				else
					pRadioBox->SetPressedButton( IID_RADIOBTN_WHISPER_OFF );

			}

			pCtrl = pContainer->Find( IID_RADIOBOX_ADDFRIEND );
			assert( pCtrl );
			if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
			{
				CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
				if( m_CommunityOption.iAddFriend )
					pRadioBox->SetPressedButton( IID_RADIOBTN_ADDFRIEND_ON );
				else
					pRadioBox->SetPressedButton( IID_RADIOBTN_ADDFRIEND_OFF );
			}

			pCtrl = pContainer->Find( IID_RADIOBOX_EXCHANGE );
			assert( pCtrl );
			if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
			{
				CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
				if( m_CommunityOption.iExchange )
					pRadioBox->SetPressedButton( IID_RADIOBTN_EXCHANGE_ON );
				else
					pRadioBox->SetPressedButton( IID_RADIOBTN_EXCHANGE_OFF );
			}

			pCtrl = pContainer->Find( IID_RADIOBOX_PARTY );
			assert( pCtrl );
			if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
			{
				CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
				if( m_CommunityOption.iParty )
					pRadioBox->SetPressedButton( IID_RADIOBTN_PARTY_ON );
				else
					pRadioBox->SetPressedButton( IID_RADIOBTN_PARTY_OFF );
			}

			pCtrl = pContainer->Find( IID_RADIOBOX_MESSANGER );
			assert( pCtrl );
			if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
			{
				CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
				if( m_CommunityOption.iMessanger )
					pRadioBox->SetPressedButton( IID_RADIOBTN_MESSANGER_ON );
				else
					pRadioBox->SetPressedButton( IID_RADIOBTN_MESSANGER_OFF );
			}


		}
		///비디오
		pContainer = pPane->GetTabContainer( IID_TABVIDEO );
		assert( pContainer );

	
		int iResolutionIndex = GetCurrentResolutionIndex();
		pCtrl = pContainer->Find( IID_COMBO_RESOLUTION );
		if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		{
			CTComboBox* pComboBox = (CTComboBox*)pCtrl;
			pComboBox->SetSelectedItem( iResolutionIndex );
		}

		
		pCtrl = pContainer->Find( IID_COMBO_FREQUENCY );
		if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		{
			CTComboBox* pComboBox = (CTComboBox*)pCtrl;
			std::vector< int >::iterator iter;
			short iIndex = 0;
			short iSelectIndex = 0;
			pComboBox->ClearItem();
			for( iter = m_SupportedScreens[iResolutionIndex].m_Frequencys.begin();
				 iter != m_SupportedScreens[iResolutionIndex].m_Frequencys.end();
				 ++iter, ++iIndex )
			{
				pComboBox->AddItem( CStr::Printf("%dHz",*iter ) );
				if( *iter == m_VideoOption.tResolution.iFrequency )
					iSelectIndex = iIndex;

			}
			pComboBox->SetSelectedItem( iSelectIndex );
		}

		pCtrl = pContainer->Find( IID_LIST_ANTIALSING );
		if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		{
			CTComboBox* pComboBox = (CTComboBox*)pCtrl;
			std::vector< SupportedAntiAlising >::iterator iter;
			short iIndex = 0;
			short iSelectIndex = 0;
			pComboBox->ClearItem();
			for( iter = m_SupportedAntiAlising.begin();
				 iter != m_SupportedAntiAlising.end();
				 ++iter, ++iIndex )
			{
				pComboBox->AddItem( CStr::Printf("%d",iter->m_iAntiAlising ) );
				if( iter->m_iAntiAlising == m_VideoOption.iAntiAlising )
					iSelectIndex = iIndex;

			}
			pComboBox->SetSelectedItem( iSelectIndex );
		}


		/// 그리고 해당 해상도에 맞는 주파수를 넣어주고 저장된 주파수를 선택한다.
		pCtrl = pContainer->Find( IID_SCROLLBAR_CAMERA );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
		{
			CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
			pScrollBar->SetValue( m_VideoOption.iCamera );
		}

		pCtrl = pContainer->Find( IID_SCROLLBAR_PERFORMANCE );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
		{
			CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
			pScrollBar->SetValue( m_VideoOption.iPerformance );
		}

		
		pCtrl = pContainer->Find( IID_RADIOBOX_FULLSCREEN );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			if( m_VideoOption.iFullScreen )
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_FULLSCREEN );
			else
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_WINDOWMODE );

		}


		///사운드 볼륨

		pContainer = pPane->GetTabContainer( IID_TABAUDIO );
		assert( pContainer );

		CWinCtrl* pCtrl = pContainer->Find( IID_SCROLLBAR_BGM );
		if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
		{
			CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
			if( m_SoundOption.iBgmVolume < 0 || m_SoundOption.iBgmVolume >= MAX_BGM_VOLUME )
				m_SoundOption.iBgmVolume = DEFAULT_BGM_VOLUME ;

			pScrollBar->SetValue( m_SoundOption.iBgmVolume );
		}

		pCtrl = pContainer->Find( IID_SCROLLBAR_EFFECTSOUND );
		if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
		{
			CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
			if( m_SoundOption.iEffectVolume < 0 || m_SoundOption.iEffectVolume >= MAX_EFFECT_VOLUME )
				m_SoundOption.iEffectVolume = DEFAULT_EFFECT_VOLUME ;

			pScrollBar->SetValue( m_SoundOption.iEffectVolume );
		}

		////Play Option
		pContainer = pPane->GetTabContainer( IID_TABPLAY );
		assert( pContainer );
		
		pCtrl = pContainer->Find( IID_RADIOBOX_CONTROL );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			if( m_PlayOption.uiControlType )
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_SEVENHEARTSCONTROL );
			else
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_DEFAULTCONTROL );
		}

		pCtrl = pContainer->Find( IID_CHECKBOX_SHOWPCNAME );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
		{
			CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
			if( m_PlayOption.iShowPcName )
				pCheckBox->SetCheck();
			else
				pCheckBox->SetUncheck();
		}


		pCtrl = pContainer->Find( IID_CHECKBOX_SHOWNPCNAME );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
		{
			CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
			if( m_PlayOption.iShowNpcName )
				pCheckBox->SetCheck();
			else
				pCheckBox->SetUncheck();
		}

		///Keyboard
		pContainer = pPane->GetTabContainer( IID_TABKEYBOARD );
		assert( pContainer );
		if( pContainer )
		{
			pCtrl = pContainer->Find( IID_RADIOBOX_KEYBOARD );
			assert( pCtrl );
			if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
			{
				CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
				if( m_KeyboardOption.iChattingMode )
					pRadioBox->SetPressedButton( IID_RADIOBTN_NORMALCHATTING );
				else
					pRadioBox->SetPressedButton( IID_RADIOBTN_ENTERCHATTING );
			}
		}
	}
}

CTComboBox*	COptionDlg::GetComboBox( int ComboBoxID )
{
	CWinCtrl* pCtrl = Find( ComboBoxID );
	if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		return (CTComboBox*)pCtrl;

	return NULL;
}

///Alt+Enter로 화면모드 전환시 처리할 사항
void COptionDlg::ChangeScreenModeByHotKey()
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		CJContainer* pContainer = pPane->GetTabContainer( IID_TABVIDEO );
		assert( pContainer );

		pCtrl = pContainer->Find( IID_RADIOBOX_FULLSCREEN );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			if( g_pCApp->IsFullScreenMode() )
			{
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_FULLSCREEN );
				m_VideoOption.iFullScreen = 1;
				g_ClientStorage.SetVideoFullScreen( 1 );
			}
			else
			{
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_WINDOWMODE );
				m_VideoOption.iFullScreen = 0;
				g_ClientStorage.SetVideoFullScreen( 0 );
			}
		}
	}
}

void COptionDlg::AddSupportedScreen( int iWidth, int iHeight, int iDepth, int iFrequency )
{
	std::vector<SupportedScreen>::iterator iter;
	for( iter = m_SupportedScreens.begin(); iter != m_SupportedScreens.end(); ++iter )
	{
		if( iter->IsEqual( iWidth, iHeight, iDepth ) )
		{
			iter->m_Frequencys.push_back( iFrequency );
			return;
		}
	}

	SupportedScreen Screen;
	Screen.m_iWidth = iWidth;
	Screen.m_iHeight = iHeight;
	Screen.m_iDepth = iDepth;
	Screen.m_Frequencys.push_back( iFrequency );
	m_SupportedScreens.push_back( Screen );
}

void COptionDlg::AddSupportedAntiAlising( int iOption )
{
	std::vector<SupportedAntiAlising>::iterator iter;
	for( iter = m_SupportedAntiAlising.begin(); iter != m_SupportedAntiAlising.end(); ++iter )
	{
		if( iter->IsEqual( iOption)  )
		{
			return;
		}
	}
	SupportedAntiAlising AntiAlising;
	AntiAlising.m_iAntiAlising = iOption;
	m_SupportedAntiAlising.push_back( AntiAlising );
}

int  COptionDlg::GetCurrentResolutionIndex()
{
	std::vector<SupportedScreen>::iterator iter;
	int iCount = 0;
	for( iter = m_SupportedScreens.begin(); iter != m_SupportedScreens.end(); ++iter, ++iCount )
	{
		if( iter->IsEqual( m_VideoOption.tResolution.iWidth,
			m_VideoOption.tResolution.iHeight,
			m_VideoOption.tResolution.iDepth ) )
		{
			return iCount;
		}
	}
	assert( 0 && "Invalid Save Resolution Value");
	return 0;
}

void COptionDlg::ChangeCommunityOption()
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		CJContainer* pContainer = pPane->GetTabContainer( IID_TABCOMMUNITY );
		assert( pContainer );
		
		if( pContainer == NULL ) return;

		pCtrl = pContainer->Find( IID_RADIOBOX_WHISPER );
		assert( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			switch( pRadioBox->GetPressedButtonID() )
			{
			case IID_RADIOBTN_WHISPER_ON:
				m_CommunityOption.iWhisper = 1;
				break;
			case IID_RADIOBTN_WHISPER_OFF:
				m_CommunityOption.iWhisper = 0;
				break;
			default:
				break;
			}
		}

		pCtrl = pContainer->Find( IID_RADIOBOX_ADDFRIEND );
		assert( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			switch( pRadioBox->GetPressedButtonID() )
			{
			case IID_RADIOBTN_ADDFRIEND_ON:
				m_CommunityOption.iAddFriend = 1;
				break;
			case IID_RADIOBTN_ADDFRIEND_OFF:
				m_CommunityOption.iAddFriend = 0;
				break;
			default:
				break;
			}
		}

		pCtrl = pContainer->Find( IID_RADIOBOX_EXCHANGE );
		assert( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			switch( pRadioBox->GetPressedButtonID() )
			{
			case IID_RADIOBTN_EXCHANGE_ON:
				m_CommunityOption.iExchange = 1;
				break;
			case IID_RADIOBTN_EXCHANGE_OFF:
				m_CommunityOption.iExchange = 0;
				break;
			default:
				break;
			}
		}


		pCtrl = pContainer->Find( IID_RADIOBOX_PARTY );
		assert( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			switch( pRadioBox->GetPressedButtonID() )
			{
			case IID_RADIOBTN_PARTY_ON:
				m_CommunityOption.iParty = 1;
				break;
			case IID_RADIOBTN_PARTY_OFF:
				m_CommunityOption.iParty = 0;
				break;
			default:
				break;
			}
		}

		pCtrl = pContainer->Find( IID_RADIOBOX_MESSANGER );
		assert( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			switch( pRadioBox->GetPressedButtonID() )
			{
			case IID_RADIOBTN_MESSANGER_ON:
				m_CommunityOption.iMessanger = 1;
				break;
			case IID_RADIOBTN_MESSANGER_OFF:
				m_CommunityOption.iMessanger = 0;
				break;
			default:
				break;
			}
		}

	}
}
void COptionDlg::InitVideoOption()
{

}

void COptionDlg::InitAudioOption()
{
	
}

void COptionDlg::InitPlayOption()
{
	
}

void COptionDlg::InitCommunityOption()
{
	
}
void COptionDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();
	if( IID_TABKEYBOARD == m_iTab )
	{
		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
		::setTransformSprite( mat );
		
		
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 75,  97, g_dwBLACK,  STR_ENTERCHATTING_MODE );
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 75, 136, g_dwBLACK,  STR_NORMALCHATTING_MODE );
		
	}
}