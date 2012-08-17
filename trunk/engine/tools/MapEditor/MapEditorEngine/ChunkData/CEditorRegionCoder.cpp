#include "stdafx.h"
#include "../ChunkData/CEditorRegionCoder.h"

BEGIN_VERSION_MAP(CEditorRegionCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CEditorRegionCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	return true;
}

bool sqr::CEditorRegionCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	
	return true;
}

class CAnyObject;


CEditorRegionFormat::CEditorRegionFormat(void)
: CFileFormatMgr( 'SRFT', 0 )
{
	InitRegionCode();
}

CEditorRegionFormat::~CEditorRegionFormat(void)
{ /*do nothing~~~*/ }

void sqr::CEditorRegionFormat::InitRegionCode()
{
	RegisterCoder(new CEditorRegionCoder);
}

bool sqr::CEditorRegionFormat::LoadRegionFile( const string& strFile )
{
	CAnyObject* pObj = (CAnyObject*)(0xeefefeff); //指向要读取/存储的对象
	return Load( strFile, pObj );
}

bool sqr::CEditorRegionFormat::SaveRegionFile( const string& strFile )
{
	CAnyObject* pObj = (CAnyObject*)(0xeefefeff); //指向要读取/存储的对象
	return Save( strFile, pObj ); 
}