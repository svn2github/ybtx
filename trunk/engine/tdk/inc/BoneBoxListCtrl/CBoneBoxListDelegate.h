#pragma once

namespace sqr
{
	class CEventDelegate;
	class CEventOwner;
	class CMsg;
}

namespace sqr_tools
{
	class CBoneBoxListCtrlImp;
	class CBoneBoxListDelegate : public CEventDelegate
	{
		friend class CEventOwner;
	public:
		CBoneBoxListDelegate(CBoneBoxListCtrlImp* handle);
		virtual ~CBoneBoxListDelegate(void);

	public:
		virtual bool	MsgProc(const CMsg& msg);
		virtual void    Update(std::vector<std::string> str_vector);

	private:
		CBoneBoxListCtrlImp* m_pBonegBoxListImp;
	};
}