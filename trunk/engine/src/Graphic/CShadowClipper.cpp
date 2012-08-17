#include "StdAfx.h"
#include "CShadowClipper.h"

namespace sqr
{

//------------------------------------------------------------------------------
void
CShadowClipper::AddCasterBox( const CMatrix& transform, const CAxisAlignedBox& b )
{
	CAxisAlignedBox box = b;
	box.transformBox(transform);
	m_CasterBoxes.mergeBox(box);
}

//------------------------------------------------------------------------------
CShadowClipper::CShadowClipper()
{
	// empty
}

//------------------------------------------------------------------------------
const CAxisAlignedBox&
CShadowClipper::GetCastersAABB( const CMatrix& transform )
{
	static CAxisAlignedBox aabb;
	aabb = m_CasterBoxes;
	aabb.transformBox(transform);
	return aabb;
}

const CAxisAlignedBox&
CShadowClipper::GetCastersAABBWithProj( const CMatrix& transform )
{
	static CAxisAlignedBox aabb;
	aabb = m_CasterBoxes;
	aabb.transformBoxWithProj(transform);
	return aabb;
}

//------------------------------------------------------------------------------
void
CShadowClipper::AddReceiverBox( const CAxisAlignedBox& box )
{
	m_ReceiverBoxes.mergeBox(box);
}

//------------------------------------------------------------------------------
const CAxisAlignedBox&
CShadowClipper::GetReceiverAABB( const CMatrix& transform )
{
	static CAxisAlignedBox aabb;
	aabb = m_ReceiverBoxes;
	aabb.transformBox(transform);
	return aabb;
}
}// namespace sqr
