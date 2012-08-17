#include "stdafx.h"
#include "TPackedPos.h"
#include "CPos.h"
#include "TSqrAllocator.inl"

template<typename ValueType>
TPackedPos<ValueType>::TPackedPos()
{
	x=0;
	y=0;
}

template<typename ValueType>
template<typename PosValueType>
TPackedPos<ValueType>::TPackedPos(const TPos<PosValueType>& Pos)
{
	Assign(Pos);
}

template<typename ValueType>
template<typename PosValueType>
void TPackedPos<ValueType>::Assign(const TPos<PosValueType>& Pos)
{
	x=ValueType(Pos.x);
	y=ValueType(Pos.y);
}

template<typename ValueType>
template<typename PosValueType>
TPackedPos<ValueType>::operator TPos<PosValueType>()const
{
	return TPos<PosValueType>( PosValueType(x), PosValueType(y) );
}
