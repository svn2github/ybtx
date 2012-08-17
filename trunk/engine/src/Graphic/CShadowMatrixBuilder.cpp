//------------------------------------------------------------------------------
//  CShadowMatrixBuilder.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "CShadowMatrixBuilder.h"
#include "CShadowClipper.h"
#include "CCamera.h"
#include "CFrustum.h"
#include "CShadowClipper.h"
#include "CRenderPipeline.h"

namespace sqr
{

//------------------------------------------------------------------------------
CShadowMatrixBuilder::CShadowMatrixBuilder()
		: m_Camera(NULL)
		, m_ShadowMapSize(512)
{
	// empty
}

//------------------------------------------------------------------------------
CShadowMatrixBuilder::~CShadowMatrixBuilder()
{
	// empty
}

//------------------------------------------------------------------------------
void
CTsmMatrixBuilder::BuildMatrix(CShadowClipper* clipper)
{
	//这个函数优化空间很大，把矩阵下面手动算完，推导出公式最后放到程序中写成一个简单结果就行
	//目前这种没有必要的矩阵乘法太多，有时间建议优化下
	Ast(NULL != m_Camera);
	Ast(NULL != clipper);

	m_ViewMatrix = m_Camera->getViewMatrix();
	//  get the near and the far plane (points) in eye space.
	CVector3f frustumPnts[CFrustum::FrustumCornerNum];
	CFrustum eyeFrustum(m_Camera->getProjectionMatrix());
	for (index_t i = 0; i < 4; ++i)
	{
		frustumPnts[i]   = *(CVector3f*)&(eyeFrustum.GetCorner((i<<1)));       // far plane
		frustumPnts[i+4] = *(CVector3f*)&(eyeFrustum.GetCorner((i<<1) | 0x1)); // near plane
	}

	//   we need to transform the eye into the light's post-projective space.
	//   however, the sun is a directional light, so we first need to find an appropriate
	//   rotate/translate matrix, before constructing an ortho projection.
	//   this matrix is a variant of "light space" from LSPSMs, with the Y and Z axes permuted

	CVector3f leftVector, upVector, viewVector;
	const CVector3f eyeVector( 0.f, 0.f, -1.f );  //  eye is always -Z in eye space

	// lightDir is defined in eye space, so xform it
	upVector = m_LightDir;
	upVector.MultiplyNormal(m_ViewMatrix);
	leftVector = upVector.Cross(eyeVector);
	leftVector.Normalize();
	viewVector = upVector.Cross(leftVector);

	CMatrix lightSpaceBasis;
	lightSpaceBasis._11 = leftVector.x,lightSpaceBasis._12 = viewVector.x,lightSpaceBasis._13 = -upVector.x,lightSpaceBasis._14 = 0.f;
	lightSpaceBasis._21 = leftVector.y,lightSpaceBasis._22 = viewVector.y,lightSpaceBasis._23 = -upVector.y,lightSpaceBasis._24 = 0.f;
	lightSpaceBasis._31 = leftVector.z,lightSpaceBasis._32 = viewVector.z,lightSpaceBasis._33 = -upVector.z,lightSpaceBasis._34 = 0.f;
	lightSpaceBasis._41 = 0.f,lightSpaceBasis._42 = 0.f,lightSpaceBasis._43 = 0.f,lightSpaceBasis._44 = 1.f;

	//  rotate the view frustum into light space
	for ( int i = 0; i< CFrustum::FrustumCornerNum; ++i )
		frustumPnts[i] *= lightSpaceBasis;

	//  build an off-center ortho projection that translates and scales the eye frustum's 3D AABB to the unit cube
	CAxisAlignedBox frustumBox;
	for (index_t i = 0; i < CFrustum::FrustumCornerNum; ++i)
		frustumBox.mergeBox(frustumPnts[i]);
	const CAxisAlignedBox& casterBox = clipper->GetCastersAABB(lightSpaceBasis);

	float min_z = min(frustumBox.getMinimum().z, casterBox.getMinimum().z);
	float max_z = max(frustumBox.getMaximum().z, casterBox.getMaximum().z);

	// virtually forward
	if ( min_z <= 0.f )
	{
		CMatrix lightSpaceTranslate;
		lightSpaceTranslate.SetTranslate( 0.f, 0.f, -min_z + 1.f );
		max_z = -min_z + max_z + 1.f;
		min_z = 1.f;
		lightSpaceBasis *= lightSpaceTranslate;

		for ( int i = 0; i< CFrustum::FrustumCornerNum; ++i )
			frustumPnts[i] *= lightSpaceTranslate;

		frustumBox.setNull();
		for (index_t i = 0; i < CFrustum::FrustumCornerNum; ++i)
			frustumBox.mergeBox(frustumPnts[i]);
	}

	// build orthogonal projection matrix
	CMatrix lightSpaceOrtho;
	lightSpaceOrtho.SetOrthoOffCenterLH(frustumBox.getMinimum().x,
	                                    frustumBox.getMaximum().x, frustumBox.getMinimum().y,
	                                    frustumBox.getMaximum().y, min_z, max_z );

	//  transform the view frustum by the new matrix
	for ( int i = 0; i< CFrustum::FrustumCornerNum; ++i )
		frustumPnts[i] *= lightSpaceOrtho;


	CVector2f centerPts[2];
	//  near plane
	centerPts[0].x = 0.25f * (frustumPnts[4].x + frustumPnts[5].x + frustumPnts[6].x + frustumPnts[7].x);
	centerPts[0].y = 0.25f * (frustumPnts[4].y + frustumPnts[5].y + frustumPnts[6].y + frustumPnts[7].y);
	//  far plane
	centerPts[1].x = 0.25f * (frustumPnts[0].x + frustumPnts[1].x + frustumPnts[2].x + frustumPnts[3].x);
	centerPts[1].y = 0.25f * (frustumPnts[0].y + frustumPnts[1].y + frustumPnts[2].y + frustumPnts[3].y);

	CVector2f centerOrig = (centerPts[0] + centerPts[1])*0.5f;

	CMatrix xlate_center(1.f,			0.f,			0.f, 0.f,
	                     0.f,			1.f,			0.f, 0.f,
	                     0.f,			0.f,			1.f, 0.f,
	                     -centerOrig.x,	-centerOrig.y,	0.f, 1.f );

	float half_center_len = CVector2f(centerPts[1] - centerOrig).Mag();
	float x_len = centerPts[1].x - centerOrig.x;
	float y_len = centerPts[1].y - centerOrig.y;

	float cos_theta = x_len / half_center_len;
	float sin_theta = y_len / half_center_len;

	CMatrix rot_center( cos_theta, -sin_theta, 0.f, 0.f,
	                    sin_theta, cos_theta, 0.f, 0.f,
	                    0.f,        0.f, 1.f, 0.f,
	                    0.f,        0.f, 0.f, 1.f );

	//  this matrix transforms the center line to y=0.
	//  since Top and Base are orthogonal to Center, we can skip computing the convex hull, and instead
	//  just find the view frustum X-axis extrema.  The most negative is Top, the most positive is Base
	//  Point Q (trapezoid projection point) will be a point on the y=0 line.
	m_ProjMatrix = xlate_center * rot_center;

	for ( int i = 0; i< CFrustum::FrustumCornerNum; ++i )
		frustumPnts[i] *= m_ProjMatrix;

	CAxisAlignedBox frustumAABB2D;
	for (index_t i = 0; i < CFrustum::FrustumCornerNum; ++i)
		frustumAABB2D.mergeBox(frustumPnts[i]);

	float x_scale = max( fabsf(frustumAABB2D.getMaximum().x), fabsf(frustumAABB2D.getMinimum().x) );
	float y_scale = max( fabsf(frustumAABB2D.getMaximum().y), fabsf(frustumAABB2D.getMinimum().y) );
	x_scale = 1.f/x_scale;
	y_scale = 1.f/y_scale;

	//  maximize the area occupied by the bounding box
	CMatrix scale_center(x_scale, 0.f, 0.f, 0.f,
	                     0.f, y_scale, 0.f, 0.f,
	                     0.f,     0.f, 1.f, 0.f,
	                     0.f,     0.f, 0.f, 1.f);
	m_ProjMatrix *= scale_center;

	//  scale the frustum AABB up by these amounts (keep all values in the same space)
	frustumAABB2D.scaleBox( CVector3f( x_scale, y_scale, 1.0f ) );

	//  compute eta.
	float lambda = frustumAABB2D.getMaximum().x - frustumAABB2D.getMinimum().x;
	float delta_proj = m_FocusRegion * lambda; //focusPt.x - frustumAABB2D.minPt.x;

	const float xi = -0.6f;  // 80% line

	float eta = (lambda*delta_proj*(1.f+xi)) / (lambda*(1.f-xi)-2.f*delta_proj);

	//  compute the projection point a distance eta from the top line.  this point is on the center line, y=0
	CVector2f projectionPtQ( frustumAABB2D.getMaximum().x + eta, 0.f );

	//  find the maximum slope from the projection point to any point in the frustum.  this will be the
	//  projection field-of-view
	float max_slope = -1e32f;
	float min_slope =  1e32f;

	for ( index_t i = 0; i < CFrustum::FrustumCornerNum; ++i )
	{
		CVector2f tmp( frustumPnts[i].x*x_scale, frustumPnts[i].y*y_scale );
		float x_dist = tmp.x - projectionPtQ.x;
		if ( !(gIsZero(tmp.y) || gIsZero(x_dist)))
		{
			max_slope = max(max_slope, tmp.y/x_dist);
			min_slope = min(min_slope, tmp.y/x_dist);
		}
	}

	float xn = eta;
	float xf = lambda + eta;

	CMatrix ptQ_xlate(-1.f, 0.f, 0.f, 0.f,
	                  0.f, 1.f, 0.f, 0.f,
	                  0.f, 0.f, 1.f, 0.f,
	                  projectionPtQ.x, 0.f, 0.f, 1.f );
	m_ProjMatrix *= ptQ_xlate;

	//  this shear balances the "trapezoid" around the y=0 axis (no change to the projection pt position)
	//  since we are redistributing the trapezoid, this affects the projection field of view (shear_amt)
	float shear_amt = (max_slope + fabsf(min_slope))*0.5f - max_slope;
	max_slope = max_slope + shear_amt;

	CMatrix trapezoid_shear( 1.f, shear_amt, 0.f, 0.f,
	                         0.f,       1.f, 0.f, 0.f,
	                         0.f,       0.f, 1.f, 0.f,
	                         0.f,       0.f, 0.f, 1.f );

	m_ProjMatrix *= trapezoid_shear;

	float z_aspect = (frustumBox.getMaximum().z-frustumBox.getMinimum().z) /
	                 (frustumAABB2D.getMaximum().y-frustumAABB2D.getMinimum().y);

	//  perform a 2DH projection to 'unsqueeze' the top line.
	CMatrix trapezoid_projection( xf/(xf-xn),	0.f,					  0.f, 1.f,
	                              0.f, 1.f/max_slope,					  0.f, 0.f,
	                              0.f,           0.f, 1.f/(z_aspect*max_slope), 0.f,
	                              -xn*xf/(xf-xn),0.f,					  0.f, 0.f );
	m_ProjMatrix *= trapezoid_projection;

	//  the x axis is compressed to [0..1] as a result of the projection, so expand it to [-1,1]
	CMatrix biasedScaleX( 2.f, 0.f, 0.f, 0.f,
	                      0.f, 1.f, 0.f, 0.f,
	                      0.f, 0.f, 1.f, 0.f,
	                      -1.f, 0.f, 0.f, 1.f );
	m_ProjMatrix *= biasedScaleX;
	m_ProjMatrix = lightSpaceOrtho * m_ProjMatrix;
	m_ProjMatrix = lightSpaceBasis * m_ProjMatrix;

	// now, focus on shadow receivers.
	m_ShadowMatrix = m_ViewMatrix * m_ProjMatrix;
	CAxisAlignedBox rcvrBox = clipper->GetCastersAABBWithProj(m_ShadowMatrix);
	rcvrBox.m_vMaximum.x = min( 1.f, rcvrBox.m_vMaximum.x );
	rcvrBox.m_vMinimum.x = max(-1.f, rcvrBox.m_vMinimum.x );
	rcvrBox.m_vMaximum.y = min( 1.f, rcvrBox.m_vMaximum.y );
	rcvrBox.m_vMinimum.y = max(-1.f, rcvrBox.m_vMinimum.y );
	float boxWidth  = rcvrBox.m_vMaximum.x - rcvrBox.m_vMinimum.x;
	float boxHeight = rcvrBox.m_vMaximum.y - rcvrBox.m_vMinimum.y;
	//  the receiver box is degenerate, this will generate specials (and there shouldn't be any shadows, anyway).
	if ( !(gIsZero(boxWidth) || gIsZero(boxHeight)) )
	{
		//  the divide by two's cancel out in the translation, but included for clarity
		float boxX = (rcvrBox.m_vMaximum.x+rcvrBox.m_vMinimum.x) / 2.f;
		float boxY = (rcvrBox.m_vMaximum.y+rcvrBox.m_vMinimum.y) / 2.f;
		CMatrix trapezoidUnitCube( 2.f/boxWidth,        0.f, 0.f, 0.f,
		                           0.f,       2.f/boxHeight, 0.f, 0.f,
		                           0.f,                 0.f, 1.f, 0.f,
		                           -2.f*boxX/boxWidth, -2.f*boxY/boxHeight, 0.f, 1.f );
		m_ProjMatrix *= trapezoidUnitCube;
	}
	m_ShadowMatrix = m_ViewMatrix * m_ProjMatrix;
	//set special texture matrix for shadow mapping
	float fOffsetX = 0.5f + (0.5f / (float)m_ShadowMapSize);
	float fOffsetY = 0.5f + (0.5f / (float)m_ShadowMapSize);
	CMatrix texCoordMatrix( 0.5f,     0.0f,     0.0f, 0.0f,
	                        0.0f,    -0.5f,     0.0f, 0.0f,
	                        0.0f,     0.0f,     1.0f, 0.0f,
	                        fOffsetX, fOffsetY, 0.0f, 1.0f );
	m_ShadowMatrix *= texCoordMatrix;
}

//------------------------------------------------------------------------------
CTsmMatrixBuilder::CTsmMatrixBuilder()
		: m_FocusRegion(0.5f)
{
	// empty
}

//------------------------------------------------------------------------------
void
COrthoMatrixBuilder::BuildMatrix( CShadowClipper* clipper )
{
	const CMatrix& worldView = m_Camera->getViewMatrix();
	const CVector3f zAxis(0.f, 0.f, 1.f);
	const CVector3f yAxis(0.f, 1.f, 0.f);
	CVector3f eyeLightDir(m_LightDir);
	eyeLightDir.MultiplyNormal(worldView);

	CAxisAlignedBox casterAABB = clipper->GetCastersAABB(worldView);
	
	CVector3f frustumCenter = casterAABB.getCenter();
	float t;
	casterAABB.intersects( &t, frustumCenter, eyeLightDir );

	CVector3f lightPt = frustumCenter + 2.0f * t * eyeLightDir;
	CVector3f axis;

	if ( fabsf(eyeLightDir.Dot(yAxis))>0.99f )
		axis = zAxis;
	else
		axis = yAxis;
	m_ViewMatrix.SetLookAtLH( lightPt, frustumCenter, axis );
	
	casterAABB.transformBox(m_ViewMatrix);

	//  use a small fudge factor for the near plane, to avoid some minor clipping artifacts
	m_ProjMatrix.SetOrthoOffCenterLH(casterAABB.getMinimum().x, casterAABB.getMaximum().x,
	                                 casterAABB.getMinimum().y, casterAABB.getMaximum().y,
	                                 casterAABB.getMinimum().z, casterAABB.getMaximum().z);

	m_ViewMatrix =  worldView * m_ViewMatrix;
	m_ShadowMatrix = m_ViewMatrix * m_ProjMatrix;
	//set special texture matrix for shadow mapping
	float fOffsetX = 0.5f + (0.5f / (float)m_ShadowMapSize);
	float fOffsetY = 0.5f + (0.5f / (float)m_ShadowMapSize);
	CMatrix texCoordMatrix( 0.5f,     0.0f,     0.0f, 0.0f,
	                        0.0f,    -0.5f,     0.0f, 0.0f,
	                        0.0f,     0.0f,     1.0f, 0.0f,
	                        fOffsetX, fOffsetY, 0.0f, 1.0f );
	m_ShadowMatrix *= texCoordMatrix;
}

//在世界空间中计算光影矩阵

float COrthoMatrixBuilder::BuildMatrixEx( CShadowClipper* clipper)
{
	//包围盒修正
    CMatrix Identity;
 	CAxisAlignedBox casterAABB = clipper->GetCastersAABB(Identity);

	const CVector3f zAxis(0.f, 0.f, 1.f);
	const CVector3f yAxis(0.f, 1.f, 0.f);
	CVector3f eyeLightDir(-m_LightDir);
	eyeLightDir.normalize();
	
	CVector3f frustumCenter = casterAABB.getCenter();

	CVector3f lightPt;
	lightPt.y=casterAABB.getMaximum().y;
	//if((lightPt.y-casterAABB.getMinimum().y)<= 64 )
	//{
	//	lightPt.y+=64;                 //拉高64米 避免包围盒上下面贴在一起
	//	casterAABB.m_vMaximum.y+=64;
	//}
	//lightPt.y=frustumCenter.y+lightPt.y;

	float rat=(frustumCenter.y-lightPt.y)/eyeLightDir.y;
	lightPt.x=frustumCenter.x-rat*eyeLightDir.x;
	lightPt.z=frustumCenter.z-rat*eyeLightDir.z;

	CVector3f axis;

	if ( fabsf(eyeLightDir.Dot(-yAxis))>0.99f )//天光
		axis = zAxis;
	else
		axis = yAxis;

	m_ViewMatrix.SetLookAtLH( lightPt, frustumCenter, axis );
	
	casterAABB.transformBox(m_ViewMatrix);

	//  use a small fudge factor for the near plane, to avoid some minor clipping artifacts
	m_ProjMatrix.SetOrthoOffCenterLH(casterAABB.getMinimum().x, casterAABB.getMaximum().x,
	                                 casterAABB.getMinimum().y, casterAABB.getMaximum().y,
	                                 casterAABB.getMinimum().z, casterAABB.getMaximum().z);

	m_ShadowMatrix = m_ViewMatrix * m_ProjMatrix;
	//set special texture matrix for shadow mapping
	float fOffsetX = 0.5f + (0.5f / (float)m_ShadowMapSize);
	float fOffsetY = 0.5f + (0.5f / (float)m_ShadowMapSize);
	CMatrix texCoordMatrix( 0.5f,     0.0f,     0.0f, 0.0f,
	                        0.0f,    -0.5f,     0.0f, 0.0f,
	                        0.0f,     0.0f,     1.0f, 0.0f,
	                        fOffsetX, fOffsetY, 0.0f, 1.0f );
	m_ShadowMatrix *= texCoordMatrix;
	return 0;
}

}// namespace sqr

