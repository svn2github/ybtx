#pragma once
#include "CStageNode.h"

namespace sqr
{
	struct ParentLink
	{
		uint32	m_uParentIndex;
		BYTE	m_uParamsIndex;
	};

	class CStageNodeInst : public CGraphicMallocObject
	{
	public:
		CStageNodeInst(uint32 Index,StageFlag sf);
		void		BinLoad(CBufFile& FileStream);
		void		Refrash(void);
		StageFlag	GetStage(void);
		void		Execute();
	protected:
		typedef vector<ParentLink> ParentLinkPool;
		StageFlag		m_StageIndex;
		ParentLinkPool	m_vecParent;
		uint32			m_uIndex;
		CStageNode*		m_pState;
		uint32			m_pNodeId;
	};
}