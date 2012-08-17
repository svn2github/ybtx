#pragma once

namespace sqr
{

#define IDSTACK_INVALID_INDEX -1

	/*
		ID Stack管理器
		用来管理可使用的id
	*/

	//IDContainer只能使用vector或者deque实现，默认是vector

	template<typename IDType, class IDContainer = vector<IDType> >
	class TIDStack
	{
	public:
		TIDStack();
		TIDStack(size_t stCount);
		~TIDStack();

		//获取一个空闲id
		IDType WithdrawIdleID();
		//归还已经使用的id
		void RestoreUsedID(IDType ID);
		//是否为空闲id
		bool IsIdleID(IDType ID);
		//从空闲id中移除指定id
		void RemoveIdleID(IDType ID);
		//将空闲的id最大索引扩展到MaxID
		void ExpandIdleID(IDType MaxID);
		//查看ID是否是stack管理的
		bool IsStackID(IDType ID);
		//查看当前还有多少剩余idle id
		size_t GetIdleIDSize();
		//得到此时stack管理的ID数量
		IDType GetIDSize(); 
		//判断是否后为空
		bool Empty();

	private:
		IDContainer	m_IDStack;  //
		IDContainer m_IDIndex;  //指明该ID在stack上面索引
		size_t	m_CurIdlePos;

	};
}
