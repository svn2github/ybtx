#include "stdafx.h"
#include "CurvePatch\BezierControlPoint.h"
#include "CGraphic.h"
#include "CWindowTarget.h"

sqr::CBezierControlPoint::CBezierControlPoint( void )
{

}

sqr::CBezierControlPoint::~CBezierControlPoint( void )
{

}

void sqr::CBezierControlPoint::CreatePointSquare( const CVector3f& point )
{

}

bool sqr::CBezierControlPoint::IsSelect( const CVector3f& point )
{
	return false;
}

void sqr::CBezierControlPoint::SetIsSelect( const bool b )
{

}

CVector3f sqr::CBezierControlPoint::GetControlPoint() const
{
	return m_centerPoint;
}

void sqr::CBezierControlPoint::Render()
{

}