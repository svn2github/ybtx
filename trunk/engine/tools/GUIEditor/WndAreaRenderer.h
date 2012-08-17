#pragma once
#include "CRendererBase.h"
#include "SQRWnd.h"
#include "TSingleton.h"

namespace sqr
{
	// wnd area
	class CWndAreaRendere
		: public CRendererBase
		, public Singleton<CWndAreaRendere>
	{
	public:
		CWndAreaRendere() ;
		virtual ~CWndAreaRendere();
		
		virtual void Render();
		virtual void RenderRootRect();
		virtual void RenderSelWndList();
		void	SetCurRect(const CRect &rt);
		void	SetIsRender(const bool b);

		void	SetRootRect(const CRect &rt);
		void	SetIsRootRender(const bool b);
		void	SetWndRenderList(vector<SQRWnd*>* plist);

	private:
		CRect	m_curRect;		// 当前选中矩形
		bool	m_bRender;		// 是否画矩形框
		CRect	m_rootRect;		// 根矩形
		bool	m_bRootRender;	// 是否画根矩形
		vector<SQRWnd*>*	m_renderWndList;
	};
}