#pragma once
#include "CTrMirrorBuffer.h"

namespace sqr
{
	/*
		在设计场景四叉树的时候，我们把所有的四叉树对象用一个triple buffer，同时该
		buffer也管理四叉树这个tree，为什么要这么做，是因为对于场景四叉树来说，
		四叉树对象和四叉树是一体的，如果采用两个buffer分别管理，那么会有如下问题

		1，四叉树创建
		2，四叉树对象创建  

		因为一个buffer对应的是一个patch，如果我们先上的四叉树buffer的patch，然后在上
		四叉树对象的patch，对于上面的创建没有什么问题，但是如果如下

		1，四叉树对象删除
		2，四叉树删除

		那么就会造成上patch的时候是四叉树删除，然后在四叉树对象删除，这可能会有问题
	*/

	class CMirrorBuffer;

	class CTrMirrorQuadBuffer : public CTrMirrorBuffer
	{
	public:
		static CTrMirrorBuffer* CreateBufferMgr(uint32 uDataNum);
		void Release();

	private:
		CTrMirrorQuadBuffer(uint32 uDataSize, uint32 uDataNum);
		~CTrMirrorQuadBuffer();
	};

}
