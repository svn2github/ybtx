#include "stdafx.h"
#include "CCoreObjectCalculator.h"
#include "ICoreObjectCalculatorHandler.h"
#include "CoreCommon.h"

CCoreObjectCalculator::CCoreObjectCalculator(ICoreObjectCalculatorHandler* pHandler,CCoreSceneServer* pScene)
:CCoreObjectServer(pHandler, pScene)
{
}

EServerObjectType CCoreObjectCalculator::GetType()const
{
	return eSOT_Calculator;
}

CCoreObjectMediator* CCoreObjectCalculator::CastToObjMediator()
{
	return NULL;
}

CCoreObjectDictator* CCoreObjectCalculator::CastToObjDictator()
{
	return NULL;
}

ICoreObjectCalculatorHandler* CCoreObjectCalculator::GetHandler()const
{
	ICoreObjectServerHandler* pHandler=CCoreObjectServer::GetHandler();
	return pHandler ? pHandler->CastToObjCalHandler() : NULL;
}
