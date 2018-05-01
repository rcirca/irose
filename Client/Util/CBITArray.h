#ifndef	__CBIT_ARRAY_H
#define __CBIT_ARRAY_H
typedef unsigned char       BYTE;
#ifndef NULL
	#ifdef __cplusplus
		#define NULL    0
	#else
		#define NULL    ((void *)0)
	#endif
#endif
//-------------------------------------------------------------------------------------------------

class CBITARRAY {
protected:
	BYTE   *m_pDATA;
	int		m_iMaxDataCount;
	int		m_iByteLength;

public   :
	void SetArray (BYTE *pDATA, short nDataByteCnt);
	BYTE*GetArray (void)		{	return m_pDATA;		}
	void ClearAll (void);
	void FillAll (void);
} ;

//-------------------------------------------------------------------------------------------------

class C1BITARRAY : public CBITARRAY {
private :
	enum { DEFAULT_LENGTH=64, BIT_SHIFT=3, WHICH_BIT=0x07 };
	BYTE	m_btBitMask[8];

public :
	C1BITARRAY (int iLength=DEFAULT_LENGTH);
	~C1BITARRAY ();

	BYTE GetBit (int iIndex);
	void SetBit (int iIndex);
	void ClearBit (int iIndex);
	BYTE FlipBit (int iIndex);
} ;

//-------------------------------------------------------------------------------------------------

class C2BITARRAY : public CBITARRAY {
private :
	enum { DEFAULT_LENGTH=64, BIT_SHIFT=2, WHICH_BIT=0x03, MAX_VALUE=0x03 };
	BYTE	m_btBitMask[4];

public :
	C2BITARRAY (int iLength=DEFAULT_LENGTH);
	~C2BITARRAY ();

	BYTE GetValue (int iIndex);
	void SetValue (int iIndex, BYTE btValue);
	void ClearValue (int iIndex);
} ;

//-------------------------------------------------------------------------------------------------

class C4BITARRAY : public CBITARRAY {
private :
	enum { DEFAULT_LENGTH=64, BIT_SHIFT=1, WHICH_BIT=0x01, MAX_VALUE=0x0f };
	BYTE	m_btBitMask[2];

public :
	C4BITARRAY (int iLength=DEFAULT_LENGTH);
	~C4BITARRAY ();

	BYTE GetValue (int iIndex);
	void SetValue (int iIndex, BYTE btValue);
	void ClearValue (int iIndex);
} ;


//-------------------------------------------------------------------------------------------------

#endif