#pragma once
#include "TSingleton.h"
#include <stack>

namespace sqr_tools
{
	class CCmd;

	class CCmdMgr : public Singleton<CCmdMgr>
	{
	private:
		friend class Singleton<CCmdMgr>;
		friend class CCmd;
		CCmdMgr();
		~CCmdMgr();
		int	   m_nDirtyCnt;

		typedef stack<CCmd*>	CommandPool;
		CommandPool	m_CmdQueue;
		CommandPool	m_UnDoQueue;

		void ClearPool(CommandPool& Pool);
		void Do(CCmd * pCmd);
	public:
		void ReDo(void);
		void UnDo(void);
		void Clear(void);
	};
}