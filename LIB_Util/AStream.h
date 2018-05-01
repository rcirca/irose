#ifndef	__ASTREAM_H
#define	__ASTREAM_H
//-------------------------------------------------------------------------------------------------
/*
In TStream

~TStream
		CopyFrom
		FixupResourceHeader
		Read
		ReadBuffer
		ReadComponent
		ReadComponentRes
		ReadResHeader
		Seek
		SetSize
		TStream
		Write
		WriteBuffer
		WriteComponent
		WriteComponentRes
		WriteDescendent
		WriteDescendentRes

		WriteResourceHeader
*/
class AStream {
protected:
public :
	virtual int Read(void *pBuffer, int iCount) = 0;
	virtual int Write(const void *pBuffer, int iCount) = 0;
	virtual int Seek(long lOffset, int iOrigin) = 0;
	virtual int Position () = 0;
	int ReadInteger( int &iValue )	{	return Read( &iValue, sizeof(int) );	}
	int ReadFloat( float &fValue )	{	return Read( &fValue, sizeof(float) );	}
	
} ;

/*
class AMemoryStream : public AStream {
} ;
*/

class AFileStream : public AStream {
private:
	FILE *m_FP;

public :
	AFileStream( const char *szFileName, const char *szMode );
	~AFileStream ();

	// inherited from AStream
	int Read(void *pBuffer, int iCount);
	int Write(const void *pBuffer, int iCount);
	int Seek(long lOffset, int iOrigin);
	int Position ();
} ;

//-------------------------------------------------------------------------------------------------
#endif