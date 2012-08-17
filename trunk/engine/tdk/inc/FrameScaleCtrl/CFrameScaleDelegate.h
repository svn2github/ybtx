#pragma once

namespace sqr
{
	class CEventDelegate;
	class CWindowTarget;
	class CEventOwner;
	class CMsg;
}

namespace sqr_tools
{
	class CEditModel;
	class CFrameScaleCtrlImp;
	class CFrameScaleDelegate : public CEventDelegate
	{
		friend class CEventOwner;
	public:
		CFrameScaleDelegate(CFrameScaleCtrlImp* handle);
		virtual ~CFrameScaleDelegate(void);

	public:
		virtual bool	MsgProc(const CMsg& msg);
		virtual void    Render(void);
		virtual void    Resize(void);

	private:
		CWindowTarget* m_pWindowTarget;
		CEditModel* m_pEditModel;
		CFrameScaleCtrlImp* m_pFrameScaleEditImp;
	};
}