#pragma once
#include "CShadowFilter.h"
#include "../Graphic/CShadowClipper.h"

/**
	@class CLightMapFilter

	光照贴图过滤器.

	地编用于生成lightmap.
	阴影包围盒只需要地形的, 物体的都忽略.
	地形与物体的阴影分开处理, 因为地形的自阴影需要进行模糊处理. 
 */
namespace sqr
{
class CCamera;
SmartPointer(CRenderTexture);
SmartPointer(CShadowMatrixBuilder);
class CLightMapFilter :	public CShadowFilter
{
public:
	CLightMapFilter(CRenderGroup* pRenderGroup);
	virtual ~CLightMapFilter();
	void Begin();
	void Render(void);
	void End();
	EShadowType GetShadowType() const;
	void ResetFilterQueue(RenderQueueFlag& fQueueFlag);
	void SetShadowParams(STATIC_RS& RS);
	void UpdateShadowParams();;
	void UpdateRenderStyle(const RenderParam& RP);
	void AddReceiverBox(const CAxisAlignedBox& box);
	void ResetBox();
	void SetZOffset(float f);

	void SetTerrainShadowOn(bool b);
	void SetObjectShadowOn(bool b);

	CRenderTexture* GetDepthTarget() const;
	const CMatrix& GetShadowMatrix() const;

protected:
	void RenderTerrain();
	bool isTerrainShadowOn;
	bool isObjectShadowOn;
	float zOffset;	
	
public:
	CShadowClipper			m_BoxClipper;
protected:

	CRenderTexturePtr		shadowTexture;
	CShadowMatrixBuilderPtr matrixBuilder;
protected:
	static const DWORD st_LightMapVSID;
	static const DWORD st_LightMapPSID;

public:
	void SetShadowTexOut(CRenderTexturePtr shadowTex);
	void SetMatrixBuildOut(CShadowMatrixBuilderPtr shadowMatrixBuild);
	bool EnableRenderOut();
	bool DisableRenderOut();
protected:
	bool RenderOut;
	CRenderTexturePtr		shadowTextureOut;
	CShadowMatrixBuilderPtr matrixBuilderOut;

	void BeginEx();
	void EndEx();
	void UpdateShadowParamsEx();

};

//------------------------------------------------------------------------------
inline CRenderTexture*
CLightMapFilter::GetDepthTarget() const
{
	return this->shadowTexture.get();
}

//------------------------------------------------------------------------------
inline EShadowType
CLightMapFilter::GetShadowType() const
{
	return ST_TOOLEX_SHADOW;
}

//------------------------------------------------------------------------------
inline void
CLightMapFilter::SetZOffset( float f )
{
	this->zOffset = f;
}

//------------------------------------------------------------------------------
inline void
CLightMapFilter::SetTerrainShadowOn( bool b )
{
	this->isTerrainShadowOn = b;
}

//------------------------------------------------------------------------------
inline void
CLightMapFilter::SetObjectShadowOn( bool b )
{
	this->isObjectShadowOn = b;
}

//------------------------------------------------------------------------------
inline void
CLightMapFilter::ResetBox()
{
	m_BoxClipper.Reset();
}

}// namespace sqr
