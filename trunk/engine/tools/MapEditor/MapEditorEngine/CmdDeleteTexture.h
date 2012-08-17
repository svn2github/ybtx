#pragma once
#include "Cmd.h"
#include "ToolSetTileTexture.h"

namespace sqr
{
	class CCmdDeleteTexture : public CCmd
	{
		struct STextureNodeInfo
		{
			STextureNodeInfo()
			{
				strTextureNames[0] = strTextureNames[1] = strTextureNames[2] = strTextureNames[3] = "";
			}

			DWORD dwGridIndex;
			string strTextureNames[4];
		};

		STextureNodeInfo nodeInfo;

		vector<STextureNodeInfo> m_vecDiffs;

		CToolSetTileTexture::SProcessParams m_processParams;

		DWORD m_dwGridCnt;

	public:
		CCmdDeleteTexture() : m_dwGridCnt(0) { }
		~CCmdDeleteTexture() { }
		virtual void _Execute();
		virtual void _UnExecute();
		void AddGrid(DWORD dwGridIndex);
		DWORD GetGridCnt()
		{
			return m_dwGridCnt;
		}
		void SetProcessParams(CToolSetTileTexture::SProcessParams processParams);
	};
}