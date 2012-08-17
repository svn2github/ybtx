#include "StdAfx.h"
#include "TerrainBaker.h"
#include "CGraphic.h"
#include "CRenderPipeline.h"
#include "ShadowMapBakerPool.h"
#include "LightMapWriter.h"
#include "CLightMapFilter.h"
#include "CWindowTarget.h"
#include "OfflineRender.h"

#include "RegionShadowMapBaker.h"

CTerrainBaker::CTerrainBaker(void)
{
	m_RegionBakeHelp=new RegionShadowMapBaker();
}

CTerrainBaker::~CTerrainBaker(void)
{
	if(m_RegionBakeHelp)
		delete m_RegionBakeHelp;
}

void CTerrainBaker::Setup()
{
	CLightMapBaker::Setup();
	tileIndexList.clear();

	
	OfflineRender::GetInst()->UpdateTerrainColor(true);
}

void CTerrainBaker::Discard()
{
	OfflineRender::GetInst()->UpdateTerrainColor(false);
	CLightMapBaker::Discard();
}

void CTerrainBaker::TilePrepare(index_t x,index_t z)
{
	CLightMapBaker::TilePrepare(x,z);
	tileIndexList.push_back(x+z*tileWidth);

	OfflineRender::GetInst()->AddLightsToIlluminator(true);
}

void CTerrainBaker::RenderReceive()
{
	Ast(NULL != this->terrain);
	CRenderPipeline::GetInst()->SetRenderStyle(terrain->m_Param.m_RS);
	CRenderPipeline::GetInst()->SetWorld(terrain->m_Param.m_Mat);
	m_pShadowFilter->UpdateShadowParams();
	terrain->RenderEx();
}

void CTerrainBaker::OnProcess(index_t x,index_t z)
{
	//区域阴影Help
	m_RegionBakeHelp->RenderTile(x,z);
	//相机切换
	CRenderGroup* tempRenderGroup=CMainWindowTarget::GetInst()->GetGroup();
	CCamera* oldCamera=tempRenderGroup->GetCamera();

	this->ChangeCamera(x,z);
	CRenderPipeline::GetInst()->SetCamera(LightMapCamera);

	CRenderTexturePtr m_LightMapTarget_help;
	m_LightMapTarget_help = CGraphic::GetInst()->CreateRenderTarget();
	m_LightMapTarget_help->Release();            //RTT显存正确释放补丁
	m_LightMapTarget_help->SetWidth(512);
	m_LightMapTarget_help->SetHeight(512);
	m_LightMapTarget_help->SetFormat(TFMT_X8R8G8B8);
	m_LightMapTarget_help->Create();
	m_LightMapTarget_help->SetClearColor(CColor::White);
	m_LightMapTarget_help->Begin();
	m_LightMapTarget_help->End();

	//MultPassShader And RenderLightMap
	SHADER_HANDLE MultPassHandle;
	MultPassHandle |= MakeSysVS_ID(MakeGroupID(1) + 8);
	MultPassHandle |= MakeSysPS_ID(MakeGroupID(1) + 9);
	terrain->m_Param.m_RS.m_ShaderType=MultPassHandle;
	vector<int>::iterator it=tileIndexList.begin();
	while(it!=tileIndexList.end())
	{
		ShadowMapBuf tempBuf;
		if( CShadowMapBakingPool::GetInst()->GetShadowBuf((*it),tempBuf) )
		{
			if(tempBuf.Flag!=FLAG_NONE)
			{
				//传递参数
				Ast(m_pShadowFilter!=NULL);
				m_pShadowFilter->SetShadowTexOut(tempBuf.ShadowTexture);
				m_pShadowFilter->SetMatrixBuildOut(tempBuf.MatrixBuild);
				m_pShadowFilter->EnableRenderOut();

				CRenderPipeline::GetInst()->_SetSamplerState(6, SAMP_BORDERCOLOR, 0xffffffffUL);
				CRenderPipeline::GetInst()->_SetSamplerState(6,SAMP_ADDRESSU,TADDRESS_BORDER );
				CRenderPipeline::GetInst()->_SetSamplerState(6,SAMP_ADDRESSV,TADDRESS_BORDER );
	 			CRenderPipeline::GetInst()->_SetSamplerState(6, SAMP_MINFILTER, TEXF_POINT);
				CRenderPipeline::GetInst()->_SetSamplerState(6, SAMP_MAGFILTER, TEXF_POINT);
				
				CRenderPipeline::GetInst()->_SetTexture(6,m_LightMapTarget_help);
				m_LightMapTarget->Begin();
				this->RenderReceive();
				m_LightMapTarget->End();

				m_pShadowFilter->DisableRenderOut();
				//swap buf
				CRenderTexturePtr tempPtr=m_LightMapTarget_help; 
				m_LightMapTarget_help=m_LightMapTarget;
				m_LightMapTarget=tempPtr;
			}
		}
		it++;
	}	
	//BlurShader 
	SHADER_HANDLE BlurHandle;
	BlurHandle |= MakeSysVS_ID(MakeGroupID(1) + 6);
	BlurHandle |= MakeSysPS_ID(MakeGroupID(1) + 7);
	terrain->m_Param.m_RS.m_ShaderType=BlurHandle;
	//区域阴影纹理
	CRenderPipeline::GetInst()->_SetSamplerState(5, SAMP_ADDRESSU,TADDRESS_CLAMP );
	CRenderPipeline::GetInst()->_SetSamplerState(5, SAMP_ADDRESSV,TADDRESS_CLAMP );
	CRenderPipeline::GetInst()->_SetSamplerState(5, SAMP_MINFILTER, TEXF_LINEAR);
	CRenderPipeline::GetInst()->_SetSamplerState(5, SAMP_MAGFILTER, TEXF_LINEAR);
	CRenderPipeline::GetInst()->_SetTexture(5,m_RegionBakeHelp->m_RegionTex); 
	//模糊前LightMap
	CRenderPipeline::GetInst()->_SetSamplerState(6, SAMP_ADDRESSU,TADDRESS_CLAMP );
	CRenderPipeline::GetInst()->_SetSamplerState(6, SAMP_ADDRESSV,TADDRESS_CLAMP );
	CRenderPipeline::GetInst()->_SetSamplerState(6, SAMP_MINFILTER, TEXF_LINEAR);
	CRenderPipeline::GetInst()->_SetSamplerState(6, SAMP_MAGFILTER, TEXF_LINEAR);
	CRenderPipeline::GetInst()->_SetTexture(6,m_LightMapTarget_help);

	m_LightMapTarget->Begin();
	this->RenderReceive();
	m_LightMapTarget->End(); 
	//写入数据
	m_LightMapWriter->Write(x, z, m_LightMapTarget);
	m_LightMapTarget_help=NULL;

	tileIndexList.clear();
	CLightMapBaker::OnProcess(x,z);
	//相机还原
	tempRenderGroup->SetCamera(oldCamera);
}
