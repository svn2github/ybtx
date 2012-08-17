#pragma once
#include "TMsgHandler.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"


template<typename ImpClass,typename MsgClass>
TMsgHandler<ImpClass,MsgClass>::TMsgHandler(HandlerFun_t pFun)
:m_pHandlerFun(pFun)
{
}

template<typename ImpClass,typename MsgClass>
ECallResult TMsgHandler<ImpClass,MsgClass>::Call(ImpClass* pDispatcher,const void* pData,uint32 uSize,void* pParam,uint32& uCmdSize)
{
	if ( uSize < MsgClass::GetHeaderSize() )
		return eCR_Again;

	const MsgClass* pMsg = static_cast<const MsgClass*>(pData);

	if ( !pMsg->ChecksumCorrect() )
		return eCR_Error;

	uCmdSize = pMsg->GetPayloadSize() + MsgClass::GetHeaderSize();
	if ( uCmdSize>uSize )
		return eCR_Again;

	SQR_TRY
	{
		//cout<<typeid(MsgClass).name()<< "\t" << uCmdSize << endl;
		static_cast<ImpClass*>(pDispatcher)->LogMsgRecvTrafficForCpp(MsgClass::GetName(), MsgClass::GetId(), uCmdSize);
		( pDispatcher->*GetHandlerFun() )( pMsg, pParam );
	}
	SQR_CATCH(exp)
	{
		LogExp(exp, static_cast<ImpClass*>(pDispatcher)->GetLogOwner());
		ostringstream str;
		str << exp.ErrorMsg() << endl << exp.StackInfo() << endl;
		static_cast<ImpClass*>(pDispatcher)->SendErrMsg(str.str().c_str());
		static_cast<ImpClass*>(pDispatcher)->OnCaughtNetException();
	}
	SQR_TRY_END;
	return eCR_Success;
}


template<typename ImpClass,typename MsgClass>
typename TMsgHandler<ImpClass,MsgClass>::HandlerFun_t TMsgHandler<ImpClass,MsgClass>::GetHandlerFun()const
{
	return m_pHandlerFun;
}

