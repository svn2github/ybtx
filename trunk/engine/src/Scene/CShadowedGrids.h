#pragma once
/**
	@class CShadowedGrids

	 阴影遮盖的格子.
	 每个格子保存一个灰度值

	 (C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"
#include "CColor.h"
#include "CSceneMallocObject.h"

namespace sqr
{

SmartPointer(CShadowedGrids);
class CShadowedGrids 
	: public CRefObject
	, public CSceneMallocObject
{
public:
	CShadowedGrids();
	virtual ~CShadowedGrids();

	/// 从map/[MAPNAME]/lightmap/下加载文件数据
	void Load(const string& path, uint w, uint h);

	/// 根据物体计算其是否处于阴影中, 并返回环境颜色值
	CColor GetShadowedColor(uint xGrid, uint yGrid);
	bool   ShadowedColorIsEmpty() const;

private:
	SVector<BYTE> lowGrids;

	uint widthInGrid;
	uint heightInGrid;
};

}// namespace sqr