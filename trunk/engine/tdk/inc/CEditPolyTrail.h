#pragma once
#include "CPolyTrail.h"
#include "CEditUnitPropHelp.h"

class CEditPolyTrailProp
	:public CPolyTrailProp
	,public CEditUnitPropHelp
{
public:
	CEditPolyTrailProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual void			SetUintPropData();

	virtual int32			GetProp( EFFECT_PROP* pProp, int32 nBufLen );
	virtual void			SetProp( const EFFECT_PROP* pProp, int32 nBufLen );
	virtual void			SetAdvancedProp( const ADVANCE_PROP* pOffset,  int32 nOffLen,
		const ADVANCE_PROP* pTexture, int32 nTextLen,
		const ADVANCE_PROP* pColor,   int32 nColorLen,
		const ADVANCE_PROP* pScale,   int32 nScaleLen );
protected:
	virtual void			_UIDataUpdate();

};