#pragma once
#include "SQRWnd.h"
// 用来放置richtooltip的容器窗体，无管理作用 
namespace sqr
{
	class SQRWnd;
	class SQRTipHolder 
		: public Singleton<SQRTipHolder>
	{
	public:
		SQRTipHolder();
		~SQRTipHolder();

		void  CreatHolderWnd(SQRWnd* wnd);
		SQRWnd* GetHolder();

	private:
		SQRWnd* m_pTipHolder;
	};

	//-----------------------------------------------------------------
	inline SQRWnd*
	SQRTipHolder::GetHolder()
	{
		return m_pTipHolder;
	}
}