//------------------------------------------------------------------------------
//  CGuiHandler.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "CGuiHandler.h"
#include "IEffectManager.h"
#include "CCamera.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"
#include "CGraphic.h"
#include "SQRWnd.h"
#include "CIme.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

//------------------------------------------------------------------------------
namespace sqr
{
	const float CanvasWidth = 1024;
	const float CanvasHeight = 768;

	//------------------------------------------------------------------------------
	CGuiHandler::CGuiHandler( IEffectManager* manager, const string& fxPath )
		: effectManager(manager)
	{
		m_fxDefPath = fxPath.c_str();

		this->renderGroup = new CRenderGroup;
		this->renderGroup->EnableFog(false);
		RenderQueueFlag sceneflag;
		sceneflag << ID_SCENE_FILTER << ID_ALPHA_SCENE_FILTER << ID_DYNAMIC_VB ;
		this->renderGroup->SetRenderQueueFlag(sceneflag);
		CCamera* camera = this->renderGroup->GetCamera();
		camera->SetOrthoProject(true);
		camera->SetPosition(0, 0, -512);
		camera->lookTo(CVector3f(0, 0, 1));
		this->viewport = CMainWindowTarget::GetInst()->AddViewport();
		this->viewport->Release();
		this->viewport->SetRenderGroup(this->renderGroup);
	}

	//------------------------------------------------------------------------------
	CGuiHandler::~CGuiHandler()
	{
		SafeRelease(this->renderGroup);
		this->viewport = NULL;
		EffectMap::iterator it = this->ImeEffectMap.begin();
		while (this->ImeEffectMap.end() != it)
		{
			IEffect* effect = it->second.m_pEffect;
			if (NULL != effect)
			{
				SafeRelease(effect);
			}
			it++;
		}
		ImeEffectMap.clear();

		it = this->effectMap.begin();
		while (this->effectMap.end() != it)
		{
			IEffect* effect = it->second.m_pEffect;
			if (NULL != effect)
			{
				SafeRelease(effect);
			}
			it++;
		}
		effectMap.clear();
		if (this->effectManager)
			this->effectManager->ClearEffectDesc();
	}

	//------------------------------------------------------------------------------
	void
		CGuiHandler::OnPlayFx( SQRWnd* pWnd, const char* fxName, EEffectRun eEffrun )
	{
		OnPlayFxEx(pWnd, m_fxDefPath.c_str(), fxName, eEffrun);
	}
	//------------------------------------------------------------------------------
	void*
		CGuiHandler::OnPlayFxIme( SQRWnd* pWnd, const char* fxName, EEffectRun eEffrun )
	{
		return OnPlayFxImeEx(pWnd, m_fxDefPath.c_str(), fxName, eEffrun);
	}
	//--------------------------------------------------------------------------------
	void  
		CGuiHandler::OnPlayFxEx( SQRWnd* pWnd, const char* fxFileName, const char* fxName, EEffectRun eEffrun )
	{
		if (NULL == fxName || 0 == fxName[0] || NULL == fxFileName || 0 == fxFileName[0])
			return;
		EffectKey key = make_pair(pWnd, UIString(fxName));
		EffectMap::iterator it = this->effectMap.find(key);
		if (this->effectMap.end() != it)
			return;
		
		bool bAsyn = CPkgFile::IsEnableAsyncRead();
		CPkgFile::EnableAsyncRead(false);
		
		IEffectGroup*		eftGroup = NULL;
		IEffect*			effect = NULL;
		if( FAILED( effectManager->CreateEffectGroup( fxFileName, &eftGroup ) ) )
		{
			SafeRelease( eftGroup );
			CPkgFile::EnableAsyncRead(bAsyn);
			return;
		}

		this->effectManager->CreateEffect(eftGroup, fxName, &effect);
		SafeRelease( eftGroup );
		CPkgFile::EnableAsyncRead(bAsyn);

		if (NULL != effect)
		{
			this->effectMap.insert(make_pair(key, CGuiEffect(0,effect,eEffrun)));
		}
	}
	//--------------------------------------------------------------------------------
	void* 
		CGuiHandler::OnPlayFxImeEx( SQRWnd* pWnd, const char* fxFileName, const char* fxName, EEffectRun eEffrun )
	{
		if (NULL == fxName || 0 == fxName[0] || NULL == fxFileName || 0 == fxFileName[0])
			return NULL;
		EffectKey key = make_pair(pWnd, UIString(fxName));
		EffectMap::iterator it = this->ImeEffectMap.find(key);
		if (this->ImeEffectMap.end() != it)
		{
			it->second.m_uEndTime = 0;
			return NULL;
		}

		bool bAsyn = CPkgFile::IsEnableAsyncRead();
		CPkgFile::EnableAsyncRead(false);

		IEffectGroup*		eftGroup = NULL;
		IEffect*			effect = NULL;
		if( FAILED( effectManager->CreateEffectGroup( fxFileName, &eftGroup ) ) )
		{
			SafeRelease( eftGroup );
			CPkgFile::EnableAsyncRead(bAsyn);
			return NULL;
		}

		this->effectManager->CreateEffect(eftGroup, fxName, &effect);
		SafeRelease( eftGroup );
		CPkgFile::EnableAsyncRead(bAsyn);


		if (NULL != effect)
		{
			this->ImeEffectMap.insert(make_pair(key, CGuiEffect(0,effect,eEffrun)));
			return effect;
		}
		return NULL;
	}

	//------------------------------------------------------------------------------
	void
		CGuiHandler::OnClearFx( SQRWnd* pWnd, const char* fxName )
	{
		if (NULL == fxName || 0 == fxName[0])
			return;
		EffectKey key = make_pair(pWnd, UIString(fxName));
		EffectMap::iterator it = this->effectMap.find(key);
		if (this->effectMap.end() != it)
		{
			IEffect* effect = it->second.m_pEffect;
			this->effectMap.erase(it);
			if (NULL != effect)
			{
				SafeRelease(effect);
			}
			
		}
	}

	void
		CGuiHandler::OnClearFx( SQRWnd* pWnd )
	{
		EffectMap::iterator it = this->effectMap.begin();
		while (this->effectMap.end() != it)
		{
			if (it->first.first == pWnd)
			{
				IEffect* effect = it->second.m_pEffect;
				this->effectMap.erase(it);
				if (NULL != effect)
				{
					SafeRelease(effect);
				}

				it = this->effectMap.begin();
			}
			else
				++it;
		}
	}

	//------------------------------------------------------------------------------
	void
		CGuiHandler::OnClearImeFx( SQRWnd* pWnd, const char* fxName )
	{
		if (NULL == fxName || 0 == fxName[0])
			return;
		EffectKey key = make_pair(pWnd, UIString(fxName));
		EffectMap::iterator it = this->ImeEffectMap.find(key);
		if (this->ImeEffectMap.end() != it)
		{
			uint32 et = it->second.m_uEndTime;
			IEffect* effect = it->second.m_pEffect;
			if (NULL != effect)
			{
				this->effectMap.insert(make_pair(key, CGuiEffect(et,effect)));
			}
			this->ImeEffectMap.erase(it);
		}
	}

	//------------------------------------------------------------------------------
	void
		CGuiHandler::OnUpdateFx( uint curTime, float zoom )
	{
		SQR_TRY
		{
			CGraphic::GetInst()->DrawStack();
		}
		SQR_CATCH(exp)
		{
			exp.AppendType("CGuiHandler::OnUpdateFx");
			GfkLogExp(exp);
			return;
		}
		SQR_TRY_END;

		CGraphic::GetInst()->ActiveRenderTarget(this->viewport);
		CIRect rect;
		this->viewport->GetRect(rect);

		for (EffectMap::iterator it = this->effectMap.begin(); it != this->effectMap.end(); )
		{
			SQRWnd* window = it->first.first;
			IEffect* effect = it->second.m_pEffect;
			CMatrix transform;
			if (NULL != window)
			{
				float x = float(window->GetWndLeft());
				float y = float(window->GetWndTop());
				x += window->GetWndWidth() * 0.5f;
				y += window->GetWndHeight() * 0.5f;
				transform.SetTranslate(x, -y, 0);
			}
			else
				transform.SetTranslate(rect.Width() * 0.5f, -rect.Height() * 0.5f, 0);

			transform._11 = transform._22 = transform._33 = zoom;

			if( 0==it->second.m_uEndTime )
				it->second.m_uEndTime = curTime + effect->GetTimeRange();
			// Normal 运行模式
			if (it->second.m_eEffRunMode == eEffRun_Normal)
			{
				if( it->second.m_uEndTime > curTime)
				{
					effect->UpdateTime(curTime);
					effect->Render(transform);
					++it;
				}
				else
				{
					effect->UpdateTime(it->second.m_uEndTime);
					effect->Render(transform);
					this->effectMap.erase(it++);
					SafeRelease(effect);
				}
			}
			// Repeat 循环运行模式 需要手动删除
			else if (it->second.m_eEffRunMode == eEffRun_Repeat)
			{
				effect->UpdateTime(curTime);
				effect->Render(transform);
				++it;
			}
			// Last 超过时间播放最后一帧模式 需要手动删除
			else if (it->second.m_eEffRunMode == eEffRun_Last)
			{
				if( it->second.m_uEndTime > curTime)
				{
					effect->UpdateTime(curTime);
					effect->Render(transform);
				}
				else
				{
					effect->Render(transform);
				}

				++it;
			}
		}

		CGraphic::GetInst()->ActiveRenderTarget(NULL);
		this->viewport->Render();
		CMainWindowTarget::GetInst()->UpdateViewport();
	}

	//------------------------------------------------------------------------------
	bool 
		CGuiHandler::OnRenderFx( EffectKey& key, uint curTime, float zoom )
	{
		SQR_TRY
		{
			CGraphic::GetInst()->DrawStack();
		}
		SQR_CATCH(exp)
		{
			exp.AppendType("CGuiHandler::OnRenderFx");
			GfkLogExp(exp);
			return false;
		}
		SQR_TRY_END;

		bool succ = false;
		EffectMap::iterator it = this->ImeEffectMap.find(key);
		if ( it != this->ImeEffectMap.end() )
		{
			CGraphic::GetInst()->ActiveRenderTarget(this->viewport);
			CIRect rect;
			this->viewport->GetRect(rect);

			SQRWnd* window = it->first.first;
			IEffect* effect = it->second.m_pEffect;
			effect->SetVisible(true);
			CMatrix transform;
			if (NULL != window)
			{
				float x = float(window->GetWndLeft());
				float y = float(window->GetWndTop());
				x += window->GetWndWidth() * 0.5f;
				y += window->GetWndHeight() * 0.5f;
				transform.SetTranslate(x, -y, 0);
			}
			else
				transform.SetTranslate(rect.Width() * 0.5f, -rect.Height() * 0.5f, 0);

			transform._11 = transform._22 = transform._33 = zoom;

			if( 0==it->second.m_uEndTime )
				it->second.m_uEndTime = curTime + effect->GetTimeRange();
			
			// Normal 运行模式
			if (it->second.m_eEffRunMode == eEffRun_Normal)
			{
				if( it->second.m_uEndTime > curTime)
				{
					effect->UpdateTime(curTime);
					effect->Render(transform);
					succ = true;
				}
				else
				{
					effect->SetVisible(false);
					this->ImeEffectMap.erase(it);
					SafeRelease(effect);
					succ = false;
				}
			}
			// Repeat 循环运行模式 需要手动删除
			else if (it->second.m_eEffRunMode == eEffRun_Repeat)
			{
				effect->UpdateTime(curTime);
				effect->Render(transform);
				succ = true;
			}
			// Last 超过时间播放最后一帧模式 需要手动删除
			else if (it->second.m_eEffRunMode == eEffRun_Last)
			{
				if( it->second.m_uEndTime > curTime)
				{
					effect->UpdateTime(curTime);
					effect->Render(transform);
				}
				else
				{
					effect->Render(transform);
				}
				succ = true;
			}

			CGraphic::GetInst()->ActiveRenderTarget(NULL);
			this->viewport->Render();
			CMainWindowTarget::GetInst()->UpdateViewport();
			if(effect)
				effect->SetVisible(false);
		}
		
		return succ;
	}

	//------------------------------------------------------------------------------
	void
		CGuiHandler::OnEnableIme( bool b)
	{
		if (CIme::GetIme())
			CIme::GetIme()->EnableIme(b);
	}

	//------------------------------------------------------------------------------
	void
		CGuiHandler::OnWndSized( const CFRect& rect )
	{
		CIRect rect_tmp;
		rect_tmp.left	= (int32)rect.left;
		rect_tmp.top	= (int32)rect.top;
		rect_tmp.right	= (int32)rect.right;
		rect_tmp.bottom	= (int32)rect.bottom;
		this->viewport->SetRect(rect_tmp);
		float halfWidth = rect.Width() * 0.5f;
		float halfHeight = rect.Height() * 0.5f;
		CCamera* camera = this->renderGroup->GetCamera();
		camera->SetOrthoSize(rect.Width(), rect.Height());
		camera->SetPosition(halfWidth, -halfHeight, -512);
	}

}// namespace sqr
