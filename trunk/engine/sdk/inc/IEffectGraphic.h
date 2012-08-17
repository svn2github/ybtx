#pragma once

#include "IGraphic.h"

namespace sqr
{

typedef void (*SetFun)(float);
typedef float (*GetFun)(void);

typedef void (*SetboolFun)(bool);
typedef bool (*GetboolFun)(void);
class IEffectGraphic : public IGraphic
{
public:
	virtual void						SetEffectLevel(float level) = 0;
	virtual float						GetEffectLevel(void) = 0;
	virtual void						SetEffectCallBack( SetFun sfun , GetFun gfun ) = 0;

	virtual void						SetCameraShake(bool b) = 0;
	virtual bool						GetCameraShake(void) = 0;
	virtual void						SetCameraShakeCallBack( SetboolFun sfun , GetboolFun gfun ) = 0;
};

}