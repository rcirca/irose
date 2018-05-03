#ifndef _CClanMarkView_
#define _CClanMarkView_

class CObjCHAR;

/**
* 클랜마크를 보여주기를 담당하는 클래스-CNameBox의 맴버
*  - 기존 (BackImage + CenterImage), 유저가 등록한 Image, Client에 없는 등록된 Image의 세가지 상태를 가진다.
* 
* @Author	최종진
*
* @Date		2005/3/23
*/
class CClanMarkView
{
public:
	CClanMarkView(void);
	~CClanMarkView(void);

	static void Draw( CObjCHAR* pObjChar/*, const POINT& ptDraw*/ , const D3DVECTOR&  vDrawPos);
};
#endif