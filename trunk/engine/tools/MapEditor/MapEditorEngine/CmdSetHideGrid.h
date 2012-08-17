#pragma once
#include "Cmd.h"

namespace sqr
{
	class CCmdSetHideGrid : public CCmd
	{

	private:

		vector<int> m_vecGridIndices;
		bool bHide;
	public:
		CCmdSetHideGrid() : bHide(false)
		{
			;
		}

		~CCmdSetHideGrid() { }

		virtual void _Execute();
		virtual void _UnExecute();

		void AddGrid(int nGridIndex);
		void AddGrids(vector<int> & gridindexs);
		void SetGridIsHide(bool b);
	};
}