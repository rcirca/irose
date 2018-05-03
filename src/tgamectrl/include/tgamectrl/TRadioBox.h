#ifndef _RADIOBOX_
#define _RADIOBOX_

#include "winctrl.h"
class CTRadioButton;

typedef map< unsigned int, CTRadioButton* >	RADIOBUTTON_MAP;	
typedef RADIOBUTTON_MAP::iterator			RADIOBUTTON_MAP_ITOR;
/**
* 같이 동작할 RadioButton 을 묶어 줄 RadioBox Class
*
* @Author	최종진
*
* @Data		2005/8/30
*/
class TGAMECTRL_API CTRadioBox : public CWinCtrl
{
public:
	CTRadioBox(void);
	virtual ~CTRadioBox(void);
	virtual void Show();

	void RegisterCtrl( CTRadioButton* pCtrl );			/// 라디오 버튼 등록
	void UnregisterCtrl( CTRadioButton* pCtrl );		/// 라디오 버튼 등록 해제
	void EventChangeSelect( unsigned int iID );			/// 등록된 라디오 버튼이 클릭되었을때 라디오 버튼이 호출하는 Method(다른 버튼들의 상태를 Normal로 바꾸어준다)

	unsigned int		GetPressedButtonID(){ return m_iPressedButton; }
	bool				SetPressedButton( unsigned int iID );

protected:
	RADIOBUTTON_MAP		m_Children;
	unsigned int		m_iPressedButton;				/// 현재 선택된 라디오 버튼 ID
};

#endif