#pragma once
#include "Cmd.h"
#include "ToolSetTileTexture.h"

namespace sqr
{
	class CCmdSetSerialTexture : public CCmd
	{
		//刷四方连续纹理
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
		vector<int> m_vecBrushGridIndices;
		DWORD m_dwGridCnt;

	public:
		CCmdSetSerialTexture() : m_dwGridCnt(0) { }
		~CCmdSetSerialTexture() { }
		virtual void _Execute();
		virtual void _UnExecute();
		void AddGrid(DWORD dwGridIndex);
		void SetProcessParams(CToolSetTileTexture::SProcessParams processParams);
		inline CToolSetTileTexture::SProcessParams GetProcessParams() const
		{
			return m_processParams;
		}

		inline void SetBrushGridIndices(const vector<int>& vecBrushGridIndices)
		{
			m_vecBrushGridIndices = vecBrushGridIndices;
		}

		inline vector<int> GetBrushGridIndices() const
		{
			return m_vecBrushGridIndices;
		}

		inline DWORD GetGridCnt()
		{
			return m_dwGridCnt;
		}
	};
}