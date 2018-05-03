#ifndef _CClanMarkUserDefined_
#define _CClanMarkUserDefined_

/**
* 유저가 등록한 클랜마크 이미지 클래스
*	- CClanMarkManager가 개체를 가지고 있고, CObjCHAR가 개체포인터를 가지고 있는다 
*	
* @Warning		레퍼런스 카운팅에 유의	
*
* @Author		최종진
* 
* @Date			2005/9/5
*/
class CClanMarkUserDefined
{
public:
	CClanMarkUserDefined(void);
	~CClanMarkUserDefined(void);

	void	Draw( const D3DVECTOR& vDrawPos );
	void	Draw( float x, float y, float z );

	void	Free();
	void	SetNode( HNODE hNode );
	void	SetName( const char* pszName );
	DWORD	GetLastUsedTime();
	const std::string& GetName();

	void	SetClanID( int iClanID )	{ m_iClanID = iClanID; }
	int		GetClanID()					{ return m_iClanID; }


	bool	IsLoaded()					{ return m_bLoaded; }
	void	SetLoadFlag( bool bLoaded ) { m_bLoaded = bLoaded; }
	void	SetFileCRC( WORD wCRC )		{ m_wFileCRC = wCRC; }
	WORD	GetFileCRC()				{ return m_wFileCRC; }

	void	AddRef();
	void	Release();
	int		GetRefCount();

	static  std::string NewClanMarkFileName;
	static  SIZE		ClanMarkSize;
	static  std::string ClanMarkFileDirectory;
	static  void		GetClanMarkFileName( int server_id, int clan_id , std::string& filename );


protected:

	int				m_iRefCount;
	HNODE			m_hNode;		
	DWORD			m_dwLastUsedTime;
	std::string		m_strTextureName;

	int				m_iClanID;
	bool			m_bLoaded;
	WORD			m_wFileCRC;
};
#endif