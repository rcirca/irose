// FileName			: RgnBitmap.h
// Author			: Im Ji-hyuk (icedac@email.com)
//
// Purpose		    : RgnBitmap.h
//			related windows API
//			HRGN
//			::SetWindowRgn();
//			::OffsetRgn();
//			::CombineRgn();
//			
//
// Date Of Creation	: 2000-12-20
// +--- Modification History ---+
// Date             Modifications
// 
///////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RGNBITMAP_H__F4D57321_D5A0_11D4_8A61_0001023719D3__INCLUDED_)
#define AFX_RGNBITMAP_H__F4D57321_D5A0_11D4_8A61_0001023719D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define COLOR16_THRESHHOLD	RGB16(16, 16, 16)

//class CImage;

class CRgnBitmap  
{
public:
	//static HRGN BitmapToRegion( CImage* pImage, COLOR16 cTransparentColor = COLOR16_BLACK, COLOR16 cTolerance = COLOR16_THRESHHOLD );
	static HRGN BitmapToRegion( HBITMAP hBitmap, COLORREF cTransparentColor = 0, COLORREF cTolerance = 0x101010 );

private:
	CRgnBitmap();
	~CRgnBitmap();
};

#endif // !defined(AFX_RGNBITMAP_H__F4D57321_D5A0_11D4_8A61_0001023719D3__INCLUDED_)
