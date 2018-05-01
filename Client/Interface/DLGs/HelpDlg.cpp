#include "stdafx.h"
#include ".\helpdlg.h"
#include "../InterfaceType.h"
#include "../it_mgr.h"
#include "../IO_ImageRes.h"


#include "TDefaultTreeNode.h"
#include "TDefaultTreeModel.h"
#include "TDefaultTreeNodeRenderer.h"
#include "Tree.h"
#include "TImage.h"
#include "TListBox.h"
#include "TScrollBar.h"
#include "ResourceMgr.h"
#include "TControlMgr.h"
#include "JContainer.h"
#include "TabbedPane.h"
#include "../../Util/Localizing.h"
#include "../../Common/IO_STB.h"


CHelpDlg::CHelpDlg(void)
{
	m_pTreeModel = NULL;		
	m_pTreeNodeRenderer = NULL;
}

CHelpDlg::~CHelpDlg(void)
{
	if( m_pTreeModel )
	{
		m_pTreeModel->clear();	
		delete m_pTreeModel;
		m_pTreeModel = NULL;
	}

	if( m_pTreeNodeRenderer )
	{
		delete m_pTreeNodeRenderer;
		m_pTreeNodeRenderer = NULL;
	}
	if( m_pTree )
	{
		delete m_pTree;
		m_pTree = NULL;
	}

	std::map< std::string, CHelpPage* >::iterator iter;
	CHelpPage* pPage;
	for( iter = m_PageList.begin(); iter != m_PageList.end(); )
	{
		pPage = iter->second;
		iter = m_PageList.erase( iter );
		delete pPage;
	}
}

bool CHelpDlg::LoadHelpContents( const char* szFileName , CTree* pTree )
{
	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( szFileName ) == false )
	{
		(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );
		return false;
	}

	/// RootNode 및 TreeModel 생성




	int iBufSize = pFileSystem->ReadPascalStringLength() + 1;
	char* pszBuf = new char[iBufSize];
	pFileSystem->ReadPascalString( pszBuf, iBufSize);///버젼
	delete []pszBuf;

	/// Tree Model 구성
	int iNodeCount = 0;
	pFileSystem->ReadInt32( &iNodeCount );
	if( iNodeCount <= 0 )
		return false;

	
	///RootNode구성 : 항상 첫번째에는 Root Node가 있는것으로 처리한다.
	iBufSize = pFileSystem->ReadPascalStringLength() + 1;
	pszBuf = new char[iBufSize*2]; /// UTF8->MBCS 로 변환을 위해 충분한 버퍼를 생성..
	pFileSystem->ReadPascalString( pszBuf, iBufSize);
	
	//-------------------------------------------------------------------------------
	/// UTF8 -> MBCS
	//-------------------------------------------------------------------------------
	CLocalizing::GetSingleton().UTF8ToMBCS( pszBuf, iBufSize*2 );


	CHelpPage* pRootPage = new CHelpPage( pszBuf );
	CTDefaultTreeNode* pRootNode = new CTDefaultTreeNode( pRootPage );
	m_pTreeModel		= new CTDefaultTreeModel( pRootNode );
	pTree->SetTreeModel( m_pTreeModel );
	m_PageList.insert( std::map< std::string, CHelpPage* >::value_type( pszBuf, pRootPage ));
	delete []pszBuf;

	pFileSystem->ReadInt32( &iNodeCount );
	for( int i = 0; i < iNodeCount; ++i )
		LoadNodeCollection(pFileSystem, pRootNode );

	/// Contents Load및 Node에 추가
	LoadPageList( pFileSystem, m_pTreeModel );
	
	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );


	return true;
}

bool CHelpDlg::LoadNodeCollection(  CFileSystem* pFileSystem, CTDefaultTreeNode* pNode )
{

	int iBufSize = pFileSystem->ReadPascalStringLength() + 1;
	char* pszBuf = new char[iBufSize*2]; /// UTF8->MBCS 로의 변환을 위해 충분한 버퍼 생성
	

	pFileSystem->ReadPascalString( pszBuf, iBufSize);

	//-------------------------------------------------------------------------------
	/// UTF8 -> MBCS
	//-------------------------------------------------------------------------------
	CLocalizing::GetSingleton().UTF8ToMBCS( pszBuf, iBufSize*2 );


	CHelpPage* pHelpPage = new CHelpPage( pszBuf );
	
	m_PageList.insert( std::map< std::string, CHelpPage* >::value_type( pszBuf, pHelpPage ));
	
	CTDefaultTreeNode* pChildNode = new CTDefaultTreeNode( pHelpPage );
	pNode->add( pChildNode );
	
	int iNodeCount = 0;
	pFileSystem->ReadInt32( &iNodeCount );
	
	for( int i = 0; i < iNodeCount; ++i )
		LoadNodeCollection(pFileSystem, pChildNode );

	delete []pszBuf;
	return true;
}

bool CHelpDlg::LoadPageList( CFileSystem* pFileSystem, CTDefaultTreeModel* pTreeModel )
{
	int iPageCount = 0;
	
	char* pszTitle;
	char* pszContent;
	int   iTitleSize = 0;
	int   iContentSize = 0;


	std::map< std::string, CHelpPage* >::iterator iter;
	pFileSystem->ReadInt32( &iPageCount );
	
	for( int i = 0; i < iPageCount; ++i )
	{

		iTitleSize = pFileSystem->ReadPascalStringLength() + 1;
		pszTitle = new char[ iTitleSize*2 ];
		pFileSystem->ReadPascalString( pszTitle,  iTitleSize );

		//-------------------------------------------------------------------------------
		/// UTF8 -> MBCS		
		CLocalizing::GetSingleton().UTF8ToMBCS( pszTitle, iTitleSize*2 );
		//-------------------------------------------------------------------------------

		iContentSize = pFileSystem->ReadPascalStringLength() + 1;
		pszContent   = new char[ iContentSize*2 ];
		pFileSystem->ReadPascalString( pszContent, iContentSize );

		//-------------------------------------------------------------------------------
		/// UTF8 -> MBCS		
		CLocalizing::GetSingleton().UTF8ToMBCS( pszContent, iContentSize*2 );
		//-------------------------------------------------------------------------------

		iter = m_PageList.find( pszTitle );
		if( iter != m_PageList.end() )
			iter->second->SetPageContents( pszContent );

		delete []pszTitle;
		delete []pszContent;

	}
	return true;
}

bool CHelpDlg::Create( const char* IDD )
{
	CTDialog::Create( IDD );
	m_pTree = new CTree;
	m_pTree->Init( CTRL_TREE, 15, 70, 175, 246 );
	m_pTree->SetOffset( 20, 40 );
	m_pTree->SetControlID( IID_TREE  );

	//-------------------------------------------------------------------------
	/// 각 언어에 맞는 헬프파일 선택
	//-------------------------------------------------------------------------
	STBDATA stbFiles;
	if( stbFiles.Load( "3DData\\STB\\List_Help.STB", true ) )
	{
		int iCurrentLang = CStringManager::GetSingleton().GetLanguageIDInGame( CLocalizing::GetSingleton().GetCurrentCharSet() );

		if( stbFiles.m_ppNAME[ iCurrentLang ] != NULL )
		{
			if( LoadHelpContents( stbFiles.m_ppNAME[ iCurrentLang ] , m_pTree ) )
			{
				m_pTreeNodeRenderer = new CTDefaultTreeNodeRenderer;
				
				int iModuleID  = IMAGE_RES_UI;
				int iGraphicID = CResourceMgr::GetInstance()->GetImageNID( iModuleID, "UI14_MINUS" );

				CTImage* pImage = CTControlMgr::GetInstance()->MakeImage( 100,0,0,9,9,iGraphicID, iModuleID );
				pImage->Show();
				m_pTreeNodeRenderer->setOpenImage( pImage );

				iGraphicID = CResourceMgr::GetInstance()->GetImageNID( iModuleID, "UI14_PLUS" );
				pImage = CTControlMgr::GetInstance()->MakeImage( 101,0,0,9,9,iGraphicID, iModuleID );
				pImage->Show();
				m_pTreeNodeRenderer->setCloseImage( pImage );



				iGraphicID = CResourceMgr::GetInstance()->GetImageNID( iModuleID, "UI14_CIRCLE_FILLED" );
				pImage = CTControlMgr::GetInstance()->MakeImage( 102,0,0,9,9,iGraphicID, iModuleID );
				m_pTreeNodeRenderer->setParentImage( pImage );

				iGraphicID = CResourceMgr::GetInstance()->GetImageNID( iModuleID, "UI14_CIRCLE" );
				pImage = CTControlMgr::GetInstance()->MakeImage( 103,0,0,9,9,iGraphicID, iModuleID );
				m_pTreeNodeRenderer->setLeafImage( pImage );





				m_pTreeNodeRenderer->Show();
				m_pTree->SetNodeRenderer( m_pTreeNodeRenderer );


				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				assert( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					CJContainer* pContainer = pPane->GetTabContainer( IID_TABLIST );
					assert( pContainer );
					if( pContainer )
					{
						pCtrl = pContainer->Find( IID_TREE_SCROLL );
						if( pCtrl )
						{
							CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
							pScrollBar->SetModel( m_pTree );
						}
						
						pContainer->Add( m_pTree );
					}
				}
			}		
			return true;
		}

		stbFiles.Free();
	}
	return false;
}

UINT CHelpDlg::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if( !IsVision() ) return 0;

	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process( uiMsg, wParam, lParam ))
	{
		switch( uiMsg )
		{
		case WM_LBUTTONDOWN:
			{
				if( iProcID == IID_TREE )
				{

					CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
					assert( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE );
					if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
					{
						CTabbedPane* pPane = (CTabbedPane*)pCtrl;
						CJContainer* pContainer = pPane->GetTabContainer( IID_TABLIST );
						assert( pContainer );
						if( pContainer )
						{
							pCtrl = pContainer->Find( IID_TREE );
							assert( pCtrl );
							if( pCtrl )
							{
								CTree* pTree = (CTree*)pCtrl;
								ITreeNode* pNode = pTree->GetSelectNode();
								if( pNode && pNode->isLeaf())
								{
									CTObject* pObject  = pNode->getUserObject();

									pContainer = pPane->GetTabContainer( IID_TABDESCRIPTION );
									assert( pContainer );
									if( pContainer )
									{
										pCtrl = pContainer->Find( IID_LIST_DESCRIPTION );
										assert( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX );
										if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
										{
											CTListBox* pList = (CTListBox*)pCtrl;
											pList->ClearText();
											CHelpPage* pHelpPage = (CHelpPage*)pObject;
											for( int i = 0;  i < pHelpPage->GetPageContentsCount(); ++i )
											{
												pList->AppendText( pHelpPage->GetPageContent( i ), D3DCOLOR_ARGB(255,0,0,0) );
											}
											
											pCtrl = pContainer->Find( IID_SCROLLBAR_DESCRIPTION );
											assert( pCtrl );
											if( pCtrl )
											{
												CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
												pScrollBar->SetValue(0);
											}
											pList->SetValue(0);
											pPane->SetSelectedIndex( IID_TABDESCRIPTION );
										}
									}
								}
							}
						}
					}
				}
				break;
			}
		case WM_LBUTTONUP:
			{
				switch( iProcID )
				{
				case IID_BTN_ICONIZE:
					g_itMGR.AddDialogIcon( 	GetDialogType() );
					break;
				case IID_BTN_CLOSE:
					Hide();
					break;
				default:
					break;
				}
			}
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CHelpDlg::Draw()
{
	if( IsVision() )
		CTDialog::Draw();
}
//*---------------------------------------------------------------------------------------------*/
CHelpPage::CHelpPage( char* pPageName, char* pPageContents )
{
	m_strPageName = std::string( (pPageName)? pPageName:"NULL" );
	SetPageContents( pPageContents );
}

CHelpPage::~CHelpPage()
{
	m_PageContents.clear();
}

void CHelpPage::SetPageContents( char* pszContent )
{ 
	m_PageContents.clear();
	///라인피드 검색해서 한줄씩 넣자.
	assert( pszContent );
	if( pszContent )
	{
		char* p = strtok( pszContent, "\n" );
		while( p != NULL )
		{
			m_PageContents.push_back( p );
			p = strtok( NULL,"\n" );
		}
	}
}
	
int CHelpPage::GetPageContentsCount()
{
	return m_PageContents.size();
}

const char* CHelpPage::GetPageContent( int i )
{
	assert( i >= 0 && i < (int)m_PageContents.size() );
	if( i >= 0 && i < (int)m_PageContents.size() )
		return m_PageContents[i].c_str();	

	return NULL;
}
//*---------------------------------------------------------------------------------------------*/