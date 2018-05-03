/*
	$Header: /Client/Event/GF_EFFECT.CPP 2     03-09-04 4:35p Icarus $
*/

#include 	"stdAFX.h"
#include 	"Quest_FUNC.h"
#include 	"Game.h"
#include 	"IO_Effect.h"
#include 	"CCamera.h"
#include	"OBJECT.h"

//-------------------------------------------------------------------------------------------------
void	GF_playSound ( ZSTRING szSoundFile,	int iRepeat, int iOuterOpt )
{
	
	g_pSoundLIST->PlaySoundFile ( (char *)szSoundFile );
}

//-------------------------------------------------------------------------------------------------
void	GF_rotateCamera ( int iPole, float iDegree )
{
	HNODE hCamera = g_pCamera->GetZHANDLE() ;

	if( iPole == RCPOLE_CAMERA )
		::rotateCamera( hCamera , 0 , iDegree ) ;
	else if( iPole == RCPOLE_AVATA )
		g_pCamera->Add_YAW( (short)iDegree );
}

//-------------------------------------------------------------------------------------------------
void	GF_zoomCamera (int iPercent )
{
	g_pCamera->Add_Distance( (float)-(iPercent) ) ;
}

//-------------------------------------------------------------------------------------------------
void	GF_playEffect ( int iEffect , float x , float y )
{
	CEffect *		pEffect ;
	D3DVECTOR		d3dvecCharPos ;

	if (iEffect <= 0 || iEffect >= g_pEffectLIST->GetFileCNT () ) {
		return;
	}

	pEffect = g_pEffectLIST->Add_EffectWithIDX (iEffect, true);
	d3dvecCharPos = g_pAVATAR->Get_CurPOS();
	d3dvecCharPos.x += x;
	d3dvecCharPos.y += y;

	pEffect->LinkNODE ( g_pAVATAR->GetZMODEL() );
	pEffect->UnlinkVisibleWorld ();
	if ( g_pAVATAR->IsVisible() )
		pEffect->InsertToScene ();
}




//-------------------------------------------------------------------------------------------------
