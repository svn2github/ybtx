#pragma once
/**
	@class CGameGacMapSaver

	游戏客户端的amp,ars, arg文件保存

	//////////////////////////////////////////////////////////////////////////
	.amp文件
	1.4个时间段的光参信息
	2.水，波浪等信息
	3.背景图相关信息
	4.区域光参信息

	//////////////////////////////////////////////////////////////////////////
	.ars文件
	1.拼接游戏大纹理
	2.场景模型，动画及特效信息，以便arg直接保存相应的索引值

	//////////////////////////////////////////////////////////////////////////
	.arg文件采用Chunk结构, 每个Chunk结构如下:
	====================================
	0x0000: FourCC
	0x0004: Version
	0x0008: ChunkSize
	[ChunkSize bytes]
	====================================
	共14个分支Chunk
	REGN------------------------(文件头标志)
		SRGN--------------------(四分之一区域)
		VHGT--------------------(顶点高度)
		VDIF--------------------(顶点颜色)
		GATR--------------------(阻挡)
		EBLK--------------------(精确阻挡)
		TEXT--------------------(纹理)
		MODL--------------------(场景模型)
		TRAP--------------------(Trap点，已经没有用)
		LHGT--------------------(逻辑高)
		SBVT--------------------(SubVertex，暂时没有作用)
		OBJT--------------------(Object)
		WATR--------------------(水，暂时没有作用)
		SCLT--------------------(场景点光源)
		SPLT--------------------(场景聚光灯)
		AUDI--------------------(场景音源)
		LOGI--------------------(登录模型)

	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
class IProgressCallback;
SmartPointer(CGameGacMapSaver);

class CGameGacMapSaver : public CRefObject
{
public:
	CGameGacMapSaver();
	/// 设置terrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);
	/// 设置进度条回调
	void SetProcessCallback(IProgressCallback* progress);

	/// Save GameGac File
	bool SaveAsClient(const string& path);

private:
	///.ars save
	void SaveResSetFile(const string& path);
	///.amp save
	void SaveAmpSetFile(const string& path);
	///.arg save
	void SaveRegionFile(const string& path);
	///拼接大纹理
	void MakeAtlasTexture(const string& path);

private:
	CTerrainMesh* terrain;
	IProgressCallback* progress;
};

//------------------------------------------------------------------------------
inline void
CGameGacMapSaver::SetTerrainMesh( CTerrainMesh* t )
{
	Ast(NULL != t);
	this->terrain = t;
}

//------------------------------------------------------------------------------
inline void
CGameGacMapSaver::SetProcessCallback( IProgressCallback* p )
{
	Ast(NULL != p);
	this->progress = p;
}

}// namespace sqr