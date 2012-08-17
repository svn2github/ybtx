#pragma once
/**
	@class CGameGasMapSaver

	游戏服务端的arg文件保存

	文件采用Chunk结构, 每个Chunk结构如下:
	====================================
	0x0000: FourCC
	0x0004: Version
	0x0008: ChunkSize
	[ChunkSize bytes]
	====================================
	共四个分支Chunk
	REGN------------------------(文件头标志)
		GHGT--------------------(格子高度)
		GATR--------------------(阻挡信息)
		EBLK--------------------(精确阻挡信息)
		TRAP--------------------(Trap点信息，已经没有用)
	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
class IProgressCallback;
SmartPointer(CGameGasMapSaver);

class CGameGasMapSaver : public CRefObject
{
public:
	CGameGasMapSaver();
	/// 设置terrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);
	/// 设置进度条回调
	void SetProcessCallback(IProgressCallback* progress);

	/// Save GameGac File
	bool SaveAsServer(const string& path);

private:
	CTerrainMesh* terrain;
	IProgressCallback* progress;
};

//------------------------------------------------------------------------------
inline void
CGameGasMapSaver::SetTerrainMesh( CTerrainMesh* t )
{
	Ast(NULL != t);
	this->terrain = t;
}

//------------------------------------------------------------------------------
inline void
CGameGasMapSaver::SetProcessCallback( IProgressCallback* p )
{
	Ast(NULL != p);
	this->progress = p;
}

}// namespace sqr