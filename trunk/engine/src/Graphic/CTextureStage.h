#pragma once
#include "CFixStage.h"

namespace sqr
{
	class CTextureStage : public CFixStage
	{
	public:
		CTextureStage(StageFlag sf = SF_Shader);

		void	Execute( CRenderStyleExInst* pRSInst );

	protected:
		typedef map<float, uint32>		TexIndexMap;
		typedef TexIndexMap::iterator	TexIndexMapIter;

		uint32	m_nTexIdx;

		friend class CTextureStageCoder;
	};
}