#pragma once
#include "CNull.h"
#include "CEditUnitPropHelp.h"

class CEditNullProp
	:public CNullProp
	,public CEditUnitPropHelp
{
public:
	CEditNullProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual void			SetUintPropData();

	virtual int32			GetProp( EFFECT_PROP* pProp, int32 nBufLen );
	virtual void			SetProp( const EFFECT_PROP* pProp, int32 nBufLen );

	virtual uint32			GetAdvancedProp( ADVANCE_PROP* pOffset,       int32& nOffLen,
		ADVANCE_PROP* pTexture,   int32& nTextLen,
		ADVANCE_PROP* pColor,     int32& nColorLen,
		ADVANCE_PROP* pScale,     int32& nScaleLen, 
		ADVANCE_PROP* pXYZcale,   int32& nXYZcaleLen,
		ADVANCE_PROP* pUVRCenter, int32& nUVRCenter);

	virtual void			SetAdvancedProp( const ADVANCE_PROP* pOffset,       int32 nOffLen,
		const ADVANCE_PROP* pTexture,   int32 nTextLen,
		const ADVANCE_PROP* pColor,     int32 nColorLen,
		const ADVANCE_PROP* pScale,     int32 nScaleLen,
		const ADVANCE_PROP* pXYZcale,   int32 nXYZcaleLen,
		const ADVANCE_PROP* pUVRCenter, int32 nUVRCenter);
protected:
	virtual void			_UIDataUpdate();
};