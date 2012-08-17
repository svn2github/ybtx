#pragma once
#include "CMirrorBufferData.h"

namespace sqr
{
	template<typename T> struct TRect;
	typedef TRect<float> CFRect;

	class CCoreQuadScene;

	class CMirrorQuadScene : public CMirrorBufferData
	{
	public:
		CCoreQuadScene* GetScene() const { return m_pScene; }

	protected:
		CMirrorQuadScene(uint32 uWidth, uint32 uHeight, uint32 uPrecision);
		~CMirrorQuadScene();

	protected:
		CCoreQuadScene*	m_pScene;
	};

}
