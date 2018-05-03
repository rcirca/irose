/** 
 * @file zz_unit_test.cpp
 * @brief unit test class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-jan-2004
 *
 * $Header: /engine/src/zz_unit_test.cpp 4     04-02-27 5:54p Zho $
 * $History: zz_unit_test.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-27   Time: 5:54p
 * Updated in $/engine/src
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-28   Time: 2:15p
 * Updated in $/engine/src
 * more bug-findable version.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-01-17   Time: 2:11p
 * Created in $/engine/src
 * Added unit_test class from Applied C++ book.
 */

#ifdef ZZ_USE_UNITTEST

#include "zz_tier0.h"
#include ".\zz_unit_test.h"
#include <exception>
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdio.h>

#pragma comment (lib, "Winmm.lib") // for timeGetTime()

zz_unit_test_function::zz_unit_test_function (const std::string& name)
: result_ (eNotRun), name_ (name), verbose_ (false), elapsed_ (0)
{}

void zz_unit_test_function::run (bool verbose)
{
	std::string error;
	verbose_ = verbose;

	// Run the test
	if (verbose) 
		std::cout << "Running " << name_.c_str() << std::endl;
	result_ = eRunning;

	zz_elapsed_time time;
	try {
		test ();
	}
	catch (const std::exception& ex) {
		// We caught an STL exception
		error = std::string("Exception '") + ex.what() + "' caught";
		if (verbose)
			std::cout << error << " in " << name_.c_str() << std::endl;
		add_message (error);
		result_ = eFailure;
	}
	catch (...) {
		// We caught an unknown exception
		error = "Unknown exception caught";
		if (verbose)
			std::cout << error << " in " << name_.c_str() << std::endl;
		add_message (error);
		result_ = eFailure;
	}
	elapsed_ = time.msec ();

	// Make sure the test() function set a result or set eUnknown
	if (result_ != eSuccess && result_ != eFailure)
		result_ = eUnknown;

	if (verbose_) {
		std::cout << "  Result of " << name_.c_str() << " is " 
			<< result_string().c_str() << " (" << elapsed_ 
			<< " msec)" << std::endl;
		if (message_.size() > 0)
			std::cout << "  Messages: " << message_.c_str() 
			<< std::endl;
	}
}


bool zz_unit_test_function::verify (bool state, const std::string& message)
{
	if (!state) {
		result_ = eFailure;
		add_message (message);
		if (verbose_) 
			std::cout << " FAILURE " << name_.c_str() << " : " 
			<< message.c_str() << std::endl;
	}
	else if (result_ != eFailure)
		// Make sure we mark the unit test success, if possible.
		result_ = eSuccess;
	return state;
}

bool zz_unit_test_function::verify_float (double d1, double d2)
{
	double delta = fabs (d1-d2);
	if (delta > 1.e-5) {
		char buffer[128];
		sprintf (buffer, "%.11f != %.11f", d1, d2);
		return verify (false, buffer);
	}

	return verify (true);
}


void zz_unit_test_function::add_message (const std::string& message)
{
	if (message_.size() > 0) message_ += "\n";
	message_ += message;
}


// Not a const reference because it isn't safe
std::string zz_unit_test_function::result_string () const
{
	switch (result_)
	{
	case eNotRun:
		return "Not Run";
	case eRunning:
		return "Running";
	case eUnknown:
		return "Unknown";
	case eSuccess:
		return "Success";
	case eFailure:
		return "***** Failure *****";
	default:
		break;
	}

	return "Unknown Result";
}

zz_unit_test * zz_unit_test::s_only_ = NULL;

unsigned long zz_unit_test::count_ = 0;

zz_unit_test& zz_unit_test::g_only ()
{
	if (!s_only_)  s_only_ = new zz_unit_test ();
	return *s_only_;
}


zz_unit_test::zz_unit_test () : start_ (0), stop_ (0)
{}


zz_unit_test::~zz_unit_test ()
{
	--count_;
	if (count_ == 0) {
		delete s_only_;
		s_only_ = 0;
	}
}

void zz_unit_test::add_test (const std::string& /*name*/, 
						  zz_unit_test_function* test)
{
	// Keep track of this unit test object to run
	tests_.push_back (test);
	count_++;
}

bool zz_unit_test::run (bool verbose)
{
	bool state = true;
	start_ = time(0);

	for (unsigned int i=0; i<tests_.size(); i++) {
		// Fetch the next test to run and run it
		zz_unit_test_function* test = tests_[i];
		test->run (verbose);
		if (test->result() != zz_unit_test_function::eSuccess) 
			state = false;
	}

	stop_ = time(0);
	return state;
}


const zz_unit_test_function* zz_unit_test::retrieve (int index) const
{
	if (index < 0 || index >= size()) return 0;
	return tests_[index];
}


void zz_unit_test::dump_results (std::ostream& out)
{
	int nSuccess = 0;
	int nFailed  = 0;
	int nOther   = 0;

	out << "Unit Test started at  " << ctime(&start_) << std::endl;
	out << "Unit Test finished at " << ctime(&stop_) << std::endl;

	for (unsigned int i=0; i<tests_.size(); i++) {
		zz_unit_test_function* test = tests_[i];

		switch (test->result())
		{
		case zz_unit_test_function::eSuccess:
			nSuccess++;
			break;
		case zz_unit_test_function::eFailure:
			nFailed++;
			break;
		default:
			nOther++;
			break;
		};

		out << "Test " << i+1 << ": " << test->result_string().c_str() 
			<< " : " << test->name().c_str() << " : ";
		if (test->description().size() > 0)
			out << test->description().c_str();
		out << " : " << test->elapsed() << " msec" << std::endl;
		if (test->message().size() > 0)
			out << "  Messages: " << std::endl 
			<< test->message().c_str() << std::endl;
	}

	out << std::endl << std::endl;
	out << "Passed: " << nSuccess << ", Failed: " << nFailed 
		<< ", Other: " << nOther << std::endl;
}

#endif // ZZ_USE_UNITTEST