#ifndef _MOUSETARGETEFFECT_
#define _MOUSETARGETEFFECT_



class CEffect;

//------------------------------------------------------------------------------------------------
/// CMouseTargetEffect
/// @brief 마우스 타겟에 출력할 이펙트 처리 클래스
//-------------------------------------------------------------------------------------------------

class CMouseTargetEffect
{
private:
	CEffect*			m_pEffect;
public:
	CMouseTargetEffect(void);
	~CMouseTargetEffect(void);

	void				Init();
	void				Clear();

	void				Start( float x, float y, float z );
	void				Proc();
};


#endif // _MOUSETARGETEFFECT_
