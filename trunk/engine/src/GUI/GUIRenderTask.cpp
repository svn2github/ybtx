#include "stdafx.h"
#include "GUIRenderTask.h"
#include "TSqrAllocator.inl"

FLASHRenderTask::FLASHRenderTask()
{
	for (size_t i = 0; i < 2; ++i)
		m_FlashWnd[i].clear();
}

FLASHRenderTask::~FLASHRenderTask()
{
	for (size_t i = 0; i < 2; ++i)
	{
		UIVector<SQRFlashLayer*>::iterator itr = m_FlashWnd[i].begin();
		UIVector<SQRFlashLayer*>::iterator itrend = m_FlashWnd[i].end();

		while ( itr != itrend )
		{
			delete *itr;
			++itr;
		}
		m_FlashWnd[i].clear();
	}
}

void FLASHRenderTask::AddFlashTask(UIString flashTex, const CFRect& rt, SQRWnd* pChildWnd, 
								   uint32 ct, float v, float lx, float ty ,float rx, float by)
{
	SQRFlashLayer* pFlashWnd = new SQRFlashLayer(flashTex, rt, pChildWnd, ct, v, lx, ty, rx, by);
	m_FlashWnd[0].push_back(pFlashWnd);
}

void  FLASHRenderTask::AddFlashTask(FlashWndInfo* flash, SQRWnd* pwnd, uint32 st)
{
	if (st >= 2 )
		return;
	bool bf = false;
	if (st == 1)
	{
		uint32 sz = m_FlashWnd[st].size();
		for (size_t i = 0; i < sz; ++i)
		{
			if ( m_FlashWnd[st][i] && m_FlashWnd[st][i]->CheckParendWnd(pwnd) &&
				(!(m_FlashWnd[st][i]->CheckFlashContShow())) )
			{
				m_FlashWnd[st][i]->ReInitFlash(flash);
				bf = true;
			}
		}
	}
	if ( bf || flash == NULL)
		return;
	SQRFlashLayer* pFlashWnd = new SQRFlashLayer(flash, pwnd);
	m_FlashWnd[st].push_back(pFlashWnd);
}

void FLASHRenderTask::RenderTask()
{
	// 渲染整体flash内容
	for (size_t i = 0; i < 2; ++i)
	{
		if (m_FlashWnd[i].size() == 0)
			continue;
		for (size_t j = 0; j < m_FlashWnd[i].size(); ++j)
			m_FlashWnd[i][j]->DrawBackground();

		UIVector<SQRFlashLayer*>::iterator itr = m_FlashWnd[i].begin();
		UIVector<SQRFlashLayer*>::iterator itrend = m_FlashWnd[i].end();

		while ( itr != itrend )
		{
			if ( (*itr)->GetFlashWillDel() )
			{
				delete *itr;
				m_FlashWnd[i].erase(itr);

				itr = m_FlashWnd[i].begin();
				itrend = m_FlashWnd[i].end();
			}
			else
				++itr;
		}
	}
	// not a good idea
	for (size_t i = 0; i < 2; ++i)
	{
		if (m_FlashWnd[i].size() == 0)
			continue;
		for (size_t j = 0; j < m_FlashWnd[i].size(); ++j)
			m_FlashWnd[i][j]->SetDrawed();
	}
}

// 删除窗体上的所有闪烁效果
void FLASHRenderTask::DelFlashTaskAll(SQRWnd* pWnd)
{
	for (size_t i = 0; i < 2; ++i)
	{
		UIVector<SQRFlashLayer*>::iterator itr = m_FlashWnd[i].begin();
		UIVector<SQRFlashLayer*>::iterator itrend = m_FlashWnd[i].end();

		while( itr != itrend )
		{
			if ( (*itr)->CheckParendWnd(pWnd) )
			{
				delete *itr;
				m_FlashWnd[i].erase(itr);

				itr = m_FlashWnd[i].begin();
				itrend = m_FlashWnd[i].end();
			}
			else
				++itr;
		}
	}
}

// 删除窗体上的特定闪烁效果
void FLASHRenderTask::DelFlashTask(SQRWnd* pWnd, UIString flash)
{
	for (size_t i = 0; i < 2; ++i)
	{
		UIVector<SQRFlashLayer*>::iterator itr = m_FlashWnd[i].begin();
		UIVector<SQRFlashLayer*>::iterator itrend = m_FlashWnd[i].end();

		while( itr != itrend )
		{
			if ( (*itr)->CheckFlushWnd(pWnd, flash) )
			{
				delete *itr;
				m_FlashWnd[i].erase(itr);

				itr = m_FlashWnd[i].begin();
				itrend = m_FlashWnd[i].end();
			}
			else
				++itr;
		}
	}
}

bool FLASHRenderTask::HasCountFlash()
{
	for (size_t i = 0; i < 2; ++i)
	{
		UIVector<SQRFlashLayer*>::iterator itr = m_FlashWnd[i].begin();
		UIVector<SQRFlashLayer*>::iterator itrend = m_FlashWnd[i].end();

		while( itr != itrend )
		{
			if ( (*itr) && (*itr)->CheckFlashContShow() )
				return true;
			++itr;
		}
	}
	return false;
}

// 文字渲染对象
TextRenderTask::TextRenderTask()
: m_pDlg(NULL)
{
}

TextRenderTask::~TextRenderTask()
{
	m_pDlg = NULL;
}

void TextRenderTask::RenderTask()
{
	// render the text
	if (m_pDlg)
		m_pDlg->FlushDraw();
}

FxRenderTask::_FxTask::_FxTask(SQRWnd* wnd, uint ct, float zm)
{
	pWnd	= wnd;
	curTime	= ct;
	zoom	= zm;
}

FxRenderTask::FxRenderTask()
{}

FxRenderTask::~FxRenderTask()
{
	m_FxTaskVec.clear();
}

void FxRenderTask::RenderTask()
{
	// 清空
	size_t sz = m_FxTaskVec.size();
	for ( size_t i = 0; i < sz; ++i )
		m_FxTaskVec[i].pWnd->FlashImeFx(m_FxTaskVec[i].curTime, m_FxTaskVec[i].zoom);

	m_FxTaskVec.clear();
}

void FxRenderTask::PushRenderStack(SQRWnd* pWnd, uint curTime, float zoom)
{
	if (pWnd)
	{
		_FxTask fxTask(pWnd, curTime, zoom);
		m_FxTaskVec.push_back(fxTask);
	}
}

