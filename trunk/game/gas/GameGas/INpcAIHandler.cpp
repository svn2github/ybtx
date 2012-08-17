#include "stdafx.h"
#include "INpcAIHandler.h"
#include "CNpcAI.h"
#include "CMemberAI.h"
#include "CDummyAI.h"
#include "CServantAI.h"
#include "CTruckAI.h"
#include "CLittlePetAI.h"

CNpcAI* INpcAIHandler::CastToNpcAI()
{ 
	return class_cast<CNpcAI*>(this);
}

CServantAI* INpcAIHandler::CastToServantAI()
{
	return class_cast<CServantAI*>(this);
}

CDummyAI* INpcAIHandler::CastToDummyAI()
{ 
	if( GetType() != ECharacterInnerHandlerType_Dummy )
		return NULL;
	return class_cast<CDummyAI*>(this);
}

CMemberAI* INpcAIHandler::CastToMemberAI()
{ 
	if ( GetType() != ECharacterInnerHandlerType_Member )
		return NULL;
	return class_cast<CMemberAI*>(this);
}

CTruckAI* INpcAIHandler::CastToTruckAI()
{
	return class_cast<CTruckAI*>(this);
}

CLittlePetAI* INpcAIHandler::CastToLittlePetAI()
{
	return class_cast<CLittlePetAI*>(this);
}
