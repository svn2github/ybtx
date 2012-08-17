#pragma once
//------------------------------------------------------------------------------
/**
    @struct ClipStatus 

	裁剪状态.
	CLR风格的枚举定义, 防止全局名字冲突.

    (C) 2009 ThreeOGCMan
*/

//------------------------------------------------------------------------------
namespace sqr
{

struct ClipStatus
{
	enum Type
	{
		Outside,	///< 外部
		Intersect,	///< 相交
		Inside,		///< 内部

		Invalid
	};
};

}// namespace sqr