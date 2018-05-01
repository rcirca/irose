#include "StdAfx.h"
#include ".\tcontaineritem.h"
#include <assert.h>
CTContainerItem::CTContainerItem(void)
{

}

CTContainerItem::~CTContainerItem(void)
{

}

const char*		CTContainerItem::GetIndetify() const 
{ 
	return m_strIdentify.c_str(); 
}

const char*		CTContainerItem::GetValue() const 
{ 
	return m_strValue.c_str(); 
}

void			CTContainerItem::SetValue( const char* pszValue )
{ 
	assert( pszValue );
	m_strValue = pszValue; 
}

void			CTContainerItem::SetIdentify( const char* pszIdentify )
{ 
	assert( pszIdentify );
	if( pszIdentify )
		m_strIdentify = pszIdentify; 
}
