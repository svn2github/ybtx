// --------------------------------------------------------
// 名称：特效辅助容器类
// 说明：对特效框架的扩充，可放置多种数据，供特效使用
// --------------------------------------------------------

#pragma once
//#include "TSingleton.h"
#include "CVector3.h"


class ILogicHandler// : public Singleton<ILogicHandler>
{
public:
	// 用此函数，可使特效理解地表
	virtual float GetTerrainHeight( float xGround, float yGround )	= 0;
	/// 格三角形排列是反的还是正的
	virtual bool IsBackGrid(const sqr::CVector3f& pos) const = 0;
};