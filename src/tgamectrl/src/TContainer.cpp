#include "StdAfx.h"
#include ".\tcontainer.h"
#include <crtdbg.h>

CTContainer::CTContainer(void) 
{
	m_strString = "TContainer";
	m_uSelectedItemIndex = 0;
}

CTContainer::~CTContainer(void)
{

}

void CTContainer::Clear()
{
	std::deque< CTObject* >::iterator iter;
	//unsigned iSize = m_listItem.size();
	for( iter = m_listItem.begin(); iter != m_listItem.end(); )
	{
		delete *iter;
		iter = m_listItem.erase( iter );
	}
	m_uSelectedItemIndex = 0;
}

void CTContainer::AddItem( CTObject* item ) 
{
	if( item == NULL )
	{
		_RPT0( _CRT_WARN,"Argument is NULL @TContainer::AddItem() \n");
		return ;
	}
	m_listItem.push_back( item );
}

const CTObject* CTContainer::GetItem( unsigned iSubscript )
{
	if( iSubscript <= 0 || iSubscript > m_listItem.size() )
	{
		_RPT1( _CRT_WARN,"Invalid Subscript(%d) @TContainer::GetItem() \n",iSubscript );
		return NULL;
	}
	return m_listItem[ iSubscript - 1 ];
}

const CTObject*	CTContainer::GetSelectedItem()
{
	if( m_uSelectedItemIndex <= 0 || m_uSelectedItemIndex > m_listItem.size() )
	{
		_RPT1( _CRT_WARN,"Invalid SelectedItemIndex(%d) @TContainer::GetSelectedItem() \n",m_uSelectedItemIndex );
		return NULL;
	}
	return m_listItem[ m_uSelectedItemIndex - 1 ];
}

unsigned CTContainer::GetSelectedItemIndex()
{
	if( m_uSelectedItemIndex > m_listItem.size() )
	{
		_RPT0( _CRT_WARN,"Invalid SelectedItemIndex ^ SelectedItemIndex = 0 @TContainer::GetSelectedItemIndex \n" );
		return 0;
	}
	return m_uSelectedItemIndex;
}

void CTContainer::SelectItem( unsigned iSubscript )
{
	if( iSubscript <= 0 || iSubscript > m_listItem.size() )
	{
		_RPT1( _CRT_WARN,"Invalid Subscript(%d) @TContainer::SelectItem() \n",iSubscript );
		return;
	}
	m_uSelectedItemIndex = iSubscript;
}

unsigned CTContainer::GetSize()
{
	return m_listItem.size();
}

std::deque< CTObject* >& CTContainer::GetItemList()
{
	return m_listItem;
}