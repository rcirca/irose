#include "stdafx.h"
#include "TargetObjectDLG.h"
#include "../CTDrawImpl.h"
#include "../it_Mgr.h"
#include "../IO_ImageRes.h"
#include "../../OBJECT.h"
#include "../../Game.h"

#include "../../JCommandState.h"
#include "tgamectrl/resourcemgr.h"




CTargetObjectDLG::CTargetObjectDLG()
{

}

CTargetObjectDLG::~CTargetObjectDLG()
{

}


void CTargetObjectDLG::Draw()
{
	/// 선택된 타겟 출력.
	if ( g_UserInputSystem.GetCurrentTarget() ) 
	{		
		
		CResourceMgr*   pResourceMgr = CResourceMgr::GetInstance();
		CTDrawImpl* pDrawObj = g_itMGR.GetDrawImplPtr();
		if( pDrawObj == NULL )
			return;

		CObjCHAR *pObj = (CObjCHAR*)g_pObjMGR->Get_CharOBJ( g_UserInputSystem.GetCurrentTarget(), true );
		/// 유효하지 않은 타겟이다.. 마우스 컴맨드 초기화
		if( pObj == NULL )
		{
			g_UserInputSystem.ClearMouseState();
			return;
		}
		
		CTDialog::Draw();
		D3DCOLOR d3Color;

		switch( pObj->Get_TYPE() ) 
		{
			case OBJ_NPC :
				d3Color = g_dwGREEN;	// D3DCOLOR_XRGB( 0,255,0 );
				break;
			case OBJ_MOB :
				{
					///몹과 아바타의 레벨차에 따라 색깔 틀리게 표시 
					/*int iMobLv = pObj->Get_LEVEL();
					int iAvatarLv = g_pAVATAR->Get_LEVEL();
					int iDiffLv =  iAvatarLv - iMobLv;


					if( iDiffLv <= -23 )
						d3Color = g_dwVioletName;
					else if( iDiffLv <= -16 )
						d3Color = g_dwPinkName;
					else if( iDiffLv <= -10 )
						d3Color = g_dwRedName;
					else if( iDiffLv <= -5 )
						d3Color = g_dwOrangeName;
					else if( iDiffLv <= 0 )
						d3Color = g_dwYellowName;
					else if( iDiffLv <= 5 )
						d3Color = g_dwGreenName;
					else if( iDiffLv <= 11 )
						d3Color = g_dwBlueName;
					else if( iDiffLv <= 18 )
						d3Color = g_dwLightBlueName;
					else 
						d3Color = g_dwGrayName;
					break;*/

				}
			default :
				d3Color = g_dwWHITE;
				break;
		}


		/// 캐릭터 얼굴 아이콘..
		//if( ( pObj->Get_TYPE() == OBJ_NPC || pObj->Get_TYPE() == OBJ_MOB )  ) 
		//{
		//	short nFaceIcon = NPC_FACE_ICON( pObj->Get_CharNO() );
		//	pDrawObj->Draw( m_sPosition.x - 20, m_sPosition.y - 24 ,IMAGE_RES_FACE, nFaceIcon );
		//}


		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x - 20, (float)m_sPosition.y - 24,0.0f);
		::setTransformSprite( mat );
		
		
		
		
		RECT rt;
		SetRect( &rt,	85, 
						34, 
						85 + 148, 
						34 + 30 );


		int iTargetLevel = pObj->Get_LEVEL();

		if( 0 )
		{			
			if( pObj->Get_TYPE() == OBJ_AVATAR )
				::drawFontf(g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, &rt, d3Color,DT_CENTER | DT_VCENTER,"%d/%d", pObj->Get_HP(), pObj->Get_MaxHP() );
			else
				::drawFontf(g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, &rt, d3Color,DT_CENTER | DT_VCENTER,"%s(%d)", pObj->Get_NAME(), iTargetLevel );
		}
		else
		{		
			::drawFontf(g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, &rt, d3Color,DT_CENTER | DT_VCENTER,"%s", pObj->Get_NAME() );
		}

		if( pObj->Get_TYPE() == OBJ_MOB ) 
		{
			int iCurrentHP = pObj->Get_HP();
			int iMaxHP = pObj->Get_MaxHP();
			
			if( iCurrentHP > iMaxHP )
			{
				iCurrentHP = iMaxHP;
				pObj->Set_HP( iCurrentHP );
			}

			if( iMaxHP != 0 )
			{
				short nWidth = ( 151 * iCurrentHP ) / iMaxHP;			
//				if( nWidth > 151 )
//					nWidth = 151;
				pDrawObj->Draw( m_sPosition.x + 60, m_sPosition.y + 33, nWidth, IMAGE_RES_UI, pResourceMgr->GetImageNID( IMAGE_RES_UI, "ID_GUAGE_RED") );	
			}
		}
	}
}

unsigned int CTargetObjectDLG::Process( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{	
	if(!IsVision()) return 0;	
	if( g_UserInputSystem.GetCurrentTarget() == 0 ) return 0;
	return CTDialog::Process( uiMsg, wParam, lParam );
}
