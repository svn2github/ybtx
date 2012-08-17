#include "stdafx.h"
#include "CSyncCoreObjectCalculator.h"
#include "CoreCommon.h"

CSyncCoreObjectCalculator::CSyncCoreObjectCalculator(uint32 uID, CSyncCoreSceneServer* pScene, const CFPos& PixelPos)
:CSyncCoreObjectServer(uID, pScene, PixelPos)
{
}

EServerObjectType CSyncCoreObjectCalculator::GetType()const
{
	return eSOT_Calculator;
}
