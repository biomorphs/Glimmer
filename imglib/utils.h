#ifndef IMGCONVERTER_UTILS_INCLUDED
#define IMGCONVERTER_UTILS_INCLUDED

// This is a collection of small utility functions (asserts, function mocking, etc)

#ifdef _DEBUG	// Should be enabled at the project level
	#define ASSERTS_ENABLED
#endif

#ifdef _RUNNING_TESTS	// Should be defined at the project level when running unit tests
	// Very simple function-level mocking
	#define MOCKABLE virtual
#else
	#define MOCKABLE 
#endif

#ifdef ASSERTS_ENABLED

	// Platform-specific debug break (I didn't test compilation on GCC but afaik it should be ok)
	#ifdef _MSC_VER
		#define DEBUG_BREAK __debugbreak()
	#elif defined(__GNUG__)
		#define DEBUG_BREAK __builtin_trap()
	#endif

	#include <iostream>

	#define Assert( condition, ... )	if( !(condition) )	{ std::cout << "Assertion Failed: " << #condition << std::endl; DEBUG_BREAK; }

#else

	#define Assert( condition, ... )

#endif	//ASSERTS_ENABLED

#endif	//IMGCONVERTER_UTILS_INCLUDED
