/** 
* @file zz_cursor.CPP
* @brief cursor class.
* @author Jiho Choi (zho@korea.com)
* @version 1.0
* @date    24-jul-2004
*
* $Header: /engine/src/zz_cursor.cpp 6     04-07-27 9:50p Zho $
*/

#include "zz_tier0.h"
#include "zz_node.h"
#include "zz_renderer_d3d.h"
#include "zz_cursor.h"
#include "zz_system.h"
#include "zz_view.h"

ZZ_IMPLEMENT_DYNCREATE(zz_cursor, zz_node)

zz_cursor * zz_cursor::current = NULL;

bool zz_cursor::d3d_mouse_ = false;

zz_cursor::zz_cursor () :
hcursor_(NULL),
surface_(NULL),
xhotspot_(0),
yhotspot_(0)
{
}

zz_cursor::~zz_cursor ()
{
	unload();
}

//-----------------------------------------------------------------------------
// From: d3dutil.cpp in DX9 SDK
// Name: D3DUtil_SetDeviceCursor
// Desc: Gives the D3D device a cursor with image and hotspot from hCursor.
//-----------------------------------------------------------------------------
HRESULT zz_cursor::CreateCursorSurface ( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DSURFACE9& pCursorSurface, HCURSOR hCursor,
										int& xhotspot_out, int& yhotspot_out, BOOL bAddWatermark )
{
	HRESULT hr = E_FAIL;
	ICONINFO iconinfo;
	BOOL bBWCursor;
	HDC hdcColor = NULL;
	HDC hdcMask = NULL;
	HDC hdcScreen = NULL;
	BITMAP bm;
	DWORD dwWidth;
	DWORD dwHeightSrc;
	DWORD dwHeightDest;
	COLORREF crColor;
	COLORREF crMask;
	UINT x;
	UINT y;
	BITMAPINFO bmi;
	COLORREF* pcrArrayColor = NULL;
	COLORREF* pcrArrayMask = NULL;
	DWORD* pBitmap;
	HGDIOBJ hgdiobjOld;

	ZeroMemory( &iconinfo, sizeof(iconinfo) );
	if( !GetIconInfo( hCursor, &iconinfo ) )
		goto End;

	if (0 == GetObject((HGDIOBJ)iconinfo.hbmMask, sizeof(BITMAP), (LPVOID)&bm))
		goto End;
	dwWidth = bm.bmWidth;
	dwHeightSrc = bm.bmHeight;

	if( iconinfo.hbmColor == NULL )
	{
		bBWCursor = TRUE;
		dwHeightDest = dwHeightSrc / 2;
	}
	else 
	{
		bBWCursor = FALSE;
		dwHeightDest = dwHeightSrc;
	}

	// Create a surface for the fullscreen cursor
	if( FAILED( hr = pd3dDevice->CreateOffscreenPlainSurface( dwWidth, dwHeightDest, 
		D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pCursorSurface, NULL ) ) )
	{
		goto End;
	}

	pcrArrayMask = new DWORD[dwWidth * dwHeightSrc];

	ZeroMemory(&bmi, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = dwWidth;
	bmi.bmiHeader.biHeight = dwHeightSrc;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	hdcScreen = GetDC( NULL );
	hdcMask = CreateCompatibleDC( hdcScreen );
	if( hdcMask == NULL )
	{
		hr = E_FAIL;
		goto End;
	}
	hgdiobjOld = SelectObject(hdcMask, iconinfo.hbmMask);
	GetDIBits(hdcMask, iconinfo.hbmMask, 0, dwHeightSrc, 
		pcrArrayMask, &bmi, DIB_RGB_COLORS);
	SelectObject(hdcMask, hgdiobjOld);

	if (!bBWCursor)
	{
		pcrArrayColor = new DWORD[dwWidth * dwHeightDest];
		hdcColor = CreateCompatibleDC( hdcScreen );
		if( hdcColor == NULL )
		{
			hr = E_FAIL;
			goto End;
		}
		SelectObject(hdcColor, iconinfo.hbmColor);
		GetDIBits(hdcColor, iconinfo.hbmColor, 0, dwHeightDest, 
			pcrArrayColor, &bmi, DIB_RGB_COLORS);
	}

	// Transfer cursor image into the surface
	D3DLOCKED_RECT lr;
	pCursorSurface->LockRect( &lr, NULL, 0 );
	pBitmap = (DWORD*)lr.pBits;
	for( y = 0; y < dwHeightDest; y++ )
	{
		for( x = 0; x < dwWidth; x++ )
		{
			if (bBWCursor)
			{
				crColor = pcrArrayMask[dwWidth*(dwHeightDest-1-y) + x];
				crMask = pcrArrayMask[dwWidth*(dwHeightSrc-1-y) + x];
			}
			else
			{
				crColor = pcrArrayColor[dwWidth*(dwHeightDest-1-y) + x];
				crMask = pcrArrayMask[dwWidth*(dwHeightDest-1-y) + x];
			}
			if (crMask == 0)
				pBitmap[dwWidth*y + x] = 0xff000000 | crColor;
			else
				pBitmap[dwWidth*y + x] = 0x00000000;

			// It may be helpful to make the D3D cursor look slightly 
			// different from the Windows cursor so you can distinguish 
			// between the two when developing/testing code.  When
			// bAddWatermark is TRUE, the following code adds some
			// small grey "D3D" characters to the upper-left corner of
			// the D3D cursor image.
			if( bAddWatermark && x < 12 && y < 5 )
			{
				// 11.. 11.. 11.. .... CCC0
				// 1.1. ..1. 1.1. .... A2A0
				// 1.1. .1.. 1.1. .... A4A0
				// 1.1. ..1. 1.1. .... A2A0
				// 11.. 11.. 11.. .... CCC0

				const WORD wMask[5] = { 0xccc0, 0xa2a0, 0xa4a0, 0xa2a0, 0xccc0 };
				if( wMask[y] & (1 << (15 - x)) )
				{
					pBitmap[dwWidth*y + x] |= 0xff808080;
				}
			}
		}
	}
	pCursorSurface->UnlockRect();

	xhotspot_out = iconinfo.xHotspot;
	yhotspot_out = iconinfo.yHotspot;

	hr = S_OK;

End:
	if( iconinfo.hbmMask != NULL )
		DeleteObject( iconinfo.hbmMask );
	if( iconinfo.hbmColor != NULL )
		DeleteObject( iconinfo.hbmColor );
	if( hdcScreen != NULL )
		ReleaseDC( NULL, hdcScreen );
	if( hdcColor != NULL )
		DeleteDC( hdcColor );
	if( hdcMask != NULL )
		DeleteDC( hdcMask );
	SAFE_DELETE_ARRAY( pcrArrayColor );
	SAFE_DELETE_ARRAY( pcrArrayMask );
	return hr;
}


LPDIRECT3DDEVICE9 zz_cursor::get_device ()
{
	if (!znzin) return NULL;
	if (!znzin->renderer) return NULL;
	if (!znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d))) return NULL;

	return ((zz_renderer_d3d*)(znzin->renderer))->get_device();
}

// create managed objects
bool zz_cursor::init_device_objects ()
{

	return true;
}

// destroy managed objects
bool zz_cursor::delete_device_objects ()
{

	return true;
}

// create unmanaged objects
bool zz_cursor::restore_device_objects ()
{
	LPDIRECT3DDEVICE9 device = get_device();

	if (!device) return true;

	if (!hcursor_) return true; // no cursor is set
	
	d3d_mouse_ = znzin->renderer->support_hw_mouse() && znzin->view->get_fullscreen();

	if (d3d_mouse_ && !surface_) {
		HRESULT hr;

		BOOL watermark;

#ifdef _DEBUG
		watermark = TRUE;
#else
		watermark = FALSE;
#endif

		if (FAILED(hr = CreateCursorSurface(device, surface_, hcursor_, xhotspot_, yhotspot_, watermark /* watermark for debugging */))) {
			zz_assertf(0, "cursor: [%s]->create surface failed. [%s]\n", get_name(), hr);
			return false;
		}

#if (0) // for debugging
		RECT rcClip;

		GetClipCursor(&rcClip);

		rcClip.left = 0;
		rcClip.right = znzin->view->get_width();
		rcClip.top =0;
		rcClip.bottom = znzin->view->get_height();

		ZZ_LOG("RECT (%d, %d, %d, %d)\n", rcClip.left, rcClip.right, rcClip.top, rcClip.bottom );
		
		ClipCursor(&rcClip);
#endif

	}

	if (current == this) {
		show_cursor(true);
	}

	return true;
}

// destroy unmanaged objects
bool zz_cursor::invalidate_device_objects ()
{
	SAFE_RELEASE(surface_);

	if (current == this) {
		current = NULL;
	}

	return true;
}

void zz_cursor::set_property (HCURSOR hcursor_in)
{
	hcursor_ = hcursor_in;
}

void zz_cursor::set_position (int x, int y)
{
	//ZZ_LOG("cursor: set_position(%d, %d) %s\n", x, y, d3d_mouse_ ? "d3d" : "win32" );

	if (!d3d_mouse_) {
		// do not call SetCursorPos();
		return;
	}

	LPDIRECT3DDEVICE9 device = get_device();
	
	if (!device) return;
	if (!surface_) return;

	device->SetCursorPosition(x, y, D3DCURSOR_IMMEDIATE_UPDATE);
}

void zz_cursor::hide_cursor ()
{
	if (!d3d_mouse_) {
		ShowCursor( FALSE );
		return;
	}

	LPDIRECT3DDEVICE9 device = get_device();
	
	if (!device) return;

	device->ShowCursor(false);
}

void zz_cursor::show_cursor (bool show_in)
{
	if (!d3d_mouse_) { // use win32 mouse
		if (current != this) {
			SetCursor( hcursor_ );
		}

		ShowCursor( show_in );
		return;
	}

	LPDIRECT3DDEVICE9 device = get_device();

	if (!device) {
		ZZ_LOG("cursor: show_cursor() failed. no device\n"); // zhotest
		return;
	}
	
	if (!surface_) { // not yet reset
		ZZ_LOG("cursor: show_cursor() failed. no surface\n"); // zhotest
		return;
	}

	HRESULT hr;

	if (show_in) {
		if (current != this) {
			// Set the device cursor
			if( FAILED( hr = device->SetCursorProperties(	xhotspot_, yhotspot_, surface_ ) ) )
			{
				zz_assertf(0, "cursor: setcursorproperties(%s) failed. (%d-%d). [%s]\n",
					get_name(), xhotspot_, yhotspot_, zz_renderer_d3d::get_hresult_string(hr));
				return;
			}
		}
		current = this;
	}
	else if (current == this) {
		current = NULL;
	}

	device->ShowCursor(show_in);
}

bool zz_cursor::load ()
{
	if (init_device_objects()) {
		if (restore_device_objects()) {
			return true;
		}
	}
	return false;
}

bool zz_cursor::unload ()
{
	if (delete_device_objects()) {
		if (invalidate_device_objects()) {
			return true;
		}
	}
	return false;
}