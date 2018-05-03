#ifndef _HELPMGR_
#define _HELPMGR_

#include <map>
using namespace std;
typedef pair< unsigned short, unsigned short >  PAIR_LIMIT_LV;
typedef map< int, PAIR_LIMIT_LV >				MAP_HELP;			///< STB INDEX, PAIR_LIMIT_LV >
typedef MAP_HELP::iterator						ITER_MAP_HELP;
typedef MAP_HELP::reverse_iterator				RITER_MAP_HELP;


/**
* 일정시간마다 화면 상단에 Help(Tip)을 보여주기를 관리하는 Class
*		- g_TblHELP에서 AVATAR의 레벨에 맞는 Help들의 Index를 만들어서 가지고 있는다.
*
* @Author		최종진
*
* @Date			2005/9/5	
*/
class CHelpMgr
{
public:
	CHelpMgr(void);
	virtual ~CHelpMgr(void);
	void SetNextHelpMsg();

	/// 1프레임마다 시간을 체크하여 다음 Tip을 보여줄 시간이 됬는지를 계산한다.
	void UpdateFrame();
	///최초 실행시와 아바타가 레벨업시마다 호출하여 g_pAVATAR의 레벨로 g_TblHELP에서 Map을 갱신한다.
	void Update();
	
	void SetShowTip( bool b ){ m_bShowTip = b; }

protected:
	int GetFirstIndex();
	int GetLastIndex();
	int GetNextIndex( int iPrevIndex );

protected:
	int			m_iPrevShowHelpID;		/// 이전 Help(Tip)의 g_TblHELP의 Index
	bool		m_bShowTip;
	DWORD		m_dwSetMsgTime;			/// 이번 Help(Tip)을 보여주기 시작한 시간
	DWORD		m_dwSetMsgTimeSpace;	/// 다음 Help(Tip)을 보여주기까지의 시간

	MAP_HELP	m_HelpData;				///현재 아바타 레벨에 맞는 Help(Tip) Msg들의 Index
};

extern CHelpMgr	g_HelpMgr;
#endif