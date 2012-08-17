#pragma once
//------------------------------------------------------------------------------
/**
    @class CLogicalHeightMeshBuilder

	从TerrainMesh里建造逻辑高mesh.

    (C) 2009 ThreeOGCMan
*/
#include "CRenderOperation.h"

namespace sqr
{
	SmartPointer(LogicalMesh);
	class LogicalMesh : public CRefObject
	{
	public:
		virtual ~LogicalMesh();
		CRenderOperation rp;
		CAxisAlignedBox aabb;
	};
	class CLogicalHeightMeshBuilder
	{
	public:
		static void Build(vector<LogicalMeshPtr>& outMeshes);
	};
}