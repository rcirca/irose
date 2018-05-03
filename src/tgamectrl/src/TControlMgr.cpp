#include "StdAfx.h"
#include ".\tcontrolmgr.h"
#include "TButton.h"
#include "TPushButton.h"
#include "TCaption.h"
#include "TDialog.h"
#include "TEditBox.h"
#include "TImage.h"
#include "TListBox.h"
#include "TScrollBar.h"
#include "TScrollBox.h"
#include "TStatic.h"
#include "TRadioBox.h"
#include "TRadioButton.h"
#include "TComboBox.h"
#include "TMsgBox.h"
#include "ResourceMgr.h"
#include "tree.h"
#include "TGuage.h"
#include "jlistbox.h"
#include "jcombobox.h"
#include "JContainer.h"
#include "TabbedPane.h"
#include "ZListBox.h"
#include "TCheckBox.h"
#include "TPane.h"
#include "JTable.h"

//#include "CToolTip.h"
#include <TChar.h>
#include <string>


CTControlMgr* CTControlMgr::s_pInstance = NULL;

CTControlMgr::CTControlMgr(void)
{
	m_pDrawMgr  = NULL;
	m_pSoundMgr = NULL;

	m_uiCodePage = GetACP();
	m_nKeyboardInputType = INPUTTYPE_AUTOENTER;
}

CTControlMgr::~CTControlMgr(void)
{
}

CTControlMgr* CTControlMgr::GetInstance()
{
	if( s_pInstance == NULL)
		s_pInstance = new CTControlMgr;

	return s_pInstance;
}

void CTControlMgr::Destroy()
{
	if( s_pInstance )
	{
		delete s_pInstance;
		s_pInstance = NULL;
	}
}

CTButton*		CTControlMgr::MakeButton(int ID,int iScrX, int iScrY, int iWidth, int iHeight, int iNormalGraphicID, int iOverGraphicID, int iDownGraphicID ,int iModuleID )
{
	CTButton* pButton = new CTButton;
	pButton->Create(iScrX, iScrY, iWidth, iHeight, iNormalGraphicID, iOverGraphicID, iDownGraphicID ,iModuleID);
	pButton->SetControlID( ID );
	return pButton;
}

CTPushButton*		CTControlMgr::MakePushButton(int ID,int iScrX, int iScrY, int iWidth, int iHeight, int iNormalGraphicID, int iOverGraphicID, int iDownGraphicID ,int iModuleID )
{
	CTPushButton* pButton = new CTPushButton;
	pButton->Create(iScrX, iScrY, iWidth, iHeight, iNormalGraphicID, iOverGraphicID, iDownGraphicID ,iModuleID);
	pButton->SetControlID( ID );
	return pButton;
}

CTCaption*		CTControlMgr::MakeCaption(int ID,int iScrX, int iScrY, int iWidth, int iHeight)
{
	CTCaption* pCaption = new CTCaption;
	pCaption->Create(iScrX, iScrY, iWidth, iHeight);
	pCaption->SetControlID( ID );
	return pCaption;
}

CTDialog*		CTControlMgr::MakeDialog(int ID,int iScrX, int iScrY, int iWidth, int iHeight)
{
	CTDialog* pDialog = new CTDialog;
	pDialog->Create( iScrX, iScrY, iWidth, iHeight );
	pDialog->SetControlID( ID );
	return pDialog;

}

CTEditBox*		CTControlMgr::MakeEditBox(int ID,int iScrX, int iScrY, int iWidth, int iHeight, int iCharWidth )
{
	CTEditBox* pEditBox = new CTEditBox;
	pEditBox->Create( iScrX, iScrY, iWidth, iHeight ,iCharWidth );
	pEditBox->SetControlID( ID );
	return pEditBox;
}

CTImage*		CTControlMgr::MakeImage(int ID,int iScrX, int iScrY, int iWidth, int iHeight, int iGraphicID,int iModuleID )
{
	CTImage* pImage = new CTImage;
	pImage->Create( iScrX, iScrY, iWidth, iHeight, iGraphicID ,iModuleID );
	pImage->SetControlID( ID );
	return pImage;
}

CTListBox*		CTControlMgr::MakeListBox(int ID,int iScrX, int iScrY, int iWidth, int iHeight, int iExtent ,int iCharWidth, int iCharHeight)
{
	CTListBox* pListBox = new CTListBox;
	pListBox->Create( iScrX, iScrY, iWidth, iHeight ,iExtent ,iCharWidth, iCharHeight );
	pListBox->SetControlID( ID );
	return pListBox;
}

CJListBox*		CTControlMgr::MakeJListBox(int ID,int iScrX, int iScrY, int iWidth, int iHeight , int iItemHeight )
{
	CJListBox* pListBox = new CJListBox;
	pListBox->Create( iScrX, iScrY, iWidth, iHeight , iItemHeight );
	pListBox->SetControlID( ID );
	return pListBox;
}

CTScrollBar*	CTControlMgr::MakeScrollBar(int ID,int iScrX, int iScrY, int iWidth, int iHeight , int iType )
{
	CTScrollBar* pScrollBar = new CTScrollBar;
	pScrollBar->Create(iScrX,iScrY,iWidth,iHeight , iType );
	pScrollBar->SetControlID( ID );
	return pScrollBar;
}

CTScrollBox*	CTControlMgr::MakeScrollBox(int ID, int iWidth, int iHeight, int iGraphicID ,int iModuleID, int iType )
{
	CTScrollBox* pScrollBox = new CTScrollBox;
	pScrollBox->Create( iWidth, iHeight, iGraphicID ,iModuleID , iType );
	pScrollBox->SetControlID(ID );
	return pScrollBox;
}

CTStatic*		CTControlMgr::MakeStatic(int ID, int iScrX, int iScrY, int iWidth, int iHeight)
{
	CTStatic* pStatic = new CTStatic;
	pStatic->Create( iScrX, iScrY, iWidth, iHeight );
	pStatic->SetControlID(ID);
	return pStatic;
}

CTRadioBox*		CTControlMgr::MakeRadioBox(int ID)
{
	CTRadioBox* pRadioBox = new CTRadioBox;
	pRadioBox->SetControlID( ID );
	return pRadioBox;
}

CTRadioButton*  CTControlMgr::MakeRadioButton(int ID,int iScrX, int iScrY, int iWidth, int iHeight, int iNormalGraphicID, int iOverGraphicID, int iDownGraphicID ,int iModuleID )
{
	CTRadioButton* pRadioButton = new CTRadioButton;
	pRadioButton->Create( iScrX, iScrY, iWidth, iHeight, iNormalGraphicID, iOverGraphicID, iDownGraphicID ,iModuleID );
	pRadioButton->SetControlID( ID );
	return pRadioButton;
}
CTComboBox*		CTControlMgr::MakeComboBox(int ID, int iScrX, int iScrY, int iWidth, int iHeight )
{
	CTComboBox* pComboBox = new CTComboBox;
	pComboBox->Create( iScrX, iScrY, iWidth, iHeight );
	pComboBox->SetControlID( ID );
	return pComboBox;
}

CJComboBox*		CTControlMgr::MakeJComboBox(int ID, int iScrX, int iScrY, int iWidth, int iHeight )
{
	CJComboBox* pComboBox = new CJComboBox;
	pComboBox->Create( iScrX, iScrY, iWidth, iHeight );
	pComboBox->SetControlID( ID );
	return pComboBox;
}

/*
bool			CTControlMgr::ReleaseCtrl( unsigned int iID )
{
	WINCTRL_MAP_ITOR iter;
	iter = m_mapControls.find( iID );
	if( iter == m_mapControls.end() )
		return false;

	CWinCtrl* pCtrl = iter->second;
	delete pCtrl;
	m_mapControls.erase( iter );

	return true;
}
*/
/*
CWinCtrl*		CTControlMgr::Find( unsigned int iID )
{
	WINCTRL_MAP_ITOR iter;
	iter = m_mapControls.find( iID );
	if( iter == m_mapControls.end() )
		return NULL;

	return iter->second;
}
*/
bool CTControlMgr::MakeMsgBoxByXML( char* szIDD, CTMsgBox* pBase )
{
	MSXML::IXMLDOMDocument*		document = NULL;
	MSXML::IXMLDOMElement*		element = NULL;	
	MSXML::IXMLDOMNodePtr		pNode = NULL;
	MSXML::IXMLDOMNodeList*		childList = NULL;
	MSXML::IXMLDOMNamedNodeMap*	pNamedNodeMap = NULL;
//	MSXML::IXMLDOMParseError*	parseError = NULL;

	HRESULT hr = CoCreateInstance(MSXML::CLSID_DOMDocument, NULL, 
		CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, 
		MSXML::IID_IXMLDOMDocument, (LPVOID*)&document);

	bool bRet = true;

	if (!document) {
		return false;
	}


	try{
		BSTR nodeName;
		_variant_t AttributeValue;

		int iScrX, iScrY, iWidth, iHeight,iModal;
		std::string TempString;
		char szNodeName[256];
		char szFileName[512];
		sprintf( szFileName,"3DData\\CONTROL\\XML\\%s.XML",szIDD );

		document->put_async(VARIANT_FALSE);
		/*VARIANT_BOOL varOkay = */document->load( szFileName );

		hr = document->get_documentElement(&element);///ROOT
		if( FAILED( hr )){
			throw 2;
		}


		hr = element->get_attributes( &pNamedNodeMap );

		iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X");
		iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y");
		iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH");
		iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT");
		iModal = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "MODAL");
		pBase->Init( CTRL_MSGBOX, iScrX, iScrY, iWidth, iHeight );

		if( iModal > 0)
			pBase->SetModal();
		else
			pBase->SetModeless();


		pNamedNodeMap->Release();
		pNamedNodeMap = NULL;

		element->get_childNodes(&childList);


		long iSize = 0;
		(childList)->get_length(&iSize);

		CTImage* pImageTop		= NULL;
		CTImage* pImageMiddle	= NULL;
		CTImage* pImageBottom	= NULL;
		CTListBox* pListBox		= NULL;
		CTButton* pButtonOk		= NULL;
		CTButton* pButtonCancel	= NULL;
		CTCaption* pCaption		= NULL;
		for( int i = 0 ; i < iSize; i++ )
		{

			pNode = (childList)->Getitem( i );
			pNode->get_nodeName( &nodeName );
			sprintf( szNodeName,"%s",(char*)(_bstr_t)nodeName );
			if( strcmp( szNodeName, "IMAGETOP" ) == 0 )
				pImageTop = MakeImageByXML2( pNode );
			else if( strcmp( szNodeName,"IMAGEMIDDLE" ) == 0 )
				pImageMiddle = MakeImageByXML2( pNode );
			else if( strcmp( szNodeName,  "IMAGEBOTTOM" ) == 0 )
				pImageBottom = MakeImageByXML2( pNode );
			else if( strcmp( szNodeName, "BUTTONOK" ) == 0 )
				pButtonOk = MakeButtonByXML2( pNode );
			else if( strcmp( szNodeName, "BUTTONCANCEL") == 0 )
				pButtonCancel = MakeButtonByXML2( pNode );
			else if( strcmp( szNodeName, "CAPTION" ) == 0 )
				pCaption = MakeCaptionByXML2( pNode );
		}

		pBase->SetButtons( pButtonOk, pButtonCancel );
		pBase->SetImages( pImageTop, pImageMiddle, pImageBottom );
		pBase->SetCaption( pCaption );

	}catch( ... )
	{
		bRet = false;
		char szMsg[255];
		sprintf("XML로 MsgBox생성중 에러 발생:%s",szIDD );
		MessageBox( NULL,szMsg,"Error",MB_OK );
	}

	if( element )
	{
		element->Release() ;
		element = NULL;
	}

	if( pNamedNodeMap )
	{
		pNamedNodeMap->Release();
		pNamedNodeMap = NULL;
	}

	if( document )
	{
		document->Release();
		document = NULL;
	}


	return bRet;
}
//////////////////////////////////////////////////////////////////////////////////////
/// 2003 / 12 / 1 / 최종진
/// 주의)
///		1. 이 operation안에서 Initialize와 CoUninitialize를 사용하면 return시 에러발생
///			:	IXMLDOMNodePtr이 스마트 포인터여서 생긴 문제였음. scope를 벗어날때 삭제되기 때문에 발생함.
///				IXMLDoMNode(raw pointer)로 수정하여 문제 해결함 => 다시 문제 발생 :이름 못 가져옴
//////////////////////////////////////////////////////////////////////////////////////
bool CTControlMgr::MakeDialogByXML( const char* IDD, CTDialog* pBase )
{

	MSXML::IXMLDOMDocument*		document = NULL;
	MSXML::IXMLDOMElement*		element = NULL;	
	MSXML::IXMLDOMNodePtr		pNode = NULL;
	MSXML::IXMLDOMNodeList*		childList = NULL;
	MSXML::IXMLDOMNamedNodeMap*	pNamedNodeMap = NULL;
	//MSXML::IXMLDOMParseError*	parseError = NULL;

	HRESULT hr = CoCreateInstance(MSXML::CLSID_DOMDocument, NULL, 
		CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, 
		MSXML::IID_IXMLDOMDocument, (LPVOID*)&document);

	bool bRet = true;

	if (!document) {
		return false;
	}

	try{
		char szBuf[512];
		char szNodeName[256];
		BSTR nodeName;
		_variant_t AttributeValue;

		int iScrX, iScrY, iWidth, iHeight, iModal, iShowSoundID, iHideSoundID;
		int iDefaultPosX, iDefaultPosY, iDefaultAdjustPosX, iDefaultAdjustPosY,iDefaultVisible;
//		std::string TempString;
//		std::string sFileName = "3DDATA\\CONTROL\\XML\\";

//		sFileName.append(IDD);
//		sFileName.append(".XML");
		sprintf( szBuf, "3DDATA\\CONTROL\\XML\\%s.XML", IDD );
		
		document->put_async(VARIANT_FALSE);

		/*VARIANT_BOOL varOkay = */document->load( szBuf );

		hr = document->get_documentElement(&element);///ROOT
		if( FAILED( hr )){
			throw 2;
		}


		hr = element->get_attributes( &pNamedNodeMap );


		iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X");
		iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y");
		iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH");
		iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT");
		iModal = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "MODAL");
		iShowSoundID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "SHOWSID");
		iHideSoundID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HIDESID");
		int iExtent  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "EXTENT");
		if( iExtent > 1 )
			pBase->SetExtent( iExtent );

		///디폴트 위치 
		iDefaultPosX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "DEFAULT_X");
		iDefaultPosY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "DEFAULT_Y");
		iDefaultAdjustPosX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ADJUST_X");
		iDefaultAdjustPosY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ADJUST_Y");

		iDefaultVisible = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "DEFAULT_VISIBLE");

		int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID");	

		std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );
		pBase->SetControlName( strCtrlName.c_str() );

		
		if( iID > 0 )
			pBase->SetControlID( iID );
		else
			pBase->SetControlID( 1 );


		pBase->Init( CTRL_DIALOG, iScrX, iScrY, iWidth, iHeight );
		
		if( iModal > 0)
			pBase->SetModal();

		if( iShowSoundID > 0 )
			pBase->SetSoundShowID( iShowSoundID );

		if( iHideSoundID > 0 )
			pBase->SetSoundHideID( iHideSoundID );

		pBase->SetDefaultPosX( iDefaultPosX );
		pBase->SetDefaultPosY( iDefaultPosY );
		pBase->SetDefaultAdjustPosX( iDefaultAdjustPosX );
		pBase->SetDefaultAdjustPosY( iDefaultAdjustPosY );

		
		if( iDefaultVisible > 0 )
			pBase->SetDefaultVisible( true );
		
		pNamedNodeMap->Release();
		pNamedNodeMap = NULL;

		element->get_childNodes(&childList);


		long iSize = 0;
		(childList)->get_length(&iSize);

		for( int i = 0 ; i < iSize; i++ )
		{
			pNode = (childList)->Getitem( i );
			pNode->get_nodeName( &nodeName );

			sprintf(szNodeName,"%s",(char*)(_bstr_t)nodeName );

			if( strcmp( szNodeName,"IMAGE" ) == 0 )
				MakeImageByXML( pNode, pBase );				
			else if( strcmp( szNodeName,"BUTTON" ) == 0 )
				MakeButtonByXML( pNode, pBase );
			else if( strcmp( szNodeName, "PUSHBUTTON" ) == 0 )
				MakePushButtonByXML( pNode, pBase );
			else if( strcmp( szNodeName, "CAPTION" ) == 0 )
				MakeCaptionByXML( pNode, pBase );
			else if( strcmp( szNodeName,"EDITBOX" ) == 0 )
				MakeEditBoxByXML( pNode, pBase );
			else if( strcmp( szNodeName, "LISTBOX" ) == 0 )
				MakeListBoxByXML( pNode, pBase );
			else if( strcmp( szNodeName, "STATIC" ) == 0 )
				MakeStaticByXML( pNode, pBase );
			else if( strcmp( szNodeName, "SCROLLBAR" ) == 0 )
				MakeScrollBarByXML( pNode, pBase );
			else if( strcmp( szNodeName, "RADIOBOX" ) == 0 )
				MakeRadioBoxByXML( pNode, pBase );
			else if( strcmp( szNodeName, "RADIOBUTTON" ) == 0 )
				MakeRadioButtonByXML( pNode, pBase );
			else if( strcmp( szNodeName,"COMBOBOX" ) == 0 )
				MakeComboBoxByXML( pNode, pBase );
			else if( strcmp( szNodeName,"JCOMBOBOX" ) == 0 )
				MakeJComboBoxByXML( pNode, pBase );
			else if( strcmp( szNodeName,"JLISTBOX" ) == 0 )
				MakeJListBoxByXML( pNode, pBase );
			else if( strcmp( szNodeName, "GUAGE") == 0 )
				AddGuageByXML( pNode, pBase );
			else if( strcmp( szNodeName, "TABBEDPANE" ) == 0 )
				AddTabbedPane( pNode, pBase );
			else if( strcmp( szNodeName,"BACKGROUNDIMAGE" ) == 0 )
			{
				CTImage* pImage = MakeImageByXML2( pNode );
				if( pImage )
					pBase->SetImage( pImage );
			}
			else if( strcmp( szNodeName,"ZLISTBOX" ) == 0 )
			{
				CZListBox* pListBox = MakeZListBoxByXML( pNode );
				if( pListBox )
					pBase->Add( pListBox );
			}
			else if( strcmp( szNodeName,"CHECKBOX" ) == 0 )
			{
				CTCheckBox* pCheckBox = MakeCheckBoxByXML( pNode );
				if( pCheckBox )
					pBase->Add( pCheckBox );
			}
			else if( strcmp( szNodeName,"PANE" ) == 0 )
			{
				if( CTPane* pPane = MakePaneByXML( pNode ) )
					pBase->Add( pPane );
			}
			else if( strcmp( szNodeName,"TABLE" ) == 0 )
			{
				if( CJTable* pTable = MakeTableByXML( pNode ) )
					pBase->Add( pTable );
					
			}

		}

	}catch( ... )
	{
		bRet = false;
		char szMsg[255];
		sprintf(szMsg,"XML로 UI생성중 에러 발생:%s",IDD );
		MessageBox( NULL,szMsg,"Error",MB_OK );
	}

	if( element )
	{
		element->Release() ;
		element = NULL;
	}

	if( pNamedNodeMap )
	{
		pNamedNodeMap->Release();
		pNamedNodeMap = NULL;
	}

	if( document )
	{
		document->Release();
		document = NULL;
	}


	return true;
}

int CTControlMgr::GetNodeValueFromNamedNodeMapByName(MSXML::IXMLDOMNamedNodeMap* pNodeMap ,char* name)
{
	VARIANT	value;
	string TempString;	
	char szBuf[256];
	MSXML::IXMLDOMNodePtr		Attribute = NULL;
	_bstr_t bstrName = name;
	
	Attribute = pNodeMap->getNamedItem( bstrName );	
	
	if( Attribute == NULL )
		return 0;

	Attribute->get_nodeValue( &value );

	sprintf( szBuf,"%s",(char*)(_bstr_t)value );

	if( szBuf == NULL )
		return 0;

	if( strlen(szBuf) < 1 )
		return 0;
	
	int iRet = atoi( szBuf );


	return iRet;
}

float CTControlMgr::GetNodeFloatValueFromNamedNodeMapByName(MSXML::IXMLDOMNamedNodeMap* pNodeMap , char* name)
{
	VARIANT	value;
	string TempString;	
	char szBuf[256];
	MSXML::IXMLDOMNodePtr		Attribute = NULL;
	_bstr_t bstrName = name;
	
	Attribute = pNodeMap->getNamedItem( bstrName );	
	
	if( Attribute == NULL )
		return 0;

	Attribute->get_nodeValue( &value );

	sprintf( szBuf,"%s",(char*)(_bstr_t)value );

	if( szBuf == NULL )
		return 0;

	if( strlen(szBuf) < 1 )
		return 0;
	
	float fRet = (float)atof( szBuf );

	return fRet;
}

std::string CTControlMgr::GetNodeStringValueFromNamedNodeMapByName(MSXML::IXMLDOMNamedNodeMap* pNodeMap , char* name)
{
	VARIANT	value;
	string TempString;	
	char szBuf[512];
	MSXML::IXMLDOMNodePtr		Attribute = NULL;
	_bstr_t bstrName = name;
	
	Attribute = pNodeMap->getNamedItem( bstrName );	
	
	if( Attribute == NULL )
		return "";

	Attribute->get_nodeValue( &value );

	sprintf( szBuf,"%s",(char*)(_bstr_t)value );
	TempString = szBuf;

	return TempString;
}
CTCheckBox* CTControlMgr::MakeCheckBoxByXML( MSXML::IXMLDOMNodePtr pNode )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );

	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );	

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );

	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );

	int iModuleID    = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "MODULEID" );
	std::string sCheckGID	= GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "CHECKGID" );
	std::string sUncheckGID = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "UNCHECKGID" );

	CResourceMgr* pMgr = CResourceMgr::GetInstance();
	int iCheckGID	= pMgr->GetImageNID(iModuleID, sCheckGID.c_str());
	int iUncheckGID = pMgr->GetImageNID(iModuleID, sUncheckGID.c_str());

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;

	if( iCheckGID == -1 )
	{
		_RPTF1( _CRT_ASSERT, "%s is not found", sCheckGID.c_str());
		return NULL;
	}

	//if( iUncheckGID == -1 )
	//	_RPT3( _CRT_ASSERT, "%s is not found %s %s", sUncheckGID.c_str() ,__FILE__, __LINE__);


	CTCheckBox* pCheckBox = new CTCheckBox;
	if( pCheckBox )
	{
		pCheckBox->Create( iScrX, iScrY, iWidth, iHeight, iModuleID, iCheckGID, iUncheckGID );
		pCheckBox->SetControlID( iID );
		pCheckBox->SetControlName( strCtrlName.c_str() );
		pCheckBox->SetOffset( iOffsetX, iOffsetY );
	}
	return pCheckBox;
}
CTButton* CTControlMgr::MakeButtonByXML2( MSXML::IXMLDOMNodePtr pNode)
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );

	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );

	int iModuleID    = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "MODULEID" );
	std::string sNormalGID  = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NORMALGID" );
	std::string sOverGID    = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "OVERGID" );
	std::string sDownGID    = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "DOWNGID" );
	std::string sBlinkGID	= GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "BLINKGID" );

	std::string strDisableGID = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "DISABLEGID" );
	

	CResourceMgr* pMgr = CResourceMgr::GetInstance();
	int iNormalGID	= pMgr->GetImageNID(iModuleID, sNormalGID.c_str());
	int iOverGID    = pMgr->GetImageNID(iModuleID, sOverGID.c_str());
	int iDownGID	= pMgr->GetImageNID(iModuleID, sDownGID.c_str());
	int iBlinkGID   = pMgr->GetImageNID(iModuleID, sBlinkGID.c_str());


	int iDisableGID = pMgr->GetImageNID(iModuleID, strDisableGID.c_str());

	int iOverSoundID	= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OVERSID" );
	int iClickSoundID	= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "CLICKSID" );
	int iNoImage		= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "NOIMAGE" );

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;

	if( iNoImage == -1 && (iDownGID == -1 || iOverGID == -1 || iNormalGID == -1 ))
	{
		if( iNormalGID == -1)
		{
			_RPTF1( _CRT_ASSERT, "%s is not found", sNormalGID.c_str());
			return NULL;
		}


		if( iOverGID == -1 )
		{
			_RPTF1( _CRT_ASSERT, "%s is not found", sOverGID.c_str());
			return NULL;
		}


		if( iDownGID == -1 ) 
		{
			_RPTF1( _CRT_ASSERT, "%s is not found", sDownGID.c_str());
			return NULL;
		}

		return NULL;
	}
	
	CTButton* pBtn = MakeButton(iID, iScrX, iScrY, iWidth, iHeight, iNormalGID,iOverGID, iDownGID,iModuleID );

	if( pBtn )
	{
		if( iNoImage > 0 )
			pBtn->SetNoImage();
		if( iOverSoundID > 0 )
			pBtn->SetSoundOverID( iOverSoundID );
		if( iClickSoundID > 0) 
			pBtn->SetSoundClickID( iClickSoundID );
		if( iDisableGID > 0 )
			pBtn->SetDisableGrahicID( iDisableGID );

		if( iBlinkGID > 0 )
			pBtn->SetBlinkGid( iBlinkGID );

		pBtn->SetOffset( iOffsetX, iOffsetY );

		pBtn->SetControlName( strCtrlName.c_str() );
	}
	return pBtn;

}

CTPushButton* CTControlMgr::MakePushButtonByXML2( MSXML::IXMLDOMNodePtr pNode)
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );

	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );

	int iModuleID    = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "MODULEID" );
	std::string sNormalGID  = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NORMALGID" );
	std::string sOverGID    = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "OVERGID" );
	std::string sDownGID    = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "DOWNGID" );

	
	CResourceMgr* pMgr = CResourceMgr::GetInstance();
	int iNormalGID	= pMgr->GetImageNID(iModuleID, sNormalGID.c_str());
	int iOverGID    = pMgr->GetImageNID(iModuleID, sOverGID.c_str());
	int iDownGID	= pMgr->GetImageNID(iModuleID, sDownGID.c_str());


	int iOverSoundID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OVERSID" );
	int iClickSoundID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "CLICKSID" );


	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;

	if( iDownGID == -1 || iOverGID == -1 || iNormalGID == -1 )
		return NULL;
	
	CTPushButton* pBtn = MakePushButton(iID, iScrX, iScrY, iWidth, iHeight, iNormalGID,iOverGID, iDownGID,iModuleID );

	if( pBtn )
	{
		if( iOverSoundID > 0 )
			pBtn->SetSoundOverID( iOverSoundID );
		if( iClickSoundID > 0) 
			pBtn->SetSoundClickID( iClickSoundID );
		pBtn->SetOffset( iOffsetX, iOffsetY );

		pBtn->SetControlName( strCtrlName.c_str() );
	}
	return pBtn;
}

void CTControlMgr::MakeButtonByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase)
{

	CTButton* pButton = MakeButtonByXML2( pNode );

	if( pButton )
		pBase->Add( pButton );

}

void CTControlMgr::MakePushButtonByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase)
{

	CTPushButton* pButton = MakePushButtonByXML2( pNode );

	if( pButton )
		pBase->Add( pButton );

}


CTImage* CTControlMgr::MakeImageByXML2( MSXML::IXMLDOMNodePtr pNode )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );

	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );

	int iModuleID    = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "MODULEID" );
	int iAlphaValue  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ALPHAVALUE" );

	float fScaleWidth  = GetNodeFloatValueFromNamedNodeMapByName(pNamedNodeMap, "SCALEWIDTH" );
	float fScaleHeight = GetNodeFloatValueFromNamedNodeMapByName(pNamedNodeMap, "SCALEHEIGHT" );

	int iSizeFit = GetNodeFloatValueFromNamedNodeMapByName(pNamedNodeMap, "SIZEFIT" );

	std::string sGID = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "GID" );

	CResourceMgr* pRMgr = CResourceMgr::GetInstance();
	int iGID    = pRMgr->GetImageNID( iModuleID, sGID.c_str() );
	
	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;
	
	if( sGID.size() && iGID == -1 )
	{
		_RPTF1( _CRT_ASSERT, "%s is not Found", sGID.c_str() );
		return NULL;
	}

	CTImage* pImage = MakeImage( iID, iScrX, iScrY, iWidth, iHeight, iGID,iModuleID );
	if( pImage )
	{
		pImage->SetOffset( iOffsetX, iOffsetY );
		pImage->SetAlphaValue( (BYTE)iAlphaValue );
		pImage->SetScaleHeight( fScaleHeight );
		pImage->SetScaleWidth( fScaleWidth );
		pImage->SetControlName( strCtrlName.c_str() );
		pImage->SetSizeFit( iSizeFit );
	}
	return pImage;

}
void CTControlMgr::MakeImageByXML(MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase)
{
	CTImage* pImage = MakeImageByXML2( pNode );
	if( pImage )
		pBase->Add( pImage );
}
CTCaption* CTControlMgr::MakeCaptionByXML2( MSXML::IXMLDOMNodePtr pNode )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;

	CTCaption* pCaption = MakeCaption(iID, iScrX, iScrY, iWidth, iHeight );
	pCaption->SetControlName( strCtrlName.c_str() );


	BSTR nodeName;

	MSXML::IXMLDOMNodeList *childList = NULL;
	MSXML::IXMLDOMNodePtr	pChildNode = NULL;	
	pNode->get_childNodes(&childList);
	long iSize = 0;
	(childList)->get_length(&iSize);
	char szNodeName[256];
	CTButton* pButton;
	CTImage*  pImage;
	for( int i = 0; i < iSize; i++ )
	{
		pChildNode = (childList)->Getitem( i );
		pChildNode->get_nodeName( &nodeName );
		sprintf( szNodeName,"%s",(char*)(_bstr_t)nodeName );
		if( strcmp( szNodeName, "ICONBUTTON" ) == 0 )
		{
			pButton = MakeButtonByXML2( pChildNode );
			if( pButton )
				pCaption->SetIconButton( pButton );
		}
		else if( strcmp( szNodeName, "CLOSEBUTTON" ) == 0 )
		{
			pButton = MakeButtonByXML2( pChildNode );
			if( pButton )
				pCaption->SetCloseButton( pButton );
		}
		else if( strcmp( szNodeName, "BGIMAGE" ) == 0 )
		{
			pImage = MakeImageByXML2( pChildNode );
			if( pImage )
				pCaption->SetImage( pImage );
		}
		else if( strcmp( szNodeName, "ICONIMAGE" ) == 0 )
		{
			pImage = MakeImageByXML2( pChildNode );
			if( pImage )
				pCaption->SetIcon( pImage );
		}
	}

	return pCaption;
}

void CTControlMgr::MakeCaptionByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase)
{
	CTCaption* pCaption = MakeCaptionByXML2( pNode );
	if( pCaption )
		pBase->SetCaption( pCaption );
}

void CTControlMgr::MakeStaticByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );

	CTStatic* pStatic = MakeStatic(iID, iScrX, iScrY, iWidth, iHeight );
	if( pStatic )
	{
		pBase->Add( pStatic );
		pStatic->SetControlName( strCtrlName.c_str() );
	}


	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;

}

CTEditBox* CTControlMgr::MakeEditBoxByXML2( MSXML::IXMLDOMNodePtr pNode )
{
	CTEditBox* pEditBox = NULL;
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );

	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );

	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );

	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );
	int iCharWidth  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "CHARWIDTH" );
	int iCharHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "CHARHEIGHT" );
	int iNumber = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "NUMBER" );
	int iLimitText = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "LIMITTEXT" );
	int iPassword   = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "PASSWORD" );
	int iHideCursor = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HIDECURSOR" );
	int iTextAlign  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "TEXTALIGN" );
	int iMultiline  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "MULTILINE" );
	int iTextColor	= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "TEXTCOLOR" );
	
	pEditBox = MakeEditBox( iID, iScrX, iScrY, iWidth, iHeight ,iCharWidth );
	
	if( pEditBox )
	{
		if( iCharHeight )
			pEditBox->SetCharHeight( iCharHeight );

		pEditBox->SetTextColor( iTextColor );

		if( iMultiline )
			pEditBox->SetMultiline( true );

		if( iNumber )
			pEditBox->SetType( CTEditBox::TES_NUMBER );

		if( iPassword )
			pEditBox->AddType( CTEditBox::TES_PASSWORD );

		if( iHideCursor )
			pEditBox->AddType( CTEditBox::TES_HIDECURSOR );

		if( iTextAlign )
			pEditBox->SetTextAlign( iTextAlign );

		if( iLimitText > 0 )
			pEditBox->SetLimitText( iLimitText );

		pEditBox->SetOffset( iOffsetX, iOffsetY );
		pEditBox->SetControlName( strCtrlName.c_str() );
	}

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;
	
	return pEditBox;
}

void CTControlMgr::MakeEditBoxByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase)
{
	CTEditBox* pEditBox = MakeEditBoxByXML2( pNode );
	if( pEditBox )
		pBase->Add( pEditBox );
}

CTListBox*	CTControlMgr::MakeListBoxByXML2( MSXML::IXMLDOMNodePtr pNode )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );
	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );
	int iExtent = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "EXTENT" );
	int iLineSpace = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "LINESPACE" );
	int iSelectable = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "SELECTABLE" );
	int iCharWidth  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "CHARWIDTH" );
	int iCharHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "CHARHEIGHT" );
	int iMaxSize    = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "MAXSIZE" );
	int iOwnerDraw  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OWNERDRAW" );
	int iFont		= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "FONT" );

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;


	CTListBox* pListBox = MakeListBox(iID, iScrX, iScrY, iWidth, iHeight ,iExtent, iCharWidth, iCharHeight);

	if( pListBox )
	{
		pListBox->SetMaxSize( iMaxSize );
		pListBox->SetLineSpace( (short) iLineSpace );
		if( iSelectable )
			pListBox->SetSelectable( true );

		if( iOwnerDraw )
			pListBox->SetOwnerDraw( true );

		pListBox->SetOffset( iOffsetX, iOffsetY );
		pListBox->SetFont( iFont );

		pListBox->SetControlName( strCtrlName.c_str() );

	}
	return pListBox;

}

CZListBox*	CTControlMgr::MakeZListBoxByXML( MSXML::IXMLDOMNodePtr pNode )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );
	int iExtent  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "EXTENT" );
	int iWidth   = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );
	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;

	CZListBox* pListBox = new CZListBox;


	if( pListBox )
	{
		pListBox->SetPosition( iScrX, iScrY );
		pListBox->SetOffset( iOffsetX, iOffsetY );
		pListBox->SetExtent( iExtent );
		pListBox->SetControlID( iID );
		pListBox->SetHeight( iHeight );
		pListBox->SetWidth( iWidth );
		pListBox->SetControlName( strCtrlName.c_str() );
	}
	return pListBox;
}
CJListBox*	CTControlMgr::MakeJListBoxByXML2( MSXML::IXMLDOMNodePtr pNode )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );
	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );
	int iItemHeight  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ITEMHEIGHT" );
	int iOwnerDraw  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OWNERDRAW" );
	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;


	CJListBox* pListBox = MakeJListBox(iID, iScrX, iScrY, iWidth, iHeight , iItemHeight );

	if( pListBox )
	{
		if( iOwnerDraw )
			pListBox->SetOwnerDraw( true );

		pListBox->SetOffset( iOffsetX, iOffsetY );
		pListBox->SetControlName( strCtrlName.c_str() );

	}
	return pListBox;

}

void CTControlMgr::MakeListBoxByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase )
{
	CTListBox* pListBox = MakeListBoxByXML2( pNode );
	if( pListBox )
		pBase->Add( pListBox );
}

void CTControlMgr::MakeJListBoxByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase )
{
	CJListBox* pListBox = MakeJListBoxByXML2( pNode );
	if( pListBox )
		pBase->Add( pListBox );
}
CTComboBox*	CTControlMgr::MakeComboBoxByXML2( MSXML::IXMLDOMNodePtr pNode )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;
	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);

	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );
	int iOwnerDraw  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OWNERDRAW" );
	int iOffsetX	= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY	= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;
	CTComboBox* pComboBox = MakeComboBox(iID, iScrX, iScrY, iWidth, iHeight );

	if( pComboBox == NULL )
	{
		return NULL;
	}

	pComboBox->SetControlName( strCtrlName.c_str() );

	if( iOwnerDraw )
		pComboBox->SetOwnerDraw( true );

	BSTR nodeName;
	char szNodeName[256];
	MSXML::IXMLDOMNodeList *childList = NULL;
	MSXML::IXMLDOMNodePtr	pChildNode = NULL;	
	pNode->get_childNodes(&childList);
	long iSize = 0;
	(childList)->get_length(&iSize);


	//CTButton*	pButton			= NULL;
	CTImage*	pImageTop		= NULL;
	CTImage*	pImageMiddle	= NULL;
	CTImage*	pImageBottom	= NULL;
	//CTListBox*	pListBox		= NULL;
	

	for( int i = 0; i < iSize; i++ )
	{
		pChildNode = (childList)->Getitem( i );
		pChildNode->get_nodeName( &nodeName );
		sprintf( szNodeName,"%s",(char*)(_bstr_t)nodeName );
		if( strcmp( szNodeName,"BUTTON" ) == 0 )
			pComboBox->SetButton( MakeButtonByXML2( pChildNode ));
		else if( strcmp( szNodeName, "TOPIMAGE" ) == 0 )
			pImageTop = MakeImageByXML2( pChildNode );
		else if( strcmp( szNodeName, "MIDDLEIMAGE" ) == 0 )
			pImageMiddle = MakeImageByXML2( pChildNode );
		else if( strcmp( szNodeName, "BOTTOMIMAGE" ) == 0 )
			pImageBottom = MakeImageByXML2( pChildNode );
		else if( strcmp( szNodeName, "LISTBOX" ) == 0 )
			pComboBox->SetListBox( MakeListBoxByXML2( pChildNode ));
	}

	pComboBox->SetImage( pImageTop, pImageMiddle, pImageBottom );
	POINT ptOffset = { iOffsetX, iOffsetY };
	pComboBox->SetOffset( ptOffset );

	return pComboBox;
}

void CTControlMgr::MakeComboBoxByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase )
{
	CTComboBox* pComboBox = MakeComboBoxByXML2( pNode );
	if( pComboBox )
		pBase->Add( pComboBox );
	else
		_ASSERT( 0 && "Not Created ComboBox" );
}

void CTControlMgr::MakeJComboBoxByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;
	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);

	if( FAILED( hr ) )
		return;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );
	int iOwnerDraw  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OWNERDRAW" );

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;
	CJComboBox* pComboBox = MakeJComboBox(iID, iScrX, iScrY, iWidth, iHeight );

	if( pComboBox == NULL )
		return;

	pComboBox->SetControlName( strCtrlName.c_str() );

	if( iOwnerDraw )
		pComboBox->SetOwnerDraw( true );

	BSTR nodeName;
	char szNodeName[256];
	MSXML::IXMLDOMNodeList *childList = NULL;
	MSXML::IXMLDOMNodePtr	pChildNode = NULL;	
	pNode->get_childNodes(&childList);
	long iSize = 0;
	(childList)->get_length(&iSize);


	//CTButton*	pButton			= NULL;
	CTImage*	pImageTop		= NULL;
	CTImage*	pImageMiddle	= NULL;
	CTImage*	pImageBottom	= NULL;
	//CTListBox*	pListBox		= NULL;
	

	for( int i = 0; i < iSize; i++ )
	{
		pChildNode = (childList)->Getitem( i );
		pChildNode->get_nodeName( &nodeName );
		sprintf( szNodeName,"%s",(char*)(_bstr_t)nodeName );
		if( strcmp( szNodeName,"BUTTON" ) == 0 )
			pComboBox->SetButton( MakeButtonByXML2( pChildNode ));
		else if( strcmp( szNodeName, "TOPIMAGE" ) == 0 )
			pImageTop = MakeImageByXML2( pChildNode );
		else if( strcmp( szNodeName, "MIDDLEIMAGE" ) == 0 )
			pImageMiddle = MakeImageByXML2( pChildNode );
		else if( strcmp( szNodeName, "BOTTOMIMAGE" ) == 0 )
			pImageBottom = MakeImageByXML2( pChildNode );
		else if( strcmp( szNodeName, "JLISTBOX" ) == 0 )
			pComboBox->SetListBox( MakeJListBoxByXML2( pChildNode ));
	}

	pComboBox->SetImage( pImageTop, pImageMiddle, pImageBottom );

	pBase->Add( pComboBox );
}

void CTControlMgr::AddTab( MSXML::IXMLDOMNodePtr pNode , CTabbedPane* pPane )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;
	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);

	if( FAILED( hr ) )
		return;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;

	CJContainer* pContainer = new CJContainer;
	CTRadioButton* pRadioButton = NULL;

	BSTR nodeName;

	MSXML::IXMLDOMNodeList *childList = NULL;
	MSXML::IXMLDOMNodePtr	pChildNode = NULL;	
	pNode->get_childNodes(&childList);
	long iSize = 0;
	(childList)->get_length(&iSize);
	char szNodeName[256];
	for( int i = 0; i < iSize; i++ )
	{
		pChildNode = (childList)->Getitem( i );
		pChildNode->get_nodeName( &nodeName );
		sprintf( szNodeName,"%s",(char*)(_bstr_t)nodeName );
		if( strcmp( szNodeName, "TABBUTTON" ) == 0 )
		{
			pRadioButton = MakeRadioButtonByXML( pChildNode, NULL );
		}
		else if( strcmp( szNodeName, "IMAGE" ) == 0 )
		{
			CTImage* pImage = MakeImageByXML2( pChildNode );
			pContainer->Add( pImage );
		}
		else if( strcmp( szNodeName, "BUTTON" ) == 0 )
		{
			CTButton* pButton = MakeButtonByXML2( pChildNode );
			pContainer->Add( pButton );
		}
		else if( strcmp( szNodeName,"ZLISTBOX" ) == 0 )
		{
			CZListBox* pZListBox = MakeZListBoxByXML( pChildNode );
			pContainer->Add( pZListBox );
		}
		else if( strcmp( szNodeName,"LISTBOX" ) == 0 )
		{
			CTListBox* pListBox = MakeListBoxByXML2( pChildNode );
			pContainer->Add( pListBox );
		}
		else if( strcmp( szNodeName,"SCROLLBAR" ) == 0 )
		{
			unsigned iOutScrollModel = 0;
			CTScrollBar* pScrollBar = MakeScrollBarByXML2( pChildNode , iOutScrollModel );
			CZListBox* pZListBox = (CZListBox*)pContainer->Find( iOutScrollModel );
			pScrollBar->SetModel( pZListBox );
			pContainer->Add( pScrollBar );
		}
		else if( strcmp( szNodeName,"COMBOBOX" ) == 0 )
		{
			CTComboBox* pComboBox = MakeComboBoxByXML2( pChildNode );
			if( pComboBox )
				pContainer->Add( pComboBox );
		}
		else if( strcmp( szNodeName,"RADIOBOX" ) == 0 )
		{
			CTRadioBox* pRadioBox = MakeRadioBoxByXML2( pChildNode );
			if( pRadioBox )
				pContainer->Add( pRadioBox );
			else
				_ASSERT( 0 && "Can't Make RadioBox" );
		}
		else if( strcmp( szNodeName,"RADIOBUTTON" ) == 0 )
		{
			unsigned iOutRadioBox = 0;
			CTRadioButton* pRadioButton = MakeRadioButtonByXML2( pChildNode , iOutRadioBox );
			if( pRadioButton )
			{
				if( iOutRadioBox )
				{
					CWinCtrl* pCtrl = pContainer->Find( iOutRadioBox );
					if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
					{
						CTRadioBox* pRadioBox = ( CTRadioBox* )pCtrl;
						pRadioBox->RegisterCtrl( pRadioButton );
					}
				}
				pContainer->Add( pRadioButton );
			}
		}
		else if( strcmp( szNodeName,"EDITBOX") == 0 )
		{
			CTEditBox* pEditBox = MakeEditBoxByXML2( pChildNode );
			if( pEditBox )
				pContainer->Add( pEditBox );
		}
		else if( strcmp( szNodeName,"CHECKBOX" ) == 0 )
		{
			CTCheckBox* pCheckBox = MakeCheckBoxByXML( pChildNode );
			if( pCheckBox )
				pContainer->Add( pCheckBox );
		}
		else if( strcmp( szNodeName, "GUAGE" ) == 0 )
		{
			CTGuage* pGuage = MakeGuageByXML( pChildNode );
			if( pGuage )
				pContainer->Add( pGuage );
		}
	}

	pPane->AddTab( pRadioButton, pContainer );
}

void CTControlMgr::AddTabbedPane( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase )
{
	if( CTabbedPane* pPane =MakeTabbedPane( pNode ) )
		pBase->Add( pPane );
}

CTabbedPane*	CTControlMgr::MakeTabbedPane( MSXML::IXMLDOMNodePtr pNode )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;
	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);

	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );

	CTabbedPane* pPane      = new CTabbedPane;

	pPane->SetControlID( iID );
	pPane->SetPosition( iScrX, iScrY );
	pPane->SetOffset( iOffsetX, iOffsetY );
	pPane->SetControlName( strCtrlName.c_str() );

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;

	BSTR nodeName;

	MSXML::IXMLDOMNodeList *childList = NULL;
	MSXML::IXMLDOMNodePtr	pChildNode = NULL;	
	pNode->get_childNodes(&childList);
	long iSize = 0;
	(childList)->get_length(&iSize);
	char szNodeName[256];
	for( int i = 0; i < iSize; i++ )
	{
		pChildNode = (childList)->Getitem( i );
		pChildNode->get_nodeName( &nodeName );
		sprintf( szNodeName,"%s",(char*)(_bstr_t)nodeName );
		if( strcmp( szNodeName, "TAB" ) == 0 )
		{
			AddTab( pChildNode, pPane);
		}
	}
	return pPane;
}
CTScrollBar*	CTControlMgr::MakeScrollBarByXML2( MSXML::IXMLDOMNodePtr pNode, unsigned& iOutScrollModel )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;
	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);

	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );

	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );
	iOutScrollModel = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "LISTBOXID" );
	int iType	   = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "TYPE" );

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;
	CTScrollBar* pScrollBar = MakeScrollBar(iID, iScrX, iScrY, iWidth, iHeight ,iType );


	BSTR nodeName;

	MSXML::IXMLDOMNodeList *childList = NULL;
	MSXML::IXMLDOMNodePtr	pChildNode = NULL;	
	pNode->get_childNodes(&childList);
	long iSize = 0;
	(childList)->get_length(&iSize);
	char szNodeName[256];
	CTButton* pButton;
	for( int i = 0; i < iSize; i++ )
	{
		pChildNode = (childList)->Getitem( i );
		pChildNode->get_nodeName( &nodeName );
		sprintf( szNodeName,"%s",(char*)(_bstr_t)nodeName );
		if( strcmp( szNodeName, "PREVBUTTON" ) == 0 )
		{
			pButton = MakeButtonByXML2( pChildNode );
			if( pButton == NULL )
				return NULL;

			pScrollBar->SetPrevButton( pButton );
		}
		else if( strcmp( szNodeName, "NEXTBUTTON" ) == 0 )
		{
			pButton = MakeButtonByXML2( pChildNode );
			if( pButton == NULL )
				return NULL;

			pScrollBar->SetNextButton( pButton );
		}
		else if( strcmp( szNodeName, "SCROLLBOX" ) == 0 )
		{
			CTScrollBox* pScrollBox = MakeScrollBoxByXML( pChildNode, iType );
			if( pScrollBox == NULL )
				return NULL;

			pScrollBar->SetScrollBox( pScrollBox );
		}
	}

	POINT ptOffset = { iOffsetX, iOffsetY };
	pScrollBar->SetOffset( ptOffset );
	pScrollBar->SetControlName( strCtrlName.c_str() );

	return pScrollBar;
}
void CTControlMgr::MakeScrollBarByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;
	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);

	if( FAILED( hr ) )
		return;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );
	int iListBoxID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "LISTBOXID" );
	int iType	   = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "TYPE" );
	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;
	CTScrollBar* pScrollBar = MakeScrollBar(iID, iScrX, iScrY, iWidth, iHeight ,iType );
	if( pScrollBar )
	{
		pScrollBar->SetControlName( strCtrlName.c_str() );
	}	

	BSTR nodeName;

	MSXML::IXMLDOMNodeList *childList = NULL;
	MSXML::IXMLDOMNodePtr	pChildNode = NULL;	
	pNode->get_childNodes(&childList);
	long iSize = 0;
	(childList)->get_length(&iSize);
	char szNodeName[256];
	CTButton* pButton;
	for( int i = 0; i < iSize; i++ )
	{
		pChildNode = (childList)->Getitem( i );
		pChildNode->get_nodeName( &nodeName );
		sprintf( szNodeName,"%s",(char*)(_bstr_t)nodeName );
		if( strcmp( szNodeName, "PREVBUTTON" ) == 0 )
		{
			pButton = MakeButtonByXML2( pChildNode );
			if( pButton == NULL )
				return;

			pScrollBar->SetPrevButton( pButton );
		}
		else if( strcmp( szNodeName, "NEXTBUTTON" ) == 0 )
		{
			pButton = MakeButtonByXML2( pChildNode );
			if( pButton == NULL )
				return;

			pScrollBar->SetNextButton( pButton );
		}
		else if( strcmp( szNodeName, "SCROLLBOX" ) == 0 )
		{
			CTScrollBox* pScrollBox = MakeScrollBoxByXML( pChildNode, iType );
			if( pScrollBox == NULL )
				return;

			pScrollBar->SetScrollBox( pScrollBox );
		}
	}

	CWinCtrl* pCtrl = pBase->Find( iListBoxID );
	if( pCtrl )
	{
		switch( pCtrl->GetControlType() )
		{
		case CTRL_LISTBOX:
			pScrollBar->SetModel( (CTListBox*)pCtrl );
			break;
		case CTRL_ZLISTBOX:
			pScrollBar->SetModel( (CZListBox*)pCtrl );
			break;
		case CTRL_TABLE:
			pScrollBar->SetModel( (CJTable*)pCtrl );
			break;
		default:
			_ASSERT( 0 && "Invalid ScrollModel Type" );
			break;
		}

	}

	if( pScrollBar )
		pBase->Add( pScrollBar );

}



CTScrollBox* CTControlMgr::MakeScrollBoxByXML( MSXML::IXMLDOMNodePtr pNode ,int iType )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );

	int iModuleID    = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "MODULEID" );
	int	iTickMove	 = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "TICKMOVE" );
	
	std::string sGID  = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "GID" );
	int iGID  = CResourceMgr::GetInstance()->GetImageNID( iModuleID, sGID.c_str());


	int IsBlink			= GetNodeValueFromNamedNodeMapByName( pNamedNodeMap,"BLINK" );
	int iBlinkMID		= GetNodeValueFromNamedNodeMapByName( pNamedNodeMap,"BLINKMID" );
	std::string sBlinkGID		= GetNodeStringValueFromNamedNodeMapByName( pNamedNodeMap,"BLINKGID" );
	int iBlinkSwapTime	= GetNodeValueFromNamedNodeMapByName( pNamedNodeMap,"BLINKSWAPTIME" );
	int iBlinkGID = CResourceMgr::GetInstance()->GetImageNID( iModuleID, sBlinkGID.c_str());


	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;

	if( iGID == -1 )
		return NULL;
	
	CTScrollBox* pScrollBox = MakeScrollBox(iID, iWidth, iHeight, iGID ,iModuleID , iType );

	if( pScrollBox )
	{
		pScrollBox->SetTickMove( iTickMove );
		pScrollBox->SetControlName( strCtrlName.c_str() );

		if( IsBlink )
		{
			CTImage* pImage = new CTImage;
			pImage->Create( 0,0, iWidth, iHeight, iGID, iModuleID );
			pImage->SetBlinkImage( iBlinkGID, iBlinkMID );
			pImage->SetBlinkSwapTime( iBlinkSwapTime );
			pImage->Show();
			pScrollBox->SetBlinkImage( pImage );
		}
	}

	return pScrollBox;
}


CTRadioBox* CTControlMgr::MakeRadioBoxByXML2(  MSXML::IXMLDOMNodePtr pNode )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	CTRadioBox* pRadioBox = MakeRadioBox( iID );

	if( pRadioBox )
	{
		pRadioBox->SetControlName( strCtrlName.c_str() );
	}
	

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;
	return pRadioBox;
}

void CTControlMgr::MakeRadioBoxByXML(  MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase )
{
	CTRadioBox* pRadioBox = MakeRadioBoxByXML2( pNode );
	if( pRadioBox )
		pBase->Add( pRadioBox );
}

CTRadioButton* CTControlMgr::MakeRadioButtonByXML2(  MSXML::IXMLDOMNodePtr pNode , unsigned &iOutRadioBox )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	iOutRadioBox    = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "RADIOBOXID" );

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );
	int iGhost   = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "GHOST" );

	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );

	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );
	int iModuleID    = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "MODULEID" );

	int iDisableSID  = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "DISABLESID" );

	std::string sNormalGID  = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NORMALGID" );
	std::string sOverGID    = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "OVERGID" );
	std::string sDownGID    = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "DOWNGID" );

	
	CResourceMgr* pMgr = CResourceMgr::GetInstance();
	int iNormalGID	= pMgr->GetImageNID( iModuleID, sNormalGID.c_str() );
	int iOverGID	= pMgr->GetImageNID( iModuleID, sOverGID.c_str() );
	int iDownGID    = pMgr->GetImageNID( iModuleID, sDownGID.c_str() );

	CTRadioButton* pButton = NULL;
	if( iNormalGID != -1 && iOverGID != -1 && iDownGID != -1 )
	{
	  pButton =	MakeRadioButton(iID, iScrX, iScrY, iWidth, iHeight, iNormalGID,iOverGID, iDownGID ,iModuleID );
	  pButton->SetControlName( strCtrlName.c_str() );
	}
	else
	{
		if( iNormalGID == -1 )
		{
			_RPTF1( _CRT_ASSERT,"%s is not found", sNormalGID.c_str() );
			return NULL;
		}

		if( iOverGID == -1 )
		{
			_RPTF1( _CRT_ASSERT,"%s is not found", sOverGID.c_str() );
			return NULL;
		}

		if( iDownGID == -1 )
		{
			_RPTF1( _CRT_ASSERT, "%s is not found", sDownGID.c_str() );
			return NULL;
		}
	}

	if( pButton )
	{
		pButton->SetOffset( iOffsetX, iOffsetY );

		if( iDisableSID > 0 )
			pButton->SetSoundDisableID( iDisableSID );
		if( iGhost )
			pButton->SetGhost();

	}

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;

	return pButton;
}

CTRadioButton* CTControlMgr::MakeRadioButtonByXML(  MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase )
{
	unsigned iOutRadioBox = 0;
	CTRadioButton* pButton = MakeRadioButtonByXML2( pNode, iOutRadioBox );
	if( pButton )
	{
		if( iOutRadioBox > 0 )
		{
			CWinCtrl* pCtrl = pBase->Find( iOutRadioBox );
			if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
			{
				CTRadioBox* pRadioBox = ( CTRadioBox* )pCtrl;
				pRadioBox->RegisterCtrl( pButton );
			}
		}

		if( pBase )
			pBase->Add( pButton );
		
	}
	return pButton;
}
/*
CTree* CTControlMgr::MakeTree( int ID,  int iScrX, int iScrY, int iWidth, int iHeight )
{
	CTree* pTree = new CTree;
	pTree->Init( CTRL_TREE, iScrX, iScrY, iWidth, iHeight );
	pTree->SetControlID( ID );
	m_Controls.push_back( pTree );
	return pTree;
}
*/

void CTControlMgr::AddGuageByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase )
{
	CTGuage* pGuage = MakeGuageByXML( pNode );
	if( pGuage )
		pBase->Add( pGuage );
}

CTGuage* CTControlMgr::MakeGuageByXML( MSXML::IXMLDOMNodePtr pNode )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );

	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );
	int iModuleID    = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "MODULEID" );
	
	std::string sGID = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "GID" );
	std::string strBGID = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "BGID" );

	CResourceMgr* pRMgr = CResourceMgr::GetInstance();
	int iGID    = pRMgr->GetImageNID( iModuleID, sGID.c_str() );
	int iBGID   = pRMgr->GetImageNID( iModuleID, strBGID.c_str() );
	if( iBGID < 0 )
		iBGID = 0;

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;
	
	if( iGID == -1 )
		return NULL;
	CTGuage* pGuage = MakeGuage( iID, iScrX, iScrY, iWidth, iHeight, iGID, iBGID,iModuleID );;
	if( pGuage )
	{
		pGuage->SetOffset( iOffsetX, iOffsetY );
		pGuage->SetControlName( strCtrlName.c_str() );
	}
	return pGuage;
}

CTGuage* CTControlMgr::MakeGuage( int ID,int iScrX, int iScrY, int iWidth, int iHeight,int iGraphicID ,int iBGraphicID, int iModuleID )
{
	CTGuage* pGuage = new CTGuage;
	pGuage->Create( iScrX, iScrY, iWidth, iHeight, iGraphicID, iBGraphicID ,iModuleID );
	pGuage->SetControlID( ID );
	return pGuage;
}

CTPane* CTControlMgr::MakePaneByXML( MSXML::IXMLDOMNodePtr pNode )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;
	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);

	if( FAILED( hr ) )
		return NULL;

	CTPane* pPane = new CTPane;

	int iID = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iScrX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );
	int iOffsetX = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );

	int iWidth = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight = GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );

	pPane->SetControlID( iID );
	pPane->SetPosition( iScrX, iScrY );
	pPane->SetOffset( iOffsetX, iOffsetY );
	pPane->SetWidth( iWidth );
	pPane->SetHeight( iHeight );
	pPane->SetControlName( strCtrlName.c_str() );
	
	
	MSXML::IXMLDOMNodeList *childList = NULL;
	MSXML::IXMLDOMNodePtr	pChildNode = NULL;	
	pNode->get_childNodes(&childList);
	long iSize = 0;

	(childList)->get_length(&iSize);
	BSTR nodeName;
	char szNodeName[256];

	CWinCtrl* pCtrl = NULL;

	for( int i = 0; i < iSize; i++ )
	{
		pChildNode = (childList)->Getitem( i );
		pChildNode->get_nodeName( &nodeName );
		sprintf( szNodeName,"%s",(char*)(_bstr_t)nodeName );
		
		pCtrl = NULL;
		
		if( strcmp( szNodeName,"IMAGE" ) == 0 )
			pCtrl = MakeImageByXML2( pChildNode );
		else if( strcmp( szNodeName,"BUTTON" ) == 0 )
			pCtrl = MakeButtonByXML2( pChildNode );
		else if( strcmp( szNodeName, "PUSHBUTTON" ) == 0 )
			pCtrl = MakePushButtonByXML2( pChildNode );
		else if( strcmp( szNodeName, "CAPTION" ) == 0 )
			pCtrl = MakeCaptionByXML2( pChildNode );
		else if( strcmp( szNodeName,"EDITBOX" ) == 0 )
			pCtrl = MakeEditBoxByXML2( pChildNode );
		else if( strcmp( szNodeName, "LISTBOX" ) == 0 )
			pCtrl = MakeListBoxByXML2( pChildNode );
	/*	else if( strcmp( szNodeName, "STATIC" ) == 0 )
			pCtrl = MakeStaticByXML2( pNode );*/
		else if( strcmp( szNodeName, "SCROLLBAR" ) == 0 )
		{
			unsigned iOutScrollModel = 0;
			pCtrl = MakeScrollBarByXML2( pChildNode , iOutScrollModel );
			CZListBox* pZListBox = (CZListBox*)pPane->FindChild( iOutScrollModel );
			CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
			pScrollBar->SetModel( pZListBox );
		}
		else if( strcmp( szNodeName, "RADIOBOX" ) == 0 )
			pCtrl = MakeRadioBoxByXML2( pChildNode );
		else if( strcmp( szNodeName, "RADIOBUTTON" ) == 0 )
		{
			unsigned iOutRadioBox = 0;
			pCtrl = MakeRadioButtonByXML2( pChildNode , iOutRadioBox );
			if( pCtrl )
			{
				if( iOutRadioBox )
				{
					CWinCtrl* pRadioBoxCtrl = pPane->FindChild( iOutRadioBox );
					if( pRadioBoxCtrl && pRadioBoxCtrl->GetControlType() == CTRL_RADIOBOX )
					{
						CTRadioBox* pRadioBox = ( CTRadioBox* )pRadioBoxCtrl;
						pRadioBox->RegisterCtrl( (CTRadioButton*)pCtrl );
					}
				}
			}
		}
		else if( strcmp( szNodeName,"COMBOBOX" ) == 0 )
			pCtrl = MakeComboBoxByXML2( pChildNode );
	/*	else if( strcmp( szNodeName,"JCOMBOBOX" ) == 0 )
			pCtrl = MakeJComboBoxByXML2( pNode );*/
		else if( strcmp( szNodeName,"JLISTBOX" ) == 0 )
			pCtrl = MakeJListBoxByXML2( pChildNode );
		else if( strcmp( szNodeName, "GUAGE") == 0 )
			pCtrl = MakeGuageByXML( pChildNode );
		else if( strcmp( szNodeName, "TABBEDPANE" ) == 0 )
			pCtrl = MakeTabbedPane( pChildNode );
		else if( strcmp( szNodeName,"BACKGROUNDIMAGE" ) == 0 )
			pCtrl = MakeImageByXML2( pChildNode );
		else if( strcmp( szNodeName,"ZLISTBOX" ) == 0 )
			pCtrl = MakeZListBoxByXML( pChildNode );
		else if( strcmp( szNodeName,"CHECKBOX" ) == 0 )
			pCtrl = MakeCheckBoxByXML( pChildNode );
		else if( strcmp( szNodeName,"PANE" ) == 0 )
			pCtrl = MakePaneByXML( pChildNode );
		
		if( pCtrl )
		{
			pPane->AddChild( pCtrl );
			pCtrl = NULL;
		}
	}

	if( pNamedNodeMap )
	{
		pNamedNodeMap->Release();
		pNamedNodeMap = NULL;
	}

	return pPane;
}

void CTControlMgr::SetCodePage( unsigned uiCodePage )
{
	m_uiCodePage = uiCodePage;
}
unsigned CTControlMgr::GetCodePage()
{
	return m_uiCodePage;
}

CJTable*	CTControlMgr::MakeTableByXML( MSXML::IXMLDOMNodePtr pNode )
{
	MSXML::IXMLDOMNamedNodeMap* pNamedNodeMap = NULL;

	HRESULT hr = pNode->get_attributes(&pNamedNodeMap);
	if( FAILED( hr ) )
		return NULL;

	int iID			= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ID" );
	std::string strCtrlName = GetNodeStringValueFromNamedNodeMapByName(pNamedNodeMap, "NAME" );

	int iOffsetX	= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETX" );
	int iOffsetY	= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "OFFSETY" );
	int iExtent		= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "EXTENT" );
	int iWidth		= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "WIDTH" );
	int iHeight		= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "HEIGHT" );
	int iScrX		= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "X" );
	int iScrY		= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "Y" );

	int iCellWidth		= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "CELLWIDTH" );
	int iCellHeight		= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "CELLHEIGHT" );
	int iColumnCount	= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "COLUMNCOUNT" );
	int iRowMargin		= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "ROWMARGIN" );
	int iColMargin		= GetNodeValueFromNamedNodeMapByName(pNamedNodeMap, "COLMARGIN" );

	pNamedNodeMap->Release();
	pNamedNodeMap = NULL;

	CJTable* pTable = new CJTable;


	if( pTable )
	{
		pTable->SetControlID( iID );

		pTable->SetPosition( iScrX, iScrY );
		pTable->SetOffset( iOffsetX, iOffsetY );

		pTable->SetHeight( iHeight );
		pTable->SetWidth( iWidth );

		pTable->SetExtent( iExtent );
		pTable->SetCellHeight( iCellHeight );
		pTable->SetCellWidth( iCellWidth );
		pTable->SetColMargin( iColMargin );
		pTable->SetRowMargin( iRowMargin );
		pTable->SetColumnCount( iColumnCount );
		pTable->SetControlName( strCtrlName.c_str() );
	}
	return pTable;
}

short CTControlMgr::GetKeyboardInputType()
{
	return m_nKeyboardInputType;
}

void CTControlMgr::SetKeyboardInputType( short nInputType )
{
	m_nKeyboardInputType = nInputType;
	if( m_nKeyboardInputType == INPUTTYPE_NORMAL )
	{
		if( CTEditBox::s_pFocusEdit )
			CTEditBox::s_pFocusEdit->SetFocus( false );
	}
}