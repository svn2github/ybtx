#pragma once

namespace sqr
{

	class CCoreServerJob 
	{
	public:
		static void* operator new(size_t stSize);
		static void operator delete(void* pMem){};
		
		//将这个job加入job队列里面，本来是准备直接在CCoreServerJob的构造函数里完成这个的
		//但是可能出现如下情况，在父类构造完毕加入队列之后，子类发生异常，这时候队列里面的数据
		//就是一个错误的值，直接会导致后续处理这个队列的时候出错，所以我们还是让子类自己决定何时加入
		void Add();

		virtual ~CCoreServerJob(){}

		static void* AllocMem(size_t stSize);
		static char* CloneString(const char* szStr);
		static char* CloneString(const std::string& szStr);
		static void* CloneData(const void* pData, size_t stSize);
	};

}
