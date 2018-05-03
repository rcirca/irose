#ifndef _CTContainerItem_
#define _CTContainerItem_
#include "tobject.h"
#include <string>


/**
* TObject를 Item으로 가지는 Container에 Item으로 넣기 위한 Class
*	- 표시하고자 하는 값과 실제 값을 std::string으로 가지고 있다
*
* @Author	최종진
*
* @Date		2005/8/30
*/
class TGAMECTRL_API CTContainerItem :	public CTObject
{
	std::string		m_strIdentify;///표시에 사용되는 string
	std::string		m_strValue;///값을 저장하는 string
public:
	CTContainerItem(void);
	virtual ~CTContainerItem(void);

	const char*		GetIndetify() const;
	const char*		GetValue() const;
	void			SetValue( const char* pszValue );
	void			SetIdentify( const char* pszIndentify );
};
#endif