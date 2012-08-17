#include "stdafx.h"
#include "../ChunkData/CEditorBlockCoder.h"
#include "ToolSetTileBlock.h"

BEGIN_VERSION_MAP(CEditorBlockCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CEditorBlockCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
// 	int8 nBlockTpyeCnt = CToolSetTileBlock::EBT_CNT - 1;
// 	int  nBlockTpye = -1;
// 	for ( int8 i = 0; i < nBlockTpyeCnt; ++i )
// 	{
// 		nBlockTpye = i + 1;
// 
// 		set<string> texnames = CToolSetTileBlock::GetInst()->GetBlockTypeTexNames(nBlockTpye);
// 		size_t size			 = texnames.size();
// 
// 		wdi.Write(&size, sizeof(size));
// 		if( size != 0 )
// 		{
// 			string name("");
// 			int dwLen = 0;
// 
// 			set<string>::iterator beg = texnames.begin();
// 			set<string>::iterator end = texnames.end();
// 			for ( beg; beg != end; ++beg )
// 			{
// 				name = *beg;
// 				dwLen = name.size();
// 
// 				wdi.Write(&dwLen,		sizeof(dwLen));
// 				wdi.Write(name.c_str(), dwLen);
// 			}
// 		}
// 	}
	return true;
}

bool sqr::CEditorBlockCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	return true;
}

class CAnyObject;


CEditorBlcokFormat::CEditorBlcokFormat(void)
: CFileFormatMgr( 'SBFT', 0 )//'SBFT' src block format
{
	InitBlockCode();
}

CEditorBlcokFormat::~CEditorBlcokFormat(void)
{ /*do nothing~~~*/ }

void sqr::CEditorBlcokFormat::InitBlockCode()
{
	RegisterCoder(new CEditorBlockCoder);
}

bool sqr::CEditorBlcokFormat::LoadBlockFile( const string& strFile )
{
	CAnyObject* pObj = (CAnyObject*)(0xeefefeff); //指向要读取/存储的对象
	return Load( strFile, pObj );
}

bool sqr::CEditorBlcokFormat::SaveBlockFile( const string& strFile )
{
	CAnyObject* pObj = (CAnyObject*)(0xeefefeff); //指向要读取/存储的对象
	return Save( strFile, pObj ); 
}
