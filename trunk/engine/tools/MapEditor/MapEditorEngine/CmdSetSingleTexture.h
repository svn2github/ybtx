#pragma once
#include "Cmd.h"
#include "ToolSetTileTexture.h"

namespace sqr
{
	// À¢µ•’≈Œ∆¿Ì
	class CCmdSetSingleTexture : public CCmd
	{
		struct SDiffGridTextureInfo
		{
			SDiffGridTextureInfo()
			{
				bChiffLayerOne = false;
				bChiffLayerTwo = false;
			}

			bool bChiffLayerOne;
			bool bChiffLayerTwo;
			int  nGridIndex;

			sqr::STextureNodeInfo nodeInfo;
			CVector2f  uv[2][4];
		};

		vector<SDiffGridTextureInfo> m_vecDiffs;

		CToolSetTileTexture::SProcessParams m_processParams;

		DWORD m_dwGridCnt;
	public:
		CCmdSetSingleTexture() : m_dwGridCnt(0) { }
		~CCmdSetSingleTexture() { }
		virtual void _Execute();
		virtual void _UnExecute();
		void AddGrid(DWORD dwGridIndex);
		void SetProcessParams(CToolSetTileTexture::SProcessParams processParams);

		inline DWORD GetGridCnt()
		{
			return m_dwGridCnt;
		}
	};
}