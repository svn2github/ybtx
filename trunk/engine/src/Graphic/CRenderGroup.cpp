#include "stdafx.h"
#include "CRenderGroup.h"
#include "CRenderPipeline.h"
#include "MathAlgorithm.h"
#include "CShadowHeader.h"
#include "BaseHelper.h"
#include "CGraphic.h"
#include "CRefractFilter.h"
#include "CPortraitTransFilter.h"
#include "CReceiveShadowFilter.h"
#include "CAlphaFilter.h"
#include "CSkeletalMesh.h"
#include "CVertexAniMesh.h"
#include "CHardwareCapacity.h"
#include "CRenderFilter.h"
#include "CBatchFilter.h"
#include "CGpuProgramMgr.h"
#include "CShowPassFilter.h"
#include "CBloomTarget.h"
#include "TSqrAllocator.inl"
#include "StringHelper.h"

namespace sqr
{
	CRenderGroup::CRenderGroup(void)
		: m_BaseId(0)
		, m_isOpt(true)
		, m_ShadowType(ST_NONE)
		, m_dwFogColor(0xff000000)
		, m_tFogMod(FMT_NONE)
		, m_fFogParam1(0.0f)
		, m_fFogParam2(0.0f)
		, m_isEnableFog(false)
		, m_pShadowFilter(0)
	{
		m_LightSystem = new CLightSystem(this);
		m_pActiveList = &m_ltRenderNode;
		memset( m_Filters,NULL,sizeof(m_Filters));
		m_Filters[ID_NPC_EFFECT_FILTER] = &CGraphic::GetInst()->m_EffectNpcVB;
		m_Filters[ID_SCENE_EFFECT_FILTER] = &CGraphic::GetInst()->m_EffectSceneVB;
		m_Filters[ID_DYNAMIC_VB] =  &CGraphic::GetInst()->m_DynamicVB;;
		m_pActiveCamera = &m_Camera;
		Init();
		AddRef();
	}

	CRenderGroup::~CRenderGroup()
	{
		delete m_LightSystem;
		Destroy();
	}

	void CRenderGroup::Render(void)
	{
		if (m_FilterQueue[0]==0)
			return;

		if (m_isEnableFog)
		{
			CRenderPipeline::GetInst()->SetFogMode(m_tFogMod);
			CRenderPipeline::GetInst()->SetFogColor(m_dwFogColor);
			switch (m_tFogMod)
			{
			case FMT_EXP:
				CRenderPipeline::GetInst()->SetFogDensity(m_fFogParam1);
				break;
			case FMT_EXP2:
				CRenderPipeline::GetInst()->SetFogDensity(m_fFogParam1);
				break;
			case FMT_LINEAR:
				CRenderPipeline::GetInst()->SetLinerFogPara(m_fFogParam1,m_fFogParam2);
				break;
			}
		}
		CRenderPipeline::GetInst()->EnableFog(m_isEnableFog);
		m_LightSystem->Reset();
		for (int i = 1;m_FilterQueue[i]!=RenderQueueFlag::RENDER_INDEX(ID_END_FILTER);++i)
		{
			if(m_Filters[m_FilterQueue[i]])
				m_Filters[m_FilterQueue[i]]->Render();
		}
		CRenderPipeline::GetInst()->EnableFog(false);
	}

	void  CRenderGroup::PostRender(void)
	{
		if  (m_Filters[ID_REFRACT_FILTER] )
			m_Filters[ID_REFRACT_FILTER]->Render();
	}

	void CRenderGroup::SetRenderQueueFlag( const RenderQueueFlag& QueueFlag)
	{
		m_FilterQueue = QueueFlag;
		if(m_pShadowFilter)
			m_pShadowFilter->ResetFilterQueue(m_FilterQueue);
	}


	void CRenderGroup::Destroy()
	{
		m_Filters[ID_NPC_EFFECT_FILTER] = NULL;
		m_Filters[ID_SCENE_EFFECT_FILTER] = NULL;
		m_Filters[ID_DYNAMIC_VB] = NULL;

		for (int i=0;i<ID_MAX_FILTER_NUM;++i)
			SafeRelease(m_Filters[i]);

		memset( m_Filters,NULL,sizeof(m_Filters));
	}

	void CRenderGroup::SetGeomeotryInstParams(const CGeometryParams& params)
	{
		m_GeometryParams = params;
	}

	void CRenderGroup::Init(void)
	{
		m_Filters[ID_SCENE_FILTER]					= new CDefaultFilter(ROS_LIGHTMAT1,this);//(fMFF&MFF_SCENE_SHADOW)?new CAlphaShadowFilter(ROS_LIGHTMAT1,this):new CDefaultFilter(ROS_LIGHTMAT1,this);
		m_Filters[ID_NPC_FILTER]					= new CDefaultFilter(ROS_LIGHTMAT2,this);//(fMFF&MFF_NPC_SHADOW)?new CAlphaShadowFilter(ROS_LIGHTMAT2,this):new CDefaultFilter(ROS_LIGHTMAT2,this);
		m_Filters[ID_ALPHA_SCENE_FILTER]			= new CAlphaFilter(ROS_ALPHA_LIGHT1,this);
		m_Filters[ID_ALPHA_NPC_FILTER]				= new CDefaultFilter(ROS_ALPHA_LIGHT2,this);

		m_Filters[ID_REFRACT_FILTER]				= new CRefractFilter(ROS_REFRACT);
		m_Filters[ID_GEOMETRY_INST_FILTER]			= new CBatchFilter( &m_GeometryParams,NULL );
		
		m_Filters[ID_SHADOW_FILTER]					= m_pShadowFilter = new CNoneShadowFilter(this);
		m_Filters[ID_TRANS_FILTER]					= CGpuProgramMgr::GetInst()->GetShaderVer()<Shader_V_2?
			((CRenderBatch*)(new CNormalTransFilter(ROS_TRANS_LIGHTMAT1,this))):
			((CRenderBatch*)(new CPortraitTransFilter(ROS_TRANS_LIGHTMAT1,this)));

		m_Filters[ID_SCENE_SHADOWRECEIVET_FILTER]	= new CReceiveShadowFilter(ROS_SHADOWRECEIVE_LIGHT1,this,m_pShadowFilter);
		m_Filters[ID_NPC_SHADOWRECEIVET_FILTER]		= new CReceiveShadowFilter(ROS_SHADOWRECEIVE_LIGHT2,this,m_pShadowFilter);
		m_Filters[ID_GEOMETRY_INST_SR_FILTER]		= new CBatchFilter( &m_GeometryParams, m_pShadowFilter );
		m_Filters[ID_SHOW_ZFAILD_FILTER]			= new CShowPassFilter(this);
	}

	void CRenderGroup::SetShadowFilter( CShadowFilter* pSFilter )
	{
		SQR_TRY
		{
			m_ShadowType = pSFilter->GetShadowType();
			m_pShadowFilter = pSFilter;
			if(m_Filters[ID_SHADOW_FILTER])
				m_pShadowFilter->SetLightVector(((CShadowFilter*)m_Filters[ID_SHADOW_FILTER])->GetLightVector());

			((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Refresh(m_pShadowFilter);

			CReceiveShadowFilter* pSceneReceive = new CReceiveShadowFilter(ROS_SHADOWRECEIVE_LIGHT1,this,m_pShadowFilter);
			CReceiveShadowFilter* pNpcReceive	= new CReceiveShadowFilter(ROS_SHADOWRECEIVE_LIGHT2,this,m_pShadowFilter);

			((CRenderFilter*)m_Filters[ID_SCENE_SHADOWRECEIVET_FILTER])->Refresh(pSceneReceive);
			((CRenderFilter*)m_Filters[ID_NPC_SHADOWRECEIVET_FILTER])->Refresh(pNpcReceive);

			m_pShadowFilter->ResetFilterQueue(m_FilterQueue);
			
			SafeRelease(m_Filters[ID_SHADOW_FILTER]);
			SafeRelease(m_Filters[ID_SCENE_SHADOWRECEIVET_FILTER]);
			SafeRelease(m_Filters[ID_NPC_SHADOWRECEIVET_FILTER]);

			m_Filters[ID_SHADOW_FILTER]				  = m_pShadowFilter;
			m_Filters[ID_SCENE_SHADOWRECEIVET_FILTER] = pSceneReceive;
			m_Filters[ID_NPC_SHADOWRECEIVET_FILTER]   = pNpcReceive;
			((CBatchFilter*)m_Filters[ID_GEOMETRY_INST_SR_FILTER])->ChangeShadowFilter(pSFilter);

			for (int i=0;i<ID_MAX_FILTER_NUM;++i)
				if(m_Filters[i])m_Filters[i]->Update();
		}
		SQR_CATCH(exp)
		{
			exp.AppendType("SetShadowFilter Failed.");
			GfkLogExpOnce(exp);
		}
		SQR_TRY_END
	}

	void CRenderGroup::SetShadowType( EShadowType fShadow )
	{
		/*if( CGpuProgramMgr::GetInst()->GetShaderVer()<Shader_V_2 && fShadow > ST_PLANE)
			fShadow = ST_PLANE;*/
		if( CGpuProgramMgr::GetInst()->GetShaderVer()<Shader_V_2 && fShadow > ST_NONE )
			fShadow = ST_NONE;

		SQR_TRY
		{
			if (m_ShadowType != fShadow)
			{
				if (m_ShadowType == ST_SHADOW_EX)
				{
					CDefaultFilter* pSceneFilter = new CUnReceiveShadowFilter(ROS_LIGHTMAT1,this);
					((CRenderFilter*)m_Filters[ID_SCENE_FILTER])->Refresh(pSceneFilter);
					((CBatchFilter*)m_Filters[ID_GEOMETRY_INST_FILTER])->ChangeShadowFilter(NULL);
					SafeRelease(m_Filters[ID_SCENE_FILTER]);
					m_Filters[ID_SCENE_FILTER] = pSceneFilter;
					//CDefaultFilter* pNpcFilter	 = new CDefaultFilter(ROS_LIGHTMAT2,this);
					//m_Filters[ID_NPC_FILTER]->Refresh(pNpcFilter);
					//SafeRelease(m_Filters[ID_NPC_FILTER]);
					//m_Filters[ID_NPC_FILTER]   = pNpcFilter;
				}

				m_ShadowType = fShadow;
				switch (fShadow)
				{
				case ST_NONE:
					m_pShadowFilter = new CNoneShadowFilter(this);
					break;
					/*case ST_PLANE:
					m_pShadowFilter = new CPlaneShadowFilter(this);
					break;*/
				case ST_SHADOWMAP:
					{
						if (TFMT_UNKNOWN == CHardwareCapacity::GetInst()->GetHwDepthTextureFormat())
							m_pShadowFilter = new CProjectShadowFilter(this);
						else
							m_pShadowFilter = new CShadowMapFilter(this, false);
					}
					break;
				case ST_SHADOW_EX:
					{
						if (TFMT_UNKNOWN == CHardwareCapacity::GetInst()->GetHwDepthTextureFormat())
							m_pShadowFilter = new CProjectShadowFilter(this);
						else
						{
							m_pShadowFilter = new CShadowMapFilter(this, true);

							CDefaultFilter* pSceneFilter = new CReceiveShadowFilter(ROS_LIGHTMAT1,this,m_pShadowFilter);
							((CRenderFilter*)m_Filters[ID_SCENE_FILTER])->Refresh(pSceneFilter);
							SafeRelease(m_Filters[ID_SCENE_FILTER]);
							m_Filters[ID_SCENE_FILTER] = pSceneFilter;
							((CBatchFilter*)m_Filters[ID_GEOMETRY_INST_FILTER])->ChangeShadowFilter(m_pShadowFilter);
							//CDefaultFilter* pNpcFilter	 = new CReceiveShadowFilter(ROS_LIGHTMAT2,this,m_pShadowFilter);
							//m_Filters[ID_NPC_FILTER]->Refresh(pNpcFilter);
							//SafeRelease(m_Filters[ID_NPC_FILTER]);
							//m_Filters[ID_NPC_FILTER]   = pNpcFilter;
						}
					}
					break;
				default:
					m_pShadowFilter = new CNoneShadowFilter(this);
					break;
				}
				SetShadowFilter(m_pShadowFilter);	
			}
		}
		SQR_CATCH(exp)
		{
			exp.AppendType(GraphicErr::GetErrTypeStr(RenderRuntime_Err));
			string szErrMsg;
			format("shadow type: %d\n", m_ShadowType);
			exp.AppendMsg(szErrMsg);
			GfkLogExp(exp);
		}
		SQR_TRY_END;

	}

	void CRenderGroup::RegisterBatch(RENDER_BATCH_ID fID,CRenderBatch* pFilter)
	{
		if (pFilter!=m_Filters[fID])
		{
			SafeRelease(m_Filters[fID]);
			m_Filters[fID] = pFilter;
			m_Filters[fID]->Update();
		}
	}

	void CRenderGroup::UnRegisterBatch(RENDER_BATCH_ID fID,CRenderBatch* pFilter)
	{
		if(pFilter==m_Filters[fID])
		{
			m_FilterQueue.Delete(fID);
			m_Filters[fID] = NULL;
		}	
	}

	CRenderBatch* 	CRenderGroup::GetBatch(RENDER_BATCH_ID fID)
	{
		return m_Filters[fID];
	}

	void CRenderGroup::Begin(CModel* pModel )
	{
		m_pActiveList = &(pModel->m_vRenderNode);
	}

	void CRenderGroup::Begin( NodePool* pRenderList )
	{
		m_pActiveList = pRenderList;
	}

	void CRenderGroup::End(void)
	{
		m_pActiveList = &m_ltRenderNode;
	}

	//void   CRenderGroup::SetImmediate(void)
	//{
	//	m_fpInertFn = &CRenderGroup::_InsertI;

	//	//m_fpBufferFn = &CRenderGroup::_SetParamBufferI;
	//	m_LightSystem->TurnOnGlobalLight(1);
	//	m_isOpt = false;
	//	CParamsBufferMgr::GetInst()->SetImmediate(!m_isOpt);
	//}

	//void   CRenderGroup::SetOptimize(void)
	//{
	//	m_fpInertFn = &CRenderGroup::_InsertC;
	//	//m_fpBufferFn = &CRenderGroup::_SetParamBufferC;
	//	//m_isOpt = true;
	//	CParamsBufferMgr::GetInst()->SetImmediate(!m_isOpt);
	//}

	//FilterNode* CRenderGroup::_InsertI(FilterNode* fn)
	//{
	//	if (((FilterNode*)fn)->m_eType==ROS_SHADOW)
	//		return fn;

	//	m_LightSystem->TurnOnGlobalLight((((FilterNode*)fn)->m_eType&1));
	//	fn->param_render(*fn);
	//	CRenderPipeline::GetInst()->Render(fn,&(((FilterNode*)fn)->m_RP));
	//	return NULL;
	//}


	FilterNode* CRenderGroup::InsertRender(FilterNode* fn)
	{
		m_pActiveList->push_back(*(FilterNode*)fn);
		FilterNode* ifn = &m_pActiveList->back();
		ifn->m_ParentGroup = this;

		switch (fn->m_eType)
		{
		case ROS_LIGHTMAT1:
			{
				return ((CRenderFilter*)m_Filters[ID_SCENE_FILTER])->Filter(ifn);
				break;
			}
		case ROS_LIGHTMAT2:
			{
				return ((CRenderFilter*)m_Filters[ID_NPC_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ALPHA_LIGHT1:
			{
				return ((CRenderFilter*)m_Filters[ID_ALPHA_SCENE_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ALPHA_LIGHT2:
			{
				return ((CRenderFilter*)m_Filters[ID_ALPHA_NPC_FILTER])->Filter(ifn);
				break;
			}

		case ROS_SHADOW:
			{
				return ((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(ifn);
				break;
			}

		case ROS_SHADOW_LIGHT1:
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_SCENE_FILTER])->Filter(ifn);
				break;
			}
		case ROS_SHADOW_LIGHT2:
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_NPC_FILTER])->Filter(ifn);
				break;
			}
		case ROS_SHADOW_ALPHALIGHT1:
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_ALPHA_SCENE_FILTER])->Filter(ifn);
				break;
			}
		case ROS_SHADOW_ALPHALIGHT2:
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_ALPHA_NPC_FILTER])->Filter(ifn);
				break;
			}

		case ROS_REFRACT:
			return ((CRenderFilter*)m_Filters[ID_REFRACT_FILTER])->Filter(ifn);
			break;

		case ROS_TRANS:
		case ROS_TRANS_LIGHTMAT1:
		case ROS_TRANS_LIGHTMAT2:
			return ((CRenderFilter*)m_Filters[ID_TRANS_FILTER])->Filter(ifn);
			break;

		case ROS_SHADOWRECEIVE_LIGHT1:
			{
				return ((CRenderFilter*)m_Filters[ID_SCENE_SHADOWRECEIVET_FILTER])->Filter(ifn);
				break;
			}
		case ROS_SHADOWRECEIVE_LIGHT2:
			{
				return ((CRenderFilter*)m_Filters[ID_NPC_SHADOWRECEIVET_FILTER])->Filter(ifn);
				break;
			}

		case ROS_SHADOWRECEIVE_LIGHT1_SHADOW:
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_SCENE_SHADOWRECEIVET_FILTER])->Filter(ifn);
			}
			break;

		case ROS_SHADOWRECEIVE_LIGHT2_SHADOW:
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_NPC_SHADOWRECEIVET_FILTER])->Filter(ifn);
			}
			break;
		case ROS_ZFAILD:
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				return ((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ZFAILD_LIGHT1:	
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_SCENE_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ZFAILD_LIGHT2:						
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_NPC_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ZFAILD_ALPHAT_LIGHT1:			
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_ALPHA_SCENE_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ZFAILD_ALPHAT_LIGHT2:				
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_ALPHA_NPC_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ZFAILD_SHADOW_LIGHT1:				
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				m_pActiveList->push_back(*(FilterNode*)ifn);
				iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_SCENE_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ZFAILD_SHADOW_LIGHT2:				
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				m_pActiveList->push_back(*(FilterNode*)ifn);
				iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_NPC_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ZFAILD_SHADOW_ALPHALIGHT1:			
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				m_pActiveList->push_back(*(FilterNode*)ifn);
				iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_ALPHA_SCENE_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ZFAILD_SHADOW_ALPHALIGHT2:			
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				m_pActiveList->push_back(*(FilterNode*)ifn);
				iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_ALPHA_NPC_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ZFAILD_SHADOWRECEIVE:				
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				m_pActiveList->push_back(*(FilterNode*)ifn);
				iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				return ((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				break;
			}
		case ROS_ZFAILD_SHADOWRECEIVE_LIGHT1:			
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_SCENE_SHADOWRECEIVET_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ZFAILD_SHADOWRECEIVE_LIGHT2:			
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_NPC_SHADOWRECEIVET_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ZFAILDS_SHADOWRECEIVE_LIGHT1_SHADOW:	
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				m_pActiveList->push_back(*(FilterNode*)ifn);
				iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_SCENE_SHADOWRECEIVET_FILTER])->Filter(ifn);
				break;
			}
		case ROS_ZFAILDS_SHADOWRECEIVE_LIGHT2_SHADOW:
			{
				m_pActiveList->push_back(*(FilterNode*)ifn);
				FilterNode* iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHADOW_FILTER])->Filter(iFN);
				m_pActiveList->push_back(*(FilterNode*)ifn);
				iFN = &m_pActiveList->back();
				iFN->m_ParentGroup = this;
				((CRenderFilter*)m_Filters[ID_SHOW_ZFAILD_FILTER])->Filter(iFN);
				return ((CRenderFilter*)m_Filters[ID_NPC_SHADOWRECEIVET_FILTER])->Filter(ifn);
				break;
			}
		default:
			return NULL;
		}
	}

	void CRenderGroup::InsertGeometry( CGeometryInst* pGI , ERenderObjStyle eObjStyle)
	{
		switch (eObjStyle)
		{
		case ROS_LIGHTMAT1:
		case ROS_LIGHTMAT2:
		case ROS_ALPHA_LIGHT1:
		case ROS_ALPHA_LIGHT2:
		{
			((CGeometryBatch*)m_Filters[ID_GEOMETRY_INST_FILTER])->AddGeometryInst(pGI);
			break;
		}
		case ROS_SHADOW:
		case ROS_SHADOW_LIGHT1:
		case ROS_SHADOW_LIGHT2:
		case ROS_SHADOW_ALPHALIGHT1:
		case ROS_SHADOW_ALPHALIGHT2:
			{
				break;
			}

		case ROS_REFRACT:
			break;

		case ROS_TRANS:
		case ROS_TRANS_LIGHTMAT1:
		case ROS_TRANS_LIGHTMAT2:
			break;

		case ROS_SHADOWRECEIVE_LIGHT1:
		case ROS_SHADOWRECEIVE_LIGHT2:
		case ROS_SHADOWRECEIVE_LIGHT1_SHADOW:
		case ROS_SHADOWRECEIVE_LIGHT2_SHADOW:
		{
			((CGeometryBatch*)m_Filters[ID_GEOMETRY_INST_SR_FILTER])->AddGeometryInst(pGI);
			break;
		}
		default:
			return;
		}
	}

	void   CRenderGroup::Reset(void)
	{
		m_BaseId = 0;
		m_ltRenderNode.clear();
	}

	EShadowType CRenderGroup::GetShadowType()
	{
		return ((CShadowFilter*)m_Filters[ID_SHADOW_FILTER])->GetShadowType();
	}

	//------------------------------------------------------------------------------


	void	CRenderGroup::SetFogColor( const DWORD Color )
	{
		m_dwFogColor = Color;
	}

	void	CRenderGroup::EnableFog( bool bEnable )
	{
		m_isEnableFog = bEnable;
		if (m_isEnableFog&&m_tFogMod==FMT_LINEAR)
			m_pActiveCamera->setFarDist(m_fFogParam2+500.0f);
		else
			m_pActiveCamera->setFarDist(Max_Far_Dist);
	}

	void	CRenderGroup::SetFog( FogModeType fMode , DWORD fColor , ... )
	{
		m_tFogMod = fMode;
		m_dwFogColor = fColor;
		va_list marker;
		va_start( marker,fColor );
		switch (fMode)
		{
		case FMT_NONE:
			m_isEnableFog = false;
			m_pActiveCamera->setFarDist(Max_Far_Dist);
			break;
		case FMT_EXP:
			m_fFogParam1 = (float)(va_arg(marker,double));
			m_pActiveCamera->setFarDist(Max_Far_Dist);
			m_isEnableFog = true;
			break;
		case FMT_EXP2:
			m_fFogParam1 = (float)(va_arg(marker,double));
			m_pActiveCamera->setFarDist(Max_Far_Dist);
			m_isEnableFog = true;
			break;
		case FMT_LINEAR:
			m_fFogParam1 = (float)(va_arg(marker,double));
			m_fFogParam2 = (float)(va_arg(marker,double));
			m_pActiveCamera->setFarDist(m_fFogParam2+500.0f);
			m_isEnableFog = true;
			break;
		}
		va_end(marker);
	}

	void	CRenderGroup::SetFogParams( FogModeType fMode, ... )
	{
		m_tFogMod = fMode;
		va_list marker;
		va_start( marker,fMode );
		switch (fMode)
		{
		case FMT_NONE:
			m_isEnableFog = false;
			m_pActiveCamera->setFarDist(Max_Far_Dist);
			break;
		case FMT_EXP:
			m_fFogParam1 = (float)(va_arg(marker,double));
			m_pActiveCamera->setFarDist(Max_Far_Dist);
			m_isEnableFog = true;
			break;
		case FMT_EXP2:
			m_fFogParam1 = (float)(va_arg(marker,double));
			m_pActiveCamera->setFarDist(Max_Far_Dist);
			m_isEnableFog = true;
			break;
		case FMT_LINEAR:
			m_fFogParam1 = (float)(va_arg(marker,double));
			m_fFogParam2 = (float)(va_arg(marker,double));
			m_pActiveCamera->setFarDist(m_fFogParam2+500.0f);
			m_isEnableFog = true;
			break;
		}
		va_end(marker);
	}
}
