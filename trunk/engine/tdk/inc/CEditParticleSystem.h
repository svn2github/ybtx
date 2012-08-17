#pragma once

#include "CParticleSystem.h"
#include "CEditUnitPropHelp.h"

class CEditParticleSystemProp
	:public CParticleSystemProp
	,public CEditUnitPropHelp
{
public:
	CEditParticleSystemProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual void				SetUintPropData();

	virtual int32				GetProp( EFFECT_PROP* pProp, int32 nBufLen );
	virtual void				SetProp( const EFFECT_PROP* pProp, int32 nBufLen );
	virtual void				SetAdvancedProp( const ADVANCE_PROP* pOffset,   int32 nOffLen,
		const ADVANCE_PROP* pTexture,		int32 nTextLen,
		const ADVANCE_PROP* pColor,		int32 nColorLen,
		const ADVANCE_PROP* pScale,		int32 nScaleLen,
		const ADVANCE_PROP* pXYZcale,		int32 nXYZcale,
		const ADVANCE_PROP* pEmiMinRad,	int32 nEmissionMinRadius,
		const ADVANCE_PROP* pEmiMaxRad,    int32 nEmissionMaxRadius);
protected:
	virtual void			_UIDataUpdate();
};