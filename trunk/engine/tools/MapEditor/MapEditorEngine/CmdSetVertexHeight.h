#pragma once
#include "Cmd.h"
#include "ToolSetTileHeight.h"

namespace sqr
{
	class CCmdSetVertexHeight : public CCmd
	{
	public:
		struct SCmdVertex
		{
			DWORD    dwVertexIndex;
			float    fHeight;
		};

	private:
		vector<SCmdVertex> m_vecDiffs;
		vector<int> m_vecGridIndices;
		CToolSetTileHeight::SProcessParams m_processParams;
		int m_nGridIndexHot;

		DWORD m_dwVertexCnt;

		float m_fReferenceHeight;

		class IsEqual
		{
			SCmdVertex v;
		public:
			IsEqual(SCmdVertex s) : v(s) { }
			~IsEqual() { }
			bool operator ()(SCmdVertex s)
			{
				return s.dwVertexIndex == v.dwVertexIndex;
			}
		};

		virtual void _Execute();
		virtual void _UnExecute();
	public:
		CCmdSetVertexHeight();

		~CCmdSetVertexHeight() { }

		void AddVertex(DWORD dwVertexIndex);
		void AddGrid(int nGridIndex);
		DWORD GetVertexCnt()
		{
			return m_dwVertexCnt;
		}
		void SetProcessParams(CToolSetTileHeight::SProcessParams processParams);
		void SetGridIndexHot(int n);
		void SetReferenceHeight(float fHeight);
	};
}