#pragma once
#include "BaseDefs.h"

NAMESPACE_BEGIN(sqr);

template
< class CoreObjectTypeArg
, class CoreObjectHandlerTypeArg
, class ActorTypeArg
, class ActorHandlerTypeArg
, class FighterTypeArg 
, class FighterHandlerTypeArg 
, class CallbackHandlerTypeArg
>

class TCharacterTraitsClient
{
    public:
        typedef CoreObjectTypeArg           CoreObjectType;
        typedef CoreObjectHandlerTypeArg    CoreObjectHandlerType;
        typedef ActorTypeArg                ActorType;
        typedef ActorHandlerTypeArg         ActorHandlerType;
        typedef FighterTypeArg              FighterType;
        typedef FighterHandlerTypeArg       FighterHandlerType;
		typedef CallbackHandlerTypeArg		CallbackHandlerType;
};

NAMESPACE_END;
