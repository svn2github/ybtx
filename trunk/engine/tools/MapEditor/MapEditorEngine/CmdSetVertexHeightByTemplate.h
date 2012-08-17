#pragma once
#include "Cmd.h"
#include "ToolSetTileHeight.h"

namespace sqr
{
	class CCmdSetVertexHeightByTemplate : public CCmd
	{
	public:
		struct SVertex
		{
			DWORD    dwVertexIndex;
			float    fHeight;
		};

	private:
		vector<SVertex> m_vecDiffs;

		CToolSetTileHeight::SProcessParams m_processParams;
		int m_nGridIndexHot;

		DWORD m_dwVertexCnt;

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

	public:
		CCmdSetVertexHeightByTemplate();

		~CCmdSetVertexHeightByTemplate() { }

		virtual void Execute();
		virtual void UnExecute();
		void AddVertex(DWORD dwVertexIndex);
		DWORD GetVertexCnt()
		{
			return m_dwVertexCnt;
		}
		void SetProcessParams(CToolSetTileHeight::SProcessParams processParams);
		void SetGridIndexHot(int n);
	};
}