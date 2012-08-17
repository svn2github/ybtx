#pragma once
//------------------------------------------------------------------------------
/**
    @class VertexColorLoader

	烘焙顶点色的载入.

	@see VertexColorSaver

    (C) 2009 ThreeOGCMan
*/
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
class IProgressCallback;
struct PieceClassColor;
SmartPointer(Stream);
SmartPointer(BinaryReader);
SmartPointer(VertexColorLoader);

class VertexColorLoader : public CRefObject
{
public:
	VertexColorLoader();

	/// 设置terrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);
	/// 设置进度条回调
	void SetProcessCallback(IProgressCallback* progress);

	/// 载入
	bool Load(const string& path);

private:
	bool ReadGridColor(index_t gridIndex);
	void ReadModelColor(index_t& originGridIndex,
						index_t& modelGroupIndex,
						string& modelName,
						vector<PieceClassColor>& classColors);
	void ReadClassColor(PieceClassColor* classColor);

private:
	CTerrainMesh* terrain;
	IProgressCallback* progress;

	StreamPtr stream;
	BinaryReaderPtr reader;
};

//------------------------------------------------------------------------------
inline void
VertexColorLoader::SetTerrainMesh( CTerrainMesh* t )
{
	this->terrain = t;
}

//------------------------------------------------------------------------------
inline void
VertexColorLoader::SetProcessCallback( IProgressCallback* p )
{
	this->progress = p;
}

}// namespace sqr
