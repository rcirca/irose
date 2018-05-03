#include "stdafx.h"
#include ".\resourcecache.h"
 
const int iDefaultCacheSize = 20;


//--------------------------------------------------------------------------------
/// default constructor
//--------------------------------------------------------------------------------
template< class T >
CResourceCache< T >::CResourceCache(void)
{
	m_iCacheSize		= iDefaultCacheSize
}

//--------------------------------------------------------------------------------
/// copy constructor
//--------------------------------------------------------------------------------
template< class T >
CResourceCache< T >::CResourceCache( int iCachingPolicy )
{
	m_iCacheSize		= iDefaultCacheSize
	m_iCachingPolicy	= iCachingPolicy;
}

//--------------------------------------------------------------------------------
/// destructor
//--------------------------------------------------------------------------------
template< class T >
CResourceCache< T >::~CResourceCache(void)
{
	ClearPool();
}

//--------------------------------------------------------------------------------
/// clear cache pool
//--------------------------------------------------------------------------------
template< class T >
void CResourceCache< T >::ClearPool()
{
	m_CachePool.clear();
}

//--------------------------------------------------------------------------------
/// Get data from cache.. Identity method
//--------------------------------------------------------------------------------
template< class T >
T* CResourceCache< T >::GetData( const char* pstrName )
{
	return NULL;
}