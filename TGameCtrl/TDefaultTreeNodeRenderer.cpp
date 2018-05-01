#include "StdAfx.h"
#include ".\tdefaulttreenoderenderer.h"
#include "TImage.h"
#include "TControlMgr.h"
CTDefaultTreeNodeRenderer::CTDefaultTreeNodeRenderer(void)
{
	m_pCloseImage = NULL;
	m_pOpenImage  = NULL;
	m_pLeafImage	= NULL;
	m_pParentImage	= NULL;
	
	m_pNodeImage	= NULL;
}

CTDefaultTreeNodeRenderer::~CTDefaultTreeNodeRenderer(void)
{
	if( m_pCloseImage )
	{
		delete m_pCloseImage;
		m_pCloseImage = NULL;
	}

	if( m_pOpenImage )
	{
		delete m_pOpenImage;
		m_pOpenImage = NULL;
	}
	

	if(  m_pLeafImage )
	{
		delete m_pLeafImage;
		m_pLeafImage = NULL;
	}
	
	if( m_pParentImage )
	{
		delete m_pParentImage;
		m_pParentImage = NULL;
	}

	m_pNodeImage	= NULL;
	m_pImage		= NULL;

}

CWinCtrl* CTDefaultTreeNodeRenderer::getTreeCellRendererComponent(CTree* tree, CTObject* value, bool selected, bool expanded, bool leaf, int row, bool hasFocus)
{
	if( leaf )
	{
		m_pImage	 = NULL;	
		m_pNodeImage = m_pLeafImage;
	}
	else
	{
		m_pNodeImage = m_pParentImage;
		if( expanded )
			m_pImage = m_pOpenImage;
		else
			m_pImage = m_pCloseImage;
	}

	if( selected )
		m_color = D3DCOLOR_ARGB(255,0,128,255 );
	else
		m_color = D3DCOLOR_ARGB(255,0,0,0);
	m_strText.assign( value->toString() );

	return this;
}
void CTDefaultTreeNodeRenderer::Draw( POINT ptDraw )
{
	m_pFontImpl = CTControlMgr::GetInstance()->GetFontMgr();
	if( !IsVision() ) return;

	if( m_pNodeImage )
	{
		if( m_pImage )
		{
			m_pImage->Draw( ptDraw );
			m_pNodeImage->SetPosition( ptDraw.x + m_pImage->GetWidth(), ptDraw.y );
		}
		else
		{
			m_pNodeImage->SetPosition( ptDraw.x + m_sizeImage.cx, ptDraw.y );
		}
		m_pNodeImage->Draw();
		m_pFontImpl->Draw( 0, true, m_pNodeImage->GetWidth(), 0, m_color, m_strText.c_str());
	}
	else
	{
		m_pFontImpl->SetTransformSprite( (float)ptDraw.x, (float)ptDraw.y );
		m_pFontImpl->Draw( 0, true, 9, 0, m_color, m_strText.c_str());
	}
}

void CTDefaultTreeNodeRenderer::setParentImage( CTImage* pImage)
{ 
	m_pParentImage = pImage;
	m_pParentImage->Show();
}

void CTDefaultTreeNodeRenderer::setLeafImage( CTImage* pImage)
{ 
	m_pLeafImage = pImage; 
	m_pLeafImage->Show();
}

void CTDefaultTreeNodeRenderer::setOpenImage( CTImage* pImage )
{ 
	_ASSERT( pImage );
	if( pImage )
	{
		m_sizeImage.cx = pImage->GetWidth();
		m_sizeImage.cy = pImage->GetHeight();
	}
	m_pOpenImage = pImage; 
}

void CTDefaultTreeNodeRenderer::setCloseImage( CTImage* pImage )
{ 
	_ASSERT( pImage );
	if( pImage )
	{
		m_sizeImage.cx = pImage->GetWidth();
		m_sizeImage.cy = pImage->GetHeight();
	}
	m_pCloseImage = pImage; 
}
