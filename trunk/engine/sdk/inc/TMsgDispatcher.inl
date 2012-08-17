#pragma once
#include "TMsgDispatcher.h"
#include "TMsgHandler.h"
#include "TMsgHandlerContainer.inl"
#include "ExpHelper.h"
#include "ErrLogHelper.h"


template<typename ImpClass,typename IdType,uint32 uId>
uint32 TBaseMsgDispatcher<ImpClass,IdType,uId>::GetHandlerNumber()
{
	return GetHandlers().GetSize();
}

template<typename ImpClass,typename IdType,uint32 uId>
bool TBaseMsgDispatcher<ImpClass,IdType,uId>::SetHandlerNumber(uint32 uSize)
{
	return SetHandlerNumber(uSize,0);
}

template<typename ImpClass,typename IdType,uint32 uId>
bool TBaseMsgDispatcher<ImpClass,IdType,uId>::SetHandlerNumber(uint32 uSize,int32 nIdOffset)
{
	return GetHandlers().Resize(uSize,nIdOffset);
}

template<typename ImpClass,typename IdType,uint32 uId>
typename TBaseMsgDispatcher<ImpClass,IdType,uId>::BaseHandler_t*
TBaseMsgDispatcher<ImpClass,IdType,uId>::GetHandler( uint32 uIndex )
{
	return GetHandlers().GetHandler(uIndex);
}


template<typename ImpClass,typename IdType,uint32 uId>
EDispatchResult TBaseMsgDispatcher<ImpClass,IdType,uId>::
Dispatch(const void* pData,uint32 uSize,void* pParam,uint32& uProcessedSize, bool bDispatchOnce )
{
#ifdef FindBugOfTBaseMsgDispatcher
	m_bFlag1 = false;
#endif
	const char* pPos=reinterpret_cast<const char*>(pData);
	uint32 uSingleProcessedSize=0;
	uProcessedSize=0;

#ifdef FindBugOfTBaseMsgDispatcher
	m_bFlag1 = true;
#endif

	ImpClass* pImpThis=static_cast<ImpClass*>(this);
	for (;;)
	{
#ifdef FindBugOfTBaseMsgDispatcher
		m_bFlag2 = false;
		m_bFlag3 = false;
		m_bFlag4 = false;
		m_bFlag5 = false;
		m_bFlag6 = false;
#endif

		if (pImpThis->DispatchIsCanceled())
			return eCanceled;

#ifdef FindBugOfTBaseMsgDispatcher
		m_bFlag2 = true;
#endif

		uint32 uLeftSize=uSize-uProcessedSize;
		if (uLeftSize<sizeof(IdType))
			return uProcessedSize>0? eSuccess:eCallAgain;

#ifdef FindBugOfTBaseMsgDispatcher
		m_bFlag3 = true;
#endif
		
		const IdType* pi = reinterpret_cast<const IdType*>(pPos);

		if ( static_cast<uint32>(*pi+GetHandlers().GetIdOffset()) > GetHandlers().GetSize() )
			return eHandlerOver;

#ifdef FindBugOfTBaseMsgDispatcher
		m_bFlag4 = true;
#endif

		TBaseMsgHandler<ImpClass>* pHandler=GetHandlers().GetHandler(*pi);
		if ( !pHandler )
			return eHandlerNotFound;

#ifdef FindBugOfTBaseMsgDispatcher
		m_bFlag5 = true;
#endif

		ECallResult eResult;

		eResult=pHandler->Call(pImpThis,pPos,uLeftSize,pParam,uSingleProcessedSize);

#ifdef FindBugOfTBaseMsgDispatcher
		m_bFlag6 = true;
#endif

		if (eResult==eCR_Success)
		{
			uProcessedSize += uSingleProcessedSize;
			pPos += uSingleProcessedSize;

			if ( bDispatchOnce )
			{
				Ast( uProcessedSize > 0 );
				return eSuccess;
			}

			continue;
		}

		if ( eResult==eCR_Again )
			return eCallAgain;

		if ( eResult==eCR_Error )
			return eCallError;
	}
}

template<typename ImpClass,typename IdType,uint32 uId>
void TBaseMsgDispatcher<ImpClass,IdType,uId>::LogMsgRecvTrafficForCpp(const char* szProtlName, uint16 uMsgID, uint32 uMsgLen)
{
}


template<typename ImpClass,typename IdType,uint32 uId>
void TBaseMsgDispatcher<ImpClass,IdType,uId>::InitMsgHandlerContainer()
{
	Ast(!IntGetHandlers());
	IntGetHandlers() = new TMsgHandlerContainer<ImpClass>;
}

template<typename ImpClass,typename IdType,uint32 uId>
void TBaseMsgDispatcher<ImpClass,IdType,uId>::UninitMsgHandlerContainer()
{
	delete IntGetHandlers();
	IntGetHandlers() = NULL;
}

template<typename ImpClass,typename IdType,uint32 uId>
TMsgHandlerContainer<ImpClass>*& TBaseMsgDispatcher<ImpClass,IdType,uId>::IntGetHandlers()
{
	static TMsgHandlerContainer<ImpClass>* ms_pHandlers = NULL;
	return ms_pHandlers;
}

template<typename ImpClass,typename IdType,uint32 uId>
TMsgHandlerContainer<ImpClass>& TBaseMsgDispatcher<ImpClass,IdType,uId>::GetHandlers()
{
	Ast(IntGetHandlers());
	return *IntGetHandlers();
}


