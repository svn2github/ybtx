#pragma once
/**
	@class CLocalMapLoginLoader

	Load .local file

	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
SmartPointer(CLocalMapLoginLoader);

class CLocalMapLoginLoader : public CRefObject
{
public:
	CLocalMapLoginLoader();

	/// ÉèÖÃterrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);

	/// Load .Login File
	void LoadAsSceneLogin(const string& path);

protected:
	void LoadLoginModelInfo(FILE *fp);
private:
	CTerrainMesh* terrain;
	DWORD		  m_dwVersion;
};

//------------------------------------------------------------------------------
inline void
CLocalMapLoginLoader::SetTerrainMesh( CTerrainMesh* t )
{
	Ast(NULL != t);
	this->terrain = t;
}

}// namespace sqr