

#include "StdAfx.h"
#include "GameCommonMapSaver.h"
#include "GraphicCommon.h"
#include "TerrainMesh.h"
#include "Engine.h"
#include "Console.h"
#include "CLinkRegionBuild.h"
#include "TBinaryHeap.inl"
#include "IoSystem.h"

namespace sqr
{

	//------------------------------------------------------------------------------
	CGameCommonMapSaver::CGameCommonMapSaver()
		: terrain(NULL)
		, progress(NULL)
	{
	}

	//------------------------------------------------------------------------------
	bool
		CGameCommonMapSaver::SaveAsCommon(const string& path)
	{
		Ast(NULL != this->terrain);
		Ast(NULL != this->progress);

		locale loc;
		locale::global(locale(""));

		// region file
		// region file
		string strOldRgnFileName = "map\\" + path + ".lrg";
		if( IoSystem::GetInst()->FileExists(strOldRgnFileName) )
		{
			bool bDelete = IoSystem::GetInst()->DeleteFile(strOldRgnFileName);
			if( !bDelete )
			{
				if ( IoSystem::GetInst()->IsReadOnly(strOldRgnFileName) )
				{
					string log = strOldRgnFileName;
					log += "该文件已被锁定，保存前先解锁，否则保存该文件失败";

					MessageBox( NULL, log.c_str() , "提示", MB_OK );
				}
			}
			MessageBox( NULL, "将原来resbin/map下对应的lrg文件删掉" , "提示", MB_OK );
		}

		string strRgnFileName = "common\\map\\" + path + ".lrg";

		use_facet<ctype<TCHAR> >( loc ).tolower
			( &strRgnFileName[0], &strRgnFileName[strRgnFileName.length()] );


		CLinkRegionBuild::Inst().SetProcessCallback(progress);
		return CLinkRegionBuild::Inst().CreateLinkRegionFile(strRgnFileName, terrain, 1);
	}

}// namespace sqr
