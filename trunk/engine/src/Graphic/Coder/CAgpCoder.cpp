#include "stdafx.h"
#include "CAgpCoder.h"
#include "CAniCoder.h"
#include "CDataSources.h"
#include "CAnimationGroup.h"
#include "CSkeletalsMgr.h"
#include <string>

#pragma warning(disable:4996)

bool CAgpCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CAnimationGroup* pAniGroup = (CAnimationGroup*)handle;
	int dataSize = (int)pAniGroup->m_SkeName.size();
	wdi.Write( (TCHAR*)&dataSize, sizeof(dataSize) );
	wdi.Write( pAniGroup->m_SkeName.data(), dataSize );

	int Size;
	int i;

	Size = (int)pAniGroup->m_AnimationList.size();
	wdi.Write( (TCHAR*)&Size, sizeof(Size) );

	TCHAR* szPreName = (TCHAR*)pAniGroup->m_szName.c_str();
	szPreName[pAniGroup->m_szName.size() - 4] = 0;

	for( i = 0; i < (int)pAniGroup->m_AnimationList.size(); i++ )
	{
		sqr::IDNAME Name = pAniGroup->m_AnimationList[i]->GetName();
		wdi.Write( (TCHAR*)(&Name), sizeof(sqr::IDNAME) );

		TCHAR	szAniName[256];
		sprintf_s( szAniName, "%s_%s.ani", szPreName, (LPCTSTR)Name );
		CAnimate* pAni = (CAnimate*)pAniGroup->m_AnimationList[i];
		CAniFormat AniFormat;
		AniFormat.Save(szAniName,pAni);
	}
	return true;
}

bool CAgpCoder::DeCode_Agp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CAnimationGroup* pAniGroup = (CAnimationGroup*)handle;
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	int dataSize;
	tData.SafeRead((TCHAR*)&dataSize,sizeof(int));
	pAniGroup->m_SkeName.resize(dataSize);
	tData.SafeRead((TCHAR*)pAniGroup->m_SkeName.data(), dataSize );

	CSkeletalsMgr::GetInst()->GetSkeInPool( pAniGroup->m_SkeName.c_str(), &pAniGroup->m_pSke );

	int Size;

	tData.SafeRead( &Size, sizeof(Size) );
	pAniGroup->m_AnimationList.resize( Size, NULL );

	pAniGroup->m_NameList.clear();
	TCHAR szAniName[256];
	TCHAR* szChild = szAniName + pAniGroup->m_szName.size() - 3;
	strncpy( szAniName, pAniGroup->m_szName.c_str(), pAniGroup->m_szName.size() - 4 );
	szChild[-1] = '_';

	CAnimate* pAnimate = NULL;
	GString strAniname;
	int i;
	for ( i = 0; i < (int)pAniGroup->m_AnimationList.size(); i++ )
	{
		tData.SafeRead( szChild, sizeof(IDNAME) );
		strAniname = szChild;
		IDNAME Name = *(IDNAME*)szChild;

		pAniGroup->m_NameList[szChild] = i;
		strcat( szChild, ".ani" );

		pAnimate = CDataSources::GetInst()->NewAnimate(pAniGroup, szAniName, Name);//new CAnimate(this, szAniName, Name );
		pAniGroup->m_AnimationList[i] = pAnimate;

		if(strAniname.find("stand")!=GString::npos)//这个地方以后要改,以后会在插件到处调整骨骼动画的加载顺序和批量处理老资源,现在临时在这里过滤下把stand在最先加载
			pAniGroup->InsertAni(pAniGroup->m_AnimationList[i]);
	}
	pAniGroup->LoadNextAni();
	pAniGroup->Release();
	return true;
}

BEGIN_VERSION_MAP(CAgpCoder)
	ON_VERSION( 1 , DeCode_Agp )
END_VERSION_MAP()

CAgpFormat::CAgpFormat(void)
: CFileFormatMgr( 'SANI'/*文件格式标示*/, 0 /*文件最新版本号*/ )
{

}

CAgpFormat::~CAgpFormat(void)
{

}