#pragma once
#include "CmdSetVertexHeight.h"

namespace sqr
{
	class CWaterSurface;
	class CWaterRegion;
}

namespace sqr_tools
{
	class CCmdSetWaterHeight :public CCmdSetVertexHeight
	{
	public:
		CCmdSetWaterHeight(CEditContext* pContext);
		~CCmdSetWaterHeight(void);

		virtual void _Execute();
		virtual void _UnExecute();
		
		void SetWaterSurface(CWaterSurface* surface);

	protected:
		void CreateWaterMesh();
		void DestroyWaterMesh();

		void ModifyWaterRegionInfo(CWaterRegion* pWaterRegion,int regionIndex);

		CWaterSurface* m_pWaterSurface;

		vector<int> m_RegionVec;
	};

//!namespace
}