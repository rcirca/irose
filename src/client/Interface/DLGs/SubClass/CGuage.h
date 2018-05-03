#ifndef _CGUAGE_
#define _CGUAGE_
/**
*	제조와 제련에 사용되는 게이지 출력class : TGameCtrl의 TGauge와 중복되는 기능이 있다.
*
*	@Author		최종진
*	@Date		2005/9/15
*/
class CGuage
{
public:

	CGuage( int iGuageImageID, int iBGImageID , int iModuleID , int iWidht);
	~CGuage(void);

	void	Draw();
	int		Update( POINT ptMouse, DWORD dwPrevTime, DWORD dwCurrTime );
	void	SetAutoIncrement(int iAutoIncrementValue, int iAutoIncrementMaxValue );
	void    SetAutoIncrementMaxValue( int iAutoIncrementMaxValue );
	void	SetPosition( POINT ptPos ); ///Offset을 무시한 강제 이동
	void	SetOffset( POINT ptOffset );///Parent Dialog의 Left-Top과의 간격
	void	MoveWindow( POINT pt );		///Offset이 적용된 위치 이동
	void	SetWidth( int iWidth );
	void	SetHeight( int iHeight );

	void	SetText( const char* szTxt);
	const   char* GetText();

	void	SetValue( int iValue );
	int		GetValue();
	
	void	SetGuageImageID( int iImageID );
	enum {
		UPDATE_DELETE,		///PARENT에게 지워달라고 요청 ( 사용에 주의 )
		UPDATE_NORMAL,		
		UPDATE_END			///UPDATE에서 할일을 다 끝냈다( PARENT가 알아서 다음에 해야할것들을 처리 )
	};

protected:
	///Image
	int				m_iBGImageID;
	int				m_iGuageImageID;
	int				m_iModuleID;
	///Data
	std::string		m_strText;

	///속성
	bool			m_bAutoIncrement;
	int				m_iAutoIncrementValue;///초당 증가할 % ( 1 ~ 100 )
	int				m_iAutoIncrementMaxValue;

	///위치
	POINT			m_ptOffset;
	POINT			m_ptPosition;
	int				m_iWidth;
	int				m_iHeight;
	///Guage를 그릴 Width
	int				m_iValue;///현재 게이지가 그려져야할 %(0 ~ 100)
};
#endif