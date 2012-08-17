#pragma once
#include "CLeading.h"
#include "CEditUnitPropHelp.h"

class CEditLeadingProp
	:public CLeadingProp
	,public CEditUnitPropHelp
{
public:
	CEditLeadingProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual void			SetUintPropData();

	virtual int32				GetProp( EFFECT_PROP* pProp, int32 nBufLen );
	virtual void				SetProp( const EFFECT_PROP* pProp, int32 nBufLen );
};