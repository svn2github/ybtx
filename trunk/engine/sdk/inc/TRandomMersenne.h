#ifndef _RANDOM_MERSENNE_H
#define _RANDOM_MERSENNE_H
#include <ctime>
#include "CommonDefs.h"

namespace sqr
{

class COMMON_API TRandomMersenne
{                // encapsulate random number generator
#if 0
	// define constants for MT11213A:
	// (32 bit constants cannot be defined as enum in 16-bit compilers)
#define MERS_N   351
#define MERS_M   175
#define MERS_R   19
#define MERS_U   11
#define MERS_S   7
#define MERS_T   15
#define MERS_L   17
#define MERS_A   0xE4BD75F5
#define MERS_B   0x655E5280
#define MERS_C   0xFFD58000
#else
	// or constants for MT19937:
#define MERS_N   624
#define MERS_M   397
#define MERS_R   31
#define MERS_U   11
#define MERS_S   7
#define MERS_T   15
#define MERS_L   18
#define MERS_A   0x9908B0DF
#define MERS_B   0x9D2C5680
#define MERS_C   0xEFC60000
#endif
public:
	TRandomMersenne(uint32 seed)         // constructor
	{
		RandomInit(seed);
	}

	TRandomMersenne()
	{
		RandomInit( uint32(time(0)) );
	}

	void RandomInit(uint32 seed);        // re-seed
	void RandomInitByArray(uint32 seeds[], int length); // seed by more than 32 bits
	int IRandom(int min, int max);       // output random integer
	double Random();                     // output random float
	uint32 BRandom();                    // output random bits
private:
	uint32 mt[MERS_N];                   // state vector
	int mti;                             // index into mt
	enum TArch {RANDOM_LITTLEENDIAN, RANDOM_BIGENDIAN, RANDOM_NONIEEE};
	TArch Architecture;                  // conversion to float depends on computer architecture
};

}



#endif

