#pragma once
/**
	@class CLoadSaveSceneLight

	Load .local file

	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
SmartPointer(CLoadSaveSceneLight);

class CLoadSaveSceneLight : public CRefObject
{
public:
	CLoadSaveSceneLight();

	/// ÉèÖÃterrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);

	/// Load light.info File
	void LoadSceneLightsInfo();
	/// Save light.info File
	void SaveSceneLightsInfo();

private:
	CTerrainMesh* terrain;
};

//------------------------------------------------------------------------------
inline void
CLoadSaveSceneLight::SetTerrainMesh( CTerrainMesh* t )
{
	Ast(NULL != t);
	this->terrain = t;
}

}// namespace sqr