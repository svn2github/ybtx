#pragma once
#include "Cmd.h"

namespace sqr_tools
{
	class CCombinCmd : public CCmd
	{
	public:
		~CCombinCmd(void);
		void	Push(CCmd* pCmd);
		bool	IsValid(void);
	protected:
		void _Execute(void);
		void _UnExecute(void);

		typedef vector<CCmd*> CommandPool;
		CommandPool	m_ComPool;
	};
}