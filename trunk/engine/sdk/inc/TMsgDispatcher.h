#pragma once

#include "Base.h"

namespace sqr
{
	template<typename ImpClass>
	class TBaseMsgHandler;

	template<typename ImpClass>
	class TMsgHandlerContainer;

	enum EDispatchResult
	{
		eSuccess = 0,
		eCanceled,
		eCallAgain,
		eHandlerOver,
		eHandlerNotFound,
		eCallError
	};


	template<typename ImpClass,typename IdType,uint32 uId=0>
	class TBaseMsgDispatcher
	{
	private:
		static TMsgHandlerContainer<ImpClass>& GetHandlers();
		static TMsgHandlerContainer<ImpClass>*& IntGetHandlers();

	public:
		typedef TBaseMsgHandler<ImpClass> BaseHandler_t;

		static void InitMsgHandlerContainer();
		static void UninitMsgHandlerContainer();
		static uint32 GetHandlerNumber();
		static bool SetHandlerNumber(uint32 uSize);

		static bool SetHandlerNumber(uint32 uSize,int32 nIdOffset);
		static BaseHandler_t* GetHandler( uint32 uIndex );

		template<typename MsgClass>
		static void SetHandler(  void (ImpClass::*HandlerFun) (const MsgClass*,void*) );

		EDispatchResult Dispatch(const void* pData,uint32 uSize,void* pParam,uint32& uProcessedSize, bool bDispatchOnce = false);

		void LogMsgRecvTrafficForCpp(const char* szProtlName, uint16 uMsgID, uint32 uMsgLen);

#define FindBugOfTBaseMsgDispatcher
#ifdef FindBugOfTBaseMsgDispatcher
		//为了查无效内存bug临时加的代码
		bool	m_bFlag1;
		bool	m_bFlag2;
		bool	m_bFlag3;
		bool	m_bFlag4;
		bool	m_bFlag5;
		bool	m_bFlag6;
#endif
	};
}
