#pragma once

namespace sqr
{
	template<typename T> struct TPos;
	typedef TPos<float>	CFPos;

	class CBaseAoiObj
	{
	protected:
		static void CalSeeRelation
			(
			bool& bASeeB,			bool& bBSeeA,
			const CFPos& AAoiPos,	const CFPos& BAoiPos,
			float fAEyeSight,		float fBEyeSight,
			float fASize,			float fBSize,
			float fAKeenness,		float fBKeenness,
			float fAStealth,		float fBStealth,
			uint32 uAViewGroup,		uint32 uBViewGroup
			);

		static bool CalSeeRelation
			(
			const CFPos& AAoiPos,	const CFPos& BAoiPos,
			float fAEyeSight,		float fBSize,
			float fAKeenness,		float fBStealth,
			uint32 uAViewGroup,		uint32 uBViewGroup
			);
	};

}

