#ifndef _CHANGE_VISIBILITY_
#define _CHANGE_VISIBILITY_


class CObjCHAR;

//-------------------------------------------------------------------------------------------------
/// Change visibility gradually
/// @brief 
//-------------------------------------------------------------------------------------------------

class CChangeVisibility
{
private:
	int						m_iProgressTime;					/// It's time to finishing the change
	int						m_iTotalElaspedTime;				/// Elapsed time until now.

	bool					m_bToDisappear;


	CObjCHAR*				m_pObjCHAR;


	CChangeVisibility(void);
public:	
	CChangeVisibility( CObjCHAR* pObjCHAR, int iProgressTime, bool bToDisappear );
	~CChangeVisibility(void);


	//-------------------------------------------------------------------------------------------------
	/// @brief Process visibility
	//-------------------------------------------------------------------------------------------------	
	bool					ProcessVisibility( int iElapsedFrameTime );
};


#endif //_CHANGE_VISIBILITY_