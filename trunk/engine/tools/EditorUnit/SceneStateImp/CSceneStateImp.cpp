#include "StdAfx.h"
#include "SceneStateImp/CSceneStateImp.h"
#include "CEditRenderPipeline.h"
#include "CEditPostProcessManager.h"
#include "CPostEffect.h"
#include "CEditPixelFillEffectPass.h"

CSceneStateImp::CSceneStateImp(void)
{
	ColorRegion[STATE_BLACK]=StateRegion(0,0);
	ColorRegion[STATE_BLUE]=StateRegion(1,3);
	ColorRegion[STATE_GREEN]=StateRegion(4,6);
	ColorRegion[STATE_YELLOW]=StateRegion(7,10);
	ColorRegion[STATE_RED]=StateRegion(11,15);
	ColorRegion[STATE_WHITE]=StateRegion(16,16);

	EffectSwitch=false;
	AlphaSwitch=false;

	pnum =0;
	fnum =0;
	texnum =0;
	dpnum =0;
}

CSceneStateImp::~CSceneStateImp(void)
{
}

void CSceneStateImp::Init()
{
}

void CSceneStateImp::GetColorRegion(COLORREGION colorflag,int& min,int& max)
{
	if(colorflag>=STATE_REGIONNUM)
		return ;
	min=ColorRegion[colorflag].min;
	max=ColorRegion[colorflag].max;
}

std::string CSceneStateImp::GetColorRegionDescription(COLORREGION colorflag)
{
	std::string description;
	if(ColorRegion[colorflag].min!=ColorRegion[colorflag].max)
	{
		description  =ColorRegion[colorflag].min;
		description += "---";
		description +=ColorRegion[colorflag].max+"层";
	}
	else
	{
		if(colorflag == STATE_BLACK)
		{
			description = ColorRegion[colorflag].min+"层";
		}
		else
		{
			description =">=";
			description +=ColorRegion[colorflag].max +"层";
		}
	}
	return description;
}

unsigned int CSceneStateImp::GetPNum()
{
	return pnum;
}

unsigned int CSceneStateImp::GetFNum()
{
	return fnum;
}

unsigned int CSceneStateImp::GetTexNum()
{
	return texnum;
}

unsigned int CSceneStateImp::GetDPNum()
{
	return dpnum;
}

void CSceneStateImp::GetPNumString(std::string& str)
{
	if(pnum>=1000)
		str=pnum%1000+"k";
	else
		str=pnum;
}

void CSceneStateImp::GetFNumString(std::string& str)
{
	str.clear();
	if(fnum>=1000)
		str=fnum%1000+"k";
	else
		str=fnum;
}

void CSceneStateImp::GetTexNumString(std::string& str)
{
	str.clear();
	str=texnum;
}

void CSceneStateImp::GetDPNumString(std::string& str)
{
	str.clear();
	str=dpnum;
}

void CSceneStateImp::EnableEffect(bool enable)
{
	EffectSwitch=enable;
	if(EffectSwitch)  //开启
	{
		CEditPostProcessManager::GetInst()->DelEffect("PixelFillCount");
		CEditRenderPipeline::GetInst()->EnablePixelFillCount();
		CPostEffectPtr PixelFillEffect=new CPostEffect();
		PixelFillEffect->SetName("PixelFillCount");
		CPixelFillEffectPass* m_PFillPass=new CPixelFillEffectPass();
		PixelFillEffect->AddPass(m_PFillPass);
		if(AlphaSwitch)
			m_PFillPass->EnableAlpha();
		else
			m_PFillPass->DisableAlpha();
		CEditPostProcessManager::GetInst()->AddEffect(PixelFillEffect);
	}
	else  //关闭
	{
		CEditPostProcessManager::GetInst()->DelEffect("PixelFillCount");
		CEditRenderPipeline::GetInst()->DisablePixelFillCount();
		//关闭模版
		StencilParam m_stencil;
		m_stencil.m_bEnable=FALSE;
		CEditRenderPipeline::GetInst()->SetStencil(m_stencil);
	}
}

void CSceneStateImp::EnableAlpha(bool enable)
{
	AlphaSwitch=enable;

	//删除旧特效 创建新特效
	CEditPostProcessManager::GetInst()->DelEffect("PixelFillCount");
	CEditRenderPipeline::GetInst()->EnablePixelFillCount();
	CPostEffectPtr PixelFillEffect=new CPostEffect();
	PixelFillEffect->SetName("PixelFillCount");
	CPixelFillEffectPass* m_PFillPass=new CPixelFillEffectPass();
	PixelFillEffect->AddPass(m_PFillPass);
	if(AlphaSwitch)
		m_PFillPass->EnableAlpha();
	else
		m_PFillPass->DisableAlpha();
	CEditPostProcessManager::GetInst()->AddEffect(PixelFillEffect);
}

bool CSceneStateImp::IsEffectEnable()
{
	return EffectSwitch;
}

bool CSceneStateImp::IsAlphaEnable()
{
	return AlphaSwitch;
}

void CSceneStateImp::Update()
{
	//pnum=CEditRenderPipeline::GetInst()->GetVertexCount();
	//fnum=CEditRenderPipeline::GetInst()->GetFaceCount();
	//dpnum=CEditRenderPipeline::GetInst()->GetPrimitiveCount();
	//texnum=CEditRenderPipeline::GetInst()->GetTexCount();

	CCtrlBase::Update();
}

bool CSceneStateImp::_ProcMsg(const CMsg& msg )
{
	switch(msg.msgSource)
	{
	case MGS_CUSTOM:
		{
			switch(msg.msgType.custom_type)
			{
			case MGT_SCENESTATE_ENABLE:
				{
					EnableEffect(true);
					break;
				}
			case MGT_SCENESTATE_DISABLE:
				{
					EnableEffect(false);
					break;
				}
			case MGT_SCENESTATE_ALPHA:
				{
					EnableAlpha(true);
					break;
				}
			case MGT_SCENESTATE_FULL:
				{
					EnableAlpha(false);
					break;
				}
			}
		}
	}
	return false;
}
