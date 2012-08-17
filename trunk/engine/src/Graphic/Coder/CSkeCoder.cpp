#include "stdafx.h"
#include "CSkeCoder.h"
#include "CDataSources.h"
#include "CSkeletalFrame.h"
#include "CMatrix.h"
#include <string>


bool CSkeCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CSkeletalFrame* pSke = (CSkeletalFrame*)handle;
	float FPS;
	CMatrix Glob;
	BYTE uSkeletalNum;
	wdi.Write( (TCHAR*)&FPS, sizeof(float) );
	wdi.Write( (TCHAR*)&Glob, sizeof(CMatrix) );
	wdi.Write( (TCHAR*)&uSkeletalNum, sizeof(uSkeletalNum) );

	((CSkeletal*)pSke->m_pSkeletalRoot)->BinSave( wdi );

	int Size;
	int i;
	Size = (int)pSke->m_SkeletalList.size();
	wdi.Write( (TCHAR*)&Size, sizeof(Size) );
	for( i = 0; i < (int)pSke->m_SkeletalList.size(); i++ )
		((CSkeletalUnit*)pSke->m_SkeletalList[i])->BinSave( wdi );

	return true;
}

bool CSkeCoder::DeCode_Ske( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	CSkeletalFrame* pSke = (CSkeletalFrame*)handle;
	Ast( pSke->m_SkeletalList.empty() );

	float FPS;
	CMatrix Glob;
	BYTE uSkeletalNum;
	tData.SafeRead( &FPS, sizeof(float) );
	tData.SafeRead( &Glob, sizeof(CMatrix) );
	tData.SafeRead( &uSkeletalNum, sizeof(uSkeletalNum) );

	CSkeletal* pSkeletals[256];
	pSke->m_pSkeletalRoot->BinLoad( tData, 0, pSkeletals );

	int Size;
	int i;
	tData.SafeRead( &Size, sizeof(Size) );
	pSke->m_SkeletalList.resize( Size, NULL );
	for ( i = 0; i < (int)pSke->m_SkeletalList.size(); i++ )
	{
		pSke->m_SkeletalList[i] = CDataSources::GetInst()->NewSkeletalUnit();
		pSke->m_SkeletalList[i]->BinLoad( tData );
		pSke->m_SkeletalList[i]->m_pSkeletal = pSkeletals[i];
	}
	return true;
}

BEGIN_VERSION_MAP(CSkeCoder)
	ON_VERSION( 1 , DeCode_Ske )
END_VERSION_MAP()

CSkeFormat::CSkeFormat(void)
: CFileFormatMgr( 'SSKE'/*文件格式标示*/, 0 /*文件最新版本号*/ )
{

}

CSkeFormat::~CSkeFormat(void)
{

}