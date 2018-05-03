
#include <windows.h>
#include "IJL.h"
#include "LIB_IJL.h"

#pragma comment (lib, "..\\lib_util\\IJL15.lib")	// Intel JEPG Library ...

//---------------------------------------------------------------------------------------
/*
union tag16RGB {
	struct {
		WORD r : 5;		// LSB
		WORD g : 6;
		WORD b : 5;		// MSB
	} sBGR565;

	struct {
		WORD b    : 5;		// LSB
		WORD g    : 5;
		WORD r    : 5;
		WORD null : 1;		// MSB
	} sRGB555;

	struct {
		WORD b : 5;		// LSB	
		WORD g : 6;
		WORD r : 5;		// MSB
	} sRGB565;

	WORD wI;
} ;
*/
/*
struct tag24RGB {
	unsigned char btR;
	unsigned char btG;
	unsigned char btB;
} ;
//---------------------------------------------------------------------------------------
static WORD Get16RGB (int iPixelFormat, SHORT r, SHORT g, SHORT b)
{
	union tag16RGB uRGB;

	uRGB.wI = 0;
	switch ( iPixelFormat ) {
		case PFM_RGB565 :
            uRGB.sRGB565.r = r >> 3;
            uRGB.sRGB565.b = b >> 3;
            uRGB.sRGB565.g = g >> 2;
			break;
		case PFM_RGB555 :
            uRGB.sRGB555.r = r >> 3;
            uRGB.sRGB555.b = b >> 3;
            uRGB.sRGB555.g = g >> 3;
			break;
		case PFM_BGR565 :
            uRGB.sBGR565.b = r >> 3;
            uRGB.sBGR565.g = g >> 2;
            uRGB.sBGR565.r = b >> 3;
			break;
	}

	return uRGB.wI;
}


//---------------------------------------------------------------------------------------
static struct tag24RGB Get24RGB (int iPixelFormat, WORD w16RGB)
{
	union  tag16RGB	uRGB;
	struct tag24RGB	sRGB;

	uRGB.wI = w16RGB;
	switch ( iPixelFormat ) {
		case PFM_RGB565 :
			sRGB.btR = uRGB.sRGB565.r << 3;
			sRGB.btG = uRGB.sRGB565.g << 2;
			sRGB.btB = uRGB.sRGB565.b << 3;
			break;
		case PFM_RGB555 :
			sRGB.btR = uRGB.sRGB555.r << 3;
			sRGB.btG = uRGB.sRGB555.g << 3;
			sRGB.btB = uRGB.sRGB555.b << 3;
			break;
		case PFM_BGR565 :
			sRGB.btR = uRGB.sBGR565.b << 3;
			sRGB.btG = uRGB.sBGR565.g << 2;
			sRGB.btB = uRGB.sBGR565.r << 3;
			break;
	}

	return sRGB;
}


//---------------------------------------------------------------------------------------
void IJL_DeleteJPEGBuffer (struct tagJPEGBUFFER *pJpegBuffer)
{
	if ( pJpegBuffer->m_pWordBuffer )
		delete[] pJpegBuffer->m_pWordBuffer;

	delete pJpegBuffer;
}


//---------------------------------------------------------------------------------------
//
// An example using the Intel(R) JPEG Library:
// -- Decode image from a JFIF buffer.
//
struct tagJPEGBUFFER *IJL_DecodeFromJPEGBuffer (char *szFileName, int iPixelFormat, long lFilePtr)
{
	FILE				 *fp;
	long				  lFPos;
	struct tagJPEGBUFFER *pImgBuffer;

	unsigned char   *lpJpgBuffer;
	unsigned long	dwJpgBufferSize;
	bool	bres;
	IJLERR  jerr;
	unsigned long   dwWholeImageSize;
	unsigned char   *lpTemp;

	if ( lFilePtr >= 0 ) {
		lFPos = lFilePtr + sizeof(long);
	} else
		lFPos = 0;

    if ( (fp= fopen (szFileName, "rb")) == NULL ) {
		iCARUS_LogString (LOG_NORMAL, "ImageSet File[ %s/%d ] not found ...\n", szFileName, lFilePtr);
		return NULL;
	}

    if ( lFilePtr >= 0 ) {
	    fseek (fp, lFilePtr, SEEK_SET);
		fread (&dwJpgBufferSize, 1, sizeof(long), fp);
	} else {
		fseek (fp, 0L, SEEK_END);
		dwJpgBufferSize = ftell (fp);
	}
    fseek (fp, lFPos, SEEK_SET);

	lpJpgBuffer = new unsigned char[ dwJpgBufferSize ];
	if ( lpJpgBuffer == NULL ) {
		fclose (fp);
		return NULL;
	}
	fread (lpJpgBuffer, sizeof(unsigned char), dwJpgBufferSize, fp);
	fclose (fp);

	//---------------------------------------------------------------------------

	pImgBuffer = new struct tagJPEGBUFFER;
	ZeroMemory (pImgBuffer, sizeof(struct tagJPEGBUFFER));

	// Allocate the IJL JPEG_CORE_PROPERTIES structure.
	JPEG_CORE_PROPERTIES jcprops;

	bres = TRUE;
	__try
	{
		// Initialize the Intel(R) JPEG Library.
		jerr = ijlInit(&jcprops);
		if ( IJL_OK != jerr )
		{
			bres = FALSE;
			__leave;
		}

		// Get information on the JPEG image
		// (i.e., width, height, and channels).
		jcprops.JPGFile      = NULL;
		jcprops.JPGunsigned chars     = lpJpgBuffer;
		jcprops.JPGSizeunsigned chars = dwJpgBufferSize;
		jerr = ijlRead(&jcprops, IJL_JBUFF_READPARAMS);
		if (IJL_OK != jerr )
		{
			bres = FALSE;
			__leave;
		}

		// Set the JPG color space ... this will always be
		// somewhat of an educated guess at best because JPEG
		// is "color blind" (i.e., nothing in the bit stream
		// tells you what color space the data was encoded from).
		// However, in this example we assume that we are
		// reading JFIF files which means that 3 channel images
		// are in the YCbCr color space and 1 channel images are
		// in the Y color space.
		switch(jcprops.JPGChannels)
		{
			case 1:
			{
				jcprops.JPGColor    = IJL_G;
				jcprops.DIBColor    = IJL_RGB;
				jcprops.DIBChannels = 3;
				break;
			}
			case 3:
			{
				jcprops.JPGColor    = IJL_YCBCR;
				jcprops.DIBColor    = IJL_RGB;
				jcprops.DIBChannels = 3;
				break;
			}
			default:
			{
				// This catches everything else, but no
				// color twist will be performed by the IJL.
				jcprops.JPGColor    = IJL_OTHER;
				jcprops.DIBColor    = IJL_OTHER;
				jcprops.DIBChannels = jcprops.JPGChannels;
				break;
			}
		}

		// Compute size of desired pixel buffer.
		dwWholeImageSize = jcprops.JPGWidth * jcprops.JPGHeight * jcprops.DIBChannels;
		// Allocate memory to hold the decompressed image data.
		lpTemp = new unsigned char[ dwWholeImageSize ];

		if ( NULL == lpTemp )
		{
			bres = FALSE;
			__leave;
		}

		// Set up the info on the desired DIB properties.
		jcprops.DIBWidth    = jcprops.JPGWidth;
		jcprops.DIBHeight   = jcprops.JPGHeight;
		jcprops.DIBPadunsigned chars = 0;
		jcprops.DIBunsigned chars    = lpTemp;
		// Now get the actual JPEG image data into the pixel buffer.
		jerr = ijlRead(&jcprops, IJL_JBUFF_READWHOLEIMAGE);
		if ( IJL_OK != jerr )
		{
			bres = FALSE;
			__leave;
		}
	} // __try

	__finally
	{
		if ( lpJpgBuffer ) 
			delete[] lpJpgBuffer;

		if ( FALSE == bres )
		{
			if ( NULL != lpTemp )
			{
				delete [] lpTemp;
				lpTemp = NULL;
			}
		}

		// Clean up the Intel(R) JPEG Library.
		ijlFree(&jcprops);

		pImgBuffer->m_wWidth			= jcprops.DIBWidth;
		pImgBuffer->m_wHeight			= jcprops.DIBHeight;
		pImgBuffer->m_wNumberOfChannels = jcprops.DIBChannels;
	} // __finally

	if ( bres ) {
		pImgBuffer->m_pWordBuffer = new WORD[ jcprops.DIBWidth*jcprops.DIBHeight ];

		// 24 Bit RGB --> 16 BIT !!!
		short nX, nY;
		unsigned char  btR, btG, btB, *p24RGB;
		
		p24RGB = lpTemp;
		for (nY=0; nY<pImgBuffer->m_wHeight; nY++)
			for (nX=0; nX<pImgBuffer->m_wWidth; nX++) {
				btR = *p24RGB ++;
				btG = *p24RGB ++;
				btB = *p24RGB ++;

				pImgBuffer->m_pWordBuffer[ nY*pImgBuffer->m_wWidth + nX ] = Get16RGB (iPixelFormat, btR, btG, btB);
			}

		delete[] lpTemp;

		return pImgBuffer;
	}

	delete pImgBuffer;

	return NULL;
} // DecodeFromJPEGBuffer()
*/

//---------------------------------------------------------------------------------------
bool IJL_EncodeToJPEGFile (char *lpszPathName, unsigned long dwWidth, unsigned long dwHeight, unsigned char *pRGB)
{
	bool	bres;
	IJLERR	jerr;
	unsigned long	dwRgbBufferSize;
	unsigned char   *lpTemp;

	// Allocate the IJL JPEG_CORE_PROPERTIES structure.
	JPEG_CORE_PROPERTIES jcprops;

	bres = true;
	__try
	{
		// Initialize the Intel(R) JPEG Library.
		jerr = ijlInit(&jcprops);
		if ( IJL_OK != jerr )
		{
			bres = false;
			__leave;
		}

		dwRgbBufferSize = dwWidth * dwHeight * 3;
		lpTemp = new unsigned char [ dwRgbBufferSize ];
		if ( NULL == lpTemp )
		{
			bres = false;
			__leave;
		}

		/*
		WORD   w16RGB, wX, wY;
		struct tag24RGB s24RGB;

		for (wY=0; wY<dwHeight; wY++) 
			for (wX=0; wX<dwWidth; wX++) {
				w16RGB = *( p16RGB + wY*dwWidth + wX );
				s24RGB = Get24RGB (iPixelFormat, w16RGB);
				*( lpTemp + wY*3*dwWidth + wX*3 + 0 ) = s24RGB.btR;
				*( lpTemp + wY*3*dwWidth + wX*3 + 1 ) = s24RGB.btG;
				*( lpTemp + wY*3*dwWidth + wX*3 + 2 ) = s24RGB.btB;
			}
		*/
		lpTemp = pRGB;

		// Set up information to write from the pixel buffer.
		jcprops.DIBWidth	= dwWidth;
		jcprops.DIBHeight	= dwHeight; // Implies a bottom-up DIB.
		jcprops.DIBBytes	= lpTemp;
		jcprops.DIBPadBytes = 0;
		jcprops.DIBChannels = 3;
		jcprops.DIBColor	= IJL_RGB;
		jcprops.JPGWidth	= dwWidth;
		jcprops.JPGHeight	= dwHeight;
		jcprops.JPGFile		= const_cast<LPSTR>(lpszPathName);
		jcprops.JPGChannels = 3;
		jcprops.JPGColor	= IJL_YCBCR;
		jcprops.JPGSubsampling = IJL_411; // 4:1:1 subsampling.
		jcprops.jquality	= 100; // Select "good" image quality
		// Write the actual JPEG image from the pixel buffer.
		jerr = ijlWrite(&jcprops,IJL_JFILE_WRITEWHOLEIMAGE);

		if ( IJL_OK != jerr )
		{
			bres = false;
			__leave;
		}
	} // __try

	__finally
	{
		if ( NULL != lpTemp )
		{
			delete[] lpTemp;
			lpTemp = NULL;
		} 

		// Clean up the Intel(R) JPEG Library.
		ijlFree(&jcprops);
	}

	return bres;
} // EncodeToJPEGFile ()



//----------------------------------------------------------
// An example using the IntelR JPEG Library:
// -- Encode a JFIF file from Windows DIB.
//----------------------------------------------------------
/*
BOOL EncodeJPGFileFromDIB (LPCSTR lpszPathName, BITMAPINFOHEADER* bmih)
{
	BOOL	bres;
	IJLERR	jerr;
	unsigned long	dib_pad_unsigned chars;
	// Allocate the IJL JPEG_CORE_PROPERTIES structure.
	JPEG_CORE_PROPERTIES jcprops;

	bres = TRUE;
	__try
	{
		// Initialize the IntelR JPEG Library.
		jerr = ijlInit(&jcprops);
		if(IJL_OK != jerr)
		{
			bres = FALSE;
			__leave;
		}
		if(bmih->biBitCount != 24)
		{
			// not supported palette images
			bres = FALSE;
			__leave;
		}
		dib_pad_unsigned chars = IJL_DIB_PAD_unsigned charS(bmih->biWidth,3);
		// Set up information to write from the pixel buffer.
		jcprops.DIBWidth = bmih->biWidth;
		jcprops.DIBHeight = bmih->biHeight; // Implies a bottom-up DIB.
		jcprops.DIBunsigned chars = reinterpret_cast<unsigned char*>(bmih) + sizeof(BITMAPINFOHEADER);
		jcprops.DIBPadunsigned chars = dib_pad_unsigned chars;
		// Note: the following are default values and thus
		// do not need to be set.
		jcprops.DIBChannels = 3;
		jcprops.DIBColor = IJL_BGR;
		jcprops.JPGFile = const_cast<LPSTR>(lpszPathName);
		// Specify JPEG file creation parameters.
		jcprops.JPGWidth = bmih->biWidth;
		jcprops.JPGHeight = bmih->biHeight;
		// Note: the following are default values and thus
		// do not need to be set.
		jcprops.JPGChannels = 3;
		jcprops.JPGColor = IJL_YCBCR;
		jcprops.JPGSubsampling = IJL_411; // 4:1:1 subsampling.
		jcprops.jquality = 75; // Select "good" image quality
		// Write the actual JPEG image from the pixel buffer.
		jerr = ijlWrite(&jcprops,IJL_JFILE_WRITEWHOLEIMAGE);
		if(IJL_OK!=jerr)
		{
			bres=FALSE;
			__leave;
		}
	}//__try

	__finally
	{
		//CleanuptheIntelRJPEGLibrary.
		ijlFree(&jcprops);
	}

	return bres;
}//EncodeJPGFileFromDIB()
*/

/*
BOOL EncodeToJPEGBuffer (unsigned char* lpRgbBuffer, unsigned long dwWidth, unsigned long dwHeight, unsigned char** lppJpgBuffer, unsigned long* lpdwJpgBufferSize)
{
	BOOL	bres;
	IJLERR	jerr;
	unsigned long	dwRgbBufferSize;
	unsigned char   *lpTemp;

	// Allocate the IJL JPEG_CORE_PROPERTIES structure.
	JPEG_CORE_PROPERTIES jcprops;
	bres = TRUE;
	__try
	{
		// Initialize the Intel(R) JPEG Library.
		jerr = ijlInit(&jcprops);
		if ( IJL_OK != jerr )
		{
			bres = FALSE;
			__leave;
		}

		dwRgbBufferSize = dwWidth * dwHeight * 3;
		lpTemp = new unsigned char [ dwRgbBufferSize ];
		if ( NULL == lpTemp )
		{
			bres = FALSE;
			__leave;
		}

		// Set up information to write from the pixel buffer.
		jcprops.DIBWidth	= dwWidth;
		jcprops.DIBHeight	= dwHeight; // Implies a bottom-up DIB.
		jcprops.DIBunsigned chars	= lpRgbBuffer;
		jcprops.DIBPadunsigned chars = 0;
		jcprops.DIBChannels = 3;
		jcprops.DIBColor	= IJL_RGB;
		jcprops.JPGWidth	= dwWidth;
		jcprops.JPGHeight	= dwHeight;
		jcprops.JPGFile		= const_cast<LPSTR>(lpszPathName);
		jcprops.JPGunsigned chars	= NULL;
//		jcprops.JPGFile		= NULL;
//		jcprops.JPGunsigned chars	= lpTemp;
//		jcprops.JPGSizeunsigned chars= dwRgbBufferSize;
		jcprops.JPGChannels = 3;
		jcprops.JPGColor	= IJL_YCBCR;
		jcprops.JPGSubsampling = IJL_411; // 4:1:1 subsampling.
		jcprops.jquality	= 75; // Select "good" image quality
		// Write the actual JPEG image from the pixel buffer.
//		jerr = ijlWrite(&jcprops,IJL_JBUFF_WRITEWHOLEIMAGE);
		jerr = ijlWrite(&jcprops,IJL_JFILE_WRITEWHOLEIMAGE);

		if(IJL_OK != jerr)
		{
			bres = FALSE;
			__leave;
		}
	} // __try

	__finally
	{
		if ( FALSE == bres )
		{
			if ( NULL != lpTemp )
			{
				delete[] lpTemp;
				lpTemp = NULL;
			}
		}

//		*lppJpgBuffer	   = lpTemp;
//		*lpdwJpgBufferSize = jcprops.JPGSizeunsigned chars;
		// Clean up the Intel(R) JPEG Library.
		ijlFree(&jcprops);
	}

	return bres;
} // EncodeToJPEGBuffer()
*/



/*
#include "ijl.h"	


bool IJL_BMP2JPEG (char *szBMPFile, char *szJPGFile)
{
	JPEG_CORE_PROPERTIES jcprops;
	ijlInit( &jcprops );

	HANDLE hbitmapfile;
	hbitmapfile = ::CreateFile (
		szBMPFile,
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		0, 0 );

	HANDLE hFileMappingObject;

	hFileMappingObject = ::CreateFileMapping (
		hbitmapfile,
		0,
		PAGE_READONLY,
		0, 0,
		0 );

	unsigned char *bitmapptr;
	bitmapptr = (unsigned char*) ::MapViewOfFile (
		hFileMappingObject,
		FILE_MAP_READ,
		0, 0, 0 );

	BITMAPFILEHEADER *bmfh = (BITMAPFILEHEADER*) bitmapptr;
	BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER*)( bitmapptr + sizeof(BITMAPFILEHEADER) );

	long DIBLineSize = (bmih->biWidth * 3 ) / 4 * 4;

	jcprops.DIBChannels = 3;
	jcprops.DIBColor = IJL_RGB;//BGR;
	jcprops.DIBHeight = bmih->biHeight;
	jcprops.DIBWidth = bmih->biWidth;
	jcprops.DIBPadBytes = IJL_DIB_PAD_BYTES( jcprops.DIBWidth, jcprops.DIBChannels );


	jcprops.JPGHeight = bmih->biHeight;
	jcprops.JPGWidth = bmih->biWidth;

	jcprops.JPGFile = szJPGFile;

	jcprops.DIBBytes = bitmapptr + sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );

	ijlWrite (&jcprops, IJL_JFILE_WRITEWHOLEIMAGE );
	ijlFree( &jcprops );

	::UnmapViewOfFile( bitmapptr );
	::CloseHandle( hFileMappingObject );
	::CloseHandle( hbitmapfile );

	return true;
}




bool IJL_BMP2JPEG2 (char *input_filename, char *output_filename)
{
	FILE* input_file;

	input_file = fopen( input_filename );


	JPEG_CORE_PROPERTIES jcprops;
	ijlInit( &jcprops );

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;

	fread( &bmfh,	1,	sizeof(BITMAPFILEHEADER),	input_file);
	fread( &bmih,	1,	sizeof(BITMAPINFOHEADER),	input_file);


	long DIBLineSize = (bmih.biWidth * 3 ) / 4 * 4;

	jcprops.DIBChannels = 3;
	jcprops.DIBColor	= IJL_BGR;
	jcprops.DIBHeight	= bmih.biHeight;
	jcprops.DIBWidth	= bmih.biWidth;
	jcprops.DIBPadBytes = IJL_DIB_PAD_BYTES( jcprops.DIBWidth, jcprops.DIBChannels );

	jcprops.JPGHeight = bmih.biHeight;
	jcprops.JPGWidth  = bmih.biWidth;

	jcprops.JPGFile = szJPGFile;

	unsigned char* DIBBuffer = new signed char[ DIBLineSize * bmih.biHeight ];
	fread( DIBBuffer,	1,	DIBLineSize * bmih.biHeight,	input_file );

	jcprops.DIBBytes = DIBBuffer;

	ijlWrite (&jcprops, IJL_JFILE_WRITEWHOLEIMAGE );
	ijlFree( &jcprops );

	delete[] DIBBuffer;

	fclose( input_file );

	return true;
}
*/
