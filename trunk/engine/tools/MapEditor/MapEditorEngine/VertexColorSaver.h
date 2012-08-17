#pragma once
/**
	@class VertexColorSaver

	烘焙物件顶点色的保存.

	文件采用Chunk结构, 每个Chunk结构如下:
	====================================
	0x0000: FourCC
	0x0004: Version
	0x0008: ChunkSize
	[ChunkSize bytes]
	====================================
	共四级Chunk
	BKCL------------------------(文件头, 保存各grid的偏移)
		GRID--------------------(格子, 保存几个模型)
			MODL----------------(模型, 保存原格子, ModelGroup索引, 几个piece class)
				CLAS------------(piece class, 保存各piece的颜色值)

	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
class IProgressCallback;
struct PieceClassColor;
SmartPointer(Stream);
SmartPointer(BinaryWriter);
SmartPointer(VertexColorSaver);

class VertexColorSaver : public CRefObject
{
public:
	VertexColorSaver();
	/// 设置terrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);
	/// 设置进度条回调
	void SetProcessCallback(IProgressCallback* progress);

	/// 保存到文件
	bool Save(const string& path);

private:
	void WriteGridColor(index_t gridIndex);
	void WriteModelColor(index_t originGridIndex,
	                     index_t modelGroupIndex,
	                     const string& modelName,
	                     const vector<PieceClassColor>& classColors);
	void WriteClassColor(const PieceClassColor* classColor);

private:
	CTerrainMesh* terrain;
	IProgressCallback* progress;

	StreamPtr stream;
	BinaryWriterPtr writer;
};

//------------------------------------------------------------------------------
inline void
VertexColorSaver::SetTerrainMesh( CTerrainMesh* t )
{
	Ast(NULL != t);
	this->terrain = t;
}

//------------------------------------------------------------------------------
inline void
VertexColorSaver::SetProcessCallback( IProgressCallback* p )
{
	Ast(NULL != p);
	this->progress = p;
}

}// namespace sqr