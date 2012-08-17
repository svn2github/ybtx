#pragma once
/**
	@class CLocalMapLoginSaver

	Save .local File

	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
SmartPointer(CLocalMapLoginSaver);

class CLocalMapLoginSaver : public CRefObject
{
public:
	CLocalMapLoginSaver();
	/// …Ë÷√terrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);

	/// Save .Local File
	void SaveAsSceneLogin(const string& path, set<int> nGridIndexs);
	
private:
	CTerrainMesh* terrain;
};

//------------------------------------------------------------------------------
inline void
CLocalMapLoginSaver::SetTerrainMesh( CTerrainMesh* t )
{
	Ast(NULL != t);
	this->terrain = t;
}

}// namespace sqr