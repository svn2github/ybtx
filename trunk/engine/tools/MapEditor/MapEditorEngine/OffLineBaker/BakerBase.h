#pragma once

/***离线渲染基类***/

#include "CRefObject.h"
#include "CRenderTexture.h"

namespace sqr
{
	class CRenderTexture;
	SmartPointer(CBakerBase);
	SmartPointer(CRenderTexture);
	class CBakerBase : public CRefObject
	{
	public:
		CBakerBase(void);
		~CBakerBase(void);
		
		//初始化
		virtual void Setup(){};
		//释放资源
		virtual void Discard(){};
		//方法准备
		virtual void OnPrepare(){};
		//准备
		virtual void TilePrepare(index_t x,index_t z){};
		//过程
		virtual void OnProcess(index_t x,index_t z){};
	};

}
