#pragma once
#include "CMathMallocObject.h"

namespace sqr
{


#pragma pack(push,1)


	template<class T> struct TPos;
	typedef TPos<int32> CPos;

	template<typename ValueType>
	class TPackedPos : public CMathMallocObject
	{
	public:
		ValueType	x;
		ValueType	y;

		TPackedPos();

		template<typename PosValueType>
		TPackedPos(const TPos<PosValueType>& Pos);

		template<typename PosValueType>
		operator TPos<PosValueType>()const;
	private:
		template<typename PosValueType>
		void Assign(const TPos<PosValueType>& Pos);
	};


	typedef TPackedPos<uint16>	CPackedPos16;
	typedef TPackedPos<uint32>	CPackedPos32;
	typedef TPackedPos<float>	CPackedPosFloat;

	typedef TPackedPos<int16>	CPackedOfs16;
	typedef TPackedPos<int32>	CPackedOfs32;


#pragma pack(pop)
}

