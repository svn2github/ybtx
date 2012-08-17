#pragma once
/**
	@class CLocalMapAudioSaver

	Save .local File

	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
SmartPointer(CLocalMapAudioSaver);

class CLocalMapAudioSaver : public CRefObject
{
public:
	CLocalMapAudioSaver();
	/// ÉèÖÃterrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);

	/// Save .Local File
	void SaveAsSceneAudio(const string& path, bool bResize);
	
private:
	CTerrainMesh* terrain;
};

//------------------------------------------------------------------------------
inline void
CLocalMapAudioSaver::SetTerrainMesh( CTerrainMesh* t )
{
	Ast(NULL != t);
	this->terrain = t;
}

}// namespace sqr