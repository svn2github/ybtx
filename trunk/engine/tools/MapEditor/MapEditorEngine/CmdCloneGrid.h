#pragma once
#include "Cmd.h"

namespace sqr
{
	class CCmdCloneGrid : public CCmd
	{
		struct SDiff
		{
			int nGridIndex;
			CTerrainMesh::STextureNodeInfo nodeInfo;
			float fY[4];
		};

		vector<SDiff> m_vecDiffs;

	public:
		CCmdCloneGrid() { }
		~CCmdCloneGrid() { }
		virtual void Execute();
		virtual void UnExecute();
		void AddGridIndexPasteTo();
		void SetGridIndicesCopyFrom(vector<int> & v);
	};
}