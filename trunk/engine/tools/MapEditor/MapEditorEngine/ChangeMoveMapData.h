#pragma once
/**
	@class CChangeMoveMapData

	Save .local File

	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
class IProgressCallback;
SmartPointer(CChangeMoveMapData);

class CChangeMoveMapData : public CRefObject
{
public:
	CChangeMoveMapData();

	/// 设置terrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);
	/// 设置进度条回调
	void SetProcessCallback(IProgressCallback* progress);

	/// nStartReginX，nStartReginZ需要移动地图的区域起始坐标， 
	void MoveAsMap(const int nStartReginX, const int nStartReginZ, const int nOffsetReginWidth, const int nOffsetReginDepth);

	void SetMoveOffset(const int xoffset, const int zoffset);

private:
	void ChangeRegionLightInfo();
	void ClearInfo();

private:
	CTerrainMesh* terrain;
	IProgressCallback* progress;

	///first 旧的格子索引值
	///second 移动后对应的新的格子索引值
	map<int, int> mapOldMapGridIndex;
	map<int, int> mapOldMapVertexIndex;
	///扩展后旧地图细化区域索引在新地图中的新索引映射 first old region index second new region index
	map<int, int> m_OldMapRegionIndexMap;

	int m_nMapXMoveOffset;//m_nMapXMoveOffset m_nMapZMoveOffset为移动的格子数
	int m_nMapZMoveOffset;
};

//------------------------------------------------------------------------------
inline void
CChangeMoveMapData::SetTerrainMesh( CTerrainMesh* t )
{
	Ast(NULL != t);
	this->terrain = t;
}

//------------------------------------------------------------------------------
inline void
CChangeMoveMapData::SetProcessCallback( IProgressCallback* p )
{
	Ast(NULL != p);
	this->progress = p;
}

//------------------------------------------------------------------------------
inline void
CChangeMoveMapData::SetMoveOffset(const int xoffset, const int zoffset)
{
	m_nMapXMoveOffset = xoffset;
	m_nMapZMoveOffset = zoffset;
}// namespace sqr
}
