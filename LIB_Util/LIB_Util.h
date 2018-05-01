#pragma once
#ifndef	__LIB_UTIL_H
#define	__LIB_UTIL_H
//-------------------------------------------------------------------------------------------------


#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif


#ifndef	GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#endif


#define IS_SIGNED(type)				( ((type) (-1)) < ((type) 0) )
#define IS_UNSIGNED(type)			( ((type) (-1)) > ((type) 0) )

#ifndef COMPILE_TIME_ASSERT
	#define COMPILE_TIME_ASSERT(expr)	{	typedef int compile_time_assert_fail[1 - 2 * !(expr)];	}
#endif

#ifndef	COMPILE_TIME_MSG
	#define TO_NAME(x)					#x 
	#define TO_NAME2(x)					TO_NAME(x) 
	#define COMPILE_TIME_MSG(szDESC)	message(__FILE__ "(" TO_NAME2(__LINE__) ") : check! " szDESC) 
#endif

#define	MAX_CHAR				0x07f
#define	MAX_UCHAR				0x0ff
#define	MAX_BYTE				MAX_UCHAR
	#define	MAX_SHORT				0x07fff
#define	MAX_USHORT				0x0ffff
#define	MAX_WORD				MAX_USHORT
#define	MAX_INT					0x07fffffff
#define	MAX_UINT				0x0ffffffff 
#define	MAX_DWORD				MAX_UINT

#define	MUL02(V)				( (V) << 1 )
#define	MUL04(V)				( (V) << 2 )
#define	MUL08(V)				( (V) << 3 )
#define	MUL16(V)				( (V) << 4 )

#define	DIV02(V)				( (V) >> 1 )
#define	DIV04(V)				( (V) >> 2 )
#define	DIV08(V)				( (V) >> 3 )
#define	DIV16(V)				( (V) >> 4 )

//-----------------------------------------------
/*
DECLARE_INSTANCE( CTest ) ==> 
private:
	static CTest *m_pCTest;
public :
	static CTest *Instance();
	void Destroy ();
*/
#define	DECLARE_INSTANCE(class_name)		\
private:									\
	static class_name *m_p##class_name;		\
public:										\
	static class_name *Instance ();			\
	static class_name *GetInstance();		\
	static void Destroy ();					\


#define	DECLARE_INSTANCE2(class_name)		\
private:									\
	static class_name *m_p##class_name;		\
public:										\
	static class_name *GetInstance();		\
	static void Destroy ();					\

//-----------------------------------------------
/*
IMPLEMENT_INSTANCE( CTest ) ==> 
CTest* CTest::m_pCTest=0;
CTest* CTest::Instance ()
{
	if ( !m_pCTest ) m_pCTest = new CTest;
	return m_CTest;
}
void CTest::Destroy ()
{
	if ( m_pCTest ) {
		delete m_pCTest;
		m_pCTest = NULL;
	}
}
*/
#define	IMPLEMENT_INSTANCE(class_name)		\
class_name* class_name::m_p##class_name=0;	\
class_name* class_name::Instance()			\
{	if ( !m_p##class_name )					\
		m_p##class_name = new class_name;	\
	return m_p##class_name;			}		\
class_name* class_name::GetInstance()		\
{	return m_p##class_name;			}		\
void class_name::Destroy()					\
{	if ( m_p##class_name ) {				\
		delete m_p##class_name;				\
		m_p##class_name=0;	}		}		\


#define	IMPLEMENT_INSTANCE2(class_name)		\
class_name* class_name::m_p##class_name=0;	\
class_name* class_name::GetInstance()		\
{	return m_p##class_name;			}		\
void class_name::Destroy()					\
{	if ( m_p##class_name ) {				\
		delete m_p##class_name;				\
		m_p##class_name=0;	}		}		\
//-----------------------------------------------


#ifndef  __BORLANDC__
	inline int round_int (double x)
	{
	int      i;
	static const float round_to_nearest = 0.5f;
	__asm
	{
		fld      x
		fadd     st, st (0)
		fadd     round_to_nearest
		fistp    i
		sar      i, 1
	}

	return (i);
	}
	inline int floor_int (double x)
	{
	int      i;
	static const float round_toward_m_i = -0.5f;
	__asm
		{
		fld      x
		fadd     st, st (0)
		fadd     round_toward_m_i
		fistp    i
		sar      i, 1
	}

	return (i);
	}
	inline int ceil_int (double x)
	{
	int      i;
	static const float round_toward_p_i = -0.5f;
	__asm
	{
		fld      x
		fadd     st, st (0)
		fsubr    round_toward_p_i
		fistp    i
		sar      i, 1
	}

	return (-i);
	}
#endif

//-------------------------------------------------------------------------------------------------
#endif