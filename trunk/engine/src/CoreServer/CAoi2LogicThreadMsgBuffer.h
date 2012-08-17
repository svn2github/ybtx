#pragma once

#include "TTrThreadMsgBuffer.h"
#include "CLogicThreadJob.h"
#include "CLogicThreadResult.h"

namespace sqr
{
	class CLogicThreadJob;
	class CLogicThreadResult;

	class CAoi2LogicThreadTraits
	{
	public:
		typedef CLogicThreadJob		LeftMsgType;
		typedef CLogicThreadResult	RightMsgType;
	};

	class CAoi2LogicThreadMsgBuffer
		:public TTrThreadMsgBuffer<CAoi2LogicThreadTraits>
	{
		typedef TTrThreadMsgBuffer<CAoi2LogicThreadTraits> Parent_t;
		friend class TTrThreadMsgBuffer<CAoi2LogicThreadTraits>;

		friend class CLogicThreadJob;
		friend class CLogicThreadResult;

	public:

		CAoi2LogicThreadMsgBuffer();
		virtual ~CAoi2LogicThreadMsgBuffer();

		static void Init();
		static void Unit();
		static CAoi2LogicThreadMsgBuffer*& Inst();

	private:
		virtual void OnWakeRight(){}
		void OnLeftException(CError& error){}
		void OnRightException(CError& error){}

	};

}
