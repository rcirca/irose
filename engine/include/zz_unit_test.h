/** 
 * @file zz_unit_test.h
 * @brief unit test class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-jan-2004
 *
 * $Header: /engine/include/zz_unit_test.h 2     04-01-22 4:39p Zho $
 * $History: zz_unit_test.h $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-22   Time: 4:39p
 * Updated in $/engine/include
 * Added UTFUNC_SKIP
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-01-17   Time: 2:11p
 * Created in $/engine/include
 * Added unit_test class from Applied C++ book.
 */

#ifndef __ZZ_UNIT_TEST_H__
#define __ZZ_UNIT_TEST_H__

//--------------------------------------------------------------------------------
// unit test :
// from Applied C++, addison wesley, 
//--------------------------------------------------------------------------------/ 

#include <string>
#include <vector>
#include <iostream>
#include <windows.h>

class zz_elapsed_time {
private:
	DWORD start;

public:
	zz_elapsed_time ()
	{
		start = timeGetTime();
	}

	long msec ()
	{
		return (timeGetTime() - start);
	}
};

//--------------------------------------------------------------------------------
// zz_unit_test_function is the base class for all unit test
// functions. The macros defined at the bottom of this file will
// construct objects derived from zz_unit_test_function.
//
// When you write a unit test function, it executes as a member
// or an object derived from zz_unit_test_function.  Useful 
// functions:
//
//   ZZ_VERIFY(condition)       A macro wrapped around verify(). If
//                           condition resolves to false, the
//                           unit test will fail.
//   verify_float (d1,d2);    Verifies that d1= d2, within a
//                           small value delta.  If this 
//                           condition is false, the unit test 
//                           will fail.
//   ZZ_VERIFYFLOAT(d1,d2);     Same as verifyFlot() to match the
//                           look of VERIFY().
//   addMessage (string);    Adds the string to our internal
//                           message string.
//
// You should call VERIFY() or VERIFYFLOAT() at least once in 
// each unit test function.  If you don't the result state of
// the function will be eUnknown.
//--------------------------------------------------------------------------------

class zz_unit_test_function {
public:
	zz_unit_test_function(const std::string& name);

	enum e_result { eNotRun, eRunning, eUnknown, eSuccess, eFailure };

	const std::string& name () const { return name_; }

	e_result result () const { return result_; }

	long elapsed () const { return elapsed_; }

	const std::string& message () const { return message_; }

	const std::string& description () const { return description_; }

	std::string result_string () const;

	void set_description (const std::string& s) { description_ = s; }

	void run (bool verbose = false);

protected:
	virtual void test () = 0;

	bool verify (bool state, const std::string& message="");

	// Verifies d1=d2, within a value delta. Used by VERIFYFLOAT()
	bool verify_float (double d1, double d2);

	void add_message (const std::string& message);

	bool verbose_;

	e_result result_;
	std::string name_;
	std::string description_;
	std::string message_;
	long elapsed_;
};


class zz_unit_test {
public:
	// The only instance of this object we create
	static zz_unit_test& g_only ();

	// Run all the unit tests. Returns true if all tests are ok
	bool run (bool verbose = false);

	// Dump results to specified stream
	void dump_results (std::ostream& out);
	
	int size () const { return static_cast<int>(tests_.size());}
	
	// Retrieves the specific test, or NULL if invalid index
	const zz_unit_test_function* retrieve (int index) const;

	// Used by our macro to add another unit test
	void add_test (const std::string& name, zz_unit_test_function * test);
	
	~zz_unit_test();

protected:
	static unsigned long count_;

private:
	zz_unit_test ();  // We will only have one instance, gOnly()

	static zz_unit_test * s_only_;  // Points to our only instance

	std::vector<zz_unit_test_function*> tests_; // Array of tests
	time_t start_, stop_;                    // Start, stop time
};


//--------------------------------------------------------------------------------
// This might look difficult, but it creates a unique class,
// derived from apUnitTestFunction. A static instance is also
// created which will add itself to the array of unit tests in
// the apUnitTest object (there is only a single, global
// instance of apUnitTest created). When the unit tests are run,
// the test() method of each derived object is called. An 
// example unit test function (which always passed) is show 
// below: 
//
//    UTFUNC(test)
//    {
//      ZZ_VERIFY (true);
//    }
//
// The ## is the merging operator.  a##b = ab
// The # is a stringization operator  #a = "a"
//--------------------------------------------------------------------------------

#define UTFUNC(utx)                            \
class UT##utx : public zz_unit_test_function      \
{                                              \
	UT##utx ();                                    \
	static UT##utx s_instance;                      \
	void test ();                                  \
};                                             \
UT##utx UT##utx::s_instance;                    \
UT##utx::UT##utx () : zz_unit_test_function(#utx) \
{                                              \
	zz_unit_test::g_only().add_test(#utx,this);      \
}                                              \
void UT##utx::test ()

#define UTFUNC_SKIP(utx)					\
class UT##utx : public zz_unit_test_function      \
{                                              \
	UT##utx ();                                    \
	static UT##utx s_instance;                      \
	void test ();                                  \
};                                             \
UT##utx UT##utx::s_instance;                    \
UT##utx::UT##utx () : zz_unit_test_function(#utx) \
{                                              \
}                                              \
void UT##utx::test ()

#define ZZ_VERIFY(condition) verify (condition, #condition)
#define ZZ_VERIFYFLOAT(d1,d2) verify_float (d1, d2)

#endif // __ZZ_UNIT_TEST_H__