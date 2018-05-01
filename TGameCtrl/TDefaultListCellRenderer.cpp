#include "StdAfx.h"
#include ".\tdefaultlistcellrenderer.h"
#include "TStatic.h"
CTDefaultListCellRenderer::CTDefaultListCellRenderer(void)
{
	m_pComponent = new CTStatic;
	m_pComponent->Show();

}

CTDefaultListCellRenderer::~CTDefaultListCellRenderer(void)
{
	if( m_pComponent )
	{
		delete m_pComponent;
		m_pComponent = NULL;
	}
}

CWinCtrl* CTDefaultListCellRenderer::GetListCellRendererComponent( const CTObject* pObject, bool selected, bool mouseover )
{
	m_pComponent->SetString( pObject->toString() );
	return m_pComponent;
}