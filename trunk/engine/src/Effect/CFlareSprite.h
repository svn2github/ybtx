#pragma once
#include "CEffectUnit.h"

namespace sqr
{
	class CFlareSpritePropCoder;
}

class EFFECT_API CFlareSpriteProp : public CEffectUnitProp
{
protected:
	CVector3f				m_vecRotate;
	CVector3f				m_vecNormal;
	float					m_fMaxAngle;
	BYTE					m_byMinAlpha;
	BYTE					m_byMaxAlpha;
	float					m_fMinSize;
	float					m_fMaxSize;
	SPR_FACE				m_eFaceType;
	SPR_BLENDMODE			m_eBlendModel;
	Z_TEST					m_eZTest;
	BOOL					m_bLightEnable;

	float					m_MaxCosAngle;
	CMatrix					m_matNormal;
	float					m_AngularVelocity;
	void					_UpdateParams(void);
	friend class CFlareSprite;
	friend class CFlareSpritePropCoder;
public:
	CFlareSpriteProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CFlareSpriteProp(void);

	virtual int32				LoadBin( IEffectRead& File );
	virtual int32				SaveBin( IEffectWrite& File );
	EBaseEffectUnit				GetUintType(void) { return eLeading; }
	virtual void				_CalculateAABB();
};

class EFFECT_API CFlareSprite : public CEffectUnit//闪光公告板
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CFlareSprite )

	float					m_CurAngle;

public:
	CFlareSprite( IEffectUnitProp* pEffectUnitProp );
	virtual ~CFlareSprite(void);

	const EFFECT_RESULT RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//渲染模型	
};
