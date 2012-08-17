#include "StdAfx.h"
#include "VideoSettingImp/CVideoSettingImp.h"
#include "CEditorConfig.h"
#include "CRenderSystem.h"

CVideoSettingImp::CVideoSettingImp(void)
{
	realState.AliaxLevel = 0;
	realState.ShadowLevel = 0;
	realState.TerrainLevel = 0;
	realState.WaterLevel = 0;
	realState.TexLevel = 0;
	realState.EffectLevel = 0;
	realState.IsRefract = false;
	realState.IsBloom = false;
	realState.IsSyn = 0;
	realState.WindowWidth = 1;
	realState.WindowHeight = 1;
	realState.FrameBorder = 1;
	tmpState=realState;
}

CVideoSettingImp::~CVideoSettingImp(void)
{
}

void CVideoSettingImp::InitState()
{
	RenderSettings tmpSet;
	CRenderSystem::GetInst()->GetRenderSettings(tmpSet);
	EAntiAliasQuality	oldAliaxLevel=tmpSet.GetAntiAliasQuality();
	EShadowType			oldShadowLevel = CRenderSystem::GetInst()->GetShadowType();
	unsigned int		oldTerrainLevel = (unsigned int)CRenderSystem::GetInst()->IsUseShaderTerrain();
	EWaterType			oldWaterLevel = CRenderSystem::GetInst()->GetWaterType();
	unsigned int		oldTexLevel = CRenderSystem::GetInst()->GetTexQualityLevel();
	float				oldEffectLevel = CRenderSystem::GetInst()->GetEffectLevel();
	bool				oldIsRefract = CRenderSystem::GetInst()->IsRefractEnabled();
	bool				oldIsBloom = CRenderSystem::GetInst()->IsBloomEnabled();
	unsigned int		oldIsSyn = CRenderSystem::GetInst()->IsShowASyn();
	//反走样质量
	if(oldAliaxLevel == AA_None)
		realState.AliaxLevel = 0;
	else if(oldAliaxLevel == AA_Low)
		realState.AliaxLevel = 1;
	else
		realState.AliaxLevel = 2;
	//阴影
	if(oldShadowLevel == ST_NONE)
		realState.ShadowLevel = 0;
	else if(oldShadowLevel == ST_SHADOWMAP)
		realState.ShadowLevel = 1;
	else 
		realState.ShadowLevel = 2;
	//地面和水面
	realState.TerrainLevel = oldTerrainLevel;
	if(oldWaterLevel == WT_Low)
		realState.WaterLevel = 0;
	else
		realState.WaterLevel = 1;
	//纹理和特效
	realState.TexLevel=oldTexLevel;
	if(oldEffectLevel <3.0f)
		realState.EffectLevel = 0; 
	else if(oldEffectLevel>7.0)
		realState.EffectLevel = 2;
	else
		realState.EffectLevel = 1;
	//其他
	realState.IsRefract = oldIsRefract;
	realState.IsBloom = oldIsBloom;
	realState.IsSyn = oldIsSyn;
	realState.WindowWidth = tmpSet.GetWindowWidth();
	realState.WindowHeight =  tmpSet.GetWindowHeight();
	realState.FrameBorder = 1;

	tmpState=realState;

	this->Update();
}

void CVideoSettingImp::ResetState()
{
	tmpState=realState;
	UpdateState();
}

void CVideoSettingImp::AppState()
{
	UpdateState();
}

void CVideoSettingImp::EnsureState()
{
	realState=tmpState;
	UpdateState();
}

void CVideoSettingImp::UpdateState()
{
	//反锯齿
	if(realState.AliaxLevel!=tmpState.AliaxLevel)
	{
		RenderSettings tmpSet;
		CRenderSystem::GetInst()->GetRenderSettings(tmpSet);
		if(tmpState.AliaxLevel==0)
			tmpSet.SetAntiAliasQuality(AA_None);
		else if(tmpState.AliaxLevel ==1)
			tmpSet.SetAntiAliasQuality(AA_Low);
		else
			tmpSet.SetAntiAliasQuality(AA_High);
		tmpSet.SetIsCtrlWindow(false);
		CRenderSystem::GetInst()->ChangeRenderSettings(&tmpSet);
	}
	//阴影
	if(realState.ShadowLevel!=tmpState.ShadowLevel)
	{
		if(tmpState.ShadowLevel == 0)
			CRenderSystem::GetInst()->SetShadowType(ST_NONE);
		else
			CRenderSystem::GetInst()->SetShadowType(ST_SHADOWMAP);
	}
	//地形
	if(realState.TerrainLevel!=tmpState.TerrainLevel)
		CRenderSystem::GetInst()->SetUseShaderTerrain(tmpState.TerrainLevel == 1);
	//水面
	if(realState.WaterLevel!=tmpState.WaterLevel)
	{
		if(tmpState.WaterLevel == 0)
			CRenderSystem::GetInst()->SetWaterType(WT_Low);
		else
			CRenderSystem::GetInst()->SetWaterType(WT_Default);
	}
	//贴图
	if(realState.TexLevel!=tmpState.TexLevel)
		CRenderSystem::GetInst()->SetTexQualityLevel(tmpState.TexLevel);
	//特效级别
	if( realState.EffectLevel!=tmpState.EffectLevel)
		CRenderSystem::GetInst()->SetEffectLevel(0.8f);
	//空气折射
	if(realState.IsRefract!=tmpState.IsRefract)
		CRenderSystem::GetInst()->SetRefractEnabled(tmpState.IsRefract);
	//全屏泛光
	if(realState.IsBloom!=tmpState.IsBloom)
		CRenderSystem::GetInst()->SetBloomEnabled(tmpState.IsBloom);
	//垂直同步 存在问题
	if(realState.IsSyn!=tmpState.IsSyn)
		CRenderSystem::GetInst()->ShowASynLoadSwitch();

	realState=tmpState;
}

void CVideoSettingImp::UpdateWindowRate(int rate_w,int rate_h)
{
	//RenderSettings tmpSet;
	//CRenderSystem::GetInst()->GetRenderSettings(tmpSet);

	//HWND m_ViewHWnd=tmpSet.GetWindowHandle();	
	//HWND m_BackHWnd=::GetParent(m_ViewHWnd);
	//CRect BackRect,ViewRect;
	//::GetClientRect(m_BackHWnd,&BackRect);
	//ViewRect.left=BackRect.left+FrameBorder;
	//ViewRect.top=BackRect.top+FrameBorder;
	//ViewRect.right=BackRect.right-FrameBorder*2;
	//ViewRect.bottom=BackRect.bottom-FrameBorder*2;

	//::ShowWindow(m_BackHWnd,SW_HIDE);
	//::InvalidateRect(m_BackHWnd,BackRect,true);
	////
	//::ShowWindow(m_ViewHWnd, SW_HIDE);
	///********* ADD CODE **************/
	//int tmpWidth=ViewRect.right-ViewRect.left;
	//float rat=(float)tmpWidth/(float)ratWidth;
	//int tmpHeight=int(rat*ratHeight);
	//if(tmpHeight>(ViewRect.bottom-ViewRect.top))
	//{
	//	tmpHeight=ViewRect.bottom-ViewRect.top;
	//	//以高度为基本
	//	rat=(float)tmpHeight/(float)ratHeight;
	//	tmpWidth=int(rat*ratWidth);
	//}

	//DWORD style = WS_CHILDWINDOW;
	//::SetWindowLong(m_ViewHWnd, GWL_STYLE, style);
	//::SetWindowPos(m_ViewHWnd,0,
	//	(ViewRect.left+ViewRect.right)/2-tmpWidth/2, (ViewRect.top+ViewRect.bottom)/2-tmpHeight/2,
	//	tmpWidth, tmpHeight,
	//	SWP_NOZORDER | SWP_FRAMECHANGED);
	//::ShowWindow(m_ViewHWnd, SW_NORMAL);
	//::ShowWindow(m_BackHWnd,SW_NORMAL);
}

unsigned int CVideoSettingImp::GetAliaxLevel()
{ return tmpState.AliaxLevel; }

unsigned int CVideoSettingImp::GetShadowLevel()
{ return tmpState.ShadowLevel; }

unsigned int CVideoSettingImp::GetTerrainLevel()
{ return tmpState.TerrainLevel; }

unsigned int CVideoSettingImp::GetWaterLevel()
{ return tmpState.WaterLevel; }

unsigned int CVideoSettingImp::GetTexLevel()
{ return tmpState.TexLevel; }

unsigned int CVideoSettingImp::GetEffectLevel()
{ return tmpState.EffectLevel; }

bool CVideoSettingImp::GetIsRefract()
{ return tmpState.IsRefract; }

bool			CVideoSettingImp::GetIsBloom()
{ return tmpState.IsBloom; }

unsigned int	CVideoSettingImp::GetIsSyn()
{ return tmpState.IsSyn; }

int	CVideoSettingImp::GetWindowWidth()
{ return tmpState.WindowWidth; }

int	CVideoSettingImp::GetWindowHeight()
{ return tmpState.WindowHeight; }

int	CVideoSettingImp::GetFrameBorder()
{ return tmpState.FrameBorder; }

void CVideoSettingImp::SetAliaxLevel(unsigned int level)
{ tmpState.AliaxLevel = level; }

void CVideoSettingImp::SetShadowLevel(unsigned int level)
{ tmpState.ShadowLevel = level; }

void CVideoSettingImp::SetTerrainLevel(unsigned int level)
{ tmpState.TerrainLevel = level; }

void CVideoSettingImp::SetWaterLevel(unsigned int level)
{ tmpState.WaterLevel = level; }

void CVideoSettingImp::SetTexLevel(unsigned int level)
{ tmpState.TexLevel = level; }

void CVideoSettingImp::SetEffectLevel(unsigned int level)
{ tmpState.EffectLevel = level; }

void CVideoSettingImp::SetIsRefract(bool enable)
{ tmpState.IsRefract = enable; }

void CVideoSettingImp::SetIsBloom(bool enable)
{ tmpState.IsBloom = enable; }

void CVideoSettingImp::SetIsSyn(unsigned int level)
{ tmpState.IsSyn = level; }

void CVideoSettingImp::SetWindowWidth(int width)
{ tmpState.WindowWidth = width; }

void CVideoSettingImp::SetWindowHeight(int height)
{ tmpState.WindowHeight = height; }

void CVideoSettingImp::SetFrameBorder(int border)
{ tmpState.FrameBorder = border; }

bool CVideoSettingImp::_ProcMsg(const CMsg& msg )
{
	switch(msg.msgSource)
	{
	case MGS_CUSTOM:
		{
			switch(msg.msgType.custom_type)
			{
			case MGT_VIDEOSET_UPDATE:
				{
					UpdateState();
					break;
				}
			case MGT_VIDEOSET_RESET:
				{
					ResetState();
					break;
				}
			case MGT_VIDEOSET_ENSURE:
				{
					EnsureState();
					break;
				}
			}
		}
	}
	return false;
}
