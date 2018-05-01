#ifndef _HELPDLG_
#define _HELPDLG_

#include "tdialog.h"
#include <map>
#include <string>


class	CHelpPage;
class	CTDefaultTreeModel;		
class	CTDefaultTreeNode;
class	CTDefaultTreeNodeRenderer;
class   CTree;
/**
* 도움말창에 사용되는 다이얼로그
*	- 툴에서 UTF8로 저장된 Data를 로드하여 Tree에 추가한다.
*
* @Author		이동현, 최종진
* @Date			2005/9/14
*/
class CHelpDlg : public CTDialog
{
protected:
	std::map< std::string, CHelpPage* >		m_PageList;
	CTree*									m_pTree;
	CTDefaultTreeModel*						m_pTreeModel;		
	CTDefaultTreeNodeRenderer*				m_pTreeNodeRenderer;


	bool LoadHelpContents( const char* szFileName , CTree* pTree );	
	bool LoadNodeCollection( CFileSystem* pFileSystem, CTDefaultTreeNode* pTreeNode );
	bool LoadPageList( CFileSystem* pFileSystem, CTDefaultTreeModel* pTreeModel ); 

	enum{
		IID_TABLIST			= 1,
		IID_TABDESCRIPTION	= 0
	};


public:
	CHelpDlg(void);
	virtual ~CHelpDlg(void);


	virtual void Draw();
	virtual bool Create( const char* IDD );
	virtual UINT Process( UINT uiMsg,WPARAM wParam,LPARAM lParam);
	void ClearPageList(){ m_PageList.clear(); };



	enum{
		IID_BTN_CLOSE				= 10,
		IID_BTN_ICONIZE				= 11,
		IID_TABBEDPANE				= 20,
		IID_TREE_SCROLL				= 26,
		IID_TREE					= 29,
		IID_LIST_DESCRIPTION		= 36,
		IID_SCROLLBAR_DESCRIPTION	= 37

	};

};

/// CHelpDlg의 CTree에서 하나의 Node에 해당하는 class
class CHelpPage : public CTObject
{
private:
	std::string				m_strPageName;
	vector< std::string >	m_PageContents;

public:
	CHelpPage(){};
	CHelpPage( const char* pNodeName )
	{
		m_strPageName = std::string( (pNodeName)? pNodeName:"NULL" );
	}
	CHelpPage( char* pPageName, char* pPageContents );
	~CHelpPage();

	const char* GetPageName(){ return m_strPageName.c_str(); }
	const char* GetPageContent( int i );
	void	SetPageContents( char* pszContent );
	int		GetPageContentsCount();

	virtual const char* toString() const { return m_strPageName.c_str(); }
};

#endif //_HELPDLG_