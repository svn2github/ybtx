#pragma once

namespace sqr
{
	class CSlowObjCmd;
	class CCoreObjectFollower;

	class CDelayedObjCmdMgr
	{
	public:
		~CDelayedObjCmdMgr(void);

		template<typename CmdType>
			void AddSlowCommand(CCoreObjectFollower* pObj,const CmdType* pCmd);

		template<typename CmdType>
			void AddFastCommand(CCoreObjectFollower* pObj,const CmdType* pCmd);

		void FlushAllCommand();

		bool IsEmpty()const;
	private:
		list<CSlowObjCmd*>	m_lstCmd;

		template<typename CmdType>
			friend class TSlowObjCmd;
		friend CSlowObjCmd;
	};

}