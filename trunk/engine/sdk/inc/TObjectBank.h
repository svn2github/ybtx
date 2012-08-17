#pragma once
#include <deque>

namespace sqr
{

	template<typename ObjType, template<typename>class TAlloc >
	class TObjectBank
	{
	public:
		TObjectBank(uint32 uSize=64);
		TObjectBank(uint32 uSize,bool bPrepare);
		~TObjectBank();

		void Push(ObjType*);
		ObjType* Pop();

		//如果对象池已经空，则new一个出来.
		//这个对象有无参数的构造函数

		//不要试图给这个类增加支持多种参数构造函数的功能，因为：满足这种需求，往往意味着使用ObjectBank并不能提升效率。
		//使用多参数的构造函数说明这个对象无法通过进行少量数据初始化完成
		ObjType* PopOrNew();
	private:
		typedef TAlloc<ObjType*> DeqAlloc_t;
		deque<ObjType*,DeqAlloc_t >	m_deqObj;

		uint32		m_uSize;
	};
}

