#pragma once
#include "ceffectunit.h"

namespace sqr
{
	class CCameraShakePropCoder;
}

class EFFECT_API CCameraShakeProp : public CEffectUnitProp
{
protected:
	float					m_fFrequence;
	float					m_fStrength;
	float					m_fMinRadius;
	float					m_fMaxRadius;
	void					_UpdateParams(void);
	friend class CCameraShake;
	friend class CCameraShakePropCoder;
public:
	CCameraShakeProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CCameraShakeProp(void);

	virtual int32				LoadBin( IEffectRead& File );
	virtual int32				SaveBin( IEffectWrite& File );
	EBaseEffectUnit				GetUintType(void) { return eCameraShake; }
};

class EFFECT_API CCameraShake : public CEffectUnit//镜头震动
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CCameraShake )
public:
	CCameraShake( IEffectUnitProp* pEffectUnitProp );
	virtual ~CCameraShake(void);
	const EFFECT_RESULT RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//渲染模型	
};
