#pragma once

#include "CCtrlBase.h"

//视图 渲染窗口
//1.封装渲染初始化功能
#include <set>

namespace sqr
{
	class CWindowTarget;
	class CRenderGroup;
	class IEditorMsgProc;
	template<class T> struct TRect;
	typedef TRect<int>		CIRect;
}

namespace sqr_tools
{
	
	class CCoreViewImp :public sqr_tools::CCtrlBase
	{
		REG_UI_IMP(CCoreViewImp);
	public:
		CCoreViewImp(void* _handle);
		~CCoreViewImp(void);

		void Initialize(void);
		void SwitchWindowType(void);
		void ReSize();

		CWindowTarget*		GetWindowTarget();
		CRenderGroup*		GetRenderGroup();
		CIRect              GetViewRect();

		bool	GetIsWindowType()			{ return m_isWindowType;}
		void 	SetWindowType(bool flag)	{ m_isWindowType=flag;}
		void	SetContext(CEditContext* pContext);
	protected:
		CWindowTarget*		m_pWindowTarget;
		CRenderGroup*		m_pRenderGroup;
		bool                m_isWindowType;
		bool                m_isMainRendeTarget;
		unsigned int		m_Width;
		unsigned int		m_Height;
		void*               m_DependHandle;
		typedef std::set<CCoreViewImp*> ViewList;
		static	ViewList	m_EditorViewList;
	public:
		bool OnCtrlSize(int width,int height);
	};

}
