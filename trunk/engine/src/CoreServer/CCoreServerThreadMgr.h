#pragma once
#include "ThreadTypes.h"
#include "CAddress.h"

#include "TTrThreadMsgBuffer.h"

namespace sqr
{
	class CError;

	class CCoreServerJob;
	class CCoreServerResult;
	class CCoreSceneServer;	
	class CPixelPath;

	template<class T> struct TPos;
	typedef TPos<int32> CPos;
	typedef TPos<float> CFPos;
	class IMulticast;
	class CSyncMetaSceneServer;

	class CTick;

	class CCoreServerThreadMgr;
	class CCoreServerThread;

	class CCoreServerThreadTraits
	{
	public:
		typedef CCoreServerJob			LeftMsgType;
		typedef CCoreServerResult		RightMsgType;
	};

	class CCoreServerThreadMgr
		: public TTrThreadMsgBuffer<CCoreServerThreadTraits>
	{
	public:
		typedef TTrThreadMsgBuffer<CCoreServerThreadTraits>	Parent_t;
		friend class TTrThreadMsgBuffer<CCoreServerThreadTraits>;

		friend class CAppServer;
		friend class CSyncAppServer;
		friend class CCoreServerThread;
		friend class CCoreServerJob;
		friend class CCoreServerResult;
		friend class CAddWIBufferSwapedResultTick;

		static void Init();
		static void Unit();
		static CCoreServerThreadMgr*& Inst();

	private:
		void OnWakeRight();
		bool FlushRightMsg();
		void IntOnRightBufferSwaped();

		void OnLeftException(CError& error){}
		void OnRightException(CError& error){}

	private:
		CCoreServerThreadMgr();
		~CCoreServerThreadMgr();

	private:

		static CCoreServerThreadMgr*	ms_pInst;
		CCoreServerThread*				m_pThread;
		CTick*							m_pWISwapedTick;		
	};
}
