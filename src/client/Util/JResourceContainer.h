///////////////////////////////////////////////////////////////////////////////////////////////////// 
//                    
//        File Name:  JResourceContainer.h 
//
//      Description:  Container for resources. This is template class.
//
//           Author:  Jeddli. LEE DONG HYEUN. 
//                    
//        Copyright:  
//                    Confidential and Proprietary Source Code For: 
//                           
//                        .. 
//                    
//        Revisions:  
//                    2003-10-5 - LDH - Original version. 
//                    
//  Copyright (c) 2002, XXXXX, Inc. All rights reserved. 
//                    
/////////////////////////////////////////////////////////////////////////////////           
#ifndef _RESOURCECONTAINER_
#define _RESOURCECONTAINER_

#include <string>
#include <map>
#include <string.h>
#include <assert.h>
#include <algorithm>

#include "JDebugNew.h"

namespace JED
{
	///
	///	Make hash code
	///
	unsigned long GetHashCode( const char* pString );	

	///
	/// Container key object( has hash code for fast search. )
	/// http://www.gpgstydy.com
	/// 
	class CResObject
	{
	private:
		std::string			m_strName;
		unsigned long		m_lHashCode;

	public:
		CResObject( const char* strName )
		{
			SetName( strName );
		}

		void SetName( const char* strName )
		{
			if( strName == NULL )
			{
				assert( "ResObject::SetName .. strName is NULL" );
				return;
			}

			m_strName = std::string( strName );
	   		m_lHashCode = GetHashCode( strName );
		}
		
		inline virtual bool operator < ( const CResObject& Obj ) const
		{
	   		if( m_lHashCode != Obj.m_lHashCode )
	   	   		return m_lHashCode < Obj.m_lHashCode;

			return strcmp( m_strName.c_str(), Obj.m_strName.c_str() ) < 0;
		}
	};


	///
	/// Container class	
	/// Must use not pointer type
	///
	template< class T >
	class CResourceContainer
	{
	private:
		std::map< CResObject, T* >		m_ObjectMap;

		std::list< T* >					m_pObjectPool;
		std::list< T* >					m_pUsedPool;

	public:
		CResourceContainer(void);
		~CResourceContainer(void);

		void	InitObjectPool( int iSize );
		T*		GetObjectInstanceFromPool();
		void	ReturnObjectToPool( T* );
		void	ClearPool();

		static  void DeletePoolObject( T* );


		bool	AddObject( const char* strName, T* obj );
		T*		GetObjectByName( const char* strName );
	};

	template< class T >
	CResourceContainer< T >::CResourceContainer(void)
	{
	}

	template< class T >
	CResourceContainer< T >::~CResourceContainer(void)
	{
		ClearPool();
	}

	//---------------------------------------------------------------------------
	/// Init object pool
	//---------------------------------------------------------------------------
	template< class T >
	void CResourceContainer< T >::InitObjectPool( int iSize )
	{
		for( int i = 0; i < iSize; i++ )
		{
			m_pObjectPool.push_back( new T() );
		}
	}

	//---------------------------------------------------------------------------
	/// get object from pool
	//---------------------------------------------------------------------------
	template< class T >
	T* CResourceContainer< T >::GetObjectInstanceFromPool()
	{
		T* pT = NULL;

		if( m_pObjectPool.empty() )
			m_pObjectPool.push_back( new T() );

		pT = m_pObjectPool.back();
		m_pObjectPool.pop_back();

		m_pUsedPool.push_back( pT );
		
		return pT;
	}

	//---------------------------------------------------------------------------
	/// return object to pool
	//---------------------------------------------------------------------------
	template< class T >
	void CResourceContainer< T >::ReturnObjectToPool( T* )
	{
		m_pUsedPool.remove( T* );
		m_pObjectPool.push_back( T* );
	}

	//---------------------------------------------------------------------------
	/// clear all object pool
	//---------------------------------------------------------------------------
	template< class T >
	void CResourceContainer< T >::ClearPool()
	{
		std::for_each( m_pObjectPool.begin(), m_pObjectPool.end(), CResourceContainer<T>::DeletePoolObject );
		std::for_each( m_pUsedPool.begin(), m_pUsedPool.end(), CResourceContainer<T>::DeletePoolObject );

		m_pObjectPool.clear();
		m_pUsedPool.clear();
	}

	//---------------------------------------------------------------------------
	/// functor..
	//---------------------------------------------------------------------------
	template< class T >
	void CResourceContainer< T >::DeletePoolObject( T* pT )
	{
		if( pT )
		{
			delete pT;
			pT = NULL;
		}
	}


	/// Add new object, if it exist already in map, return false.
	template< class T >
	bool CResourceContainer< T >::AddObject( const char* strName, T* obj )
	{
		CResObject keyObj( strName );

		std::map< CResObject, T* >::iterator itor = m_ObjectMap.find( keyObj );
		if( itor != m_ObjectMap.end() )
		{			
			return false;
		}		

		m_ObjectMap[ keyObj ] = obj;

		return true;
	}

	/// Get object by name
	template< class T >
	T* CResourceContainer< T >::GetObjectByName( const char* strName )
	{
		CResObject keyObj( strName );
		std::map< CResObject, T* >::iterator itor = m_ObjectMap.find( keyObj );
		if( itor != m_ObjectMap.end() )
		{
			return itor->second;
		}
		return NULL;
	}
}

#endif _RESOURCECONTAINER_