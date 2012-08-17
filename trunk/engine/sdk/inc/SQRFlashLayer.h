#pragma once
#include "BaseHelper.h"
#include "GUIBase.h"
#include "SQRWnd.h"

namespace sqr
{
	class GUI_API SQRFlashLayer : public virtual CDynamicObject
	{
	public:
		SQRFlashLayer(void);
		SQRFlashLayer(UIString flashTex, const CFRect& rt, SQRWnd* pChildWnd, 
					uint32 ct, float v, float lx, float ty ,float rx, float by);

		SQRFlashLayer(FlashWndInfo* flash, SQRWnd* pChildWnd);
		
		virtual ~SQRFlashLayer(void);

		// 匀速闪烁，其他闪烁区线以再实现
		virtual void DrawBackground();
		bool GetFlashWillDel();
		bool CheckParendWnd(SQRWnd* pwnd);
		bool CheckFlushWnd(SQRWnd* pwnd, UIString tex);
		bool CheckFlushWnd(FlashWndInfo* flash, SQRWnd* pwnd);
		void ReInitFlash(FlashWndInfo* flash);
		bool CheckFlashContShow();
		void SetDrawed();
	
	private:
		void InitTexture(UIString stex);
		bool RefreshFlash();
		void SQRRefreshTrans();
		bool SQRRefreshPos();
		bool CheckWillShow();
	private:
		bool		m_bDel;					//  是否删除
		CColor		m_color;				//	颜色

		FlashWndInfo* m_flash;				//  整体数据
		
		UIString	m_texStr;
		ITexture*	m_pTexture;
		SQRWnd*		m_pParentWnd;			//	挂载到的窗体
		CFRect		m_rtTex;				//  贴图大小
		float		m_v;					//  闪烁速度，现在只有匀速
		uint32		m_count;				//  闪烁次数，〇为整个生命周期一直闪烁

		float		m_Amin;
		float		m_Amax;

		// left top right bottom相对父窗体校正位置
		float		m_lx;
		float		m_ty;
		float		m_rx;
		float		m_by;

		float		m_fTransParent;			//  透明度
		DWORD		m_curSysTime;			//  当前系统时间
		uint32		m_curCount;				//  当前播放次数
		bool		m_bAdd;					//  透明度是否在增加状太
		CFRect		m_curPos;				//  当前位置
	};

	inline bool SQRFlashLayer::CheckParendWnd(SQRWnd* pwnd)
	{
		return pwnd == m_pParentWnd;
	}

	inline bool SQRFlashLayer::CheckFlushWnd(SQRWnd* pwnd, UIString tex)
	{
		return pwnd == m_pParentWnd && m_texStr == tex;
	}

	inline bool SQRFlashLayer::CheckFlushWnd(FlashWndInfo* flash, SQRWnd* pwnd)
	{
		return pwnd == m_pParentWnd && m_flash == flash;
	}

	inline bool SQRFlashLayer::GetFlashWillDel()
	{
		return m_bDel;
	}
}