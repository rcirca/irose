#ifndef ZZ_MINMAX_H
#define ZZ_MINMAX_H

#include <zz_misc.h>

template <class T> class zz_minmax
{
public:
	zz_minmax() { m_min= T(); m_max = T(); }
	zz_minmax(T tmin, T tmax) { m_min= tmin; m_max = tmax; }
	~zz_minmax() { }

	T m_min;
	T m_max;

	T get_random_num_in_range (void) { return(random_number(m_min, m_max)); }
	T get_range (void) { return(abs(m_max-m_min)); }
};

#endif