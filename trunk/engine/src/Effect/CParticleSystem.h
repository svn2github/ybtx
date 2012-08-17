#pragma once
#include "CEffectUnit.h"
#include "CModel.h"
#include "ClipPlane.h"

namespace sqr
{
	class CParticleSystemPropCoder;
}

class EFFECT_API CParticleSystemProp :	public CEffectUnitProp
{
protected:
	enum ePS_EMSType
	{
		PSEMS_eSphere		= 0,
		PSEMS_eCircle		= 1,
		PSEMS_eRectangle	= 2,
		PSEMS_eMulCorner	= 3
	};

	enum ePS_VELType
	{
		PSVEL_eRANDOM = 0,
		PSVEL_eFROMCENTER = 1,
		PSVEL_eTOCENTER = 2
	};

	enum ePS_TextureDir
	{
		PSTD_eDirCamera					= 0,
		PSTD_eDirVelocity				= 1,
		PSTD_eDirFaceToVelocity			= 2,
		PSTD_eDirAlwaysUP				= 3,
		PSTD_eDirUpUpUp 				= 4
	};

	enum ePS_ParticlCoord
	{
		PSPC_Free	= 0,
		PSPC_Follow,
		PSPC_AddSpeed,
		PSPC_Static,
		PSPC_Normal,
	};

	int32							m_nEmissionInterval;		
	int32							m_nParticlesPerEmission;		
	float							m_fMinSize;
	float							m_fMaxSize;
	CVector3f						m_vEmissionPlan;
	CVector3f						m_vMinVelocity;
	CVector3f						m_vMaxVelocity;
	CVector3f						m_vAcceleration;		//additional acceleration vector
	float							m_fMinAngularVelocity;
	float							m_fMaxAngularVelocity;
	int32							m_nMinLifeSpan;
	int32							m_nMaxLifeSpan;
	float							m_fMinRadius;
	float							m_fMaxRadius;
	float							m_fLongShortRadius;
	float							m_fLongShortDRadius;
    float							m_fCornerDivision;
	float							m_fVelocityPercent;
	float							m_fWidthRatio;
	ePS_ParticlCoord				m_eFollowType;			// Do the particles move with the system?
	ePS_EMSType						m_eEmissionType;		// The emmission method
	ePS_VELType						m_eVelType;
	ePS_TextureDir					m_eTextDir;
	IDNAME							m_sRefSkelatal;
	float							m_fZBias;

	SPR_BLENDMODE					m_eBlendModel;
	SPR_BLENDOP						m_eTextOp;
	Z_TEST							m_eZTest;
	BOOL							m_bLightEnable;

	ClipPlane						m_ClipPlane;
	EMap<float, float>				m_ClipDistMap;
	void							_UpdateParams(void);
	friend class CParticleSystem;
	friend class CParticleSystemPropCoder;

public:
	CParticleSystemProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CParticleSystemProp(void);

	virtual int32				LoadBin( IEffectRead& File );
	virtual int32				SaveBin( IEffectWrite& File );
	EBaseEffectUnit				GetUintType(void) { return eParticleSystem; }
	virtual void				_CalculateAABB();
};

class EFFECT_API CParticleSystem :	public CEffectUnit//粒子系统
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CParticleSystem )

	CVector3f						m_vecOffsetVelocity;
	uint32							m_RemainTime;
	int16							m_nRefSkeletal;
	CMatrix							m_matRefence;

	struct ParticleList
	{
		int32		nParticleNum;
		Particle*	pParticle;
	};
	EMap< ITexture*, ParticleList >	m_ParticleOrderByTexture;

public:
	void						GetDesc( char * str )		{ sprintf( str, " 当前粒子数%d", m_nTotalParticleNum); }

	CParticleSystem( IEffectUnitProp* pEffectUnitProp );
	virtual ~CParticleSystem(void);

	virtual void				OnLinked( CRenderable* pParentObj );
	virtual void				OnUnlinked( void );
	virtual const CMatrix&		GetMatrix();

	virtual void				Free();

	void						UpdateParticle( uint32 CurTime, const CMatrix& matPre, bool bAlloc );
	void						InitParticle( Particle* pParticle, const CMatrix& matPre, float fWeight );
	virtual bool				SetProp( IEffectUnitProp* pEffectUnitProp );
	const EFFECT_RESULT			RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//渲染模型	

	//下面数据成员用于粒子回收
	uint32							m_nTotalParticleNum;
	EList<IEffectUnit*>::iterator	m_it;
};
