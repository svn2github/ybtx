#pragma once

namespace sqr
{
	class CEventOwner;
	class CMsg;
}

namespace sqr_tools
{
	class CBoundingBoxEditImp;
	class CBoundingBoxDelegate : public CEventDelegate
	{
		friend class CEventOwner;
	public:
		CBoundingBoxDelegate(CBoundingBoxEditImp* handle);
		virtual ~CBoundingBoxDelegate(void);

	public:
		virtual bool	MsgProc(const CMsg& msg);
		virtual void    Render(void);

	private:
		CBoundingBoxEditImp* m_pBoundingBoxImp;
	};
}