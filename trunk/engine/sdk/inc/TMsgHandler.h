#pragma once
#include "CMsgMallocObject.h"


namespace sqr
{
	enum ECallResult {eCR_Success,eCR_Again,eCR_Error};

	template<typename ImpClass>
	class TBaseMsgHandler
		:public CMsgMallocObject
	{
	public:
		virtual ~TBaseMsgHandler() {}
		virtual ECallResult Call(ImpClass* pDispatcher,const void* pData,uint32 uSize,void* pParam,uint32& uCmdSize)=0;
	};

	template<typename ImpClass,typename MsgClass>
	class TMsgHandler
		:public TBaseMsgHandler<ImpClass>
	{
	public:

		typedef void (ImpClass::*HandlerFun_t)(const MsgClass* pData,void* pParam);

		TMsgHandler(HandlerFun_t pFun);

		ECallResult Call(ImpClass* pDispatcher,const void* pData,uint32 uSize,void* pParam,uint32& uCmdSize);

	protected:

		HandlerFun_t GetHandlerFun()const;
	private:
		HandlerFun_t m_pHandlerFun;
	};
}

