//------------------------------------------------------------------------------
//  CGuiFxPlayer.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "CGuiFxPlayer.h"
#include "IEffectManager.h"
#include "CCamera.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"
#include "SQRWnd.h"
#include "CGraphic.h"


//------------------------------------------------------------------------------
namespace sqr
{
const float CanvasWidth = 1024;
const float CanvasHeight = 768;
const float FocusX = CanvasWidth / 2;
const float FocusY = - CanvasHeight / 2;


//------------------------------------------------------------------------------
CGuiFxPlayer::CGuiFxPlayer( IEffectManager* manager, const string& fxPath )
: effectManager(manager)
, effectGroup(NULL)
, m_PreView(false)
{
	this->effectManager->CreateEffectGroup(fxPath.c_str(), &this->effectGroup);

	this->renderGroup = new CRenderGroup;
	this->renderGroup->EnableFog(false);
	CCamera* camera = this->renderGroup->GetCamera();
	camera->SetPosition(FocusX, FocusY, -512);
	camera->SetOrthoProject(true);
	camera->SetOrthoSize(CanvasWidth, CanvasHeight);
	camera->lookAt(FocusX, FocusY, 0);
	RenderQueueFlag sceneflag;
	sceneflag	<< ID_SCENE_FILTER
				<< ID_DYNAMIC_VB;
	this->renderGroup->SetRenderQueueFlag(sceneflag);
	this->viewport = CMainWindowTarget::GetInst()->AddViewport();
	this->viewport->SetRenderGroup(this->renderGroup);
	this->viewport->SetRect(CIRect(0, 0, int(CanvasWidth), int(CanvasHeight)));
}

//------------------------------------------------------------------------------
CGuiFxPlayer::~CGuiFxPlayer()
{
	this->viewport = NULL;
}

//------------------------------------------------------------------------------
void
CGuiFxPlayer::OnPlayFx( SQRWnd* pWnd, const char* fxName, EEffectRun eEffrun )
{
	if (NULL == fxName || 0 == fxName[0])
		return;
	if (!m_PreView)
		return;
	IEffect* effect = NULL;
	this->effectManager->CreateEffect(this->effectGroup, fxName, &effect);
	if (NULL != effect)
	{
		this->effects.push_back(EffectUnit(effect, pWnd));
	}
}

//------------------------------------------------------------------------------
void* 
CGuiFxPlayer::OnPlayFxIme( SQRWnd* pWnd, const char* fxName, EEffectRun eEffrun )
{
	if (NULL == fxName || 0 == fxName[0])
		return NULL;
	if (!m_PreView)
		return NULL;

	for (list<EffectUnit>::iterator it = this->effectsIme.begin(); it != this->effectsIme.end(); )
	{
		EffectUnit& unit = *it;

		if ( unit.window == pWnd && unit.fx_name == (UIString)(fxName) )
		{
			return NULL;
		}
		else
		{
			it++;
		}
	}

	IEffect* effect = NULL;
	this->effectManager->CreateEffect(this->effectGroup, fxName, &effect);
	if (NULL != effect)
	{
		this->effectsIme.push_back(EffectUnit(effect, pWnd, fxName));
	}
	return effect;
}

//------------------------------------------------------------------------------
void
CGuiFxPlayer::OnClearFx( SQRWnd* pWnd, const char* fxName )
{
	return;
}

//------------------------------------------------------------------------------
void
CGuiFxPlayer::OnClearFx( SQRWnd* pWnd )
{
	return;
}

//------------------------------------------------------------------------------
void
CGuiFxPlayer::OnClearImeFx( SQRWnd* pWnd, const char* fxName )
{
	for (list<EffectUnit>::iterator it = this->effectsIme.begin(); it != this->effectsIme.end(); )
	{
		EffectUnit& unit = *it;
		SafeRelease(unit.effect);
		this->effectsIme.erase(it++);
	}
	return;
}

//------------------------------------------------------------------------------
bool
CGuiFxPlayer::OnRenderFx( EffectKey& key, uint curTime, float zoom )
{
	CGraphic::GetInst()->DrawStack();
	CGraphic::GetInst()->ActiveRenderTarget(this->viewport);
	
	bool succ = false;
	for (list<EffectUnit>::iterator it = this->effectsIme.begin(); it != this->effectsIme.end(); )
	{
		EffectUnit& unit = *it;
		
		if ( unit.window == key.first && unit.fx_name == key.second )
		{
			float x = float(unit.window->GetWndLeft());
			float y = -float(unit.window->GetWndTop());
			x += unit.window->GetWndWidth() * 0.5f;
			y -= unit.window->GetWndHeight() * 0.5f;
			CMatrix transform;
			transform.SetTranslate(x, y, 0);
			
			bool isEnd = unit.effect->UpdateTime(curTime);
			unit.effect->Render(transform);
			if (isEnd)
			{
				SafeRelease(unit.effect);
				this->effectsIme.erase(it++);
				succ = false;
			}
			else
			{
				succ = true;
			}
			break;
		}
		else
		{
			it++;
		}
	}

	this->effectManager->Update(curTime);
	CGraphic::GetInst()->ActiveRenderTarget(NULL);
	this->viewport->Render();
	CMainWindowTarget::GetInst()->UpdateViewport();
	return succ;
}

//------------------------------------------------------------------------------
void
CGuiFxPlayer::OnUpdateFx( uint curTime, float zoom )
{
	CGraphic::GetInst()->DrawStack();
	CGraphic::GetInst()->ActiveRenderTarget(this->viewport);

	for (list<EffectUnit>::iterator it = this->effects.begin(); it != this->effects.end(); )
	{
		EffectUnit& unit = *it;
		float x = float(unit.window->GetWndLeft());
		float y = -float(unit.window->GetWndTop());
		x += unit.window->GetWndWidth() * 0.5f;
		y -= unit.window->GetWndHeight() * 0.5f;
		CMatrix transform;
		transform.SetTranslate(x, y, 0);

		bool isEnd =  unit.effect->UpdateTime(curTime);
		unit.effect->Render(transform);
		if (isEnd)
		{
			SafeRelease(unit.effect);
			this->effects.erase(it++);
		}
		else
			++it;
	}
	this->effectManager->Update(curTime);

	CGraphic::GetInst()->ActiveRenderTarget(NULL);
	this->viewport->Render();
	CMainWindowTarget::GetInst()->UpdateViewport();
}

//------------------------------------------------------------------------------
void
CGuiFxPlayer::SetBePreView(bool pre)
{
	m_PreView = pre;
}

}// namespace sqr
