#pragma once
#include "CRenderObject.h"

namespace sqr
{
class CTerrainModelCoder;

class CRenderModel 
	: public CRenderObject
	//: public CGeometryInst
{
	friend class CTerrainModelCoder;
protected:
	bool	m_bCameraCollision;
public:
	CRenderModel(ILogicHandler* pHandler = NULL);
	~CRenderModel();
	bool	Intersects( const CVector3f& Pos );
};

}