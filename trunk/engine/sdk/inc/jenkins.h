#ifndef _HASH_JENKINS_H
#define _HASH_JENKINS_H
/*
 * copy and modification from lookup2.c  from Bob Jenkins.
 * please refer to http://burtleburtle.net/bob/hash/ for more information.
 */

/*
   --------------------------------------------------------------------
   lookup2.c, by Bob Jenkins, December 1996, Public Domain.
   hash(), hash2(), hash3, and mix() are externally useful functions.
   Routines to test the hash are included if SELF_TEST is defined.
   You can use this free for any purpose.  It has no warranty.
   --------------------------------------------------------------------
   */
#include "BaseTypes.h"

namespace sqr
{
namespace hash
{
namespace jenkins
{

#define hashsize(n) ((uint32)1<<(n))
#define hashmask(n) (hashsize(n)-1)

/*
   --------------------------------------------------------------------
   mix -- mix 3 32-bit values reversibly.
   For every delta with one or two bit set, and the deltas of all three
   high bits or all three low bits, whether the original value of a,b,c
   is almost all zero or is uniformly distributed,
 * If mix() is run forward or backward, at least 32 bits in a,b,c
 have at least 1/4 probability of changing.
 * If mix() is run forward, every bit of c will change between 1/3 and
 2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
 mix() was built out of 36 single-cycle latency instructions in a
 structure that could supported 2x parallelism, like so:
 a -= b;
 a -= c; x = (c>>13);
 b -= c; a ^= x;
 b -= a; x = (a<<8);
 c -= a; b ^= x;
 c -= b; x = (b>>13);
 ...
 Unfortunately, superscalar Pentiums and Sparcs can't take advantage
 of that parallelism.  They've also turned some of those single-cycle
 latency instructions into multi-cycle latency instructions.  Still,
 this is the fastest good hash I could find.  There were about 2^^68
 to choose from.  I only looked at a billion or so.
 --------------------------------------------------------------------
 */
#define mix(a,b,c) \
			{ \
				a -= b; a -= c; a ^= (c>>13); \
				b -= c; b -= a; b ^= (a<<8); \
				c -= a; c -= b; c ^= (b>>13); \
				a -= b; a -= c; a ^= (c>>12);  \
				b -= c; b -= a; b ^= (a<<16); \
				c -= a; c -= b; c ^= (b>>5); \
				a -= b; a -= c; a ^= (c>>3);  \
				b -= c; b -= a; b ^= (a<<10); \
				c -= a; c -= b; c ^= (b>>15); \
			}

/* same, but slower, works on systems that might have 8 byte uint32's */
#define mix2(a,b,c) \
			{ \
				a -= b; a -= c; a ^= (c>>13); \
				b -= c; b -= a; b ^= (a<< 8); \
				c -= a; c -= b; c ^= ((b&0xffffffff)>>13); \
				a -= b; a -= c; a ^= ((c&0xffffffff)>>12); \
				b -= c; b -= a; b = (b ^ (a<<16)) & 0xffffffff; \
				c -= a; c -= b; c = (c ^ (b>> 5)) & 0xffffffff; \
				a -= b; a -= c; a = (a ^ (c>> 3)) & 0xffffffff; \
				b -= c; b -= a; b = (b ^ (a<<10)) & 0xffffffff; \
				c -= a; c -= b; c = (c ^ (b>>15)) & 0xffffffff; \
			}

/*
   --------------------------------------------------------------------
   hash() -- hash a variable-length key into a 32-bit value
k     : the key (the unaligned variable-length array of bytes)
len   : the length of the key, counting by bytes
level : can be any 4-byte value
Returns a 32-bit value.  Every bit of the key affects every bit of
the return value.  Every 1-bit and 2-bit delta achieves avalanche.
About 36+6len instructions.

The best hash table sizes are powers of 2.  There is no need to do
mod a prime (mod is sooo slow!).  If you need less than 32 bits,
use a bitmask.  For example, if you need only 10 bits, do
h = (h & hashmask(10));
In which case, the hash table should have hashsize(10) elements.

If you are hashing n strings (uint8 **)k, do it like this:
for (i=0, h=0; i<n; ++i) h = hash( k[i], len[i], h);

By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.  You may use this
code any way you wish, private, educational, or commercial.  It's free.

See http://burlteburtle.net/bob/hash/evahash.html
Use for hash table lookup, or anything where one collision in 2^32 is
acceptable.  Do NOT use for cryptographic purposes.
--------------------------------------------------------------------
*/

inline uint32 hash(
    register uint8 *k,        /* the key */
    register uint32  length,   /* the length of the key */
    register uint32  initval    /* the previous hash, or an arbitrary value */
)
{
	register uint32 a,b,c,len;

	/* Set up the internal state */
	len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = initval;           /* the previous hash value */

	/*---------------------------------------- handle most of the key */
	while (len >= 12)
	{
		a += (k[0] +((uint32)k[1]<<8) +((uint32)k[2]<<16) +((uint32)k[3]<<24));
		b += (k[4] +((uint32)k[5]<<8) +((uint32)k[6]<<16) +((uint32)k[7]<<24));
		c += (k[8] +((uint32)k[9]<<8) +((uint32)k[10]<<16)+((uint32)k[11]<<24));
		mix(a,b,c);
		k += 12;
		len -= 12;
	}

	/*------------------------------------- handle the last 11 bytes */
	c += length;
	switch (len)             /* all the case statements fall through */
	{
		case 11:
			c+=((uint32)k[10]<<24);
		case 10:
			c+=((uint32)k[9]<<16);
		case 9 :
			c+=((uint32)k[8]<<8);
			/* the first byte of c is reserved for the length */
		case 8 :
			b+=((uint32)k[7]<<24);
		case 7 :
			b+=((uint32)k[6]<<16);
		case 6 :
			b+=((uint32)k[5]<<8);
		case 5 :
			b+=k[4];
		case 4 :
			a+=((uint32)k[3]<<24);
		case 3 :
			a+=((uint32)k[2]<<16);
		case 2 :
			a+=((uint32)k[1]<<8);
		case 1 :
			a+=k[0];
			/* case 0: nothing left to add */
	}
	mix(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
}


/*
   --------------------------------------------------------------------
   This works on all machines.  hash2() is identical to hash() on
   little-endian machines, except that the length has to be measured
   in uint32s instead of bytes.  It is much faster than hash().  It
   requires
   -- that the key be an array of uint32's, and
   -- that all your machines have the same endianness, and
   -- that the length be the number of uint32's in the key
   --------------------------------------------------------------------
   */
inline uint32 hash2(
    register uint32 *k,        /* the key */
    register uint32  length,   /* the length of the key, in uint32s */
    register uint32  initval  /* the previous hash, or an arbitrary value */
)
{
	register uint32 a,b,c,len;

	/* Set up the internal state */
	len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = initval;           /* the previous hash value */

	/*---------------------------------------- handle most of the key */
	while (len >= 3)
	{
		a += k[0];
		b += k[1];
		c += k[2];
		mix(a,b,c);
		k += 3;
		len -= 3;
	}

	/*-------------------------------------- handle the last 2 uint32's */
	c += length;
	switch (len)             /* all the case statements fall through */
	{
			/* c is reserved for the length */
		case 2 :
			b+=k[1];
		case 1 :
			a+=k[0];
			/* case 0: nothing left to add */
	}
	mix(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
}

/*
   --------------------------------------------------------------------
   This is identical to hash() on little-endian machines (like Intel
   x86s or VAXen).  It gives nondeterministic results on big-endian
   machines.  It is faster than hash(), but a little slower than
   hash2(), and it requires
   -- that all your machines be little-endian
   --------------------------------------------------------------------
   */

inline uint32 hash3(
    register uint8 *k,        /* the key */
    register uint32  length,   /* the length of the key */
    register uint32  initval  /* the previous hash, or an arbitrary value */
)
{
	register uint32 a,b,c,len;

	/* Set up the internal state */
	len = length;
	a = b = 0x9e3779b9;    /* the golden ratio; an arbitrary value */
	c = initval;           /* the previous hash value */

	/*---------------------------------------- handle most of the key */
	if (((uint32)((size_t)k))&3)
	{
		while (len >= 12)    /* unaligned */
		{
			a += (k[0] +((uint32)k[1]<<8) +((uint32)k[2]<<16) +((uint32)k[3]<<24));
			b += (k[4] +((uint32)k[5]<<8) +((uint32)k[6]<<16) +((uint32)k[7]<<24));
			c += (k[8] +((uint32)k[9]<<8) +((uint32)k[10]<<16)+((uint32)k[11]<<24));
			mix(a,b,c);
			k += 12;
			len -= 12;
		}
	}
	else
	{
		while (len >= 12)    /* aligned */
		{
			a += *(uint32 *)(k+0);
			b += *(uint32 *)(k+4);
			c += *(uint32 *)(k+8);
			mix(a,b,c);
			k += 12;
			len -= 12;
		}
	}

	/*------------------------------------- handle the last 11 bytes */
	c += length;
	switch (len)             /* all the case statements fall through */
	{
		case 11:
			c+=((uint32)k[10]<<24);
		case 10:
			c+=((uint32)k[9]<<16);
		case 9 :
			c+=((uint32)k[8]<<8);
			/* the first byte of c is reserved for the length */
		case 8 :
			b+=((uint32)k[7]<<24);
		case 7 :
			b+=((uint32)k[6]<<16);
		case 6 :
			b+=((uint32)k[5]<<8);
		case 5 :
			b+=k[4];
		case 4 :
			a+=((uint32)k[3]<<24);
		case 3 :
			a+=((uint32)k[2]<<16);
		case 2 :
			a+=((uint32)k[1]<<8);
		case 1 :
			a+=k[0];
			/* case 0: nothing left to add */
	}
	mix(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
};

/*
 * hash the integerer ,by bob jenkins
 * from http://www.concentric.net/~Ttwang/tech/inthash.htm
 */
inline uint32 hash(uint32 key)
{
	key += (key << 12);
	key ^= (key >> 22);
	key += (key << 4);
	key ^= (key >> 9);
	key += (key << 10);
	key ^= (key >> 2);
	key += (key << 7);
	key ^= (key >> 12);
	return key;
};

/*
 * another hash function for integere, by Thomas Wang's
 * from http://www.concentric.net/~Ttwang/tech/inthash.htm
 */
inline uint32 hash2(uint32 key)
{
	key += ~(key << 15);
	key ^=  (key >> 10);
	key +=  (key << 3);
	key ^=  (key >> 6);
	key += ~(key << 11);
	key ^=  (key >> 16);
	return key;
};

inline uint32 fasthash(
    register const uint8 *k,        /* the key */
    register uint32  length   /* the length of the key */
)
{
	register uint32 a,b,c,len;

	/* Set up the internal state */
	len = length;
	a = b = 0x9e3779b9;    /* the golden ratio; an arbitrary value */
	c = 0x454f08ba;

	/*---------------------------------------- handle most of the key */
	if (((uint32)((size_t)k))&3)
	{
		while (len >= 12)    /* unaligned */
		{
			a += (k[0] +((uint32)k[1]<<8) +((uint32)k[2]<<16) +((uint32)k[3]<<24));
			b += (k[4] +((uint32)k[5]<<8) +((uint32)k[6]<<16) +((uint32)k[7]<<24));
			c += (k[8] +((uint32)k[9]<<8) +((uint32)k[10]<<16)+((uint32)k[11]<<24));
			mix(a,b,c);
			k += 12;
			len -= 12;
		}
	}
	else
	{
		while (len >= 12)    /* aligned */
		{
			a += *(uint32 *)(k+0);
			b += *(uint32 *)(k+4);
			c += *(uint32 *)(k+8);
			mix(a,b,c);
			k += 12;
			len -= 12;
		}
	}

	/*------------------------------------- handle the last 11 bytes */
	c += length;
	switch (len)             /* all the case statements fall through */
	{
		case 11:
			c+=((uint32)k[10]<<24);
		case 10:
			c+=((uint32)k[9]<<16);
		case 9 :
			c+=((uint32)k[8]<<8);
			/* the first byte of c is reserved for the length */
		case 8 :
			b+=((uint32)k[7]<<24);
		case 7 :
			b+=((uint32)k[6]<<16);
		case 6 :
			b+=((uint32)k[5]<<8);
		case 5 :
			b+=k[4];
		case 4 :
			a+=((uint32)k[3]<<24);
		case 3 :
			a+=((uint32)k[2]<<16);
		case 2 :
			a+=((uint32)k[1]<<8);
		case 1 :
			a+=k[0];
			/* case 0: nothing left to add */
	}
	mix(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
};

};
};
};

using namespace sqr::hash::jenkins;

#endif

