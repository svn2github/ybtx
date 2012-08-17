#include "StdAfx.h"
#include "LogicalHeightBaker.h"
#include "CGraphic.h"
#include "CWindowTarget.h"
#include "CRenderPipeline.h"
#include "LightMapWriter.h"
#include "CLightMapFilter.h"
#include "OfflineRender.h"
#include "ShadowMapBakerPool.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "StringHelper.h"
#include "BaseHelper.h"

namespace sqr
{

CLogicalHeightBaker::CLogicalHeightBaker(void)
:offsetY(0)
{
	CGraphic::GetInst()->CreateEmptyTexture( 64, 64, 1, TF_ARGB32, &m_pGlobalShadowTex );
	DWORD uTexData = CSceneEnvMgr::GetInst()->GetShadowColor();
	int size = 64*64;
	D3DCOLOR clrBranch[64*64];
	for( int i = 0; i < size; ++i )
		clrBranch[i] = uTexData;

	m_pGlobalShadowTex->FillFromMemory(&clrBranch, TF_ARGB32, 1, CIRect(0,0,64,64), CIRect(0,0,64,64));	
}

CLogicalHeightBaker::~CLogicalHeightBaker(void)
{
	this->meshes.clear();
	SafeRelease(m_pGlobalShadowTex);
}

//
void CLogicalHeightBaker::Setup()
{
	CLightMapBaker::Setup();
	tileIndexList.clear();

	this->renderParam.m_RS = CGraphic::GetInst()->GetStaticRS(RS_DEFAULT);
	this->renderParam.m_Mat.Identity();
	m_LightMapWriter=NULL;
	m_LightMapWriter = new CLogicalShadowWriter();

	CLogicalHeightMeshBuilder::Build(this->meshes);
	m_LightMapWriter->SetNumLightMaps(tileWidth, tileDepth);
}

void CLogicalHeightBaker::Discard()
{
	m_LightMapWriter.cast<CLogicalShadowWriter>()->Save(int(this->offsetY / 64.0f));
	CLightMapBaker::Discard();
}

void CLogicalHeightBaker::TilePrepare(index_t x,index_t z)
{
	CLightMapBaker::TilePrepare(x,z);
	tileIndexList.push_back(x+z*tileWidth);
}

void CLogicalHeightBaker::RenderReceive()
{
	m_pShadowFilter->UpdateShadowParams();
	this->renderParam.m_Mat._42 = this->offsetY;

	static CLightSystem* pLightSystem = CMainWindowTarget::GetInst()->GetGroup()->GetLightSystem();
	DWORD amb = pLightSystem->GetAmbient(0);
	DWORD sha = pLightSystem->GetGlobalShadowColor();

	CRenderPipeline* renderPipeline = CRenderPipeline::GetInst();
	for (vector<LogicalMeshPtr>::iterator it = this->meshes.begin();it != this->meshes.end(); ++it)
	{
		LogicalMeshPtr& mesh = *it;
		if (LightMapCamera->cull(mesh->aabb))
			renderPipeline->Render(&mesh->rp, &this->renderParam);
	}
}

void CLogicalHeightBaker::OnProcess(index_t x,index_t z)
{
	//相机切换
	CRenderGroup* tempRenderGroup=CMainWindowTarget::GetInst()->GetGroup();
	CCamera* oldCamera = tempRenderGroup->GetCamera();

	ChangeCamera(x,z);
	CRenderPipeline::GetInst()->SetCamera(LightMapCamera);

	CRenderTexturePtr m_LightMapTarget_help;
	m_LightMapTarget_help = CGraphic::GetInst()->CreateRenderTarget();
	m_LightMapTarget_help->Release();      // //RTT显存正确释放补丁
	m_LightMapTarget_help->SetWidth(512);
	m_LightMapTarget_help->SetHeight(512);
	m_LightMapTarget_help->SetFormat(TFMT_X8R8G8B8);
	m_LightMapTarget_help->Create();
	m_LightMapTarget_help->SetClearColor(CColor::White);
	m_LightMapTarget_help->Begin();
	m_LightMapTarget_help->End();

	//MultPassRender
	SHADER_HANDLE MultPassHandle;
	MultPassHandle |=  MakeSysVS_ID(MakeGroupID(1) + 8);
	MultPassHandle |=  MakeSysPS_ID(MakeGroupID(1) + 9);
	this->renderParam.m_RS.m_ShaderType =MultPassHandle;

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
	
	//Blur Shader
	SHADER_HANDLE BlurHandle;
	BlurHandle |=  MakeSysVS_ID(MakeGroupID(1) + 6);
	BlurHandle |=  MakeSysPS_ID(MakeGroupID(1) + 7);
	this->renderParam.m_RS.m_ShaderType=BlurHandle;

  	//区域阴影纹理
  	CRenderPipeline::GetInst()->_SetSamplerState(5, SAMP_ADDRESSU,TADDRESS_CLAMP );
  	CRenderPipeline::GetInst()->_SetSamplerState(5, SAMP_ADDRESSV,TADDRESS_CLAMP );
  	CRenderPipeline::GetInst()->_SetSamplerState(5, SAMP_MINFILTER, TEXF_LINEAR);
  	CRenderPipeline::GetInst()->_SetSamplerState(5, SAMP_MAGFILTER, TEXF_LINEAR);
  	CRenderPipeline::GetInst()->_SetTexture(5,      m_pGlobalShadowTex); 

	CRenderPipeline::GetInst()->_SetSamplerState(6,	SAMP_ADDRESSU,TADDRESS_CLAMP );
	CRenderPipeline::GetInst()->_SetSamplerState(6,	SAMP_ADDRESSV,TADDRESS_CLAMP );
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
	//相机还原
	tempRenderGroup->SetCamera(oldCamera);
	CLightMapBaker::OnProcess(x,z);
}

void CLogicalHeightBaker::SetOffsetY(float y)
{
	this->offsetY=y;
}

//!namespace
}