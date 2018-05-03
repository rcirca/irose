#ifndef _CJContainer_
#define _CJContainer_

#include "tobject.h"

class CWinCtrl;

//------------------------------------------------------------------------------------------------------------------------
///	CWinCtrl을 child로 가지는 일반적인 컨테이너 클래스
///
/// @Author				최종진
///
/// @Date				2005/08/23
//------------------------------------------------------------------------------------------------------------------------
class TGAMECTRL_API CJContainer :	public CTObject
{
public:
	CJContainer(void);
	virtual ~CJContainer(void);

	void Draw();
	void Update( POINT ptMouse );
	unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	void MoveWindow( POINT pt );
	void Show();
	void Hide();

	CWinCtrl*	Find( unsigned index );
	CWinCtrl*	Find( const char * szName );

	void		Add( CWinCtrl* pCtrl );
	void		Remove( unsigned index );
	void		RemoveAll();
	void		SetChildEnable( unsigned iID, bool bEnable );
protected:
	void		MoveCtrl2ListEnd( int iID );
protected:
	std::list< CWinCtrl* >	m_Children;
};
#endif