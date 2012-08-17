#pragma once
#include "Cmd.h"
#include "ToolSetTileBlock.h"

namespace sqr
{
	class CCmdSetTileBlock : public CCmd
	{
		struct SDiff
		{
			DWORD dwGridIndex;
			int nBlockType;
			vector<int> subindexs;

			SDiff()
			{
				dwGridIndex = 0;
				nBlockType = 0;
				subindexs.clear();
			};
		};

		vector<SDiff> m_vecDiffs;

		CToolSetTileBlock::SProcessParams m_processParams;

	public:
		CCmdSetTileBlock() { }
		~CCmdSetTileBlock() { }
		virtual void _Execute();
		virtual void _UnExecute();
		void AddGrid(DWORD dwGridIndex);
		void AddSubGrid(DWORD dwGridIndex, vector<int>& subindexs);
		void SetProcessParams(CToolSetTileBlock::SProcessParams processParams);
	};

}