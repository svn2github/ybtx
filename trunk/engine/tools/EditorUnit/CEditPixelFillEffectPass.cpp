#include "StdAfx.h"
#include "CEditPixelFillEffectPass.h"
#include "CEditRenderPipeline.h"
#include "CGraphic.h"
#include "CCamera.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"
#include "CScreenAlignedQuad.h"
#include "CVertexDeclaration.h"
#include "CRenderSystem.h"

namespace sqr
{

CPixelFillEffectPass::CPixelFillEffectPass(void)
{
	IsAlpha=false;

	zone_blue=1;
	zone_green=4;
	zone_yellow=5;
	zone_red=8;
	zone_white=13;
	 
	m_Param[0].color=0x00000000;  m_Param[0].func=CMP_EQUAL;     m_Param[0].ref=0;  //黑
	m_Param[1].color=0xffffffff;  m_Param[1].func=CMP_LESSEQUAL; m_Param[1].ref=16; //白  下限
	m_Param[2].color=0xdfff0000;  m_Param[2].func=CMP_LESSEQUAL; m_Param[2].ref=11;  //红
	m_Param[3].color=0xafffff00;  m_Param[3].func=CMP_LESSEQUAL; m_Param[3].ref=7;  //黄
	m_Param[4].color=0x8f00ff00;  m_Param[4].func=CMP_LESSEQUAL; m_Param[4].ref=4;  //绿
	m_Param[5].color=0x5f0000ff;  m_Param[5].func=CMP_LESSEQUAL; m_Param[5].ref=1;  //蓝

	flag_blue=true;
	flag_green=true;
	flag_yellow=true;
	flag_red=true;
	flag_white=true;

	CGraphic::GetInst()->GetActiveRenderTarget()->GetRect(m_CurRect);
	//构建数据
	VerText2D quad[4];
	quad[0]=VerText2D(0, 1, 0, 0,0);
	quad[1]=VerText2D(0, 0, 0, 0,1);
	quad[2]=VerText2D(1, 1, 0, 1,0);
	quad[3]=VerText2D(1, 0, 0, 1,1);

	quad[0].x*=m_CurRect.Width();
	quad[0].y*=m_CurRect.Height();
	quad[1].x*=m_CurRect.Width();
	quad[1].y*=m_CurRect.Height();
	quad[2].x*=m_CurRect.Width();
	quad[2].y*=m_CurRect.Height();
	quad[3].x*=m_CurRect.Width();
	quad[3].y*=m_CurRect.Height();

	m_Op.Create();
	m_Op.vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(VerText2D::Format);
	m_Op.useIndexes = false;
	m_Op.vertexData->vertexStart = 0;
	m_Op.vertexData->vertexCount = 4;
	m_Op.operationType = PT_TRIANGLESTRIP;
	CVertexHardwareBuffer* pBuf = CHardwareBufferMgr::GetInst()->createVertexBuffer( sizeof(VerText2D),4,HBU_DYNAMIC_WRITE_ONLY,quad);
	m_Op.vertexData->vertexBufferBinding->SetBinding(0,pBuf);

}

CPixelFillEffectPass::~CPixelFillEffectPass(void)
{
	m_Op.vertexData->Destroy();
	m_Op.Destroy();
}

void CPixelFillEffectPass::Apply()
{
	CEditRenderPipeline::GetInst()->DisablePixelFillCount();
	StencilParam tmpStencilParam;

	tmpStencilParam.m_bEnable=TRUE;
	tmpStencilParam.m_nCompFun=CMP_GREATEREQUAL;
	tmpStencilParam.m_nRefrence=0;
	tmpStencilParam.m_nFailedFlag=STENCILOP_KEEP;
	tmpStencilParam.m_nZFailedFlag=STENCILOP_KEEP;
	tmpStencilParam.m_nPassFlag=STENCILOP_KEEP;
	tmpStencilParam.m_nReadMask=0xFFFFFFFF;
	tmpStencilParam.m_nWriteMask=0xFFFFFFFF;
	CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILENABLE,		DWORD(tmpStencilParam.m_bEnable) );
	CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILREF,			DWORD(tmpStencilParam.m_nRefrence) );
	CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILFUNC,		tmpStencilParam.m_nCompFun );
	CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILFAIL,		tmpStencilParam.m_nFailedFlag );
	CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILZFAIL,		tmpStencilParam.m_nZFailedFlag );
	CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILPASS,		tmpStencilParam.m_nPassFlag );
	CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILMASK,		tmpStencilParam.m_nReadMask );
	CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILWRITEMASK,	tmpStencilParam.m_nWriteMask );

	//渲染
	CIRect tmpRect;
	CGraphic::GetInst()->GetActiveRenderTarget()->GetRect(tmpRect);

	if((tmpRect.Width()!=m_CurRect.Width()) || (tmpRect.Height()!=m_CurRect.Height()) )
	{
		m_CurRect=tmpRect;
		UpdateData();
	}
	CEditRenderPipeline::GetInst()->DisablePixelFillCount();
	if(!IsAlpha)
		RenderWithDif();
	else
		RenderWithAlpha();
	CEditRenderPipeline::GetInst()->EnablePixelFillCount();
}

void CPixelFillEffectPass::RenderWithDif()
{
	STATIC_RS tmpRS;
	//纹理混合
	tmpRS.m_TexOP_S0=TOP_SELECTARG1;
	tmpRS.m_TexArg1_S0=TA_TFACTOR;
	//Alpha混合
	tmpRS.m_AlphaBlendEnable=false;
	CEditRenderPipeline::GetInst()->SetRenderStyle(tmpRS);
	//通过测试都置成0
	CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILPASS,STENCILOP_ZERO);
	for(int i=0;i<6;i++)
	{
		CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILREF,   (DWORD)m_Param[i].ref);
		CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILFUNC,   m_Param[i].func);
		CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_TEXTUREFACTOR, m_Param[i].color);
		CRenderPipeline::GetInst()->RenderEx(&m_Op);
	}	
}

void CPixelFillEffectPass::RenderWithAlpha()
{

	//构建一张带Alpha的图片
	STATIC_RS tmpRS;
	//纹理混合
	tmpRS.m_TexOP_S0=TOP_BLENDFACTORALPHA;
	tmpRS.m_TexArg1_S0=TA_TFACTOR;
	tmpRS.m_TexArg2_S0=TA_CURRENT;

	tmpRS.m_AlphaBlendEnable=true;
	tmpRS.m_AlpOP_S0=TOP_SELECTARG1;
	tmpRS.m_AlpArg1_S0=TA_TFACTOR;

	//Alpha混合
	CEditRenderPipeline::GetInst()->SetRenderStyle(tmpRS);
	StencilParam tmpStencilParam;
	//通过测试都置成0
	CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILPASS,STENCILOP_ZERO );
	for(int i=0;i<6;i++)
	{
		CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILREF,   (DWORD)m_Param[i].ref);
		CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_STENCILFUNC,   m_Param[i].func);
		CEditRenderPipeline::GetInst()->_SetRenderStyle( RS_TEXTUREFACTOR, m_Param[i].color);
		CRenderPipeline::GetInst()->RenderEx(&m_Op);
	}
}

void CPixelFillEffectPass::UpdateData()
{
	VerText2D quad[4];
	quad[0]=VerText2D(0, 1, 0, 0,0);
	quad[1]=VerText2D(0, 0, 0, 0,1);
	quad[2]=VerText2D(1, 1, 0, 1,0);
	quad[3]=VerText2D(1, 0, 0, 1,1);

	quad[0].x*=m_CurRect.Width()+m_CurRect.left;
	quad[0].y*=m_CurRect.Height()+m_CurRect.top;
	quad[1].x*=m_CurRect.Width()+m_CurRect.left;
	quad[1].y*=m_CurRect.Height()+m_CurRect.top;
	quad[2].x*=m_CurRect.Width()+m_CurRect.left;
	quad[2].y*=m_CurRect.Height()+m_CurRect.top;
	quad[3].x*=m_CurRect.Width()+m_CurRect.left;
	quad[3].y*=m_CurRect.Height()+m_CurRect.top;
	CVertexHardwareBuffer* pBuf=m_Op.vertexData->vertexBufferBinding->GetBuffer(0);
	void* pDst=pBuf->lock(0,sizeof(VerText2D)*4,HBL_DISCARD);
	memcpy(pDst,quad,sizeof(VerText2D)*4);
	pBuf->unlock();
}

void CPixelFillEffectPass::SetBlueZone(int value)
{
	if(value<0)
		return ;
	zone_blue=value;
}
void CPixelFillEffectPass::SetGreenZone(int value)
{ 
	zone_green=value;
}
void CPixelFillEffectPass::SetYellowZone(int value)
{ 
	zone_yellow=value;
}
void CPixelFillEffectPass::SetRedZone(int value)
{	 
	zone_red=value;
}
void CPixelFillEffectPass::SetWhiteZone(int value)
{ 
	zone_white=value;
}

int CPixelFillEffectPass::GetBlueZone(){return zone_blue;}
int CPixelFillEffectPass::GetGreenZone(){return zone_green;}
int CPixelFillEffectPass::GetYellowZone(){return zone_yellow;}
int CPixelFillEffectPass::GetRedZone(){return zone_red;}
int CPixelFillEffectPass::GetWhiteZone(){return zone_white;}

}
