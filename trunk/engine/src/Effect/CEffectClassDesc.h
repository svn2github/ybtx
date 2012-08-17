#pragma once
#include "IEffectClassDesc.h"
#include "EffectCommon.h"
#include "CEffectClass.h"
#include <list>

class CEffectClassDesc : public IEffectClassDesc
{
	static	RenderObjStyle	st_RenderObjStyle;
	//For Particle system
	Particle*				m_ParticleMemory;
	WORD					m_ParticleIdleTable[MAX_PARTICLE];	
	int32						m_ParticleCurIdle;
	EList<IEffectUnit*>		m_ParticleSysList;

	EString					m_strDesc;	// 当前特效的描述信息

	//For Poly Trail
	Trail*					m_TrailMemory;
	WORD					m_TrailIdleTable[MAX_TRAIL];	
	int32					m_TrailCurIdle;
	EList<IEffectUnit*>		m_TrailList;
	uint32					m_uCurTime;
	//剩余的特效
	EList<IEffectUnit*>		m_LeavingsUnits;

	//Common
	float					m_SinBuf[256];
	float					m_CosBuf[256];
	float					m_ASinBuf[256];
	float					m_ACosBuf[256];	

	void CleanUpParticle( IEffectUnit* pUser );
	void CleanUpTrail( IEffectUnit* pUser );
	void				ClearEffect( void );

public:
	CEffectClassDesc( void );
	virtual ~CEffectClassDesc(void);
    
	virtual void				Release()								{ delete this;       };
	virtual int32				GetEffectUnitNum()						{ return eBaseUnitCount;          }
	virtual const char*			GetEffectUnitName( int32 Num );
	virtual IEffectUnitProp*	CreateEffectUnitProp( int32 Num );	
	virtual IEffectUnit*		CreateEffectUnit( IEffectUnitProp* pProp );

	virtual const GUID			GetClassID();
	virtual const char*			GetClassDescription();
	virtual const char*			GetShowedInfo( uint32 Time );
	virtual void				Render( uint32 uCurTime );
	void						AddLeavingsEffectUnit( IEffectUnit* pUnit );

	//Common
	float Sin( float Rad )
	{
		int32 n = (int32)( ( Rad/6.283f )*256 );
		return m_SinBuf[n&0xff];
	}

	float Cos( float Rad )
	{
		int32 n = (int32)( ( Rad/6.283f )*256 );
		return m_CosBuf[n&0xff];
	}

	float ASin( float v )
	{
		int32 n = (int32)( ( v + 1 )*128 );
		if( n < 0 )
			m_ASinBuf[0];
		if( n >= 256 )
			return -m_ASinBuf[0];
		return m_ASinBuf[n];
	}

	float ACos( float v )
	{
		int32 n = (int32)( ( v + 1 )*128 );
		if( n < 0 )
			m_ACosBuf[0];
		if( n >= 256 )
			return 0;
		return m_ACosBuf[n];
	}

	void SetRotate( CMatrix& mat, const CVector3f& v, float fRads )
	{
		float		fCos	= Cos( fRads );
		float		fSin	= Sin( fRads );

		mat._11 = ( v.x * v.x ) * ( 1.0f - fCos ) + fCos;
		mat._12 = ( v.x * v.y ) * ( 1.0f - fCos ) - (v.z * fSin);
		mat._13 = ( v.x * v.z ) * ( 1.0f - fCos ) + (v.y * fSin);

		mat._21 = ( v.y * v.x ) * ( 1.0f - fCos ) + (v.z * fSin);
		mat._22 = ( v.y * v.y ) * ( 1.0f - fCos ) + fCos ;
		mat._23 = ( v.y * v.z ) * ( 1.0f - fCos ) - (v.x * fSin);

		mat._31 = ( v.z * v.x ) * ( 1.0f - fCos ) - (v.y * fSin);
		mat._32 = ( v.z * v.y ) * ( 1.0f - fCos ) + (v.x * fSin);
		mat._33 = ( v.z * v.z ) * ( 1.0f - fCos ) + fCos;

		mat._14 = mat._24 = mat._34 = mat._41 = mat._42 = mat._43 = 0.0f;
		mat._44 = 1.0f;
	}

	//For Particle system
	Particle* AllocParticle( IEffectUnit* pUser )
	{
		if( m_ParticleCurIdle < 0 )
		{
			/*CleanUpParticle( pUser );
			if( m_ParticleCurIdle < 0 )*/
			return NULL;
		}
		WORD Index = m_ParticleIdleTable[m_ParticleCurIdle];
		--m_ParticleCurIdle;

		return m_ParticleMemory + Index;
	}

	void FreeParticle( Particle* pParticle )
	{
		++m_ParticleCurIdle;
		m_ParticleIdleTable[m_ParticleCurIdle] = (WORD)( pParticle - m_ParticleMemory );
	}

	EList<IEffectUnit*>::iterator InsertParticleSys( IEffectUnit* pUnit )
	{
		return m_ParticleSysList.insert( m_ParticleSysList.end(), pUnit );
	}

	void RemoveParticleSys( EList<IEffectUnit*>::iterator it )
	{
		m_ParticleSysList.erase( it );
	}

	//For Poly Trail
	Trail* AllocTrail( IEffectUnit* pUser )
	{
		if( m_TrailCurIdle < 0 )
		{
			CleanUpTrail( pUser );
			if( m_TrailCurIdle < 0 )
				return NULL;
		}
		WORD Index = m_TrailIdleTable[m_TrailCurIdle];
		m_TrailCurIdle--;
		return m_TrailMemory + Index;
	}

	void FreeTrail( Trail* pTrail )
	{
		m_TrailCurIdle++;
		m_TrailIdleTable[m_TrailCurIdle] = (WORD)( pTrail - m_TrailMemory );
	}

	EList<IEffectUnit*>::iterator InsertTrail( IEffectUnit* pUnit )
	{
		return m_TrailList.insert( m_TrailList.end(), pUnit );
	}

	void RemoveTrail( EList<IEffectUnit*>::iterator it )
	{
		m_TrailList.erase( it );
	}
};

IEffectClassDesc* CreateEffectClassDesc( void );