#pragma once
#include "Cmd.h"
#include "ToolSetTileTexture.h"

namespace sqr
{
	class CCmdSetVertexAlpha : public CCmd
	{
	public:
		struct SVertexAlpha
		{
			DWORD       dwVertexIndex;
			D3DCOLOR    oColor; ///original color 顶点原始颜色（包括刷的顶点色）
		};

	private:
		vector<SVertexAlpha> m_vecDiffs;
		vector<int> m_vecGridIndices;

		CToolSetTileTexture::SProcessParams m_processParams;

		int m_nGridIndexHot;

		DWORD m_dwVertexCnt;

		class IsEqual
		{
			SVertexAlpha v;
		public:
			IsEqual(SVertexAlpha s) : v(s) { }
			~IsEqual() { }
			bool operator ()(SVertexAlpha s)
			{
				return s.dwVertexIndex == v.dwVertexIndex;
			}
		};

	public:
		CCmdSetVertexAlpha() : m_dwVertexCnt(0)
		{
			::ZeroMemory(&m_processParams,sizeof(CToolSetTileTexture::SProcessParams));
		}

		~CCmdSetVertexAlpha() { }

		virtual void _Execute();
		virtual void _UnExecute();
		void AddVertex(DWORD dwVertexIndex);
		void AddGrid(int nGridIndex);
		DWORD GetVertexCnt()
		{
			return m_dwVertexCnt;
		}
		void SetGridIndexHot(int n);
		void SetProcessParams(CToolSetTileTexture::SProcessParams processParams);
	};
}