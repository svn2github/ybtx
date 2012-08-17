#pragma once
#include "stdafx.h"
#include "CAniCoder.h"
#include "CAgpCoder.h"
#include "CDataSources.h"
#include "CAnimation.h"
#include "CAnimationGroup.h"
#include <string>


bool CAniCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CAnimate* pAni = (CAnimate*)handle;
	pAni->m_IsCpAni = false;// 永不压缩
	wdi.Write( (TCHAR*)&pAni->m_MaxFrame, sizeof(pAni->m_MaxFrame) );
	wdi.Write( (TCHAR*)&pAni->m_FrameRatio, sizeof(pAni->m_FrameRatio) );
	wdi.Write( (TCHAR*)&pAni->m_fScale, sizeof(pAni->m_fScale) );
	wdi.Write( (TCHAR*)&pAni->m_fTransParam, sizeof(float) );
	wdi.Write( (TCHAR*)&pAni->m_GroundSpeed, sizeof(float) );
	wdi.Write( (TCHAR*)&pAni->m_IsCpAni,sizeof(bool));

	int Size = (int)pAni->m_SkeletalList.size();
	wdi.Write( (TCHAR*)&Size, sizeof(Size) );
	int i = 0;
	/*if (pAni->m_IsCpAni)
	{
		for( i = 0; i < (int)pAni->m_SkeletalList.size(); i++ )
		{
			if ( pAni->m_MaxFrame < 256 )
				((TKeyFrames<uint8>*)pAni->m_SkeletalList[i])->BinSave( wdi ,pAni->m_fTransParam);
			else
				((TKeyFrames<uint16>*)pAni->m_SkeletalList[i])->BinSave( wdi ,pAni->m_fTransParam);
		}
	}
	else
	{*/
	/*for( i = 0; i < (int)pAni->m_SkeletalList.size(); i++ )
	{
		if ( pAni->m_MaxFrame < 256 )
			((TKeyFrames<uint8>*)pAni->m_SkeletalList[i])->BinSaveFull( wdi );
		else
			((TKeyFrames<uint16>*)pAni->m_SkeletalList[i])->BinSaveFull( wdi );
	}*/
	//}

	Size = (int)pAni->m_FrameString.size();
	wdi.Write( (TCHAR*)&Size, sizeof(Size) );
	for( GMap<WORD,sqr::IDNAME>::iterator it = pAni->m_FrameString.begin(); it != pAni->m_FrameString.end(); ++it, i++ )
	{
		wdi.Write( (TCHAR*)( &(*it).first ), sizeof( WORD ) );
		wdi.Write( (TCHAR*)( &(*it).second ), sizeof( sqr::IDNAME ) );
	}
	return true;
}

bool CAniCoder::DeCode_Ani( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	CAnimate* pAni = (CAnimate*)handle;

	if ( pAni->m_szName.empty() )
		return false;

	pAni->m_fScale = 1.0f;
	pAni->m_FrameRatio = 33.3f;
	tData.SafeRead( &pAni->m_MaxFrame, sizeof(pAni->m_MaxFrame) );

	tData.SafeRead( (char*)&pAni->m_FrameRatio, sizeof(pAni->m_FrameRatio) );
	tData.SafeRead( (char*)&pAni->m_fScale, sizeof(pAni->m_fScale) );
	tData.SafeRead( (char*)&pAni->m_fTransParam, sizeof(float) );
	tData.SafeRead( (char*)&pAni->m_GroundSpeed, sizeof(float) );
	tData.SafeRead( (char*)&pAni->m_IsCpAni,sizeof(bool));

	int Size;
	tData.SafeRead( &Size, sizeof(Size) );
	pAni->m_SkeletalList.resize( Size );
	/*if(pAni->m_IsCpAni)
	{
		for ( int i = 0; i < (int)pAni->m_SkeletalList.size(); i++ )
		{
			if ( pAni->m_MaxFrame < 256 )
				pAni->m_SkeletalList[i] =  pAni->_NewKeyFrames8();
			else
				pAni->m_SkeletalList[i] =  pAni->_NewKeyFrames16();

			pAni->m_SkeletalList[i]->BinLoad( tData ,pAni->m_fTransParam);
		}
	}
	else
	{
		for ( int i = 0; i < (int)pAni->m_SkeletalList.size(); i++ )
		{
			if ( pAni->m_MaxFrame < 256 )
				pAni->m_SkeletalList[i] =  pAni->_NewKeyFrames8();
			else
				pAni->m_SkeletalList[i] =  pAni->_NewKeyFrames16();

			pAni->m_SkeletalList[i]->BinLoadFull( tData );
		}
	}*/

	tData.SafeRead( &Size, sizeof(Size) );
	for ( int i = 0; i < Size; i++ )
	{
		WORD FrameNO;
		IDNAME Name;
		tData.SafeRead( ( &FrameNO ), sizeof( WORD ) );
		tData.SafeRead( ( &Name ), sizeof( IDNAME ) );
		pAni->m_FrameString[ FrameNO ] = Name;
	}

	pAni->m_szName = "";
	pAni->GetRootGroup()->LoadNextAni();
	pAni->Release();
	return true;
}

BEGIN_VERSION_MAP(CAniCoder)
	ON_VERSION( 1 , DeCode_Ani )
END_VERSION_MAP()

CAniFormat::CAniFormat(void)
: CFileFormatMgr( 'SANI'/*文件格式标示*/, 0 /*文件最新版本号*/ )
{

}

CAniFormat::~CAniFormat(void)
{

}