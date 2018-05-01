#ifndef _CRequireMaterial_
#define _CRequireMaterial_

/**
* CManufacture에 사용되는 SubClass = 제조에 필요한 Item들을 위한 Class
*
* @Author		최종진
* @Date			2005/9/15
*/
class CRequireMaterial{
public:
	CRequireMaterial();
	~CRequireMaterial();
	
	void Clear();
	void SetItemClass( int iClass );
	void SetItemType( int iType );
	void SetItemNo( int iNo);
	void SetRequireCount( int iCount );
	void SetName( const char* pszName );///이름이 없으면

	int GetItemClass();
	int	GetItemType();
	int GetItemNo();
	int GetRequireCount();
	const char* GetName();

	bool IsEmpty();

protected:
	int			m_iItemClass;
	int			m_iItemType;
	int			m_iItemNo;		/// ItemNo혹은 Item종류가 들어갈수 있다.1000보다 작으면 종류, 1000 보다 크면 Type + ItemNo
	int			m_iRequireCount;
	std::string m_strName;

};
#endif