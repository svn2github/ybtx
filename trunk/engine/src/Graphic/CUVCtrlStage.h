#pragma once
#include "CFixStage.h"

namespace sqr
{
	class CRenderStyleExInst;

	class CUVCtrlStage : public CFixStage
	{
	public:
		CUVCtrlStage( StageFlag sf );

		void	Execute( CRenderStyleExInst* pRSInst );

	protected:
		uint32	m_uvSrc;
		uint32	m_uv;

		friend class CUVCtrlStageCoder;
	};
}