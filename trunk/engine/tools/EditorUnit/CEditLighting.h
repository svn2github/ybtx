#pragma once
#include "CLightning.h"
#include "CEditUnitPropHelp.h"

class CEditLightningProp
	: public CLightningProp
	,public CEditUnitPropHelp
{
public:
	CEditLightningProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual void			SetUintPropData();

	virtual int32			GetProp( EFFECT_PROP* pProp, int32 nBufLen );
	virtual void			SetProp( const EFFECT_PROP* pProp, int32 nBufLen );
protected:
	virtual void			_UIDataUpdate();
};