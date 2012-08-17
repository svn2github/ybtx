#pragma once
/**
	@class CLocalMapAudioLoader

	Load .local file

	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
SmartPointer(CLocalMapAudioLoader);

class CLocalMapAudioLoader : public CRefObject
{
public:
	CLocalMapAudioLoader();

	/// ÉèÖÃterrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);

	/// Load .Local File
	void LoadAsSceneAudio(const string& path);

private:
	CTerrainMesh* terrain;
};

//------------------------------------------------------------------------------
inline void
CLocalMapAudioLoader::SetTerrainMesh( CTerrainMesh* t )
{
	Ast(NULL != t);
	this->terrain = t;
}

}// namespace sqr