#pragma once

#include "CRenderTexture.h"
#include "CRenderOperation.h"

#include <vector>

#define BAKE_REGION_SIZE 8

namespace sqr
{
	class CVertexHardwareBuffer;

	class RegionShadowMapBaker
	{
	public:
		RegionShadowMapBaker(void);
		~RegionShadowMapBaker(void);

	protected:
		int m_TexWidth;
		int m_TexHeight;

		vector<DWORD> ColorList;
		void Init();
	public:
		void RenderTile(int x,int z);
		CRenderTexturePtr  m_RegionTex;
		CRenderOperation	m_Op;
		CVertexHardwareBuffer* pBuf ;
		VerColor2D quad[4];
	};

//!namespace
}