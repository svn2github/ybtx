#pragma once

#include "IEffectUnit.h"
#include "IEffectGraphic.h"

namespace sqr
{

class IEffectClassDesc
{
public:
	virtual void				Release() = 0;
	virtual int32				GetEffectUnitNum() = 0;
	virtual const char*			GetEffectUnitName( int32 Num ) = 0;
	virtual IEffectUnitProp*	CreateEffectUnitProp( int32 Num ) = 0;
	virtual IEffectUnit*		CreateEffectUnit( IEffectUnitProp* pProp ) = 0;
	virtual const GUID			GetClassID() = 0;
	virtual const char*			GetClassDescription() = 0;
	virtual const char*			GetShowedInfo( uint32 Time ) = 0;
	virtual void				Render( uint32 CurTime ) = 0;
	virtual void				ClearEffect( void ) = 0;
};

typedef IEffectClassDesc* (*CREATE_FX_DESC)(void);

}