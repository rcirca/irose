#ifndef _CMSGBOXCARTRIDE_
#define _CMSGBOXCARTRIDE_

#include "stdafx.h"
#include ".\cmsgbox.h"

class CMsgBox_CartRide : public CMsgBox
{
public:

	CMsgBox_CartRide();
	virtual ~CMsgBox_CartRide(){}

	void Update( POINT ptMouse );

	WORD m_wOwner;
	WORD m_wGuest;
};


#endif