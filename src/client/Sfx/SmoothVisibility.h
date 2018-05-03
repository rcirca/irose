#ifndef _SMOOTH_VISIBILITY_
#define _SMOOTH_VISIBILITY_



//-----------------------------------------------------------------------------------------
///
/// CSmoothVisibility
/// 
/// Smooth visibility transition
///
//-----------------------------------------------------------------------------------------

class CSmoothVisibility
{
private:
	float					m_fCurrentVisibility;
	bool					m_bToDisappear;				/// 사라지는 중이냐? 나타나는 중이냐?
	DWORD					m_dwTransitionTime;			/// 변화에 걸리는 시간

	DWORD					m_dwStartTime;
	DWORD					m_dwProgressTime;			/// It's time to finishing the change
	DWORD					m_dwTotalElapsedTime;		/// Elapsed time until now.

	bool					m_bTransitionActivity;		///	현재 진행중인가?

public:
	CSmoothVisibility(void);
	~CSmoothVisibility(void);

	void					Start( DWORD dwTransitionTime, bool bToDisappear );
	void					Proc();

	bool					GetCurrentTransitionActivity(){ return m_bTransitionActivity; }
	float					GetCurrentVisibility(){ return m_fCurrentVisibility; }
};



//-----------------------------------------------------------------------------------------
///
/// CSmoothVisibilityCombo
/// 
/// Smooth visibility transition Combo
///
//-----------------------------------------------------------------------------------------

class CSmoothVisibilityCombo
{
private:
	/// 현재의 visibility
	float					m_fCurrentVisibility;


	DWORD					m_dwStartTime;

	DWORD					m_dwToFirstTransitionTime; /// if Disappear combo, use iToDisappear time.
	DWORD					m_dwDurationTime;
	DWORD					m_dwToSecondTransitionTime;

	/// flash
	DWORD					m_dwFlashDurationTime;		/// Total duration time
	DWORD					m_dwFlashTime;				/// One flash time

	bool					m_bTransitionActivated;
	int						m_iComboType;

	
	//---------------------------------------------------------------------------------
	enum COMBO_TYPE
	{
		VISIBILITY_COMBO_TO_DISAPPEAR = 0,
		VISIBILITY_COMBO_TO_APPEAR,
		VISIBILITY_COMBO_FLASH,
	};
	//---------------------------------------------------------------------------------


public:
	CSmoothVisibilityCombo();
	~CSmoothVisibilityCombo();

	void					ResetTime();

	///
	/// @param iToDisappear : Transition time to disappear
	/// @param iDuration    : Duration time of invisible
	/// @param iToAppear    : Transition time to appear
	///
	void					StartDisappearCombo( DWORD dwToDisappear, DWORD dwDuration, DWORD dwToAppear );

	///	
	/// @param iToAppear    : Transition time to appear
	/// @param iDuration    : Duration time of invisible
	/// @param iToDisappear : Transition time to disappear	
	///
	void					StartAppearCombo( DWORD dwToAppear, DWORD dwDuration, DWORD dwToDisappear );

	/// flash effect
	void					StartFlash( DWORD dwDurationTime, DWORD dwFlashTime );


	/// update visibility
	void					UpdateVisibility();

private:
	void					UpdateDisappearCombo();
	void					UpdateAppearCombo();
	void					UpdateFlashCombo();

};


#endif //_SMOOTH_VISIBILITY_
