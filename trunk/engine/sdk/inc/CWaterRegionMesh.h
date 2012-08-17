#pragma once
#include "CRefObject.h"
#include "CRenderOperation.h"
#include "CWaterRegionMesh.h"
#include "CHardwareBufferMgr.h"
#include "CSceneMallocObject.h"


/**
	@class CWaterRegionMesh

	一块地图区域大小的水面.
	VertexBuffer是固定大小的, 绘制数量由numGrids决定.
	IndexBuffer对于所有的Region都是一样的, 所以共用一个静态对象实例.
*/
namespace sqr
{
struct VerColorTex;
SmartPointer(CWaterRegionMesh);
class CWater;
class CWaterRegionMesh : public CSceneMallocObject
{
	friend class CWaterSurface;
public:
	CWaterRegionMesh();
	virtual ~CWaterRegionMesh();

	static void InitIndexBuffer( void );

public:
	/// 返回有水的格子数
	uint GetNumGrids() const;
	/// 设置有水的格子数
	void InitRegion(CVertexHardwareBuffer* pVertex , uint num);

	///// 锁定缓存, 写入顶点数据.
	//size_t Lock(VerColorTex** pVer);
	///// 解锁缓存
	//void Unlock();
	

	/// 返回包围盒
	const CAxisAlignedBox& GetBoundingBox() const;
	/// 设置包围盒
	void SetBoundingBox(const CAxisAlignedBox& box);

	/// 绘制网格, 具体的表现形式决定于WaterEffect
	void Render();

private:
	CRenderOperation renderOperation;
	CAxisAlignedBox aabb;
	CVertexHardwareBuffer*	m_pVertexBuffer;
	uint numGrids;

private:
	
	static CIndexHardwareBuffer* st_IndexBuffer;

public://临时结构
	typedef	SVector<CWater*>	WaterPool;
	WaterPool		m_vecWater;
};

//------------------------------------------------------------------------------
inline uint
CWaterRegionMesh::GetNumGrids() const
{
	return this->numGrids;
}

//------------------------------------------------------------------------------
inline const CAxisAlignedBox&
CWaterRegionMesh::GetBoundingBox() const
{
	return this->aabb;
}

//------------------------------------------------------------------------------
inline void
CWaterRegionMesh::SetBoundingBox( const CAxisAlignedBox& box )
{
	this->aabb = box;
}

}// namespace sqr
