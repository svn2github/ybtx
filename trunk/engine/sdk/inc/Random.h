#ifndef _RANDOM_H
#define _RANDOM_H
#include "uniform.h"
#include "TRandomMersenne.h"

/*
 * Please refer to http://www.agner.org/random/randomc.htm for more information.
 */
namespace sqr
{
namespace Random
{
inline int Rand( const int Min, const int Max )
{
	static TRandomMersenne sGlob;
	return sGlob.IRandom( Min, Max );
}
}
}

using namespace sqr::Random;

#endif

