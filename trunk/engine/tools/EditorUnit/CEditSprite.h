#pragma once
#include "CSprite.h"
#include "CEditUnitPropHelp.h"

class CEditSpriteProp
	:public CSpriteProp
	,public CEditUnitPropHelp
{
public:
	CEditSpriteProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual void			SetUintPropData();

	virtual int32			GetProp( EFFECT_PROP* pProp, int32 nBufLen );
	virtual void			SetProp( const EFFECT_PROP* pProp, int32 nBufLen );
protected:
	virtual void			_UIDataUpdate();
};