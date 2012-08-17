#pragma once

#include "CPos.h"
#include "ExpHelper.h"
#include "CFindPathMallocObject.h"

#pragma pack(push,1)

namespace sqr
{

	class CLinkRegionMgr;

	struct CBaseRegionNode
		:public CFindPathMallocObject
	{
		uint32 m_nIndex;
		uint32 m_nFatherRegionId;
	};

	struct CLinkInfo
		:public CFindPathMallocObject
	{
		uint32					m_nDestRegionId;
		float					m_fFare;
	};

	class CLinkRegionNode
		:public CFindPathMallocObject
	{
		friend class CLinkRegionMgr;
	public:
		CLinkRegionNode();

		typedef	 uint8		Num_t;
		~CLinkRegionNode(void);

		CPos GetKeyPos();
		inline uint8 GetLinkIndex(uint32 nDestIndex)
		{
			uint8 i = 0;
			for ( ; i < m_nLinkNum; ++i)
			{
				if (m_pLinkList[i].m_nDestRegionId == nDestIndex)
				{
					break;
				}
			}
			Ast(i < m_nLinkNum);
			return i;
		}
	private:
		uint16				m_nKeyPosX;
		uint16				m_nKeyPosY;
		uint32				m_nFatherRegionId;		//父连通区域
		Num_t				m_nChildNum;
		uint32				m_nChildStartId;

		Num_t				m_nLinkNum;				//同级连通区域中与此连通区域相连的连通区域个数
		CLinkInfo*			m_pLinkList;			//相连的连通区域信息列表
	};
}


#pragma pack(pop)
