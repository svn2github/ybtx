#pragma once
#include "CRefObject.h"
#include "CWaterSurface.h"
#include "CSceneMallocObject.h"
/**
	@class CWaterSurface

	水面.
	按区域进行可见性裁剪.
	可以改变WaterEffect来切换水面效果(策略模式).

	@note 同一效果的水面只有一个实例.
 */
namespace sqr
{
SmartPointer(CWaterEffect);
class CWaterRegionMesh;
class CCamera;
class CWater;
class ITexture;

class CWaterSurface 
	: public CRefObject
	, public CSceneMallocObject
{
public:
	CWaterSurface();
	virtual ~CWaterSurface();

	/// 返回指定区域的水面网格, 不存在返回空指针
	//CWaterRegionMesh* GetRegionMesh(uint32 regionId) const;
	/// 创建一个新的水面区域
	void AddRegionMesh(CWaterRegionMesh* pRegion);
	/// 删除一个新的水面区域
	void RemoveRegionMesh(CWaterRegionMesh* pRegion);
	/// 取得水面区域数
	size_t GetRegionMeshNum();
	/// 是否包含指定区域
	bool HasRegionMesh(CWaterRegionMesh* pRegion);

	/// 是否隐藏
	bool IsHide() const;
	/// 设置是否隐藏
	void SetHide(bool b);

	/// 返回水面效果
	const CWaterEffectPtr& GetWaterEffect() const;
	/// 设置水面效果
	void SetWaterEffect(const CWaterEffectPtr& effect);

	/// 绘制水面
	void Render(CCamera* pCamera);
	void RenderWave(CCamera* pCamera ,ITexture* pTexture, uint32 uCurTime);
	CWater* GetWater(uint32 OrgId , int32 nGrid);
private:
	typedef SSet<CWaterRegionMesh*> RegionMeshSet;

	bool isHide;
	RegionMeshSet regionMeshSet;
	CWaterEffectPtr waterEffect;
};

//------------------------------------------------------------------------------
inline const CWaterEffectPtr&
CWaterSurface::GetWaterEffect() const
{
	return this->waterEffect;
}

//------------------------------------------------------------------------------
inline void
CWaterSurface::SetWaterEffect( const CWaterEffectPtr& effect )
{
	this->waterEffect = effect;
}

//------------------------------------------------------------------------------
inline size_t
CWaterSurface::GetRegionMeshNum()
{
	return this->regionMeshSet.size();
}

//------------------------------------------------------------------------------
inline bool
CWaterSurface::HasRegionMesh( CWaterRegionMesh* pRegion )
{
	return (this->regionMeshSet.end() != this->regionMeshSet.find(pRegion));
}

//------------------------------------------------------------------------------
inline bool
CWaterSurface::IsHide() const
{
	return this->isHide;
}

//------------------------------------------------------------------------------
inline void
CWaterSurface::SetHide( bool b )
{
	this->isHide = b;
}

}// namespace sqr
