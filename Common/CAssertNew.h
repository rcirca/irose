#ifndef CASSERTNEW_H
#define CASSERTNEW_H

#if defined( _DEBUG )
extern bool CustomAssertFunction( bool, char*, int, char*, bool & );

#define Assert( exp, description ) \
	{ static bool ignoreAlways = false; \
		if( !ignoreAlways ) { \
			if( CustomAssertFunction( exp, description, \
							__LINE__, __FILE__, ignoreAlways ) ) \
			{ _asm { int 3 } } \
		} \
	}
#else
#define Assert( exp, description )
#endif

#endif