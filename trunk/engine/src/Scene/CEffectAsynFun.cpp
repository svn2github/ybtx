#include "stdafx.h"
#include "CEffectAsynFun.h"
#include "CEffectPlayer.h"
#include "IEffectUnit.h"

SetEffectHandlerName::SetEffectHandlerName( CEffectPlayer* pEfxPlayer,uint32 nIndex, IEffectUnitHandler* pUnitHandler, const char* szUnitName )
: m_pEfxPlayer(pEfxPlayer),Index(nIndex),pHandler(pUnitHandler),Name(const_cast<char*>(szUnitName))
{
	SQR_TRY
	{
		pHandler->AddRef();
	}
	SQR_CATCH(exp)
	{
		pHandler = NULL;
		exp.AppendType(GraphicErr::GetErrTypeStr(CallBack_Err)+":初始化Handler是野指针");
		LogExp(exp);
	}
	SQR_TRY_END;
}

SetEffectHandlerName::~SetEffectHandlerName()
{
	SafeRelease(pHandler);
}

bool SetEffectHandlerName::Do(void)
{
	if(NULL!=pHandler)
		m_pEfxPlayer->SetEffectUnitHandler(Index,pHandler,Name.c_str());
	return true;
}

SetEffectHandlerType::SetEffectHandlerType( CEffectPlayer* pEfxPlayer,uint32 nIndex, IEffectUnitHandler* pUnitHandler, const char* szUnitName,uint32 eEffectUnitType )
: SetEffectHandlerName(pEfxPlayer,nIndex,pUnitHandler,szUnitName),nType(eEffectUnitType)
{
	SQR_TRY
	{
		pHandler->AddRef();
	}
	SQR_CATCH(exp)
	{
		pHandler = NULL;
		exp.AppendType(GraphicErr::GetErrTypeStr(CallBack_Err)+":初始化Handler是野指针");
		LogExp(exp);
	}
	SQR_TRY_END;
}

SetEffectHandlerType::~SetEffectHandlerType()
{
	pHandler->Release();
}

bool SetEffectHandlerType::Do(void)
{
	if(NULL!=pHandler)
		m_pEfxPlayer->SetEffectUnitHandler(Index,pHandler,Name.c_str(),nType);
	return true;
}