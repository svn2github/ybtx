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
	class CSkeletonEditImp;
	class CSkeletonDelegate : public CEventDelegate
	{
		friend class CEventOwner;
	public:
		CSkeletonDelegate(CSkeletonEditImp* handle);
		virtual ~CSkeletonDelegate(void);

	public:
		virtual bool	MsgProc(const CMsg& msg);
		virtual void    Render(void);
		virtual void    Pick(int mouseX, int mouseY);

	private:
		CWindowTarget* m_pWindowTarget;
		CEditModel* m_pEditModel;
		CSkeletonEditImp* m_pSkeletonEditImp;
	};
}