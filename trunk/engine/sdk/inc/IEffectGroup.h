#pragma once
#include "GraphicBase.h"
#include "GraphicRes.h"
#include "CEffectMallocObject.h"
namespace sqr
{

class IEffectGroup
	: public GraphicRes
	, public CEffectMallocObject
{
public:
	virtual uint32			GetEffectNum() = 0;
	virtual const char*		GetEffectName( uint32 Num ) = 0;

	virtual const char*		GetName() const = 0;

	virtual void			_FreeRes(void)	= 0;
	virtual void			_Reclaim(void)	= 0;
};

}