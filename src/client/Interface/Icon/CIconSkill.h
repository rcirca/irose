#ifndef _CIconSkill_
#define _CIconSkill_
#include "cicon.h"

class CSkill;
/**
* Class CIconSkill
* Skill 객체에 대한 View Class
* @Author		이동현, 최종진
* @Date			2005/9/6
*/
class CIconSkill : public CIcon
{
private:
	///CSkill*						m_pSkill;
	int							m_iSkillSlot;

public:
	CIconSkill(void);
	CIconSkill( int iSkillSlot );
	virtual ~CIconSkill(void);
	virtual int GetIndex();

	void						SetSkillSlotToIcon( int iSkillSlot ){ m_iSkillSlot = iSkillSlot; }
	int							GetSkillSlotFromIcon(){ return m_iSkillSlot; }
	virtual int					GetSkillLevel();
	virtual int					GetSkillIndex();
	CSkill*						GetSkill();

	//----------------------------------------------------------------------------------------------------
	/// Overrided from CIcon
	//----------------------------------------------------------------------------------------------------
	/*override*/virtual void Draw();
	/*override*/virtual void ExecuteCommand();    
	/*override*/virtual CIcon*	Clone();
	/*override*/virtual void	GetToolTip( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType );
	/*override*/virtual const char*	GetName();


	static void AddSkillRequireJob( int iSkillNo, CInfo& ToolTip );
	static void AddSkillRequireSkill( int iSkillNo, CInfo& ToolTip );
	static void AddSkillRequireAbility( int iSkillNo, CInfo& ToolTip );	///조건 능력
	static void AddSkillRequireSkillPoint2Learn( int iSkillNo, CInfo& ToolTip );

protected:
	///<- Skill Tooltip용 Method*
	void AddSkillTypeTarget( int iSkillNo , CInfo& ToolTip, bool bAddTarget = true );
	void AddSkillSummon( int iSkillNo, CInfo& ToolTip );
	void AddSkillDistanceScope( int iSkillNo,CInfo& ToolTip, bool bAddScope );
	void AddSkillScope( int iSkillNo , CInfo& ToolTip);
	//void AddSkillIncreaseAbility( int iSkillNo, CInfo& ToolTip );
	void AddSkillName( int iSkillNo , CInfo& ToolTip, bool bAddLevel = true);
	void AddSkillDuration( int iSkillNo, CInfo& ToolTip , DWORD color = g_dwWHITE );
	void AddSkillNextLevelTitle( int iSkillNo, CInfo& ToolTip );
	void AddSkillPower( int iSkillNo, CInfo& ToolTip );
	void AddSkillDesc( int iSkillNo, CInfo& ToolTip );
	void AddSkillUseProperty( int iSkillNo, CInfo& ToolTip );
	void AddSkillRequireEquip( int iSkillNo, CInfo& ToolTip );		///필요장비
	void AddSkillRequireUnion( int iSkillNo, CInfo& ToolTip );
	void AddSkillStatus( int iSkillNo, CInfo& ToolTip );
	void AddSkillSuccessRateDuration( int iSkillNo , CInfo& ToolTip);
	void AddSkillRequireSkillPoint( int iSkillNo , CInfo& ToolTip);
	void AddSkillSuction( int iSkillNo, CInfo& ToolTip );


	int		GetSkillNextLevelNo( int iSkillNo );
	bool	GetSkillIncreaseAbility( int iSkillNo, int iColumn, std::string& strOut ,bool bAddTypeName );
	int		GetNeedPoint4LevelUp( int iSkillNo );
	///->

};
#endif