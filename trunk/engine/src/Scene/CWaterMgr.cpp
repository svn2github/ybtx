#include "StdAfx.h"
#include "CWaterMgr.h"
#include "CWaterSurface.h"
#include "CTexAniWaterEffect.h"
#include "CEnvMapWaterEffect.h"
#include "CLightSystem.h"
#include "CRenderTarget.h"
#include "CRenderGroup.h"
#include "CGpuProgramMgr.h"
#include "CRenderPipeline.h"
#include "TSqrAllocator.inl"
#include "CGraphic.h"
#include "BaseHelper.h"

namespace sqr
{

	//------------------------------------------------------------------------------
	inline SHADER_HANDLE
	CWaterMgr::EffectTypeToID( CWaterMgr::WaterEffectType type )
	{
		SHADER_HANDLE handle;
		switch (type)
		{
			case TextureAnimation:
			{
				handle  = 0x00000007|MakeGroupID(4)|SSI_BASE_VS_SYSTEM_ID;
				handle |= 0x00000008|MakeGroupID(4)|SSI_BASE_PS_SYSTEM_ID;
				break;
			}
				
			case EnvironmentMapping:
			{
				handle  = 0x00000005|MakeGroupID(4)|SSI_BASE_VS_SYSTEM_ID;
				handle |= 0x00000006|MakeGroupID(4)|SSI_BASE_PS_SYSTEM_ID;
				break;
			}
		}

		return handle;
	}

	//------------------------------------------------------------------------------
	CWaterMgr::CWaterMgr(CRenderGroup* pGroup)
			: m_WaterType(WT_Default),m_LightIndex(0),m_pGroup(pGroup)
	{
		AddRef();
		m_pGroup->RegisterBatch( ID_WATER_FILTER , this );
	}

	//------------------------------------------------------------------------------
	CWaterMgr::~CWaterMgr()
	{
		m_pGroup->UnRegisterBatch( ID_WATER_FILTER , this );
		Destroy();
		ClearSurfaces();
	}

	//------------------------------------------------------------------------------
	CWaterSurface* CWaterMgr::CreateSurface( WaterEffectType type, SVector<ITexture*>& textures )
	{
		CWaterEffectPtr effect;
		WaterSurfaceKey surfaceKey = make_pair(EffectTypeToID(type), textures);
		WaterSurfaceMap::iterator it = this->surfaceMap.find(surfaceKey);
		if (this->surfaceMap.end() == it)
		{
				if ((type == TextureAnimation && !textures.empty()))
			{
				CTexAniWaterEffect* texEffect = new CTexAniWaterEffect();
				texEffect->SetAnimationTextures(textures);
				effect = texEffect;
				this->effectMap.insert(make_pair(surfaceKey, effect));
			}
			else
			{
				if( CGpuProgramMgr::GetInst()->GetShaderVer() < Shader_V_2 )
				{	
					SetWaterType(WT_Low);
					effect = m_LowestEffect;
				}
				else
					effect = new CEnvMapWaterEffect();

				this->effectMap.insert(make_pair(surfaceKey, effect));

				if ( GetWaterType() == WT_Low )
					effect = m_LowestEffect;
			}

			CWaterSurfacePtr surface = new CWaterSurface();
			surface->SetWaterEffect(effect);
			m_Surfaces.push_back(surface);
			this->surfaceMap.insert(make_pair(surfaceKey, surface));
			
			return surface.get();
		}
		else
		{
			return it->second.get();
		}
	}

	//------------------------------------------------------------------------------
	void
	CWaterMgr::Render()
	{
		CLightSystem* pLight = m_pGroup->GetLightSystem();
		pLight->TurnOnGlobalLight(m_LightIndex);
		CRenderPipeline::GetInst()->SetWorld(CMatrix::UNIT_MATRIX);
		CRenderPipeline::GetInst()->SetRenderStyle(STATIC_RS());

		SVector<CWaterSurfacePtr>::iterator it,eit = m_Surfaces.end();
		CCamera* pCam = m_pGroup->GetCamera();
		for (it = m_Surfaces.begin(); it!=eit; ++it)
			(*it)->Render(pCam);
	}

	void	CWaterMgr::RenderWave(uint32 uCurTime)
	{
		if(m_TextUnit.size() == 0)
			return;

		SVector<CWaterSurfacePtr>::iterator it,eit = m_Surfaces.end();
		CCamera* pCam = m_pGroup->GetCamera();

		ITexture* texture = m_TextUnit.back().GetTexture();
		for (it = m_Surfaces.begin(); it!=eit; ++it)
			(*it)->RenderWave(pCam,texture,uCurTime);
	}

	CWater*	CWaterMgr::GetWater(uint32 OrgId , int32 nGrid)
	{
		SQR_TRY
		{
			CWater* pWater;
			SVector<CWaterSurfacePtr>::iterator it,eit = m_Surfaces.end();
			for (it = m_Surfaces.begin(); it!=eit; ++it)
			{
				if( pWater = (*it)->GetWater(OrgId,nGrid) )
					return pWater;
			}
		}
		SQR_CATCH(exp)
		{
			GfkLogExp(exp);
		}
		SQR_TRY_END;
		return NULL;
	}	

	//------------------------------------------------------------------------------
	void
	CWaterMgr::ClearSurfaces()
	{
		m_Surfaces.clear();
		this->surfaceMap.clear();
	}

	//------------------------------------------------------------------------------
	bool
	CWaterMgr::SetWaterType( EWaterType type )
	{
		if(CGpuProgramMgr::GetInst()->GetShaderVer()<Shader_V_2)
			type = WT_Low;

		switch (type)
		{
			case WT_Low:
				SetLowestEffect();
				break;
			
			case WT_High:
				break;
			
			case WT_Default:
			{
				// restore the effect
				for (WaterSurfaceMap::iterator it = this->surfaceMap.begin();
						it != this->surfaceMap.end(); ++it)
				{
					it->second->SetWaterEffect(this->effectMap[it->first]);
				}
				m_WaterType = type;
				return true;
			}
			break;
		}
		return false;
	}

	void CWaterMgr::SetLowestEffectTexNameVec( const SVector<SString>& texNameVec )
	{
		m_LowTexNameVec.assign(texNameVec.begin(), texNameVec.end());
	}

	void CWaterMgr::SetLowestEffect()
	{
		if ( !m_LowestEffect.isvalid() )
		{
			SVector<ITexture*> textures;
			for (uint32 i = 0; i < m_LowTexNameVec.size(); ++i)
			{
				ITexture* texture = NULL;
				CGraphic::GetInst()->CreateTexture(PATH_ALIAS_RES.c_str(), m_LowTexNameVec[i].c_str(), &texture);
				if(texture)
					textures.push_back(texture);
			}

			CTexAniWaterEffect* pLowestEffect = new CTexAniWaterEffect();
			if ( 0 == pLowestEffect )
			{
				ostringstream strm;
				strm << "CWaterMgr::SetLowestEffect - new CTexAniWaterEffect failed.";
				GenErr( GraphicErr::GetErrTypeStr(AllocMemory_Err), strm.str() );
			}
			pLowestEffect->SetAnimationTextures(textures);
			m_LowestEffect = pLowestEffect;

			for (uint32 i = 0; i < textures.size(); ++i)
				SafeRelease(textures[i]);
		}

		if ( m_LowestEffect.isvalid() )
		{
			// set all the surface's effect to TexAni
			typedef SVector<CWaterSurfacePtr>::iterator WaterSurfVecIter;
			SHADER_HANDLE effectId = EffectTypeToID(CWaterMgr::TextureAnimation);
			for ( WaterSurfVecIter it = m_Surfaces.begin(); it != m_Surfaces.end(); ++it )
			{
				CWaterSurfacePtr& surface = (*it);
				CWaterEffectPtr pEffect = surface->GetWaterEffect();
				if ( pEffect == 0 || (pEffect != 0 && effectId != pEffect->GetEffectID()) )
					surface->SetWaterEffect(m_LowestEffect);
			}
		}
		m_WaterType = WT_Low;		
	}
}
