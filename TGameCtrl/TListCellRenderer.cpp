#include "StdAfx.h"
#include ".\tlistcellrenderer.h"
#include "TContainerItem.h"
#include "TStatic.h"
#include <assert.h>
CTListCellRenderer::CTListCellRenderer(void)
{
	m_pComponent = new CTStatic;
	m_pComponent->Show();
}

CTListCellRenderer::~CTListCellRenderer(void)
{
	if( m_pComponent )
	{
		delete m_pComponent;
		m_pComponent = NULL;
	}
}

CWinCtrl* CTListCellRenderer::GetListCellRendererComponent( const CTObject* pObject, bool selected, bool mouseover )
{
	assert( pObject );
	CTContainerItem* pItem = (CTContainerItem*) pObject;
	
	if( selected )
		m_pComponent->SetColor( D3DCOLOR_ARGB( 255,0,0,255 ));
	else
		m_pComponent->SetColor( D3DCOLOR_ARGB( 255,255,255,255 ));
	m_pComponent->SetString( pItem->GetIndetify() );
	
	return m_pComponent;
}