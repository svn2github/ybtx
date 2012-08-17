#include "stdafx.h"
#include "CSkeletalFrame.h"
#include "CSkeletalMask.h"
#include "BaseHelper.h"
#include "GraphicCommon.h"
#include "SyncSystem.h"
#include "TSqrAllocator.inl"
#include "CDataSources.h"

CSkeletalFrame::CSkeletalFrame(const string& szName, const wstring& Alias) 
: CBaseGraphicRead( szName,Alias )
{
	m_pSkeletalRoot = CDataSources::GetInst()->NewSkeletal(); 
}

CSkeletalFrame::~CSkeletalFrame()
{ 
	Destroy(); 
	GfkSafeDelete(m_pSkeletalRoot);
}

void CSkeletalFrame::GetIDBySocketName( int& SkeletalID, int& SocketID, IDNAME SocketName )
{
	SkeletalID = GetSkeletalIDBySocketName( SocketName, SocketID );
}

int CSkeletalFrame::GetSkeletalIDByName( const char* SkeletalName )
{
	return GetRootSkeletal().GetSkeletalIDByName( SkeletalName );
}

int CSkeletalFrame::GetSkeletalTotalNum()
{
	return GetSkeletalNum();
}

int CSkeletalFrame::GetSkeletalIDByIndex( const int Index )
{
	if ( Index >= (int)m_SkeletalList.size() ) return -1;
	return m_SkeletalList[Index]->m_pSkeletal->GetSkeletalID();
}

int CSkeletalFrame::GetSkeletalChildNum( uint32 SkeletalID )
{
	return SkeletalID < m_SkeletalList.size() ? m_SkeletalList[SkeletalID]->m_pSkeletal->GetChildNum() : 0;
}

int CSkeletalFrame::GetSkeletalChildID( uint32 SkeletalID, uint32 ChildNum )
{
	CSkeletal* pSkeletal = SkeletalID < m_SkeletalList.size() ? m_SkeletalList[SkeletalID]->m_pSkeletal : NULL;
	return pSkeletal && ChildNum < pSkeletal->GetChildNum() ? pSkeletal->GetChild( ChildNum )->GetSkeletalID() : -1;
}

int CSkeletalFrame::GetSocketNum( uint32 SkeletalID )
{
	return SkeletalID < GetSkeletalNum() ? m_SkeletalList[SkeletalID]->m_SocketList.size() : 0;
}

const char* CSkeletalFrame::GetSkeletalName( uint32 SkeletalID )
{
	static GString str = "";
	return SkeletalID < m_SkeletalList.size() ? m_SkeletalList[SkeletalID]->m_pSkeletal->GetName() : str.c_str();
}

const char* CSkeletalFrame::GetSocketName( uint32 SkeletalID, uint32 SocketID )
{
	return SkeletalID < GetSkeletalNum() ? (const char*)(  m_SkeletalList[SkeletalID]->m_SocketList[SocketID].m_SocketName ) : NULL;
}

int CSkeletalFrame::GetSkeletalIDBySocketName( IDNAME SocketName, int& SocketNum )
{
	for ( int i = 0; i < (int)m_SkeletalList.size(); i++ )
	{
		for ( int j = 0; j < (int)m_SkeletalList[i]->m_SocketList.size(); j++ )
		{
			if ( m_SkeletalList[i]->m_SocketList[j].m_SocketName == SocketName )
			{
				SocketNum = j;
				return i;
			}
		}
	}
	return -1;
}

//…Ë÷√”––ßπ«˜¿
bool CSkeletalFrame::SetSkeletalMask( SKELETAL_MASK& Mask )
{
	for ( uint32 i = 0; i < m_SkeletalList.size(); i++ )
	{
		if ( m_SkeletalList[i]->m_SocketList.size() )
			Mask.AddSkeletal( i );
	}

	for ( uint32 i = 0; i < 256; i++ )
	{
		if ( Mask.HaveSkeletal(i) )
		{
			if( i < m_SkeletalList.size() )
			{
				uint32 n = i;
				while ( n )
				{
					Mask.AddSkeletal( n );
					n = m_SkeletalList[n]->m_pSkeletal->GetParentSkeletalID();
				}
			}
			else
				return false;
		}
	}
	return true;
}

void  CSkeletalFrame::Destroy()
{
	for (int i = 0; i < (int)m_SkeletalList.size(); i++ )
		delete m_SkeletalList[i];

	m_SkeletalList.clear();
	m_pSkeletalRoot->Destroy();
}


int CSkeletalFrame::BinLoad(void)
{
	return CreateResFile();
}

void CSkeletalFrame::OnRead( CRefBufFile& GraphicFile )
{
	Ast( m_SkeletalList.empty() );

	uint32 Res;
	uint32 Version;
	GraphicFile.read( &Res, sizeof(uint32) );
	GraphicFile.read( &Version, sizeof(uint32) );
	bool bARPMark = true;

	if ( Res != MAKEFOURCC( 'S', 'K', 'E', 0 ) || Version != ARA_COMP_VERSION )
		return;

	float FPS;
	CMatrix Glob;
	BYTE uSkeletalNum;
	GraphicFile.read( &FPS, sizeof(float) );
	GraphicFile.read( &Glob, sizeof(CMatrix) );
	GraphicFile.read( &uSkeletalNum, sizeof(uSkeletalNum) );

	CSkeletal* pSkeletals[256];
	m_pSkeletalRoot->BinLoad( GraphicFile, 0, pSkeletals );

	int Size;
	int i;
	GraphicFile.read( &Size, sizeof(Size) );
	m_SkeletalList.resize( Size, NULL );
	for ( i = 0; i < (int)m_SkeletalList.size(); i++ )
	{
		m_SkeletalList[i] = CDataSources::GetInst()->NewSkeletalUnit();
		m_SkeletalList[i]->BinLoad( GraphicFile );
		m_SkeletalList[i]->m_pSkeletal = pSkeletals[i];
	}
}

bool CSkeletalFrame::IsValid()
{
	return CBaseGraphicRead::IsValid(); 
}

void CSkeletalFrame::Free(void)
{
	//FreeRes();
}
