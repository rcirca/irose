
#include <windows.h>
#include <stdio.h>

#include "classOGG.h"

//-------------------------------------------------------------------------------------------------
classOGG::classOGG ()
{
	m_fp = NULL;
	m_pOVF = NULL;
}
classOGG::~classOGG ()
{
	this->CloseFile ();
}
void classOGG::CloseFile ()
{
	if ( m_pOVF ) {
		::ov_clear( m_pOVF );
		m_pOVF = NULL;
	}

	if ( m_fp ) {
		fclose( m_fp );
		m_fp = NULL;
	}
}

bool classOGG::OpenFile (char *szOGGFile, int iBufferingSec, int iBufferBlock)
{
	this->CloseFile ();

	m_fp = fopen( szOGGFile, "rb" );
	if ( !m_fp )
		return false;

	m_pOVF = new OggVorbis_File;

	// int ov_open(FILE *f,OggVorbis_File *vf,char *initial,long ibytes);
	if ( ::ov_open( m_fp, m_pOVF, NULL, 0 ) ) {
		this->CloseFile ();
		return false;
	}

	/*
    char **ppComments = ov_comment(&sVF,-1)->user_comments;
    vorbis_info *vi=ov_info( &sVF,-1);
    while( *ppComments ) {
      // fprintf(stderr,"%s\n", *ptr);
      ++ppComments;
    }

    fprintf(stderr,"\nBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
    fprintf(stderr,"\nDecoded length: %ld samples\n", (long)ov_pcm_total(&vf,-1));
    fprintf(stderr,"Encoded by: %s\n\n",ov_comment(&vf,-1)->vendor);
	*/

	return true;
}

//-------------------------------------------------------------------------------------------------
bool classOGG::ReadData (UINT uiSizeToRead, void *pBuffer, UINT *puiActualBytesWritten)
{
	/*
	long ov_read(OggVorbis_File *vf, char *buffer, int length, int bigendianp, int word, int sgned, int *bitstream);
	Parameters ::
		vf			A pointer to the OggVorbis_File structure--this is used for ALL the externally visible libvorbisfile functions. 
		buffer		A pointer to an output buffer. The decoded output is inserted into this buffer. 
		length		Number of bytes to be read into the buffer. Should be the same size as the buffer. A typical value is 4096. 
		bigendianp	Specifies big or little endian byte packing. 0 for little endian, 1 for b ig endian. Typical value is 0. 
		word		Specifies word size. Possible arguments are 1 for 8-bit samples, or 2 or 16-bit samples. Typical value is 2. 
		sgned		Signed or unsigned data. 0 for unsigned, 1 for signed. Typically 1. 
		bitstream	A pointer to the number of the current logical bitstream. 
	Return Values ::
		OV_HOLE		indicates there was an interruption in the data. 
					(one of: garbage between pages, loss of sync followed by recapture, or a corrupt page) 
		OV_EBADLINK indicates that an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt. 
		0			indicates EOF 
		n			indicates actual number of bytes read. ov_read() will decode at most one vorbis packet per invocation, so the value returned will generally be less than length. 
	*/
	long lReadBytes;
	int	iBitStream;

	lReadBytes = ::ov_read( m_pOVF, (char*)pBuffer, uiSizeToRead, 0, 2, 1, &iBitStream);
	if ( lReadBytes == 0 ) {
		// EOF
	} else
	if ( lReadBytes < 0 ) {
		// Error in Stream ...
		return false;
	}
	
	*puiActualBytesWritten = lReadBytes;

	return true;
}
  
//-------------------------------------------------------------------------------------------------
