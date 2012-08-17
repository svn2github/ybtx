#pragma once

namespace sqr
{
	template<class T> struct TPos;
	typedef TPos<int32> CPos;
	typedef TPos<float> CFPos;

	//因为MoveBeganDerived函数的参数被修改了无数遍，所以还是做成一个结构，使得以后改起来方便一些
	struct CMoveBeganDerivedArgs
	{
		const CFPos&		EndPixelPos;
		float			fSpeed;
		EFindPathType	eFindPathType;
		EBarrierType	eBarrierType;
		float			fEndDist;
		const CPixelPath* pPixelPath;
	};

	struct CMovePathChangedDerivedArgs
	{
		const			CFPos& EndPixelPos;
		float			fSpeed;
		EFindPathType	eFindPathType;
		EBarrierType	eBarrierType;
		float			fEndDist;
		float			fMovedDist;
		const CPixelPath* pPixelPath;
	};

	struct CMoveSpeedChangedDerivedArgs
	{
		float			fSpeed;
	};
}

