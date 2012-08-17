#pragma once

/************************************************************************/
/*
该类主要是进行压力测试时候使用，对于以前的CConnClient，由于只能有一个
连接，所以我们得进行一些相关修改，使其同时能有多个连接连上服务器。
我们还要处理所有的网络协议，但是考虑到性能问题，一些网络协议我们只会
接受不会处理。
*/
/************************************************************************/
#include "TMsgDispatcher.h"
//#include "PrtlGas2GacObjectDef.h"
//#include "PrtlGac2GasGenericDef.h"
//#include "PrtlGas2GacGenericDef.h"

namespace sqr
{

	template<typename TraitsConcreateConn>
	class TClientMsgRegister:
		public TBaseMsgDispatcher<typename TraitsConcreateConn::ConcreteConn_t,uint8>
	{
	public:
		typedef typename TraitsConcreateConn::ConcreteConn_t ImpConn_t;
	public:
		static void RegisterMsgHandler();
	private:

		//连接自己处理的消息
		template<typename CmdClass>static void IntRegisterServerCommand();

		//注册场景相关消息（创建Follower对象等）
		template<typename CmdClass>static void IntRegisterSceneCommand();

		//场景管理器处理的消息
		template<typename CmdClass>static void IntRegisterSceneMgrCommand();

		//注册Follower对象快速延时消息
		template<typename CmdClass>static void IntRegisterFollowerFastDelayCommand();

		//注册Follower对象慢速延时消息
		template<typename CmdClass>static void IntRegisterFollowerSlowDelayCommand();

		//注册Follower对象消息
		template<typename CmdClass>static void IntRegisterFollowerCommand();

		//注册Director对象消息
		template<typename CmdClass>static void IntRegisterDirectorCommand();


		//public:
		//下面的交给子类继承
		//连接自己处理的消息
		template<typename CmdClass>static void RegisterServerCommand();

		//注册场景相关消息（创建Follower对象等）
		template<typename CmdClass>static void RegisterSceneCommand();

		//场景管理器处理的消息
		template<typename CmdClass>static void RegisterSceneMgrCommand();

		//注册Follower对象快速延时消息
		template<typename CmdClass>static void RegisterFollowerFastDelayCommand();

		//注册Follower对象慢速延时消息
		template<typename CmdClass>static void RegisterFollowerSlowDelayCommand();

		//注册Follower对象消息
		template<typename CmdClass>static void RegisterFollowerCommand();

		//注册Director对象消息
		template<typename CmdClass>static void RegisterDirectorCommand();
	};
}
