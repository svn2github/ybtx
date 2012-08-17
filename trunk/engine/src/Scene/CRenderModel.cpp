#include "stdafx.h"
#include "CRenderModel.h"

//#define USENEWTRANSMETHOD
//#define MixObjHeight 0//这个需要这样，必须用这样的方式推动消隐和阴影设置的规范化。 //200 //消隐物体最小高度
//
//#define MAX_ALPHA			255
//#define MIN_ALPHA			64


//------------------------------------------------------------------------------
CRenderModel::CRenderModel(ILogicHandler* pHandler)
: CRenderObject(NULL,pHandler)
, m_bCameraCollision(false) 
{
}

//------------------------------------------------------------------------------
CRenderModel::~CRenderModel()
{
}

bool  CRenderModel::Intersects( const CVector3f& Pos )
{
	if(m_bCameraCollision)
	{
		CAxisAlignedBox Temp = m_OrgAABB;
		Temp.transformBox(m_CenterMatrix);
		return Temp.intersects(Pos);
	}
	else
		return false;
}