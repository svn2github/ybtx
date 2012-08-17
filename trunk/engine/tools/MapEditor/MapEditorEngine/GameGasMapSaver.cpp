//------------------------------------------------------------------------------
//  CGameGasMapSaver.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "GameGasMapSaver.h"
#include "GraphicCommon.h"
#include "TerrainMesh.h"
#include "Engine.h"
#include "Console.h"


#define SERVER_CHUNK_COUNT (4)

namespace sqr
{

//------------------------------------------------------------------------------
CGameGasMapSaver::CGameGasMapSaver()
		: terrain(NULL)
		, progress(NULL)
{
}

//------------------------------------------------------------------------------
bool
CGameGasMapSaver::SaveAsServer(const string& path)
{
	Ast(NULL != this->terrain);
	Ast(NULL != this->progress);

	locale loc;
	locale::global(locale(""));

	// region file
	string strRgnFileName = "server\\map\\" + path + "_s.arg";

	use_facet<ctype<TCHAR> >( loc ).tolower
		( &strRgnFileName[0], &strRgnFileName[strRgnFileName.length()] );

	int nRegionCnt = terrain->GetRegionCount();
	float Total = float(nRegionCnt + 2);
	progress->SetProgress( 0.0f );

	FILE * fp = NULL;
	fp = fopen(strRgnFileName.c_str(),"wb");

	if ( fp )
	{
		// header
		fwrite("REGN",4,1,fp);

		// version
		DWORD dwVersion = SERVER_VERSION;
		fwrite(&dwVersion,sizeof(DWORD),1,fp);

		// chunk count
		DWORD dwDataChunkCnt = SERVER_CHUNK_COUNT;
		fwrite(&dwDataChunkCnt,sizeof(DWORD),1,fp);

		///服务端文件ID
		DWORD dwGameServerID = terrain->GetGameGasMapVersion();
		++dwGameServerID;
		fwrite(&dwGameServerID,sizeof(DWORD),1,fp);

		LogConsole("GameServerID: " << dwGameServerID << "\n" );
		// rgn offset table
		long lRgnOffsetTablePtr = ftell(fp);

		long lPad = 0;

		for (int i = 0; i < nRegionCnt; ++i )
		{
			fwrite(&lPad,sizeof(long),1,fp); // offset only
		}

		for (int i = 0; i < nRegionCnt; ++i )
		{
			int nReg = i;
			SRegion & reg = terrain->GetRegion(i);

			// patch the rgn offset table

			long lThisRgnOffsetPtr = ftell(fp);

			long lThisRgnOffsetTablePtr = lRgnOffsetTablePtr + i*sizeof(long);

			fseek(fp,lThisRgnOffsetTablePtr,SEEK_SET);

			fwrite(&lThisRgnOffsetPtr,sizeof(long),1,fp); // offset
			fseek(fp,lThisRgnOffsetPtr,SEEK_SET);

			//////////////////////////////////////////////////////////////////////////

			// 1 chunk name grid total height
			fwrite("GHGT",4,1,fp);

			// chunk offset and size
			long lGridHeightPt = ftell(fp);
			long lPad = 0;
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			// 2 chunk name grid block
			fwrite("GATR",4,1,fp);

			// chunk offset and size
			long lBarrPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			// 3 chunk name exact block
			fwrite("EBLK",4,1,fp);

			// chunk exact block
			long lExactBlockPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size
			//////////////////////////////////////////////////////////////////////////

			// 4 chunk name trap index
			fwrite("TRAP",4,1,fp);

			// chunk offset and size
			long lTrapPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			// 5 chunk name object info
			fwrite("OBJT",4,1,fp);

			// chunk offset and size
			long lObjectPt = ftell(fp);
			fwrite(&lPad,sizeof(long),1,fp); // offset
			fwrite(&lPad,sizeof(long),1,fp); // size

			//////////////////////////////////////////////////////////////////////////

			// vertices cnt
			DWORD dwVertexCntPerRegion = (REGION_SPACE+1)*(REGION_SPACE+1);
			DWORD dwGridCntPerRegion = (REGION_SPACE)*(REGION_SPACE);

			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////

			// 1 grid total height chunk

			// Offset backfill
			long lGridHeightOffset = ftell(fp);
			fseek(fp,lGridHeightPt,SEEK_SET);
			fwrite(&lGridHeightOffset,sizeof(long),1,fp); // size
			fseek(fp,lGridHeightOffset,SEEK_SET);

			// chunk raw data
			for (size_t i = 0; i < dwGridCntPerRegion; i++ )
			{
				SGrid * pGrid = &terrain->GetGrid(reg.vGridIndices[i]);
				short s = (short)terrain->GetGridTotalHeight(pGrid->dwGridIndex);
				fwrite(&s,sizeof(short),1,fp);
			}

			// size backfill
			long lGridHeightSize = ftell(fp) - lGridHeightOffset;
			fseek(fp,lGridHeightPt+sizeof(long),SEEK_SET);
			fwrite(&lGridHeightSize,sizeof(long),1,fp); // size
			fseek(fp,lGridHeightSize+lGridHeightOffset,SEEK_SET);

			//////////////////////////////////////////////////////////////////////////

			// 2 grid block chunk
			// Offset backfill
			long lBarrOffset = ftell(fp);
			fseek(fp,lBarrPt,SEEK_SET);
			fwrite(&lBarrOffset,sizeof(long),1,fp); // size
			fseek(fp,lBarrOffset,SEEK_SET);

			// chunk raw data
			for (size_t i = 0; i < dwGridCntPerRegion; i++ )
			{
				SGrid * pGrid = &terrain->GetGrid(reg.vGridIndices[i]);
				BYTE byBlockType = pGrid->nBlockType;

				fwrite(&byBlockType,sizeof(BYTE),1,fp);

				///road weight
				uint8 weight = (uint8)pGrid->nGridSeekRoadWeight;
				fwrite(&weight, sizeof(uint8),1,fp);
			}

			// size backfill
			long lBarrSize = ftell(fp) - lBarrOffset;
			fseek(fp,lBarrPt+sizeof(long),SEEK_SET);
			fwrite(&lBarrSize,sizeof(long),1,fp); // size
			fseek(fp,lBarrSize+lBarrOffset,SEEK_SET);

			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////

			// 3 grid attributes chunk block info lExactBlockPt
			// Offset backfill
			long lExactBlockOffset = ftell(fp);
			fseek(fp,lExactBlockPt,SEEK_SET);
			fwrite(&lExactBlockOffset,sizeof(long),1,fp); // size
			fseek(fp,lExactBlockOffset,SEEK_SET);

			// chunk raw data
			for (size_t i = 0; i < dwGridCntPerRegion; i++ )
			{
				SGrid * pGrid = &terrain->GetGrid(reg.vGridIndices[i]);

				fwrite(&pGrid->subGridTypes, sizeof(pGrid->subGridTypes),1,fp);
			}

			// size backfill
			long lExactBlockSize = ftell(fp) - lExactBlockOffset;
			fseek(fp,lExactBlockPt+sizeof(long),SEEK_SET);
			fwrite(&lExactBlockSize,sizeof(long),1,fp); // size
			fseek(fp,lExactBlockSize+lExactBlockOffset,SEEK_SET);

			//////////////////////////////////////////////////////////////////////////

			// 4 trap chunk

			// Offset backfill
			long lTrapOffset = ftell(fp);
			fseek(fp,lTrapPt,SEEK_SET);
			fwrite(&lTrapOffset,sizeof(long),1,fp); // size
			fseek(fp,lTrapOffset,SEEK_SET);

			DWORD wCnt = 0;
			fwrite(&wCnt,sizeof(WORD),1,fp); // list cnt

			// size backfill
			long lTrapSize = ftell(fp) - lTrapOffset;
			fseek(fp,lTrapPt+sizeof(long),SEEK_SET);
			fwrite(&lTrapSize,sizeof(long),1,fp); // size

			fseek(fp,lTrapOffset,SEEK_SET);
			fwrite(&wCnt,sizeof(WORD),1,fp); // cnt

			fseek(fp,lTrapSize+lTrapOffset,SEEK_SET);

			//////////////////////////////////////////////////////////////////////////

			// 5 object chunk

			// offset backfill
			long lObjectOffset = ftell(fp);
			fseek(fp,lObjectPt,SEEK_SET);
			fwrite(&lObjectOffset,sizeof(long),1,fp); // offset
			fseek(fp,lObjectOffset,SEEK_SET);

			//SaveMeAsServer(fp, nReg);
			WORD wSaveMeCnt = 0;
			fwrite(&wSaveMeCnt,sizeof(WORD),1,fp);

			// size backfill
			long lObjectSize = ftell(fp) - lObjectOffset;
			fseek(fp,lObjectPt+sizeof(long),SEEK_SET);
			fwrite(&lObjectSize,sizeof(long),1,fp); // size
			fseek(fp,lObjectSize+lObjectOffset,SEEK_SET);

			progress->SetProgress( float(nReg)/Total );
		}

		fclose(fp);

		progress->SetProgress( 1.0f );

		terrain->SetGameGasMapVersion(dwGameServerID);

		LogConsole("导出服务端文件完毕...")

		string localname = "";
		localname = terrain->m_strMapName;

		LogConsole("保存的文件的local文件名是: " << localname.c_str() << "\n");

		MessageBox(NULL,"为以免导客户端文件有问题，最好保存下地图本身文件","切记",MB_OK);
		MessageBox(NULL,"导出游戏文件完毕，一定要提交到resbin目录下","",MB_OK);
	}
	else
	{
		string log = strRgnFileName + "无法保存";

		MessageBox(NULL,log.c_str(),"错误",MB_OK);
	}

	return true;
}

}// namespace sqr
