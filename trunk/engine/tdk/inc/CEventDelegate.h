#pragma once

namespace sqr
{
	class CEventOwner;
	class CMsg;
	class CEventDelegate
	{
		friend class CEventOwner;
	public:
		CEventDelegate(void);
		virtual ~CEventDelegate(void);
		virtual bool	MsgProc(const CMsg& msg);

				void	UnBind(void);
		virtual void	Active(void);
		virtual void	Render(void);
	protected:
		virtual void	_UnActive(void);
		virtual	void	_Init(CEventOwner* pOwner);
		CEventOwner*	m_pOwner;
	};
}