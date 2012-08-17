#pragma once

class CEntityServer;
class IActorServerHandler
{
public:
	virtual ~IActorServerHandler(){}
	virtual uint32 GetEntityID() const = 0;
	virtual CEntityServer* GetEntity() const = 0;
	virtual CCharacterDictator* GetCharacter() const = 0;
};
