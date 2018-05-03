/*
	$Header: /Client/Event/QF_CONVERSATION.CPP 7     05-10-17 8:49p Raning $
*/

#include "stdAFX.h"
#include "CEvent.h"
#include "Quest_FUNC.h"


#include "./dlgs/CDialogDlg.h"
#include "InterfaceType.h"
#include "../Interface/it_mgr.h"
#include "../Interface/IO_ImageRes.h"
#include "CObjCART.h"
#include "OBJECT.h"
#include "CCamera.h"
#include "../IO_Terrain.h"

//-------------------------------------------------------------------------------------------------
int		QF_getEventOwner ( int hEvent )
{
	CEvent *pEvent=(CEvent*)hEvent;
	if ( pEvent )
		return pEvent->m_iOwnerObjIDX;
	return 0;
}

void	QF_beginCon ( void )
{
}

void	QF_closeCon ( void )
{
}


void	QF_gotoCon ( int hEvent, int iDataIDX, int iItemIDX )
{
	/*
	CEvent *pEvent=(CEvent*)hEvent;
	if ( pEvent->m_pScrDATA[ iDataIDX ].m_pScrITEM[ iItemIDX ].m_lActiveScrIDX >= 0 ) {
		// run script
		;
		;
	}

	LogString (LOG_DEBUG, "%d/%d: %s \n", iDataIDX, iItemIDX, pEvent->m_pScrDATA[ iDataIDX ].m_pScrITEM[ iItemIDX ].m_Message.Get());
	if ( pEvent->m_pScrDATA[ iDataIDX ].m_pScrITEM[ iItemIDX ].m_lChildDataIDX >= 0 ) {
		iDataIDX = pEvent->m_pScrDATA[ iDataIDX ].m_pScrITEM[ iItemIDX ].m_lChildDataIDX;
		LogString (LOG_DEBUG, "%d/%d: %s \n", iDataIDX, iItemIDX, pEvent->m_pScrDATA[ iDataIDX ].m_pScrITEM[ iItemIDX ].m_Message.Get());
	}
	*/
}


void	QF_ChangetalkImage ( ZSTRING szTalkImage )
{
	CDialogDlg* pDlg = NULL;

	pDlg = (CDialogDlg*)g_itMGR.FindDlg( DLG_TYPE_DIALOG );
	if( !pDlg && pDlg->IsInValidShow() )
	{
		return;
	}	
	
	pDlg->SetNpcFace( CImageResManager::GetSingleton().Load_NpcFace( szTalkImage ) );
	
}



void	QF_ChangetalkName ( ZSTRING szTalkName )
{
	CDialogDlg* pDlg = NULL;

	pDlg = (CDialogDlg*)g_itMGR.FindDlg( DLG_TYPE_DIALOG );
	if( !pDlg && pDlg->IsInValidShow() )
	{
		return;
	}	

	pDlg->SetNpcName( szTalkName );
}



// È«±Ù : È÷¾î·Î Äù½ºÆ®
void	QF_NpcTalkinterfaceHide( int iTime )
{
	CDialogDlg* pDlg = NULL;

	pDlg = (CDialogDlg*)g_itMGR.FindDlg( DLG_TYPE_DIALOG );
	if( !pDlg && pDlg->IsInValidShow() )
	{
		return;
	}	

	pDlg->SetNpctalkinterfaceHide( iTime );
}

// È«±Ù : È÷¾î·Î Äù½ºÆ®
void	QF_NpcTalkinterfaceView( void )
{
	CDialogDlg* pDlg = NULL;

	pDlg = (CDialogDlg*)g_itMGR.FindDlg( DLG_TYPE_DIALOG );
	if( !pDlg && pDlg->IsInValidShow() )
	{
		return;
	}	

	pDlg->SetNpctalkinterfaceHide( 0 );

}

// È«±Ù : È÷¾î·Î Äù½ºÆ®
void	QF_NpcHide( int iNpcIndex )
{	
	CObjNPC* pNPC = (CObjNPC*)( g_pObjMGR->Get_CharOBJ( iNpcIndex, false ) );
	if( pNPC )
	{
		::setVisibilityRecursive( pNPC->GetZMODEL(), 0.0f ); 	
	}	
}

// È«±Ù : È÷¾î·Î Äù½ºÆ®
void	QF_NpcView( int iNpcIndex )
{
	CObjNPC* pNPC = (CObjNPC*)( g_pObjMGR->Get_CharOBJ( iNpcIndex, false ) );
	if( pNPC )
	{
		::setVisibilityRecursive( pNPC->GetZMODEL(), 1.0f ); 
	}
}


void	QF_EffectCallSelf ( ZSTRING szEffectName )
{
	int iEffectHash = (int)g_pEffectLIST->Add_EffectFILE( (char*)szEffectName );

	if( iEffectHash > 0 )
	{
		g_pAVATAR->ShowEffectOnCharByHash( iEffectHash );
	}	
}


void	QF_EffectCallNpc ( ZSTRING szEffectName, int iNpcIndex )
{
	int iEffectHash = (int)g_pEffectLIST->Add_EffectFILE( (char*)szEffectName );

	CObjCHAR *pObjCHAR = g_pObjMGR->Get_CharOBJ( iNpcIndex, false );
	if ( pObjCHAR && iEffectHash > 0) 
	{
		pObjCHAR->ShowEffectOnCharByHash( iEffectHash );		
	}
}


void	QF_MotionCallSelf ( ZSTRING szEffectName )
{
	int iHashMOTION = g_MotionFILE.Add_FILE( (char*)szEffectName );

	if( iHashMOTION>0 )
	{
		g_pAVATAR->Set_UserMOITON( iHashMOTION, 0, 1 );
	}	
}


void	QF_MotionCallNpc ( ZSTRING szEffectName, int iNpcIndex )
{
	int iHashMOTION = g_MotionFILE.Add_FILE( (char*)szEffectName );

	CObjCHAR *pObjCHAR = g_pObjMGR->Get_CharOBJ( iNpcIndex, false );
	if ( pObjCHAR && iHashMOTION>0 ) 
	{
		pObjCHAR->Set_UserMOITON (iHashMOTION, 0, 1 );	
	}
}


void	QF_CameraworkingSelf ( ZSTRING szCameraName )
{
	HNODE hCameraMotion = ::findNode( szCameraName );
	if ( hCameraMotion == 0 )
	{
		hCameraMotion = ::loadMotion( szCameraName, szCameraName, 1, 4, 3, 1, 0 );
	}

	HNODE hMotion_camera = loadCamera( szCameraName, "cameras/camera01.zca", hCameraMotion );
    
	if( hCameraMotion>0 && hMotion_camera>0 )
	{
		g_pCamera->Init( hMotion_camera );
		g_pCamera->Set_Position( g_pAVATAR->Get_CurPOS() );

		controlAnimatable( hMotion_camera, 1 );
		setRepeatCount( hMotion_camera, 1 );
	}
}

void	QF_CameraworkingNpc ( ZSTRING szCameraName, int iNpcIndex )
{
	HNODE hCameraMotion = ::findNode( szCameraName );
	if ( hCameraMotion == 0 )
	{
		hCameraMotion = ::loadMotion( szCameraName, szCameraName, 1, 4, 3, 1, 0 );
	}

	HNODE hMotion_camera = loadCamera( szCameraName, "cameras/camera01.zca", hCameraMotion );

	CObjCHAR *pObjCHAR = g_pObjMGR->Get_CharOBJ( iNpcIndex, false );	

	if( hCameraMotion>0 && hMotion_camera>0 && pObjCHAR )
	{
		g_pCamera->Init( hMotion_camera );
		
		g_pCamera->Set_Position( pObjCHAR->Get_CurPOS() );

		controlAnimatable( hMotion_camera, 1 );
		setRepeatCount( hMotion_camera, 1 );
	}
}

void	QF_CameraworkingPoint ( ZSTRING szCameraName, int PosX, int PosY )
{
	D3DVECTOR PosENZIN;
	PosENZIN.x = 520000.0f;
	PosENZIN.y = 520000.0f;
	PosENZIN.z = 0.0f;


	HNODE hCameraMotion = ::findNode( szCameraName );
	if ( hCameraMotion == 0 )
	{
		hCameraMotion = ::loadMotion( szCameraName, szCameraName, 1, 4, 3, 1, 0 );
	}

	HNODE hMotion_camera = loadCamera( szCameraName, "cameras/camera01.zca", hCameraMotion );

	if( hCameraMotion>0 && hMotion_camera>0 )
	{
		g_pCamera->Init( hMotion_camera );

		D3DVECTOR PosENZIN;
		PosENZIN.x = PosX;
		PosENZIN.y = PosY;
		PosENZIN.z = g_pTerrain->GetHeightTop( PosX, PosY );

		g_pCamera->Set_Position( PosENZIN );

		controlAnimatable( hMotion_camera, 1 );
		setRepeatCount( hMotion_camera, 1 );
	}

}

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
