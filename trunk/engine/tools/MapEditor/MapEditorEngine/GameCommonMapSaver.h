#pragma once


#include "CRefObject.h"

namespace sqr
{
	class CTerrainMesh;
	class IProgressCallback;
	SmartPointer(CGameCommonMapSaver);

	class CGameCommonMapSaver : public CRefObject
	{
	public:
		CGameCommonMapSaver();
		/// 设置terrain mesh
		void SetTerrainMesh(CTerrainMesh* terrain);
		/// 设置进度条回调
		void SetProcessCallback(IProgressCallback* progress);

		/// Save Common File
		bool SaveAsCommon(const string& path);

	private:
		CTerrainMesh* terrain;
		IProgressCallback* progress;
	};

	//------------------------------------------------------------------------------
	inline void
		CGameCommonMapSaver::SetTerrainMesh( CTerrainMesh* t )
	{
		Ast(NULL != t);
		this->terrain = t;
	}

	//------------------------------------------------------------------------------
	inline void
		CGameCommonMapSaver::SetProcessCallback( IProgressCallback* p )
	{
		Ast(NULL != p);
		this->progress = p;
	}

}// namespace sqr