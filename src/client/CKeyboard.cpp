
#include "stdAFX.h"

#include "Game.h"
#include "CKeyboard.h"
#include "IO_Terrain.h"
#include "CCamera.h"
#include "IO_Event.h"
#include "TGameCtrl.h"
#include "Interface\\it_mgr.h"
#include "sound\CMusic.h"
#include "Network\CNetwork.h"
#include "CSkyDOME.h"
#include "Interface/ExternalUI/ExternalUILobby.h"
#include "CMouse.h"	
#include "GameProc/CDayNNightProc.h"


#include "util/ijl.h"	
#pragma comment (lib, "util/ijl15.lib")


CKeyboard *CKeyboard::m_pInstance = NULL;

/// 스크린 캡쳐시에 BMP 파일을 지울것이냐 말것이냐..
bool g_bDontDeleteCapFile = false;

bool IJL_BMP2JPEG (const char *szBMPFile, char *szJPGFile)
{
	FILE* fpBMP;

	fpBMP = fopen( szBMPFile, "rb" );

	JPEG_CORE_PROPERTIES jcprops;
	ijlInit( &jcprops );

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;

	fread( &bmfh,	1,	sizeof(BITMAPFILEHEADER),	fpBMP);
	fread( &bmih,	1,	sizeof(BITMAPINFOHEADER),	fpBMP);

	long DIBLineSize = (bmih.biWidth * 4 ) / 4 * 4;

	jcprops.DIBChannels = 4;
	jcprops.DIBColor	= IJL_RGB;
	jcprops.DIBHeight	= -bmih.biHeight;
	jcprops.DIBWidth	= bmih.biWidth;
	jcprops.DIBPadBytes = IJL_DIB_PAD_BYTES( jcprops.DIBWidth, jcprops.DIBChannels );

	jcprops.JPGHeight = bmih.biHeight;
	jcprops.JPGWidth  = bmih.biWidth;
	jcprops.jquality  = 100;

	jcprops.JPGFile = szJPGFile;

	unsigned char* DIBBuffer = new unsigned char[ DIBLineSize * bmih.biHeight ];
	fread( DIBBuffer,	1,	DIBLineSize * bmih.biHeight,	fpBMP );

	BYTE btBlue;
	for (int iP=0; iP<DIBLineSize * bmih.biHeight; iP+=4) {
		btBlue = DIBBuffer[ iP ];
		DIBBuffer[ iP ] = DIBBuffer[ iP+2 ];
		DIBBuffer[ iP+2 ]  = btBlue;
	}
	
	jcprops.DIBBytes = DIBBuffer;

	ijlWrite (&jcprops, IJL_JFILE_WRITEWHOLEIMAGE );
	ijlFree( &jcprops );

	delete[] DIBBuffer;

	fclose( fpBMP );

if( !g_bDontDeleteCapFile )
	::DeleteFile( szBMPFile );

	return true;
}

/*
Direct3D9: (ERROR) :RenderTargets are not lockable unless application specifies TRUE for the Lockable parameter
 at CreateRenderTarget or D3DPRESENTFLAG_LOCKABLE_BACKBUFFER at CreateDevice, 
 CreateAdditionalSwapChain or Reset. Lockable render targets incur a performance cost on some graphics hardware.
*/
void ScreenCAPTURE ()
{
	LPDIRECT3DSURFACE9 pSUR;
	
	if ( D3D_OK == reinterpret_cast<LPD3DDEVICE>( getDevice () )->GetRenderTarget (0, &pSUR) ) {
		D3DSURFACE_DESC sDesc;
		if ( D3D_OK != pSUR->GetDesc( &sDesc ) )
			return;

		WORD	wDumpNo=0;
		char   *pStr;
		char *pTempBMP;
		do 
		{
			pStr = CStr::Printf ("screen%02d.jpg", wDumpNo++);
			pTempBMP = CStr::Printf ("screen%02d.bmp", wDumpNo++);
		} while ( CUtil::Is_FileExist( pStr ) && wDumpNo <= 999 ) ;		


		/*D3DLOCKED_RECT LockRect;
		pSUR->LockRect( &LockRect, NULL, D3DLOCK_READONLY );

		if (IJL_EncodeToJPEGFile ( pStr, sDesc.Width, sDesc.Height, (WORD*)LockRect.pBits, sDesc.Format ) ) 
		{
			LogString (LOG_NORMAL, "Capture Screen %s", pStr);		
		}*/

		
		if ( D3D_OK == D3DXSaveSurfaceToFile( pTempBMP, D3DXIFF_BMP, pSUR, NULL,	NULL) ) 
		{
			LogString (LOG_NORMAL, "Capture Screen %s", pStr);		

			IJL_BMP2JPEG( pTempBMP, pStr );
		} else {
			LogString (LOG_NORMAL, "Capture Screen Failed");
		}
		

		SAFE_RELEASE(pSUR);
	}
}


//-------------------------------------------------------------------------------------------------
CKeyboard *CKeyboard::Instance ()
{
	if ( m_pInstance == NULL ) {
		m_pInstance = new CKeyboard;

#ifdef	__USE_IME
		m_IME.InitIme  ( g_pCApp->GetHWND(), 80 );
		m_IME.ClearString ();
#endif
	}

	return m_pInstance;
}

CKeyboard::CKeyboard ()	
{	
	m_bEnable			= true;		
}

//-------------------------------------------------------------------------------------------------
#ifdef	__USE_IME
bool CKeyboard::IME_WndMSG (UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if ( !m_IME.IsActive() )
		return false;

	switch ( m_IME.ProcIMEMessage (uiMsg, wParam, lParam) ) {
		case CIME_WMCHAR    :
		case CIME_HANDLED	:	
			return true;			// IME에서 메세지가 처리 됐다.

		case CIME_RETURN	:		// VK_RETURN
		{
			LogString (LOG_NORMAL, "CIME_RETURN \n");
		}

		case CIME_CANCEL    :		// VK_ESC
			m_IME.SetActive( false );
			m_IME.ToggleInputMode( false );	// 강제로 영문으로 전환 시킨다.
			return true;
	}

	return false;
}
#endif


//-------------------------------------------------------------------------------------------------
// 키보드에서 처리해야할 윈도우 메세지가 있으면 이곳에 추가 !!!
bool CKeyboard::Add_WndMSG (UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if( !IsEnable() )
		return false;

#ifdef	__USE_IME
	if ( IME_WndMSG(uiMsg, wParam, lParam) )
		return true;
#endif

	switch ( uiMsg ) {
		/*
		case WM_IME_STARTCOMPOSITION:	
		case WM_IME_ENDCOMPOSITION:     
		case WM_IME_COMPOSITION:        
		case WM_IME_SETCONTEXT:			
		case WM_IME_NOTIFY:        
		case WM_INPUTLANGCHANGE:		
		case WM_IME_CONTROL:
		case WM_IME_COMPOSITIONFULL:
			;
		*/
		case WM_KEYDOWN :
		case WM_KEYUP   :
		case WM_CHAR    :
			Add_WndMSG2Q (uiMsg, wParam, lParam);
			return true;
		case WM_SYSKEYDOWN :
			switch(wParam)
			{
			case VK_F4:
			//case 'S':
				return false;
			}
			//if ( wParam == VK_F10 || wParam == VK_MENU) {
				Add_WndMSG2Q (uiMsg, wParam, lParam);
				return true;
			//}
			break;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
void CKeyboard::On_WM_KEYUP (WPARAM wParam, LPARAM lParam)
{
	if ( wParam == VK_SNAPSHOT ) {
		// capture screen...
		ScreenCAPTURE ();
	}
}

//-------------------------------------------------------------------------------------------------
extern void GF_error ();


static CEffect *s_pEF=NULL;
static short s_nEffectIDX=29;
void CKeyboard::On_WM_KEYDOWN (WPARAM wParam, LPARAM lParam)
{
	static float s_fScale = 1.0f;

    if ( lParam & 0x40000000 ) {
		// 이전에 눌려 있던 키다....
		return;
	}

	switch ( wParam ) {
		case VK_TAB :
			g_pNet->Send_cli_TOGGLE( TOGGLE_TYPE_RUN );
			return;

		
		/// Hot slot key
		case VK_F1:
		case VK_F2:
		case VK_F3:
		case VK_F4:
		case VK_F5:
		case VK_F6:
		case VK_F7:
		case VK_F8:
		case VK_F9:
		case VK_F10:
			{
				g_itMGR.DoQuickSlot( wParam - VK_F1 );				
			}
			break;
	}
}

//-------------------------------------------------------------------------------------------------
void CKeyboard::On_WM_CHAR (WPARAM wParam, LPARAM lParam)
{
	switch ( wParam ) 
	{
		/*
		case 'F':
			g_pCamera->Toggle_FloowMODE ();
			return;
		
		case '1'	:
			g_pAVATAR->SetCharExpression( 0 );
			g_pAVATAR->Update( );
			break;
		case '2'	:
			g_pAVATAR->SetCharExpression( 1 );
			g_pAVATAR->Update( );
			break;
		case '3'	:
			g_pAVATAR->SetCharExpression( 2 );
			g_pAVATAR->Update( );
			break;
		case '4'	:
			g_pAVATAR->SetCharExpression( 3 );
			g_pAVATAR->Update( );
			break;
		case '0':
			{
				static int iCurrentFace = 0;
				g_pAVATAR->SetPartITEM( BODY_PART_FACE, iCurrentFace * 7 + 1 );
				g_pAVATAR->Update( );
				iCurrentFace++;
				if( iCurrentFace >= 7 )
					iCurrentFace = 0;
			}
			break;

		/// skill test
		case '8':
			{
				g_pAVATAR->SetCMD_CASTING( 1, g_pAVATAR->m_iTargetObject );
			}
			break;

		*/
		case VK_RETURN :
			LogString (LOG_NORMAL, "VK_RETURN \n");

#ifdef	__USE_IME
			m_IME.ClearString();
			m_IME.SetActive ( true );
			m_IME.ToggleInputMode ( true );	// 강제로 한글로 전환 시킨다.
#endif
			return;
	}
}

//-------------------------------------------------------------------------------------------------
void CKeyboard::Proc (void)
{
	while( this->Get_WndMSG () ) {
		if ( this->m_sCurMsg.wParam == 'W' )
			int a=0;

		// proc game interface ...
//		if( it_MsgProc(this->m_sCurMsg.uiMsg ,this->m_sCurMsg.wParam,this->m_sCurMsg.lParam) ) {
//			continue;
//		}

		if( g_pCApp->GetStatus() == AS_MAIN_MENU )
		{
			g_EUILobby.MsgProc( this->m_sCurMsg.uiMsg ,this->m_sCurMsg.wParam,this->m_sCurMsg.lParam);
			continue;
		}
			
		if(g_itMGR.MsgProc(this->m_sCurMsg.uiMsg ,this->m_sCurMsg.wParam,this->m_sCurMsg.lParam)) {
			continue;
		}
		
		switch( this->m_sCurMsg.uiMsg ) {
			case WM_KEYUP :
				On_WM_KEYUP (this->m_sCurMsg.wParam, this->m_sCurMsg.lParam);
				break;
			case WM_CHAR :
				On_WM_CHAR (this->m_sCurMsg.wParam, this->m_sCurMsg.lParam);
				break;
			case WM_KEYDOWN :
				On_WM_KEYDOWN (this->m_sCurMsg.wParam, this->m_sCurMsg.lParam);
				break;
			
			case WM_SYSKEYDOWN :
				{
					if ( this->m_sCurMsg.wParam == VK_F10 ) 
					{	// F10 Menu !!!
						g_itMGR.DoQuickSlot( this->m_sCurMsg.wParam  - VK_F1 );				
					// popup game menu ....
					}

					switch( this->m_sCurMsg.wParam )
					{		
						case '9':
							{
								g_pCMouse->ToggleUserMouseFlag();
								g_pCMouse->SetUserCursor( CURSOR_DEFAULT );
							}
							break;
#if defined( _DEBUG ) || defined( _D_RELEASE )
						case 'W': 
						{
							g_GameDATA.m_bWireMode = !g_GameDATA.m_bWireMode;
							::useWireMode( g_GameDATA.m_bWireMode );
							return;
						}	

						case 'H':
						case 'h':
							g_GameDATA.m_bNoUI = !g_GameDATA.m_bNoUI;
							break;
						
						case 'd':
						case 'D':
							g_GameDATA.m_bDisplayDebugInfo = !g_GameDATA.m_bDisplayDebugInfo;
							break;						

						case '0':
							g_bDontDeleteCapFile = !g_bDontDeleteCapFile;
							break;


						case 'K' :
						case 'k' :
							{
								static int iState = DN_NIGHT;								
								g_DayNNightProc.Start( 0, iState, 30 );

								iState++;
								if( iState > DN_NIGHT )
									iState = DN_DAY;
							}							
							break;
						case '8':
							{						
								if ( !s_pEF ) 
								{
									s_pEF = g_pEffectLIST->Add_EffectWithIDX ( s_nEffectIDX );
									if ( s_pEF ) {
										s_pEF->LinkNODE( g_pAVATAR->GetZMODEL() );
										s_pEF->InsertToScene ();
									}

									s_nEffectIDX = ( s_nEffectIDX+1 ) % g_pEffectLIST->GetFileCNT();
								} else {
									g_pEffectLIST->Del_EFFECT( s_pEF );
									s_pEF = NULL;
								}
							}
							break;
#endif
						default:
							break;
					}
				}
				break;
		}
	}

	this->Del_CurMSG ();
}

//-------------------------------------------------------------------------------------------------
