#pragma once

namespace sqr
{
	class CEventDelegate;
	class CEventOwner;
	class CMsg;
}

namespace sqr_tools
{
	class CBoneBoxEditCtrlImp;
	class CBoneBoxEditDelegate : public CEventDelegate
	{
		friend class CEventOwner;
	public:
		CBoneBoxEditDelegate(CBoneBoxEditCtrlImp* handle);
		virtual ~CBoneBoxEditDelegate(void);

	public:
		virtual bool	MsgProc(const CMsg& msg);
		virtual void    Update(std::string str);
		virtual void    Render(void);

	private:
		CBoneBoxEditCtrlImp* m_pBonegBoxEditImp;
	};
}