#ifndef _DIGITEFFECT_
#define _DIGITEFFECT_


#define MAX_DIGIT_EFFECT		50
#define INVALID_EFFECTNODE		( MAX_DIGIT_EFFECT + 1 )

struct DIGITNODE
{
	HNODE		m_hPositionDummy;
	HNODE		m_hAnimatable;	
	HNODE		m_hMesh;
	HNODE		m_hMat;
	bool		m_bVisible;

	DIGITNODE(){ m_bVisible = false; }
};

enum DIGIT_TYPE
{
	DIGIT_NORMAL = 0,
	DIGIT_ATTACKED = 1,
	MAX_DIGIT_TYPE,
};


/// 타격치 표현 이펙트..
class CDigitEffect
{
private:
	DIGITNODE				m_DigitNode[ MAX_DIGIT_EFFECT ];

	HNODE					m_hMotion;

	/// Digit texture
	HNODE					m_DigitTex[ MAX_DIGIT_TYPE ];
	HNODE					m_DigitClearTex;
	HNODE					m_DigitMissTex;

	HNODE					m_CriticalTex;

public:
	CDigitEffect();
	~CDigitEffect();


	bool		Init();
	void		ClearAll();

	void		Refresh();

	/// 현재 사용되고 있지 않는 노드를 얻는다..( 애니매이션이 끝났거나, 아예 아직 생성되지 않은것 )
	int			GetEmptyNode();

	void		CreateDigitEffect( int iPoint, float x, float y, float z, bool bIsUSER );
};



#endif //_DIGITEFFECT_