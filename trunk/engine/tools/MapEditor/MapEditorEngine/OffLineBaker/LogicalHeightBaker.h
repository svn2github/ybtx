#pragma once

#include "TerrainBaker.h"
#include "LogicalHeightMeshBuilder.h"

namespace sqr
{
class ITexture;
class CLogicalHeightBaker :public CLightMapBaker
{
public:
	CLogicalHeightBaker(void);
	~CLogicalHeightBaker(void);

	void Setup();
	void Discard();
	void TilePrepare(index_t x,index_t z);
	void OnProcess(index_t x,index_t z);
	void RenderReceive();

	void SetOffsetY(float y);
private:
	
	vector<int> tileIndexList;

	float offsetY;
	vector<LogicalMeshPtr>	meshes;
	RenderParam				renderParam;
	ITexture*				m_pGlobalShadowTex;
};

}
