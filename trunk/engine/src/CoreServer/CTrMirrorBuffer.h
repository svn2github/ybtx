#pragma once

/*
	三缓冲管理器
	三缓冲为 read buffer，write buffer，和idle buffer（下面全部以rb，wb和ib代替）

	
	工作原理：
	1，	首先初始化rb,wb和ib，将其对应的版本号设置为0
	2，	每次数据更新线程开始，将wb的版本加1，当有数据更新的时候，操作wb，并将此次更新
		的patch保存。

		譬如：	
		wb原始版本为0，（即wb0，后面都用wbn表示，n表示版本号）
		那么这一次操作之后，变成wb1，保存的更新为Patch1

	3，	当一次数据更新完成之后，交换ib和wb，这时候ib里面的数据就是最新的数据，同时将ib
		的版本号设置为wb更新之后的版本号，wb里面这时候的数据就是ib的数据，将交换前ib的
		版本和最新版本对比，对交换后的wb打上Patch。
		
		譬如：
		wb1，ib0，交换之后，ib1，wb1，因为先前ib的版本为0，而wb的版本为1
		则对wb1打上Patch1补丁，
		依次类推，如果先前ib为1，wb为10，则对wb10打上Patch2 -- Patch10补丁

	4，	当需要读取数据的时候，如果ib的版本比rb要高，则将rb和ib进行交换，同时将rb的版本设置成ib的，ib的版本设置成rb的

		譬如：
		rb0，ib1，交换之后，rb1，ib0
		
		如果ib的版本跟rb一样，证明wb还没有跟ib交换，我们仍然使用rb的数据。

		在继续2步骤
		需要注意的是，如果wb更新之后交换ib的时候，如果这时候的ib是rb交换的，那么当打完对应的
		的补丁之后，我们可以删除新rb版本之前对应的Patchs了。

		譬如：
		交换之前 rb5，ib1，wb6，交换之后，rb5，ib6，wb6，
		对wb6打上Patch2 -- Patch6补丁，
		因为此时最旧的版本是5，所以可以删除Patch6之前所有的Patch了

	5，	当在rb线程创建一个对象的时候，我们也会产生一个patch，这个patch里面包含了这帧里面
		所有新创建的对象，当rb和ib交换的时候，首先看新的rb对应的slot是不是有数据，有数据，表明
		新创建的对象已经通过wb创建并交换到ib上面，这时候我们就会删除patch里面对应这个数据，
		如果slot没有数据，我们就将patch里面对应slot的数据打入新的rb中
		当patch里面没有任何数据之后，我们删除这个patch
	
	//////////////////////////////////////////////////////////////////////////////////////

		因为根据我们程序流程，对象都是在read thread创建的
		
		对于read thread
		1，首先得到read buffer
		2，在read buffer上面创建一个对象，创建的时候read buffer会提供唯一的一个slot给这个对象
		   这样对象就能通过这个slot访问read buffer里面对应的最新数据了
		   同时每创建一个对象，产生一个创建command，加入一个patch里面
	    3，当rb和ib交换的时候，将patch打上新的rb上面，因为新的rb是没有新创建的对象的

	    对于write thread
		1，首先得到write buffer
		2，在write buffer上面创建一个对象，并且使用的是read buffer里面创建的slot，因为我们的所有对象都是在
		   read buffer里面创建的，所以这个slot绝对是唯一确定的
	    3，如果write thread上面有数据更新，产生一个更新command，参数是更新对象的slot，更新对象的数据
		   加入一个更新command列表里面，这个list command就是pPatch
	    4，当wb和ib交换的之后，找到需要处理的pPatch，如Patch1 -- Patch8，依次调用pPatch里面的command
		   这样就打上补丁了
					
*/

#include "ThreadTypes.h"
#include "CMirrorMallocObject.h"

namespace sqr
{

	class CMirrorBuffer;
	
	class CMirrorWBPatchMgr;
	class CMirrorRBPatchMgr;

	class CMirrorPatch;
	class CMirrorWBPatch;
	class CMirrorRBPatch;
	class CMirrorPatchCmd;
	
	class CTrDiscardedCmdMgr;

	class CTrMirrorBuffer : public CMirrorMallocObject
	{
	public:
		friend class CTestSuiteTripleBuffer;

		CMirrorBuffer* GetWriteBuffer();
		CMirrorBuffer* GetReadBuffer();

		CMirrorPatch*		GetCurWBPatch();
		CMirrorPatch*		GetCurRBPatch();
		
		void AddDiscardedCmd(uint32 uSlot);
		bool IsDiscardedCmd(uint32 uSlot);

		virtual void Release();

		void OnRBUpdate();
		void OnWBUpdate();

	protected:
		CTrMirrorBuffer(uint32 uDataSize, uint32 uDataNum);
		virtual ~CTrMirrorBuffer();

		void CreateTripleBuffer(uint32 uDataSize, uint32 uDataNum);
		void DeleteTripleBuffer();

		//swap the write and idle buffer
		void SwapWIBuffer();  

		//swap the read and idle buffer
		void SwapRIBuffer();

	protected:
		CMirrorBuffer* m_pWriteBuffer;
		CMirrorBuffer* m_pReadBuffer;
		CMirrorBuffer* m_pIdleBuffer;

		
		CMirrorWBPatchMgr*	m_pWBPatchMgr;
		CMirrorRBPatchMgr*	m_pRBPatchMgr;

		CTrDiscardedCmdMgr*	m_pDiscardedCmdMgr;

		HLOCK	m_Lock;
	};

}
