//////////////////////////////////////////////////////////////////////////////////
//																				//
//	CSingleton Class.. For Global Variable										//
//	See Game Programming Gems..													//
//																				//
//	This class will be improved ...												//
//																				//		
//	2001. 5. 10		jeddli...													//
//////////////////////////////////////////////////////////////////////////////////


#ifndef _JEDDLI_SINGLETON_
#define _JEDDLI_SINGLETON_

#include <assert.h>

template < typename T > 
class CJSingleton
{
private:
	static T*	m_Singleton;

public:
	CJSingleton()
	{		
		int offset = (int)(T*)1 - (int)(CJSingleton < T > *)(T*)1;
		m_Singleton = (T*)((int)this + offset);

		assert( m_Singleton && "Singleton's Member is NULL");
	}

	~CJSingleton()
	{
		assert( m_Singleton && "Singleton's Member is NULL");
		m_Singleton = NULL;
	}

	static T& GetSingleton()
	{
		assert( m_Singleton && "Singleton's Member is NULL");
		return *m_Singleton;
	}

	static T* GetSingletonPtr()
	{
		assert( m_Singleton && "Singleton's Member is NULL");
		return m_Singleton;
	}
};

template<class T> T* CJSingleton<T>::m_Singleton = 0;

#endif //_JEDDLI_SINGLETON_