#pragma once
#include "Cmd.h"
#include "CVector2.h"

namespace sqr_tools
{
	class CEditContext;
	class CCmdSetTerrainBlock : public CCmd
	{
		struct SDiff
		{
			CVector2I gridCoord;
			int nBlockType;

			SDiff()
			{
				nBlockType = 0;
			};
		};

		vector<SDiff>	m_vecDiffs;
		int8			m_nBlockType;
		CEditContext*	m_pEditContext;
	public:
		CCmdSetTerrainBlock(CEditContext* pContext);
		~CCmdSetTerrainBlock() { }

		virtual void _Execute();
		virtual void _UnExecute();

		void AddGrid(const CVector2I& gridcoord);
		void SetBlockType(const int8 nBlockType)
		{
			m_nBlockType = nBlockType;
		}
	};

}