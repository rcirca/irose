/** 
 * @file zz_assert.h
 * @brief assertion.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    24-jun-2004
 *
 * $Id: zz_visible.h,v 1.1.1.1 2002/09/06 00:44:05 Jiho Choi Exp $
 */

#ifndef __ZZ_ASSERT_H__
#define __ZZ_ASSERT_H__

#define _SHOW_ASSERTIONS_IN_RELEASE

//--------------------------------------------------------------------------------
// internal stuff
//--------------------------------------------------------------------------------

// zz_assert message rendering functions
// @return 1(ignore, ignore assertion and contine), 0(cancel, exit program)
int _zz_assert ( const char * msg, const char * filename, int linenum );

// static variables for linenumber and filename
struct zz_assert_shared_struct {
	static int s_current_line;
	static const char * s_current_file;

	template< typename T >
	void assertf ( T exp, const char * msg_format, ... );

	zz_assert_shared_struct ( const char * filename, int linenum )
	{
		s_current_line = linenum;
		s_current_file = filename;
	}
};

// We cannot assume that T is bool, int, unsigned int, short, or any pointer type.
// That is why we use template.
template< typename T >
void zz_assert_shared_struct::assertf ( T exp, const char * msg_format, ... )
{
	if (exp) return;

	static char buffer[1024];
	
	va_list va;
	va_start(va, msg_format);	
	vsprintf(buffer, msg_format, va);
	va_end(va);	

	if (!_zz_assert(buffer, zz_assert_shared_struct::s_current_file, zz_assert_shared_struct::s_current_line)) {
		_asm { int 3 }
	}
}

//--------------------------------------------------------------------------------
// public stuff
//--------------------------------------------------------------------------------
#if defined(_DEBUG)
	#define _SHOW_ASSERTIONS
#elif defined(_SHOW_ASSERTIONS_IN_RELEASE)
	#define _SHOW_ASSERTIONS
#endif // _debug

//--------------------------------------------------------------------------------
// zz_assert
//--------------------------------------------------------------------------------
// simple assertions as standard assert()
#ifdef _SHOW_ASSERTIONS
#define zz_assert( EXP ) if (!(EXP)) { if (!_zz_assert( #EXP, __FILE__, __LINE__ )) _asm { int 3 } }
#else
#define zz_assert( EXP )
#endif

//		_asm { int 3 } // this will cause the debugger to break here on PC's

//--------------------------------------------------------------------------------
// zz_assertf
//--------------------------------------------------------------------------------
// zz_assert format string version
// usage: zz_assertf( p == NULL, "pointer(%p) is not null!", p );
#ifdef _SHOW_ASSERTIONS
#define zz_assertf zz_assert_shared_struct(__FILE__, __LINE__).assertf
#else
#define zz_assertf if (0) // hope for compiler not to make any code
#endif

// message box
void zz_msgbox ( const char * caption, const char * msg );
void zz_msgboxf ( const char * caption, const char * msg_format, ... );

#endif // __ZZ_ASSERT_H__