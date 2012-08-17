#ifndef ___BITSET___
#define ___BITSET___
#include "CMathMallocObject.h"

namespace sqr
{
template< int32 nSize, typename IntType = uint64 >
class TBitset : public CMathMallocObject
{
public:

	char _Buf[ ( nSize - 1 )/8 + 1 ];

	// param1: 要获取位值的位置
	// param2: 要获得多少位
	// ret: 返回值为获得多少位的数值
	uint32 GetBit( int pos, int Num )
	{
		IntType n = *( (IntType*)( _Buf + ( pos >> 3 ) ) );
		n = n >> ( pos&0x07 );
		n = n & ( ( 1 << Num ) - 1 );

		return (uint32)n;
	}

	// param1: 要设置位值的位置
	// param2: 要设置多少位
	// param3: 要设置的数据
	void SetBit( int pos, int Num, uint32 v )
	{
		IntType  m = ( ( 1 << Num ) - 1 ) << ( pos&0x07 );
		IntType& n = *( (IntType*)( _Buf + (pos>>3) ) );

		n = ( n&(~m) )|( ( ( (IntType)v ) << (pos&0x07) )&m );
	}
};
}

#endif

