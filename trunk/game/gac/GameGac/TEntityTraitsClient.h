#pragma once

template
< class EntityTypeArg
, class CoreObjectTypeArg
, class CoreObjectHandlerTypeArg
, class ActorTypeArg
, class ActorHandlerTypeArg
, class FighterTypeArg 
, class FighterHandlerTypeArg 
, class RenderTypeArg
, class RenderHandlerTypeArg
>

class TEntityTraitsClient
{
public:
	typedef EntityTypeArg				EntityType;
	typedef CoreObjectTypeArg			CoreObjectType;
	typedef CoreObjectHandlerTypeArg    CoreObjectHandlerType;
	typedef ActorTypeArg                ActorType;
	typedef ActorHandlerTypeArg         ActorHandlerType;
	typedef FighterTypeArg              FighterType;
	typedef FighterHandlerTypeArg       FighterHandlerType;
	typedef RenderTypeArg				RenderType;
	typedef RenderHandlerTypeArg		RenderHandlerType;
};
