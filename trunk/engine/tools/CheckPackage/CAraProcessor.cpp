#include "stdafx.h"
#include ".\caraprocessor.h"


int CResSkeletal::BinLoad( ifstream& File )
{
	int Size;
	File.read( (char*)&Size, sizeof(Size) );
	m_SkeletalName.resize( Size );
	File.read( (char*)m_SkeletalName.data(), Size );
	File.read( (char*)&m_SkeletalID, sizeof(m_SkeletalID) );

	File.read( (char*)&Size, sizeof(Size) );
	m_Children.resize( Size, NULL );
	for( int i = 0; i < (int)m_Children.size(); i++ )
	{
		m_Children[i] = new CResSkeletal;
		m_Children[i]->BinLoad( File );
	}

	return 0;
}

int CSkeFrame::BinLoad( string LoadName, CBaseResProcessor* pProcessor )
{
	ifstream File( LoadName.data(), ios::binary|ios::in );
	if( File.fail() )
	{
		return GERROR;
	}

	DWORD Res;
	DWORD Version;
	File.read( (char*)&Res, sizeof(DWORD) );
	File.read( (char*)&Version, sizeof(DWORD) );

	if( Res != MAKEFOURCC( 'S', 'K', 'E', 0 ) || Version != ARA_COMP_VERSION )
		return GERROR;
	
	File.read( (char*)&m_FPS, sizeof(float) );
	File.read( (char*)&m_Globe, sizeof(D3DXMATRIX) );
	File.read( (char*)&m_SkeletalTotalNum, sizeof(m_SkeletalTotalNum) );

	m_SkeletalRoot.BinLoad( File );

	int Size;
	int i;
	File.read( (char*)&Size, sizeof(Size) );
	m_SkeletalList.resize( Size, NULL );
	for( i = 0; i < (int)m_SkeletalList.size(); i++ )
	{
		m_SkeletalList[i] = new CResSkeletalUnit;
		m_SkeletalList[i]->BinLoad( File );
	}

	//File.read( (TCHAR*)&Size, sizeof(Size) );
	//m_AnimationList.resize( Size );

	//string szStr = LoadName;
	//TCHAR* szPreName = (TCHAR*)szStr.c_str();
	//szPreName[szStr.size() - 4] = 0;

	//for( i = 0; i < (int)m_AnimationList.size(); i++ )
	//{
	//	File.read( (TCHAR*)&m_AnimationList[i], sizeof(IDNAME) );
	//	TCHAR szAniName[256];
	//	sprintf( szAniName, "%s_%s.ani", szPreName, (LPCTSTR)m_AnimationList[i] );

	//	pProcessor->GetMgr()->ProcessWith( szAniName,LoadName.c_str() );
	//}

	return 0;
}

int CResSkeletalUnit::BinLoad( ifstream& File )
{
	int i;
	File.read( (char*)&m_InverseMatrix, sizeof( D3DXMATRIX ) );

	int Size;
	File.read( (char*)&Size, sizeof( Size ) );
	m_SocketList.resize( Size );
	for( i = 0; i < (int)m_SocketList.size(); i++ )
	{
		File.read( (char*)&m_SocketList[i].m_SocketName, sizeof( IDNAME ) );
		File.read( (char*)&m_SocketList[i].m_Socket, sizeof( D3DXMATRIX ) );
	}
	
	return 0;
}

// -----------------------------------------------------------------------------

CAraProcessor::CAraProcessor(void)
{
}

CAraProcessor::~CAraProcessor(void)
{
}

bool CAraProcessor::RunProcess( const char* szFilename )
{
	CSkeFrame Ske;
	ChangeToSrcDir();
	if( FAILED( Ske.BinLoad( szFilename, this ) ) )
	{
		return false;
	}
	
	m_pMgr->PostProcessed(szFilename);
	
	return true;
}
