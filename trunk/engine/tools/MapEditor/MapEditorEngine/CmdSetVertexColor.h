#pragma once
#include "Cmd.h"
#include "ToolVertex.h"

namespace sqr
{
	class CCmdSetVertexColor : public CCmd
	{
	public:
		struct SVertex
		{
			DWORD       dwVertexIndex;
			D3DCOLOR    oColor;///original color 顶点原始颜色（包括刷的顶点色）
			D3DCOLOR    sColor;///vertex specular color
		};

	private:
		vector<SVertex> m_vecDiffs;
		vector<int> m_vecGridIndices;

		int m_nGridIndexHot;
		DWORD m_dwVertexCnt;
		DWORD m_dwBrushInX;
		DWORD m_dwBrushInZ;
		DWORD m_dwColor;

		class IsEqual
		{
			SVertex v;
		public:
			IsEqual(SVertex s) : v(s) { }
			~IsEqual() { }
			bool operator ()(SVertex s)
			{
				return s.dwVertexIndex == v.dwVertexIndex;
			}
		};

		virtual void _Execute();
		virtual void _UnExecute();
	public:
		CCmdSetVertexColor();

		~CCmdSetVertexColor() { }

		void AddVertex(DWORD dwVertexIndex);

		DWORD GetVertexCnt()
		{
			return m_dwVertexCnt;
		}
		void AddGrid(int nGridIndex);
		void SetGridIndexHot(int n);
		void SetProcessParams(const DWORD dwBrushInX, const DWORD dwBrushInZ, const DWORD dwColor);
	};
}