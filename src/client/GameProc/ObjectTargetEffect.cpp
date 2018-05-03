#include "stdafx.h"
#include ".\objecttargeteffect.h"
#include "../CObjCHAR.h"


const int s_ObjectTargetEffect = 1406;

CObjectTargetEffect::CObjectTargetEffect(void)
{
	m_pEffect = NULL;
	m_pParent = NULL;
}


CObjectTargetEffect::~CObjectTargetEffect(void)
{
}

void CObjectTargetEffect::Init()
{
	m_pEffect = g_pEffectLIST->Add_EffectWithIDX( s_ObjectTargetEffect, false );
	if( m_pEffect == NULL )
	{
		g_pCApp->ErrorBOX( "Target effect load failed", "ERROR" );
	}
}

void CObjectTargetEffect::Clear()
{
	if( m_pEffect != NULL )
	{
		delete m_pEffect;
		m_pEffect = NULL;
	}
}

void CObjectTargetEffect::Attach( CObjCHAR* pObjCHAR )
{
	if( m_pEffect == NULL )
		return;

	if( pObjCHAR == NULL || pObjCHAR->Get_HP() <= 0 )
		return;

	m_pParent = pObjCHAR;

	m_pEffect->UnlinkNODE();
	m_pEffect->LinkBONE( m_pParent->GetZMODEL(), 0 );	
}

void CObjectTargetEffect::Detach()
{
	m_pParent = NULL;

	m_pEffect->UnlinkNODE();
}