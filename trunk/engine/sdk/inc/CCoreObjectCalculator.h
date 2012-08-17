#pragma once
#include "CCoreObjectServer.h"

namespace sqr
{
	class ICoreObjectCalculatorHandler;

	class CCoreObjectCalculator
		:public CCoreObjectServer
	{
	public:
		typedef ICoreObjectCalculatorHandler	Handler_t;

		CCoreObjectCalculator(ICoreObjectCalculatorHandler* pHandler,CCoreSceneServer* pScene);

		virtual EServerObjectType GetType()const;
		virtual CCoreObjectMediator* CastToObjMediator();
		virtual CCoreObjectDictator* CastToObjDictator();
		ICoreObjectCalculatorHandler* GetHandler()const;
	};
}

