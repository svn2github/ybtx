#pragma once
#include "CCameraFilter.h"
#include "CEditUnitPropHelp.h"

class CEditCameraFilterProp 
	: public CCameraFilterProp
	, public CEditUnitPropHelp
{
public:
	CEditCameraFilterProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CEditCameraFilterProp(void);
	virtual void			SetUintPropData();

	virtual int32			GetProp( EFFECT_PROP* pProp, int32 nBufLen );
	virtual void			SetProp( const EFFECT_PROP* pProp, int32 nBufLen );

	virtual uint32			GetAdvancedProp( ADVANCE_PROP* pOffset,    int32& nOffLen,
		ADVANCE_PROP* pTexture,   int32& nTextLen,
		ADVANCE_PROP* pColor,     int32& nColorLen,
		ADVANCE_PROP* pScale,     int32& nScaleLen,
		ADVANCE_PROP* pXYZcale,   int32& nXYZcaleLen,
		ADVANCE_PROP* pUVRCenter, int32& nUVRCenter);
};