#pragma once

namespace sqr_tools
{
	class CCmdMgr;
	class CCombinCmd;
	class CCmd
	{
		friend	class CCmdMgr;
		friend	class CCombinCmd;
	protected:
		virtual void Destroy(void);

		virtual void _Execute(void)	 = 0;
		virtual void _UnExecute(void) = 0;
		virtual bool IsValid(void);
	public:
		CCmd();
		virtual ~CCmd();
		void	Execute(void);
	};
}