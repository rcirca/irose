#ifndef	__CLASSOGG_H
#define	__CLASSOGG_H
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
//-------------------------------------------------------------------------------------------------

class classOGG {
protected:
	FILE		  *m_fp;
	OggVorbis_File*m_pOVF;

public   :
	classOGG ();
	~classOGG ();

	bool	OpenFile (char *szOGGFile, int iBufferingSec, int iBufferBlock);
	void	CloseFile ();
	bool	ReadData (UINT uiSizeToRead, void *pBuffer, UINT *puiActualBytesWritten);
} ;

//-------------------------------------------------------------------------------------------------
#endif
