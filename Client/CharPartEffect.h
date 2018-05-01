#ifndef _CHAR_PART_EFFECT_
#define _CHAR_PART_EFFECT_


class CEffect;

//---------------------------------------------------------------------------------------------
///
/// 캐릭터 각 부위별 이펙트 생성, 관리
///
//---------------------------------------------------------------------------------------------

class CCharPartEffect
{
private:
	CEffect*			m_ppEFFECT[ MAX_BODY_PART ];

public:
	CCharPartEffect(void);
	~CCharPartEffect(void);

	bool				CreatePartEffect( int iPartIndex );
	void				DeletePartEffect( int iPartIndex );
	void				DeleteAllPartEffect();
};


#endif //_CHAR_PART_EFFECT_
