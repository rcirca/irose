#ifndef _OBJECT_TARGET_EFFECT_
#define _OBJECT_TARGET_EFFECT_


class CEffect;
class CObjCHAR;

//----------------------------------------------------------------------------------------------------
/// 게임내 타겟을 선택( 유져, 몬스터, NPC )했을때 선택표시 이펙트 출력
/// class CObjectTargetEffect
//----------------------------------------------------------------------------------------------------

class CObjectTargetEffect
{
	CEffect*			m_pEffect;
	CObjCHAR*			m_pParent;

public:
	CObjectTargetEffect(void);
	~CObjectTargetEffect(void);

	void				Init();
	void				Clear();

	void				Attach( CObjCHAR* pObjCHAR );
	void				Detach();	
};


#endif //_OBJECT_TARGET_EFFECT_