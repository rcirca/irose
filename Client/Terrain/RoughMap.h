#ifndef _ROUGH_TERRAIN_
#define _ROUGH_TERRAIN_



//----------------------------------------------------------------------------------------------------
/// class CRoughMap
/// 원거리 지형을 표현하기위한 저해상도 맵입니다.
/// 이맵은 패치단위가아닌 맵하나가 하나의 매쉬로 관리됩니다. 추가적으로 전체를 매핑할 텍스쳐가 필요합니다.
//----------------------------------------------------------------------------------------------------

class CRoughMap
{
private:
	HNODE			m_hTerrainBlock;
	HNODE			m_hTerrainTexture;

	/// 높이맵 정보
	int				iWidth;
	int				iHeight;
	float*			m_pHeight;


	/// 월드 옵셋
	float			m_fWorldOffsetX;
	float			m_fWorldOffsetY;

	int				m_iDetailLevel;

public:
	CRoughMap(void);
	~CRoughMap(void);

	void			SetWorldOffset( float fX, float fY );
	void			SetDetailLevel( int iLevel );

	bool			Load( char* szMapName, int iZoneMapX, int ZoneMapY );
	void			Free();

	bool			LoadEngineObject( char* szMapName, int iZoneMapX, int iZoneMapY );
	void			ReleaseEngineObject( );
	
	void			InsertToEngine();
	void			RemoveFromEngine();
};


#endif //_WIDE_TERRAIN_