#include "stdafx.h"
#include <math.h>
#include "CKeyMove.h"	
#include "../CCamera.h"	
#include "../OBJECT.h"	
#include "../Network\\CNetwork.h"
#include "../interface/it_mgr.h"
#include "../system/CGame.h"
#include "../TGameCtrl/TEditBox.h"
#include "../Misc/GameUtil.h"

#define pi 3.14159265

CKeyMove		g_KeyMove;


const float DEFAULT_YAW_VALUE = 3.0f;


CKeyMove::CKeyMove()
{
	m_bLeftKeyDown		= false;
	m_bRightKeyDown		= false;

	m_bUpKeyDown		= false;
	m_bDownKeyDown		= false;

	m_fYawValue			= 0.0f;
}

CKeyMove::~CKeyMove()
{
}

#define CAMERA_MOVE_SPEED 22 // default 10
void CKeyMove::Proc()
{	
	/// 제련중에는 커서키로 이동불가 2004.12.14
	if( g_itMGR.IsDlgOpened( DLG_TYPE_UPGRADE ) )
		return;
	///IME Candidate Window 가 떠 있을경우에는 무시한다.
	if( it_IsVisibleImeCandidateWindow() )
		return;

	//Dagnarus
	//Character should not move if the chat is focused
	if (CTEditBox::s_pFocusEdit != NULL)
		return;

	//Character should not move if the alt key is pressed
	m_bAltKeyDown = (GetAsyncKeyState(VK_MENU) < 0 ) ?true:false;
	if (m_bAltKeyDown)
		return;


	m_bLeftKeyDown = ( GetAsyncKeyState( VK_LEFT ) < 0 )?true:false;
	m_bRightKeyDown = ( GetAsyncKeyState( VK_RIGHT ) < 0 )?true:false;
	bool bHeightUp = ( GetAsyncKeyState( VK_END ) < 0 )?true:false;
	bool bHeightDown = ( GetAsyncKeyState( VK_HOME ) < 0 )?true:false;

	// Garnet: Azerty keyboard detection
	WORD wPrimaryLang =  CGameUtil::GetPrimaryLanguage();
	if ( wPrimaryLang == LANG_FRENCH ) 
	{
	//AZERTY configuration
	m_bWKeyDown =	 ( GetAsyncKeyState( 0x5a ) < 0 ) ?true:false;
	m_bSKeyDown = ( GetAsyncKeyState( 0x53 ) < 0 )?true:false;
	m_bAKeyDown = ( GetAsyncKeyState( 0x51 ) < 0 )?true:false;
	m_bDKeyDown = ( GetAsyncKeyState( 0x44 ) < 0 )?true:false;
	}
	else
	{
		//QWERTY configuration
	m_bWKeyDown =	 ( GetAsyncKeyState( 0x57 ) < 0 ) ?true:false;
	m_bSKeyDown = ( GetAsyncKeyState( 0x53 ) < 0 )?true:false;
	m_bAKeyDown = ( GetAsyncKeyState( 0x41 ) < 0 )?true:false;
	m_bDKeyDown = ( GetAsyncKeyState( 0x44 ) < 0 )?true:false;
	}
	
	if( m_bLeftKeyDown )
		g_pCamera->Add_YAW( -CAMERA_MOVE_SPEED/2 );

	if( m_bRightKeyDown )
		g_pCamera->Add_YAW( CAMERA_MOVE_SPEED/2 );

	/// 촬영 모드.
	if( g_GameDATA.m_bFilmingMode )
	{
		if( m_bUpKeyDown )
			g_pCamera->Add_Distance( CAMERA_MOVE_SPEED * 3 );

		if( m_bDownKeyDown )
			g_pCamera->Add_Distance( -CAMERA_MOVE_SPEED * 3 );

		if( bHeightUp )
			g_pAVATAR->m_fAltitude += 10.0f;
		if( bHeightDown )
			g_pAVATAR->m_fAltitude -= 10.0f;

	}else
	{
		if( m_bWKeyDown || m_bSKeyDown || m_bAKeyDown || m_bDKeyDown || bHeightUp || CGame::GetInstance().IsAutoRun() )
		{
			if( g_pAVATAR == NULL )
				return;

			static int sLastShiftGoFrame = 0;
			int CurrentShiftGoFrame = g_GameDATA.GetGameTime();

			if( CurrentShiftGoFrame - sLastShiftGoFrame > 200 )
			{
				D3DVECTOR Dir = g_pAVATAR->Get_CurPOS() - g_pCamera->Get_Position();
				D3DXVECTOR3 vDir;
				float R = sqrt(Dir.x * Dir.x + Dir.y * Dir.y);
				float alpha = atan2(Dir.y, Dir.x);
				if (m_bAKeyDown)
				{
					if (m_bWKeyDown)
						vDir = D3DXVECTOR3(R*cos(0.25*pi + alpha), R*sin(0.25*pi + alpha), Dir.z);
					else if (m_bSKeyDown)
						vDir = D3DXVECTOR3(R*cos(0.75*pi + alpha), R*sin(0.75*pi + alpha), Dir.z);	
					else
						vDir = D3DXVECTOR3( R*cos(0.5*pi + alpha), R*sin(0.5*pi + alpha), Dir.z);
				}
				else if (m_bDKeyDown)
				{
					if (m_bWKeyDown)
						vDir = D3DXVECTOR3(R*cos(alpha - 0.25*pi), R*sin(alpha - 0.25*pi), Dir.z);
					else if (m_bSKeyDown)
						vDir = D3DXVECTOR3(R*cos(alpha - 0.75*pi), R*sin(alpha - 0.75*pi), Dir.z);	
					else
						vDir = D3DXVECTOR3( R*cos(alpha - 0.5*pi), R*sin(alpha - 0.5*pi), Dir.z);
				}
				else if (m_bSKeyDown)
				{
					vDir = D3DXVECTOR3( -Dir.x, -Dir.y, Dir.z );	
				}
				else
				{	
					vDir = D3DXVECTOR3( Dir.x, Dir.y, Dir.z );	
				}
				if (bHeightUp)
				{
					vDir.x = 0;
					vDir.y = 0;
					vDir.z = 100.0f;
				}
				D3DXVec3Normalize( &vDir, &vDir );
				if (!bHeightUp && !bHeightDown)
					vDir.z = 0.0f;

				vDir = vDir * 2000.0f;      
				Dir.x = vDir.x;
				Dir.y = vDir.y;
				Dir.z = vDir.z;

				D3DVECTOR vPos = g_pAVATAR->Get_CurPOS() + Dir;
				g_pNet->Send_cli_MOUSECMD( 0, vPos );

				///CGame::GetInstance().SetMouseTargetEffect( vPos.x, vPos.y, vPos.z );

				sLastShiftGoFrame = CurrentShiftGoFrame ;
			}
		}
	}
	
}