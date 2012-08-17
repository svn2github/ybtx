#include "stdafx.h"
#include "CEffectManager.h"
#include "CEffectGroup.h"
#include "TimeHelper.h"
#include "CEffectMgrModule.h"
#include "CEffectClassDesc.h"
#include "StringHelper.h"
#include "CParticleSystem.h"
#include "CSprite.h"
#include "CPolyTrail.h"
#include "CCameraShake.h"
#include "CFlareSprite.h"
#include "CCameraFilter.h"
#include "CModelConsole.h"
#include "CDynamicLight.h"
#include "CNull.h"
#include "CTDimAudio.h"
#include "CModelRenderStyle.h"
#include "CLightning.h"
#include "CLeading.h"
#include "CShake.h"
#include "CCombination.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
#include "CEffectCoder.h"
#include "DebugHelper.h"
#include "CGraphicExp.h"

#pragma warning(disable:4101)

const wstring PATH_ALIAS_EFFECT = L"res";


CEffectManager::CEffectManager()
{
	DYNAMIC_REGISTER_EFFECTUNITBASE( eParticleSystem, IDS_PARTICLESYSTEM, CParticleSystem, CParticleSystemProp );
	DYNAMIC_REGISTER_EFFECTUNITBASE( ePolyTrail, IDS_POLYTRAIL, CPolyTrail, CPolyTrailProp );
	DYNAMIC_REGISTER_EFFECTUNITBASE( eSprite, IDS_SPRITE, CSprite, CSpriteProp );
	DYNAMIC_REGISTER_EFFECTUNITBASE( eCameraShake, IDS_CAMERASHAKE, CCameraShake, CCameraShakeProp );
	DYNAMIC_REGISTER_EFFECTUNITBASE( eFlareSprite,IDS_FLARESPRITE,CFlareSprite,CFlareSpriteProp);
	DYNAMIC_REGISTER_EFFECTUNITBASE( eCameraFilter,IDS_CAMERAFILTER,CCameraFilter,CCameraFilterProp);
	DYNAMIC_REGISTER_EFFECTUNITBASE( eModelConsole,IDS_MODELCONSOLE,CModelConsole,CModelConsoleProp);
	DYNAMIC_REGISTER_EFFECTUNITBASE( eDynamicLight,IDS_DYNAMICLIGHT,CDynamicLight,CDynamicLightProp);
	DYNAMIC_REGISTER_EFFECTUNITBASE( eNull,IDS_NULL,CNull,CNullProp);
	DYNAMIC_REGISTER_EFFECTUNITBASE( eTDimAudio,IDS_AUDIO,CTDimAudio,CTDimAudioProp);
	DYNAMIC_REGISTER_EFFECTUNITBASE( eModelRenderStyle,IDS_MODELRENDERSTYLE,CModelRenderStyle,CModelRenderStyleProp);
	DYNAMIC_REGISTER_EFFECTUNITBASE( eLightning,IDS_LIGHTNING,CLightning,CLightningProp);
	DYNAMIC_REGISTER_EFFECTUNITBASE( eLeading,IDS_LEADING,CLeading,CLeadingProp);
	DYNAMIC_REGISTER_EFFECTUNITBASE( eShake,IDS_SHAKE,CShake,CShakeProp);
	DYNAMIC_REGISTER_EFFECTUNITBASE( eEfxCombination,IDS_COMBINATION,CCombination,CCombinationProp);
	if(CGraphic::HasInst())
		CGraphic::GetInst()->SetEffectCallBack(CEffect::SetEffectRenderLod,CEffect::GetEffectRenderLod);
	if(CGraphic::HasInst())
		CGraphic::GetInst()->SetCameraShakeCallBack(CEffect::SetCameraShake,CEffect::GetCameraShake);
}

CEffectManager::~CEffectManager(void)
{

	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CParticleSystem);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CPolyTrail);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CSprite);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CCameraShake);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CFlareSprite);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CCameraFilter);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CModelConsole);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CDynamicLight);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CNull);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CTDimAudio);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CModelRenderStyle);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CLightning);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CLeading);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CShake);
	DYNAMIC_UNREGISTER_EFFECTUNITBASE(CCombination);
	EMap< EString, IEffectGroup* >::iterator itGroup = m_pEffectGroups.begin();
	while (itGroup != m_pEffectGroups.end())
	{
		// IEffectGroup的Release会从map中删除自己
		SafeRelease((itGroup++)->second);
	}
	
	for( EMap< EString, EffectGroupRecycle >::iterator it = m_Recycle.begin(); it != m_Recycle.end(); ++it )
		delete ( (CEffectGroup*)it->second.m_pEffectGroup );
	
	for( EffectClassDescMap_t::iterator itDLL = m_EffectClassDesc.begin(); 
		itDLL != m_EffectClassDesc.end(); ++itDLL )
	{
		SafeRelease( itDLL->second.m_EffectDesc );
	}
}

void CEffectManager::ClearEffectDesc( void )
{
	for( EffectClassDescMap_t::iterator itDLL = m_EffectClassDesc.begin(); 
		itDLL != m_EffectClassDesc.end(); ++itDLL )
	{
		itDLL->second.m_EffectDesc->ClearEffect();
	}
}

IEffectUnitProp* CEffectManager::CreateEffectUnitProp( const GUID Guid, int Num )
{
	EffectClassDescMap_t::iterator it = m_EffectClassDesc.find( Guid );
	if( it != m_EffectClassDesc.end() )
	{
		Ast(it->second.m_EffectDesc);
		return it->second.m_EffectDesc->CreateEffectUnitProp( Num );
	}
	return NULL;
}

IEffectUnit* CEffectManager::CreateEffectUnit( IEffectUnitProp* pEffectProp )
{
	EffectClassDescMap_t::iterator it = m_EffectClassDesc.find( pEffectProp->GetGUID() );
	if( it != m_EffectClassDesc.end() )
		return it->second.m_EffectDesc->CreateEffectUnit( pEffectProp );
	return NULL;
}

void CEffectManager::DeleteEffectGroup( IEffectGroup* pGroup )
{
	m_pEffectGroups.erase( ( (CEffectGroup*)pGroup )->GetName() ); 
	EffectGroupRecycle EGR = { pGroup, uint32(GetProcessTime()) };
	pGroup->_FreeRes();
	m_Recycle[ ( (CEffectGroup*)pGroup )->GetName() ] = EGR;
}

bool CEffectManager::InitClassDesc( void )
{
	//CREATE_FX_DESC CreateEffectClassDesc = (CREATE_FX_DESC)::GetProcAddress( hDLL, "CreateEffectClassDesc" );
	IEffectClassDesc* pEffectDesc = CreateEffectClassDesc();
	const GUID guid = pEffectDesc->GetClassID();
	if( m_EffectClassDesc.find( guid ) == m_EffectClassDesc.end() )
		m_EffectClassDesc[guid].m_EffectDesc = pEffectDesc;
	return true;
}

//////////////////////////////////////////////////////////////////////////


static void OnReadAsyn( void* pEffectGroup, CRefBufFile& BufFile )
{
	SQR_TRY	
	{
		CEffectFile EffectFile( BufFile );
		((CEffectGroup*)pEffectGroup)->BinLoad( EffectFile );
	}
	SQR_CATCH(exp)
	{
		GfkLogPkgErr(exp, ((CEffectGroup*)pEffectGroup)->GetName(), BufFile);
	}
	SQR_TRY_END;
}

IEffectGroup* CEffectManager::CreateEffectGroupFun(const char* szFileName)
{
	return new CEffectGroup( this, szFileName );
}

HRESULT CEffectManager::CreateEffectGroup( const char* szFileName, IEffectGroup** pEffectGroup )
{
	*pEffectGroup = NULL;
	EMap<EString,IEffectGroup*>::iterator it = m_pEffectGroups.find( szFileName );
	if( it != m_pEffectGroups.end() )
	{
		if(it->second)
		{
			it->second->AddRef();
			*pEffectGroup = it->second;
		}
		return it->second?0:-1;
	}

	EMap< EString, EffectGroupRecycle >::iterator itRecycle = m_Recycle.find( szFileName );
	if( itRecycle != m_Recycle.end() )
	{
		m_pEffectGroups[szFileName] = itRecycle->second.m_pEffectGroup;
		*pEffectGroup = itRecycle->second.m_pEffectGroup;
		itRecycle->second.m_pEffectGroup->AddRef();
		itRecycle->second.m_pEffectGroup->_Reclaim();
		m_Recycle.erase( itRecycle );
		return 0;
	}

	*pEffectGroup = CreateEffectGroupFun(szFileName);
	(*pEffectGroup)->AddRef();
//#ifdef _DEBUG
//	CPkgFile::EnableAsyncRead(true);//特效载入
//#else
//	CPkgFile::EnableAsyncRead( true );
//#endif

	if( CPkgFile::ReadWholeFile( PATH_ALIAS_EFFECT.c_str(), szFileName, *pEffectGroup, OnReadAsyn ) < 0 )
	{
		SafeRelease( *pEffectGroup );
		EMap< EString, EffectGroupRecycle >::iterator it = m_Recycle.find( szFileName );
		if ( it != m_Recycle.end() )
		{
			CEffectGroup* pToDel = (CEffectGroup*)it->second.m_pEffectGroup;
			m_Recycle.erase(it);
			SafeDelete(pToDel);
		}
		m_pEffectGroups[szFileName]  = NULL;
		return GERROR;
	}
	m_pEffectGroups[szFileName] = *pEffectGroup;
	return 0;
}

//HRESULT CEffectManager::CreateEffectGroup( const char* szFileName, IEffectGroup** pEffectGroup )
//{
//	*pEffectGroup = NULL;
//	EMap<EString,IEffectGroup*>::iterator it = m_pEffectGroups.find( szFileName );
//	if( it != m_pEffectGroups.end() )
//	{
//		it->second->AddRef();
//		*pEffectGroup = it->second;
//		return 0;
//	}
//
//	EMap< EString, EffectGroupRecycle >::iterator itRecycle = m_Recycle.find( szFileName );
//	if( itRecycle != m_Recycle.end() )
//	{
//		m_pEffectGroups[szFileName] = itRecycle->second.m_pEffectGroup;
//		*pEffectGroup = itRecycle->second.m_pEffectGroup;
//		itRecycle->second.m_pEffectGroup->AddRef();
//		m_Recycle.erase( itRecycle );
//		return 0;
//	}
//
//	*pEffectGroup = CreateEffectGroupFun(szFileName);
//	(*pEffectGroup)->AddRef();
//	CEffectFormatMgr effectMgr;
//	CEffectGroup* pGroup = static_cast<CEffectGroup*>(*pEffectGroup);
//	effectMgr.Load(szFileName,pGroup);
//
//	m_pEffectGroups[szFileName] = *pEffectGroup;
//	return 0;
//
//}

HRESULT CEffectManager::CreateEffect( IEffectGroup* pEffectGroup, const char* szEffectName, IEffect** pEffect )
{
	*pEffect = NULL;
	if ( !pEffectGroup )
		return GERROR;

	string effectName = szEffectName;
	slash_to_backslash(effectName);

	CEffectProp* pEffectProp = ((CEffectGroup*)pEffectGroup)->GetEffectProp( effectName.c_str() );
	if( !pEffectProp )
		return GERROR;
	SQR_TRY	
	{
		*pEffect = new CEffect( this, pEffectProp );
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("特效创建失败");
		exp.AppendType(szEffectName);
		
		SQR_TRY
		{
			CPkgFile pkgFile;
			if ( eFE_SUCCESS == pkgFile.Open(PATH_ALIAS_EFFECT.c_str(), pEffectGroup->GetName()) )
			{
				GfkLogPkgErr(exp, szEffectName, pkgFile);
				pkgFile.Close();
				return GERROR;
			}
		}
		SQR_CATCH(exp)
		{
			GfkLogExp(exp);
			return GERROR;
		}
		SQR_TRY_END;
		GfkLogExpOnce(exp);
		return GERROR;
	}
	SQR_TRY_END;
	return 0;
}

void CEffectManager::Update( uint32 CurTime )
{
	for( EMap< EString, EffectGroupRecycle >::iterator it = m_Recycle.begin(); it != m_Recycle.end(); )
	{
		if( GetProcessTime() - it->second.m_DeleteTime > 60000 && ((CEffectGroup*)it->second.m_pEffectGroup)->IsValid() )
		{
			delete ( (CEffectGroup*)it->second.m_pEffectGroup );
			m_Recycle.erase( it++ );
		}
		else
			it++;
	}
	Render(CurTime);
}

void CEffectManager::Render( uint32 uCurTime )
{
	for( EffectClassDescMap_t::iterator it = m_EffectClassDesc.begin(); it != m_EffectClassDesc.end(); ++it )
		it->second.m_EffectDesc->Render( uCurTime );
}

void CEffectManager::ClearRecycle()
{
	for( EMap< EString, EffectGroupRecycle >::iterator it = m_Recycle.begin(); it != m_Recycle.end(); )
	{
		delete ( (CEffectGroup*)it->second.m_pEffectGroup );
		m_Recycle.erase( it++ );
	}
}
