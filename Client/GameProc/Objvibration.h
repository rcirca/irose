#ifndef _OBJVIBRATION_
#define _OBJVIBRATION_



class CObjCHAR;

//----------------------------------------------------------------------------------------------------
/// class CObjvibration
/// @brief 타격혹은 기타시에 오브젝트를 살짝 흔들어 준다.
///			절대적으로 캐릭터가 정지시에만 동작해야된다.
//----------------------------------------------------------------------------------------------------
class CObjvibration
{
private:
	 int				m_iDurationTime;			///< Vibration duration time
	 int				m_iVibrationInterval;		///< Vibration interval
	 int				m_iStartTime;				///< Vibration start time
	 int				m_iElapsedTime;				///< Elapsed time from starttime
	 int				m_iElapsedVibrationTime;	///< Elapsed time from previous vibration time

	 CObjCHAR*			m_pObjCHAR;
	 D3DXVECTOR3		m_vOldObjPos;				///< Old position of char

	 bool				m_bActivated;

	 CObjvibration(void);
public:	
	~CObjvibration(void);

	CObjvibration( CObjCHAR* pObjCHAR, int iDurationTime );

	void				StartVibration();
	void				EndVibration();

	void				SetActive( bool bActivate ){ m_bActivated = bActivate; }

	void				Proc();
	void				Vibration();
};


#endif ///_OBJVIBRATION_