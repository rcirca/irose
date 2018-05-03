#ifndef _QUEST_REWARD_QUEUE_
#define _QUEST_REWARD_QUEUE_

#include <list>

#include <Net_Prototype.h>


//----------------------------------------------------------------------------------------------------
/// 퀘스트 보상을 실제 클라이언트에서 트리거를 체크후 수행하기위해 미룬다.
/// class CQuestRewardQueue
//----------------------------------------------------------------------------------------------------

class CQuestRewardQueue
{
private:
	//std::list< tag_SET_INVITEM >		m_InvItemList;
	std::list< gsv_SET_ABILITY >		m_SetAbilityList;
	std::list< gsv_SET_ABILITY >		m_AddAbilityList;
	
public:
	CQuestRewardQueue(void);
	~CQuestRewardQueue(void);


	void		Clear();
	void		ApplyReward();

	//void		PushSetInvItem( tag_SET_INVITEM& sSetInvItem );
	void		PushSetAbility( gsv_SET_ABILITY& sSetAbility );
	void		PushAddAbility( gsv_SET_ABILITY& sAddAbility );


	static void RewardItem( tag_SET_INVITEM );
	static void SetAbility( gsv_SET_ABILITY );
	static void AddAbility( gsv_SET_ABILITY );
};

extern CQuestRewardQueue g_QuestRewardQueue;

#endif ///_QUEST_REWARD_QUEUE_
