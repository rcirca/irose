#ifndef _RESOURCE_CACHE_
#define _RESOURCE_CACHE_

#include <list>
#include <string>

///
/// This is a class for caching resources.
/// We can give a some hint to class
/// This is performed by LRU.
///

template< class T >
class CResourceCache
{
public:
	enum CACHING_HINT
	{
		USE_FIFO = 0,
		USE_LRU,
		USE_MRU,
	};


private:
	struct RSC_CACHE_NODE
	{
		std::string		strName;
		T*				data;
	};


public:
	CResourceCache(void);
	CResourceCache( int iCachingPolicy );
	~CResourceCache(void);


	void			ClearPool();
	T*				GetData( const char* pstrName );
	
	
	//-------------------------------------------------------------------------------
	/// Template methods
	//-------------------------------------------------------------------------------
	virtual T*		LoadData( const char* pstrName ) = 0;
	virtual void	FreeData( T& data ) = 0;



private:
	
	int							m_iCacheSize;
	int							m_iCachingPolicy;		/// default value is USE_LRU.

	std::list< RSC_CACHE_NODE >	m_CachePool;
};


#endif //_RESOURCE_CACHE_
