#pragma once
#include "CWaterMgr.h"


namespace sqr_tools
{
	class CEditLogicDataMgr;
	class CEditWaterMgr : public CWaterMgr
	{
	public:
		CEditWaterMgr(CRenderGroup* pGroup,CEditLogicDataMgr* pLogicDataMgr);
		~CEditWaterMgr(void);

		bool GenerateWater(int tileWidth,int tileHeight);

	protected:
		CElementNode*	_CreateNode( URegionID ID );
		CEditLogicDataMgr* m_pLogicDataMgr;
	};
}