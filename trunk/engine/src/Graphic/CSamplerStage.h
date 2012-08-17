#pragma once
#include "CFixStage.h"

namespace
{
	//class CTexturePack
	//{
	//public:
	//	GVector<ITexture*> m_TexturePool;
	//};

	class CSamplerStage: public CFixStage
	{
	public:
		CSamplerStage(StageFlag sf = SF_Shader);
		void	SetStageEx(StageFlagEx sfe){};
		void	Execute( CRenderStyleExInst* pRSInst );

	protected:
		typedef GMap<_SAMPLERSTATETYPE, uint32>	SamplerPool;
		typedef SamplerPool::iterator			SamplerPoolIter;
		
		SamplerPool	m_Samplers; 

		friend class CSamplerStageCoder;
	};
}