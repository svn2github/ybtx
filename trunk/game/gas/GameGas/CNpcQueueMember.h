#pragma once

#include "CNpcAIMallocObject.h"

class CNpcQueueMember
	: public CNpcAIMallocObject
{
public:
	CNpcQueueMember(int32 uAngle, uint32 uEntityID);
	uint32 GetEntityID();
	void SetEntityID(uint32 uEntityID);
	int32 GetAngle();
private:
	int32 m_uAngle;
	uint32 m_uEntityID;
};

