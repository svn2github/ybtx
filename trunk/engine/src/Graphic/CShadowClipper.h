#pragma once
#include "TSingleton.h"
#include "CAxisAlignedBox.h"

/**
	@class CShadowClipper

	投影物体的裁剪.

	因为目前缺少LightEntity这么个东西, 先写个单件凑合着用.
	功能是计算所有投影物体的包围盒,和接收阴影区域的包围盒
 */

namespace sqr
{

class CShadowClipper
{
public:
	CShadowClipper();
	/// 加入一个AABB
	void AddCasterBox(const CMatrix& transform, const CAxisAlignedBox& box);
	void AddReceiverBox(const CAxisAlignedBox& box);
	/// 返回AABB并集
	const CAxisAlignedBox& GetCastersAABB(const CMatrix& transform);
	const CAxisAlignedBox& GetCastersAABBWithProj(const CMatrix& transform);
	const CAxisAlignedBox& GetReceiverAABB(const CMatrix& transform);

	/// 清空AABB, 重新计算
	void Reset();
private:
	friend class CShadowMapFilter;
	CAxisAlignedBox			m_CasterBoxes;
	CAxisAlignedBox			m_ReceiverBoxes;
};

//------------------------------------------------------------------------------
inline void
CShadowClipper::Reset()
{
	m_CasterBoxes.setNull();
	m_ReceiverBoxes.setNull();
}

}// namespace sqr
