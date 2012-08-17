#pragma once
#include "BaseDefs.h"

// ---- 接口定义 ------------------------------------------------------------------------------

	class IIndexGenerator
	{
	public:
		struct IndexStruct
		{
			uint32		x;
			uint32		y;
			string		name;
		};

	public:
		// ---- 传入区域宽度和高度，以此来决定切分索引数目
		virtual void	SetRaginInfo( uint32 uWidth, uint32 uHeight ) = 0;

		// ---- 按照切割规则逐次返回切割坐标以及切分结果图片名称
		//		当所有索引都已生成完毕，则返回false
		virtual bool	GetNextIndex( IndexStruct *sIndex ) = 0;

		// ---- 释放生成器
		virtual void	Release() = 0;
	};