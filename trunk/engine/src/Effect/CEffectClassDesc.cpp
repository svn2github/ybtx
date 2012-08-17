#include "stdafx.h"
#include "TimeHelper.h"
#include "CEffectClassDesc.h"
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
#include "CEffectClass.h"
#include "TSqrAllocator.inl"
RenderObjStyle CEffectClassDesc::st_RenderObjStyle = { 
			ROS_ALPHA_LIGHT2,
			true, 
			true,
			DELTARS_DIFFUSE|DELTARS_AMBIENT,
			NULL
	};
CEffectClassDesc::CEffectClassDesc( void ) 
{
	m_ParticleMemory = new Particle[MAX_PARTICLE];
	for( int32 i = 0; i < MAX_PARTICLE; ++i )
		m_ParticleIdleTable[i] = (WORD)i;
	m_ParticleCurIdle = MAX_PARTICLE - 1;

	m_TrailMemory = new Trail[MAX_TRAIL];
	for( int32 i = 0; i < MAX_TRAIL; ++i )
		m_TrailIdleTable[i] = (WORD)i;
	m_TrailCurIdle = MAX_TRAIL - 1;

	for( int32 i = 0; i < 256; ++i )
	{
		m_SinBuf[i] = sin( i*6.283f/256.0f );
		m_CosBuf[i] = cos( i*6.283f/256.0f );
		m_ASinBuf[i] = asin( ( i - 128.0f )/128.0f );
		m_ACosBuf[i] = acos( ( i - 128.0f )/128.0f );
	}
}

CEffectClassDesc::~CEffectClassDesc(void)
{
	ClearEffect();

	delete[] m_ParticleMemory;
	delete[] m_TrailMemory;
}

void CEffectClassDesc::ClearEffect( void )
{
	for( EList<IEffectUnit*>::iterator it = m_LeavingsUnits.begin(); it != m_LeavingsUnits.end(); ++it )
		delete (CEffectUnit*)(*it);
	m_LeavingsUnits.clear();
}

void CEffectClassDesc::CleanUpParticle( IEffectUnit* pUser )
{
	uint32 CurTime = uint32( GetProcessTime() );
	for( EList<IEffectUnit*>::iterator it = m_ParticleSysList.begin(); it != m_ParticleSysList.end(); ++it )
	{
		if( pUser != *it )
		{
			CParticleSystem* pParticleSys = (CParticleSystem*)(*it);
			if( pParticleSys->m_nTotalParticleNum && CurTime - pParticleSys->m_CurTime > 1000 )
				pParticleSys->UpdateParticle( CurTime, pParticleSys->GetMatrix(), false );
		}
	}
}

void CEffectClassDesc::CleanUpTrail( IEffectUnit* pUser )
{
	uint32 CurTime = uint32( GetProcessTime() );
	for( EList<IEffectUnit*>::iterator it = m_TrailList.begin(); it != m_TrailList.end(); ++it )
	{
		if( pUser != *it )
		{
			CPolyTrail* pTrail = (CPolyTrail*)(*it);
			if( pTrail->m_TrailUnitNum && CurTime - pTrail->m_CurTime > 1000 )
				pTrail->UpdateTrail( CurTime, false );
		}
	}
}

const char* CEffectClassDesc::GetEffectUnitName( int32 Num )
{	
	return CEffectClassEx::GetEffectName( (EBaseEffectUnit)Num );
}

IEffectUnitProp* CEffectClassDesc::CreateEffectUnitProp( int32 Num )
{
	return CEffectClassEx::CreateEffectUnitProp( (EBaseEffectUnit)Num, this );
}

IEffectUnit* CEffectClassDesc::CreateEffectUnit( IEffectUnitProp* pProp )
{
	IEffectUnit * pEffectUnit = NULL;
	pEffectUnit = CEffectClassEx::CreateEffectUnit( pProp );
	if( pEffectUnit )
		pEffectUnit->SetProp( pProp );
	
	return pEffectUnit;
}

const GUID CEffectClassDesc::GetClassID()
{
	static const GUID ClassID = 
	{ 0x566a9bce, 0x4aa3, 0x4807, { 0x97, 0xa2, 0x9b, 0xc6, 0x49, 0x9d, 0x9d, 0xae } };

	return ClassID;
}

const char* CEffectClassDesc::GetClassDescription()
{
	//return "Effect";
	return "Effect Version1.0";
}

const char* CEffectClassDesc::GetShowedInfo( uint32 Time)
{
	static char Buf[256];
	// 添加显示当前粒子数的信息
	//sprintf( Buf, "总粒子数:%d, 总轨迹节数:%d", MAX_PARTICLE - 1 - m_ParticleCurIdle, MAX_TRAIL - 1 - m_TrailCurIdle );	

	EList<IEffectUnit*>::iterator	it;
	CParticleSystem * p;
	int32 nTotal = 0;
	for( it = m_ParticleSysList.begin(); it != m_ParticleSysList.end(); ++it )
	{
		p = ( CParticleSystem * )(*it);
		if( p->IsVisible() )
		{
			nTotal += p->m_nTotalParticleNum;
		}
	}
	
	sprintf( Buf, "总粒子数:%d, 总轨迹节数:%d", nTotal, MAX_TRAIL - 1 - m_TrailCurIdle );

	// end

	return Buf;
}

void CEffectClassDesc::AddLeavingsEffectUnit( IEffectUnit* pUnit )
{
	pUnit->SetDistortTime(m_uCurTime);
	m_LeavingsUnits.push_back( pUnit );
}

void CEffectClassDesc::Render( uint32 uCurTime)
{
	EList<IEffectUnit*>::iterator it = m_LeavingsUnits.begin();
	EList<IEffectUnit*>::iterator eit = m_LeavingsUnits.end();
	EList<IEffectUnit*>::iterator delIt;
	string EffectName;
	uint32 ErrFlag;
	for(;;)
	{
		SQR_TRY
		{
			for( ; it != eit; )
			{
				ErrFlag = 0;
				delIt = it++;
				CEffectUnit* pUnit = (CEffectUnit*)(*delIt);
				ErrFlag = 1;
				EffectName = pUnit->GetProp()->GetEffectFileName();
				ErrFlag = 2;
				EFFECT_RESULT FxRes = pUnit->RenderEx( pUnit->GetMatrix(), uCurTime, &st_RenderObjStyle );
				if( FxRes.m_UnitbEmpty )
				{
					ErrFlag = 3;
					m_LeavingsUnits.erase(delIt);
					ErrFlag  = 4;
					delete pUnit;
				}
			}
			break;
		}
		SQR_CATCH(exp)
		{
			switch(ErrFlag)
			{
			case 0:
				exp.AppendMsg("_SetPtr");
				break;
			case 1:
				exp.AppendMsg("_Get Name");
				break;
			case 2:
				exp.AppendMsg("_Render");
				m_LeavingsUnits.erase(delIt);
				break;
			case 3:
				exp.AppendMsg("_Erase");
				break;
			case 4:
				exp.AppendMsg("_Delete");
				break;
			default:
				break;
			}
			exp.AppendMsg(EffectName.c_str());
			GfkLogExp(exp);	
		}
		SQR_TRY_END;
	}
	m_uCurTime = uCurTime;
}