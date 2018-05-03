#ifndef _TCONTROLMGR_
#define _TCONTROLMGR_

#include "WinCtrl.h"

#import "msxml.tlb" named_guids

class CZListBox;
class CTButton;
class CTCaption;
class CTDialog;
class CTEditBox;
class CTImage;
class CTListBox;
class CTScrollBar;
class CTScrollBox;
class CTStatic;
class CTRadioBox;
class CTRadioButton;
class CTComboBox;
class CTMsgBox;
class CTPushButton;
class CTree;
class CTGuage;
class CJListBox;
class CJComboBox;
class CTabbedPane;
class CTCheckBox;
class CTPane;
class CJTable;
//class CToolTip;

class ITDraw;
class ITSound;
class ITFont;


/**
* 1. XML로부터의 control생성
*	2. client에서 넘어온 각 인터페이스의 임플리먼트된 object의 포인터를 저장해둔다.
*		- ITDraw, ITFont, ITSound,
*	3. client에서의 기타 변수를 저장
*		- 국가코드, 윈도우 핸들, 입력타입( 엔터채팅타입, 세븐하츠(노말) 타입 )
*
* @Warning			ScrollBar는 생성후 PrevButton,NextButton(MakeButton),ScrollBox(MakeScrollBox)를 만들어서 넣어주어야한다
*
* @Author			최종진
*
* @Date			2005/8/30 
*/
class TGAMECTRL_API CTControlMgr
{
private:
	CTControlMgr(void);
	virtual ~CTControlMgr(void);
	
	static CTControlMgr* s_pInstance;

public:
	static CTControlMgr* GetInstance();
	void Destroy();


	bool			MakeDialogByXML( const char* IDD, CTDialog* pBase );


	/// Control 생성 
	CTButton*		MakeButton(int ID, int iScrX, int iScrY, int iWidth, int iHeight, int iNormalGraphicID, int iOverGraphicID, int iDownGraphicID ,int iModuleID );
	CTPushButton*	MakePushButton(int ID, int iScrX, int iScrY, int iWidth, int iHeight, int iNormalGraphicID, int iOverGraphicID, int iDownGraphicID ,int iModuleID );

	CTCaption*		MakeCaption(int ID,int iScrX, int iScrY, int iWidth, int iHeight);
	CTDialog*		MakeDialog(int ID,int iScrX, int iScrY, int iWidth, int iHeight);
	CTEditBox*		MakeEditBox(int ID,int iScrX, int iScrY, int iWidth, int iHeight, int iCharWidth );
	CTImage*		MakeImage(int ID,int iScrX, int iScrY, int iWidth, int iHeight,int iGraphicID ,int iModuleID );
	CTListBox*		MakeListBox(int ID,int iScrX, int iScrY, int iWidth, int iHeight,int iExtent ,int iCharWidth, int iCharHeight );
	CTStatic*		MakeStatic(int ID,int iScrX, int iScrY, int iWidth, int iHeight);
	CTComboBox*		MakeComboBox(int ID, int iScrX, int iScrY, int iWidth, int iHeight  );


	CJComboBox*		MakeJComboBox(int ID, int iScrX, int iScrY, int iWidth, int iHeight  );
	CJListBox*		MakeJListBox(int ID,int iScrX, int iScrY, int iWidth, int iHeight , int iItemHeight );


	CTScrollBar*	MakeScrollBar(int ID,int iScrX, int iScrY, int iWidth, int iHeight,int iType );
	CTScrollBox*	MakeScrollBox(int ID,int iWidth, int iHeight, int iGraphicID ,int iModuleID ,int iType);

	

	/// 2003 /11 /26 / 최종진
	/// 1. RadioBox에는 RadioButton만이 등록된다.
	///	2. RadioBox에 RadioButton이 등록될때 RadioBox는 RadioButton에게 자신의 포인터를 넘겨준다.
	CTRadioBox*		MakeRadioBox(int ID);
	CTRadioButton*  MakeRadioButton(int ID,int iScrX, int iScrY, int iWidth, int iHeight, int iNormalGraphicID, int iOverGraphicID, int iDownGraphicID ,int iModuleID );
	CTGuage*		MakeGuage( int ID,int iScrX, int iScrY, int iWidth, int iHeight,int iGraphicID ,int iBGraphicID, int iModuleID );


	/// 현재 CTDialog에 Add 시킬수 없는 MsgBox를 만드는 Operation - 2003/12/11 : 최종진
	bool			MakeMsgBoxByXML( char* szIDD, CTMsgBox* pBase );



	/// 메모리 해제
//	bool ReleaseCtrl( unsigned int iID );
	/// Control검색
	///CWinCtrl*		Find( unsigned int iID );
	ITDraw*			GetDrawMgr(){ return m_pDrawMgr; }
	void			SetDrawMgr( ITDraw* pDrawMgr ){ m_pDrawMgr = pDrawMgr; }
	ITSound*		GetSoundMgr(){ return m_pSoundMgr; }
	void			SetSoundMgr( ITSound* pSound ){ m_pSoundMgr = pSound; }
	ITFont*			GetFontMgr(){ return m_pFontMgr; }
	void			SetFontMgr( ITFont* pFont ){ m_pFontMgr = pFont; }
	///*----------------------------------------------------------------------------------//
	void			SetWindowHandle( HWND hWnd ){ m_hWnd = hWnd ;}
	HWND			GetWindowHandle(){ return m_hWnd; }

	void			SetCodePage( unsigned uiCodePage );
	unsigned		GetCodePage();

	enum{
		INPUTTYPE_NORMAL,
		INPUTTYPE_AUTOENTER,
	};
	short			GetKeyboardInputType();
	void			SetKeyboardInputType( short nInputType );

private:
	int				GetNodeValueFromNamedNodeMapByName(MSXML::IXMLDOMNamedNodeMap* pNodeMap , char* name);
	float			GetNodeFloatValueFromNamedNodeMapByName(MSXML::IXMLDOMNamedNodeMap* pNodeMap , char* name);
	std::string		GetNodeStringValueFromNamedNodeMapByName(MSXML::IXMLDOMNamedNodeMap* pNodeMap , char* name);

	CJTable*		MakeTableByXML( MSXML::IXMLDOMNodePtr pNode );

	void			MakeButtonByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	void			MakePushButtonByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	void			MakeImageByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	void			MakeCaptionByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	void			MakeStaticByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	void			MakeEditBoxByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	CTEditBox*		MakeEditBoxByXML2( MSXML::IXMLDOMNodePtr pNode );

	void			MakeListBoxByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	void			MakeScrollBarByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	CTScrollBar*	MakeScrollBarByXML2( MSXML::IXMLDOMNodePtr pNode, unsigned& iOutScrollModel );
	void			MakeRadioBoxByXML(  MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	CTRadioBox*		MakeRadioBoxByXML2(  MSXML::IXMLDOMNodePtr pNode );
	CTRadioButton*	MakeRadioButtonByXML(  MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	CTRadioButton*	MakeRadioButtonByXML2(  MSXML::IXMLDOMNodePtr pNode , unsigned &iOutRadioBox );
	void			MakeComboBoxByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	CTComboBox*		MakeComboBoxByXML2( MSXML::IXMLDOMNodePtr pNode );
	
	void			MakeJListBoxByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	void			MakeJComboBoxByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	
	CTCheckBox*		MakeCheckBoxByXML( MSXML::IXMLDOMNodePtr pNode );	

	void			AddGuageByXML( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );

	CTButton*		MakeButtonByXML2( MSXML::IXMLDOMNodePtr pNode);
	CTPushButton*	MakePushButtonByXML2( MSXML::IXMLDOMNodePtr pNode);
	CTImage*		MakeImageByXML2( MSXML::IXMLDOMNodePtr pNode );
	CTListBox*		MakeListBoxByXML2( MSXML::IXMLDOMNodePtr pNode );
	CTCaption*		MakeCaptionByXML2( MSXML::IXMLDOMNodePtr pNode );
	CTScrollBox*	MakeScrollBoxByXML( MSXML::IXMLDOMNodePtr pNode ,int iType );
	CTGuage*		MakeGuageByXML( MSXML::IXMLDOMNodePtr pNode );
	CZListBox*		MakeZListBoxByXML( MSXML::IXMLDOMNodePtr pNode );

	CJListBox*		MakeJListBoxByXML2( MSXML::IXMLDOMNodePtr pNode );
//	CToolTip*		MakeToolTipByXML( MSXML::IXMLDOMNodePtr pNode );

	CTPane*			MakePaneByXML( MSXML::IXMLDOMNodePtr pNode );
	CTabbedPane*	MakeTabbedPane( MSXML::IXMLDOMNodePtr pNode );
	void			AddTabbedPane( MSXML::IXMLDOMNodePtr pNode , CTDialog* pBase );
	void			AddTab( MSXML::IXMLDOMNodePtr pNode , CTabbedPane* pPane );

private:

	ITDraw*				m_pDrawMgr;
	ITSound*			m_pSoundMgr;
	ITFont*				m_pFontMgr;

	HWND				m_hWnd;
	short				m_nKeyboardInputType;

	unsigned			m_uiCodePage;
};
#endif ///_TCONTROLMGR_