#include "stdafx.h"
#include "CAgpProcessor.h"

#include "GraphicCommon.h"

int CAniGroup::BinLoad( string LoadName, CBaseResProcessor* pProcessor )
{
	ifstream File( LoadName.data(), ios::binary|ios::in );
	if( File.fail() )
		return GERROR;

	DWORD Res;
	DWORD Version;
	File.read( (TCHAR*)&Res, sizeof(DWORD) );
	File.read( (TCHAR*)&Version, sizeof(DWORD) );

	if( Res != MAKEFOURCC( 'A', 'G', 'P', 0 ) || Version != AGP_COMP_VERSION )
		return GERROR;

	if(Version>=10002)
	{
		int dataSize;
		File.read((TCHAR*)&dataSize,sizeof(int));
		m_IndexSkeName.resize(dataSize);
		File.read((TCHAR*)m_IndexSkeName.data(), dataSize );
	}

	int Size;
	int i;

	File.read( (TCHAR*)&Size, sizeof(Size) );
	m_AnimationList.resize( Size );

	string szStr = LoadName;
	TCHAR* szPreName = (TCHAR*)szStr.c_str();
	szPreName[szStr.size() - 4] = 0;

	for( i = 0; i < (int)m_AnimationList.size(); i++ )
	{
		File.read( (TCHAR*)&m_AnimationList[i], sizeof(IDNAME) );
		TCHAR szAniName[256];
		sprintf( szAniName, "%s_%s.ani", szPreName, (LPCTSTR)m_AnimationList[i] );

		pProcessor->GetMgr()->ProcessWith( szAniName,LoadName.c_str() );
	}
	return 0;
}

CAgpProcessor::CAgpProcessor(void)
{
}

CAgpProcessor::~CAgpProcessor(void)
{
}

bool CAgpProcessor::RunProcess( const TCHAR* szFilename )
{
	CAniGroup Group;
	ChangeToSrcDir();
	if( FAILED( Group.BinLoad( szFilename, this ) ) )
	{
		return false;
	}

	m_pMgr->PostProcessed(szFilename);

	return true;
}
