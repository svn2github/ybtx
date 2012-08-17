#pragma once
#include "CDistortedTick.h"

namespace sqr
{
	class CCoreObjectFollower;
	class CFastObjCmd;

	class CSlowObjCmd
		:public CDistortedTick
	{
	private:
		string					m_sData;
		CCoreObjectFollower*	m_pObj;
		
	protected:
		list<CFastObjCmd*>	m_lstNestedCmd;

	public:
		CSlowObjCmd(CCoreObjectFollower* pObj,const void* pData,uint32 uSize);
		virtual ~CSlowObjCmd();

		CCoreObjectFollower* GetObj()const;
		const void* GetData()const;

		void AddNestedCmd(CFastObjCmd* pCmd);
	};

	template<typename CmdType>
	class TSlowObjCmd
		:public CSlowObjCmd
	{
	public:
		TSlowObjCmd(CCoreObjectFollower* pObj,const void* pData,uint32 uSize);
	private:
		void OnTick();
	};
}