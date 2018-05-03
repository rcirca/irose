#include "StdAfx.h"
#include <imm.h>
#include ".\time2.h"
#include "crtdbg.h"
#include "TCandidateWindowHanja.h"
#include "TCandidateWindowList.h"
#include "TControlMgr.h"
#include "TEditBox.h"

#pragma comment (lib, "IMM32.lib")

HIMC CTIme::s_old_HIMC = NULL;

CTIme::CTIme(void)
{
	m_pCandidateWindow = NULL;
	m_nCursorPos  = 0;
	m_nDeltaStart = 0;
}

CTIme::~CTIme(void)
{
	
}

CTIme& CTIme::GetInstance()
{
	static CTIme s_Instance;
	return s_Instance;
}

int CTIme::Initialize( SIZE sizeScreen )
{
	m_sizeScreen = sizeScreen;
	wchar_t Underline[2] = { 0, 0 };
	char szBuf[3];
	BOOL bChangedDefaultChar;
	int  iRet;
	switch( CTControlMgr::GetInstance()->GetCodePage() )
	{
	case 949://??
		m_pCandidateWindow = new CTCandidateWindowHanja;
		m_pCandidateWindow->SetPosition( m_sizeScreen.cx - 300, m_sizeScreen.cy - 50 );
		break;
	default:
		m_pCandidateWindow = new CTCandidateWindowList;
		m_pCandidateWindow->SetPosition( 200, 200 );
		break;
	}
	
	m_strPrompt = "_";//szBuf;

	ZeroMemory( szBuf, sizeof( szBuf ));
	iRet = WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)Underline, 1, szBuf, 3, " ", &bChangedDefaultChar);
	_ASSERT( iRet );
	m_strPrompt = szBuf;


	Underline[0] = 0x2501;
	ZeroMemory( szBuf, sizeof( szBuf ));

	iRet = WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)Underline, 1, szBuf, 3, " ", &bChangedDefaultChar);
	_ASSERT( iRet );
	m_strMark_ATTR_TARGET_CONVERTED = szBuf;

	Underline[0] = 0x2500;
	ZeroMemory( szBuf, sizeof( szBuf ));
	iRet = WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)Underline, 1, szBuf, 3, " ", &bChangedDefaultChar);
	_ASSERT( iRet );
	m_strATTR_CONVERTED = szBuf;

	Underline[0] = 0x2026;
	ZeroMemory( szBuf, sizeof( szBuf ));
	iRet = WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)Underline, 1, szBuf, 3, " ", &bChangedDefaultChar);
	_ASSERT( iRet );
	m_strATTR_INPUT = szBuf;
	return 1;
}

int CTIme::Finalize()
{
//	ImmReleaseContext( m_hWnd, m_hIMC);
	if( m_pCandidateWindow )
	{
		delete m_pCandidateWindow;
		m_pCandidateWindow = NULL;
	}

	return 1;
}

int CTIme::Process( HWND hWnd, unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uiMsg )
	{
	case WM_IME_CHAR:
		DefWindowProc( hWnd, uiMsg, wParam, lParam);
		return 1;
	case WM_IME_STARTCOMPOSITION:
		_RPT0( _CRT_WARN,"WM_IME_STARTCOMPOSITION\n");
		
		return 1;
	case WM_IME_ENDCOMPOSITION:
		_RPT0( _CRT_WARN,"WM_IME_ENDCOMPOSITION\n");
		m_nCursorPos  = 0;
		m_nDeltaStart = 0;
		m_strComposition.clear();
		m_CompReadAttr.clear();
		m_CompAttr.clear();
		m_CompClauseInfo.clear();
		m_CompReadClauseInfo.clear();
		return 1;
    case WM_IME_COMPOSITION:
		{
			if( HIMC hIMC = ImmGetContext( hWnd ) )
			{

				if( lParam & GCS_COMPREADATTR )
				{
					long lRet = ImmGetCompositionString( hIMC, GCS_COMPREADATTR , NULL, 0 );
					BYTE* pBuffer = new BYTE[ lRet ];
					lRet = ImmGetCompositionString( hIMC, GCS_COMPREADATTR , pBuffer, lRet );
				
					m_CompReadAttr.clear();
					for( int i = 0 ; i < lRet  ; ++i)
						m_CompReadAttr.push_back( pBuffer[i] );

					delete []pBuffer;
				}

				if( lParam & GCS_COMPATTR )
				{
					long lRet = ImmGetCompositionString( hIMC, GCS_COMPATTR , NULL, 0 );

					BYTE* pBuffer = new BYTE[ lRet ];
					lRet = ImmGetCompositionString( hIMC, GCS_COMPATTR , pBuffer, lRet );
					m_CompAttr.clear();
					for( int i = 0 ; i < lRet  ; ++i)
						m_CompAttr.push_back( pBuffer[i] );

					delete []pBuffer;
				}


				if( lParam & GCS_COMPREADCLAUSE )
				{
					if( long lRet = ImmGetCompositionString( hIMC, GCS_COMPREADCLAUSE , NULL, 0 ) )
					{
						DWORD* pBuffer = new DWORD[ lRet / sizeof(DWORD) ];
						lRet = ImmGetCompositionString( hIMC, GCS_COMPREADCLAUSE , pBuffer, lRet );

						m_CompReadClauseInfo.clear();
						for( int i = 0 ; i < lRet / (long)sizeof( DWORD ) ; ++i)
							m_CompReadClauseInfo.push_back( pBuffer[i] );

						delete []pBuffer;

					}
				}

				//if( lParam & GCS_COMPREADSTR )
				//{
				//	_RPT0 ( _CRT_WARN, "GCS_COMPREADSTR %d\n");
				//}

				//if( lParam & GCS_RESULTREADSTR )
				//{
				//	_RPT0 ( _CRT_WARN, "GCS_RESULTREADSTR\n");
				//}

				//if( lParam & GCS_RESULTREADCLAUSE )
				//{
				//	_RPT0 ( _CRT_WARN, "GCS_RESULTREADCLAUSE\n");
				//}

				//if ( lParam & GCS_RESULTCLAUSE )
				//{
				//	_RPT0 ( _CRT_WARN, "GCS_RESULTCLAUSE\n");
				//}


				if( lParam & GCS_COMPCLAUSE )
				{
					long lRet = ImmGetCompositionString( hIMC, GCS_COMPCLAUSE , NULL, 0 );
					DWORD* pBuffer = new DWORD[lRet/sizeof(DWORD)];
					lRet = ImmGetCompositionString( hIMC, GCS_COMPCLAUSE , pBuffer, lRet );

					m_CompClauseInfo.clear();
					for( int i = 0 ; i < lRet / (long)sizeof( DWORD ) ; ++i)
						m_CompClauseInfo.push_back( pBuffer[i] );

					delete []pBuffer;
				}

				if( lParam & GCS_CURSORPOS )
					m_nCursorPos = LOWORD( ImmGetCompositionString( hIMC, GCS_CURSORPOS , NULL, 0 ) );
				
				if( lParam & GCS_DELTASTART )
					m_nDeltaStart = LOWORD( ImmGetCompositionString( hIMC, GCS_DELTASTART , NULL, 0 ) );


				if( CTEditBox::s_pFocusEdit && lParam & GCS_RESULTSTR )
				{
					if( long lRet = ImmGetCompositionString( hIMC, GCS_RESULTSTR , NULL, 0 ) )
					{
						char* pBuffer = new char[lRet + 1];
						lRet = ImmGetCompositionString( hIMC, GCS_RESULTSTR , pBuffer, lRet );
						pBuffer[lRet] = 0;				
						for( unsigned ui = 0; ui < strlen( pBuffer ); ++ui )
							CTEditBox::s_pFocusEdit->OnChar( pBuffer[ui] , 0 );

						delete []pBuffer;
					}
				}

				if( lParam & GCS_COMPSTR )
				{
					if( long lRet = ImmGetCompositionString( hIMC, GCS_COMPSTR , NULL, 0 ) )
					{
						char* pBuffer = new char[lRet + 1];
						lRet = ImmGetCompositionString( hIMC, GCS_COMPSTR , pBuffer, lRet );
						pBuffer[lRet] = 0;				
						m_strComposition = pBuffer;
						delete []pBuffer;
					}
					else
					{
						m_strComposition.clear();
						m_nCursorPos = 0;
						m_CompReadAttr.clear();
						m_CompAttr.clear();
						m_CompClauseInfo.clear();
						m_CompReadClauseInfo.clear();
					}
				}
				ImmReleaseContext( hWnd, hIMC );
			}
			return 1;
		}
	case WM_IME_SETCONTEXT:    // ?? ?? ???? ?? ?? ???? ???? ?? ?? 
		_RPT0( _CRT_WARN,"WM_IME_SETCONTEXT: Active \n" );
		//if( lParam & ISC_SHOWUICOMPOSITIONWINDOW )
		//	_RPT0( _CRT_WARN,"Shows the composition window by UI window.\n" );
		//
		//if( lParam & ISC_SHOWUICANDIDATEWINDOW )
		//	_RPT0( _CRT_WARN,"Shows the candidate window of Index 0 by UI window. \n");

		//if( lParam & ISC_SHOWUICANDIDATEWINDOW << 1 )
		//	_RPT0( _CRT_WARN,"Shows the candidate window of Index 1 by UI window. \n");

		//if( lParam & ISC_SHOWUICANDIDATEWINDOW << 2 )
		//	_RPT0( _CRT_WARN,"Shows the candidate window of Index 2 by UI window. \n");

		//if( lParam & ISC_SHOWUICANDIDATEWINDOW << 3 )
		//	_RPT0( _CRT_WARN,"Shows the candidate window of Index 3 by UI window. \n");

		//lParam &= ~ISC_SHOWUIALL;
		DefWindowProc( hWnd, uiMsg, wParam, lParam);
		return 1;
    case WM_IME_NOTIFY:        
		{
			_RPT0( _CRT_WARN,"WM_IME_NOTIFY\n" );
			switch (wParam) 
			{
			case IMN_OPENCANDIDATE:
				m_pCandidateWindow->Show();
			case IMN_CHANGECANDIDATE:
				{
					if( HIMC hIMC = ImmGetContext( hWnd )) 
					{
						DWORD dwBufLen = ImmGetCandidateList( hIMC, 0, NULL, 0);
						if( dwBufLen > 0 )
						{
							LPCANDIDATELIST pCandidateList = (LPCANDIDATELIST)new char[dwBufLen];
							
							ImmGetCandidateList( hIMC, 0, pCandidateList, dwBufLen);

							m_pCandidateWindow->Clear();
							char pszBuffer[16];
							int iStart = (  pCandidateList->dwSelection / pCandidateList->dwPageSize ) * pCandidateList->dwPageSize;
							
							std::string strItem;
							int iCount = 1;
							for( int i = iStart ; i < (iStart + pCandidateList->dwPageSize ) && ( i < pCandidateList->dwCount ) ; ++i ,++iCount)
							{
								sprintf( pszBuffer,"%2d. ",iCount);
								strItem = pszBuffer;
								strItem.append( (char*)pCandidateList + pCandidateList->dwOffset[i]  );
								m_pCandidateWindow->AddCandidate( strItem.c_str() );
							}

							m_pCandidateWindow->SetSelection( pCandidateList->dwSelection % pCandidateList->dwPageSize );
							delete []pCandidateList;
						}
						ImmReleaseContext( hWnd, hIMC );
					}
					return 1;
				}
			case IMN_CLOSECANDIDATE:
				_RPT0( _CRT_WARN,"WM_IME_NOTIFY-IMN_CLOSECANDIDATE\n" );
				m_nCursorPos = 0;
				m_pCandidateWindow->Clear();
				m_pCandidateWindow->Hide();
				return 1;
			case IMN_SETOPENSTATUS:
				_RPT0( _CRT_WARN,"WM_IME_NOTIFY-IMN_SETOPENSTATUS\n" );
				lParam &= ~(ISC_SHOWUICOMPOSITIONWINDOW | ISC_SHOWUIALLCANDIDATEWINDOW);
				break;
			default:
				break;
			}
			return 1;
			//return DefWindowProc( hWnd, uiMsg, wParam, lParam);
		}
	case WM_IME_KEYUP:
		{
			_RPT0( _CRT_WARN,"WM_IME_KEYUP\n" );
			//DefWindowProc( hWnd, uiMsg, wParam, lParam);
			return 1;
		}
	case WM_IME_KEYDOWN:
		{
			//Candidate?? ???? ??Composition Window? ???? Enter?? ????
			//????? ???? ??? ???? ??? ??????.
			//Client??? ?? WM_KEYDOWN, VK_RETURN ?? ????????.
			//SendMessage? ??? CTEdit::get_text()?? NULL? ????.
			//_RPT0( _CRT_WARN,"WM_IME_KEYDOWN\n" );
			//DefWindowProc( hWnd, uiMsg, wParam, lParam);
			return 1;
		}
	default:
		break;
	}

	///??? ime?? ???? ????? ???? 
	return 0;
}

std::string& CTIme::GetCompsitionString()
{
	return m_strComposition;
}

void CTIme::SetCandidateWindowPosition( POINT pt )
{
	if( CTControlMgr::GetInstance()->GetCodePage() != 949 )
	{
		if( pt.y + 30 + m_pCandidateWindow->GetHeight() > m_sizeScreen.cy )
			pt.y  = pt.y - 10 - m_pCandidateWindow->GetHeight();
		else
			pt.y += 20;

		m_pCandidateWindow->MoveWindow( pt );
	}
}

void CTIme::ChangeScreenSize( SIZE sizeScreen )
{
	m_sizeScreen = sizeScreen;
	switch( CTControlMgr::GetInstance()->GetCodePage() )
	{
	case 949://??
		m_pCandidateWindow->SetPosition( m_sizeScreen.cx - 200, m_sizeScreen.cy - 50 );
		break;
	default:
		m_pCandidateWindow->SetPosition( 200, 200 );
		break;
	}

}

void CTIme::Draw()
{
	m_pCandidateWindow->Draw();
}

short CTIme::GetCursorPos()
{
	return m_nCursorPos;
}

short CTIme::GetDeltaStart()
{
	return m_nDeltaStart;
}
std::vector<BYTE>& CTIme::GetCompReadAttr()
{
	return m_CompReadAttr;
}
std::vector<BYTE>& CTIme::GetCompAttr()
{
	return m_CompAttr;
}

std::vector<DWORD>& CTIme::GetCompClauseInfo()
{
	return m_CompClauseInfo;
}
std::vector<DWORD>& CTIme::GetCompReadClauseInfo()
{
	return m_CompReadClauseInfo;
}

std::string&	CTIme::GetStr_ATTR_TARGET_CONVERTED()
{
	return m_strMark_ATTR_TARGET_CONVERTED;
}
std::string&	CTIme::GetStr_ATTR_CONVERTED()
{
	return m_strATTR_CONVERTED;
}
std::string&	CTIme::GetStr_ATTR_INPUT()
{
	return m_strATTR_INPUT;
}

std::string&	CTIme::GetStrPrompt()
{
	return m_strPrompt;
}

bool CTIme::IsVisibleCandidateWindow()
{
	if( m_pCandidateWindow )
		return m_pCandidateWindow->IsVision();

	return false;
}

HIMC CTIme::GetOldHIMC()
{
	return s_old_HIMC;
}
void CTIme::SetOldHIMC( HIMC hImc )
{
	s_old_HIMC = hImc;
}
