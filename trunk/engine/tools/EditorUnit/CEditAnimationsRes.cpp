#include "stdafx.h"
#include "CEditAnimationsRes.h"
#include "CEditAnimate.h"
#include "CEditSkeletal.h"
#include "GraphicCommon.h"
#include <fstream>
#include <algorithm>
#include "CSkeletalMask.h"
#include "BaseHelper.h"
#include "CEditorConfig.h"
#include "CDataSources.h"

CEditSkeletalFrame::CEditSkeletalFrame( const CSkeletalFrame& Res)
: CSkeletalFrame( Res.GetName(),Res.GetAlias() )
{
}

CEditSkeletalFrame::CEditSkeletalFrame( const string& szName, const wstring& Alias  )
: CSkeletalFrame( szName,Alias )
{
}

bool CEditSkeletalFrame::IsFitForMe( const CEditSkeletalFrame& AG, string& ErrSkeletal )
{
	return ((CEditSkeletal*)m_pSkeletalRoot)->IsFitForMe((CEditSkeletal*)AG.m_pSkeletalRoot,ErrSkeletal);
}

int CEditSkeletalFrame::ReLoad( const string& FileName )
{
	m_szName = FileName.c_str();
	return CSkeletalFrame::BinLoad();
}

void CEditSkeletalFrame::SetCurSelSkelatal(BYTE i)
{
	((CEditSkeletalUnit*)m_SkeletalList[i])->m_IsSelected = true;
}

int	 CEditSkeletalFrame::GetCurSelSkelatal()
{
	for( UINT i = 1; i < m_SkeletalList.size(); i++ )
	{
		if( ((CEditSkeletalUnit*)m_SkeletalList[i])->m_IsSelected )
			return i;
	}
	return -1;
}

bool CEditSkeletalFrame::FillSkeletalMask( SKELETAL_MASK& Mask )
{
	Mask.Empty();
	for ( uint32 i = 0; i < 256; i++ )
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
	return true;
}

int CEditSkeletalFrame::GetSkeletalIdBySocketName( sqr::IDNAME SocketName, int* SocketNum )
{
	for( int i = 0; i < (int)m_SkeletalList.size(); i++ )
	{
		for( int j = 0; j < (int)m_SkeletalList[i]->m_SocketList.size(); j++ )
		{
			if( m_SkeletalList[i]->m_SocketList[j].m_SocketName == SocketName )
			{
				if( SocketNum )
					*SocketNum = j;
				return i;
			}
		}
	}
	return -1;
}

const CEditSkeletalFrame& CEditSkeletalFrame::operator = ( const CEditSkeletalFrame& fARes )
{
	if( this != &fARes )
	{
		Destroy();
		(*(CEditSkeletal*)m_pSkeletalRoot) = *((CEditSkeletal*)fARes.m_pSkeletalRoot);

		m_SkeletalList.resize( fARes.m_SkeletalList.size(), NULL );
		for ( size_t i = 0; i < m_SkeletalList.size(); i++ )
		{
			m_SkeletalList[i] = CDataSources::GetInst()->NewSkeletalUnit();
			*(CEditSkeletalUnit*)m_SkeletalList[i] = *(CEditSkeletalUnit*)fARes.m_SkeletalList[i];
		}
		((CEditSkeletal*)m_pSkeletalRoot)->RefreshSkeletalUnit(m_SkeletalList);
	}
	return *this;
}


BatchState CEditSkeletalFrame::SkeResLoad( const string& FileName,CEditAnimationGroup& EditAniGroup,int& filesize)
{
	BatchState bs;
	CPkgFile PkgFile;
	
	if( PkgFile.Open( PATH_ALIAS_EDITOR.c_str(), FileName.c_str() ) != eFE_SUCCESS )
		return eOpenErr;

	filesize = PkgFile.Size();

	TCHAR* pBuf = new TCHAR[PkgFile.Size()];
	PkgFile.read( pBuf, PkgFile.Size() );
	CRefBufFile* pBufFile = new CRefBufFile( pBuf, PkgFile.Size(), PkgFile.GetCheckCode() );
	pBufFile->AddRef();
	CSkeletalFrame::OnRead(*pBufFile);
	bs = EditAniGroup.AgpResLoad(FileName,*pBufFile);
	pBufFile->Release();

	return bs;
}

bool CEditSkeletalFrame::BinSave( const string& FileName )
{
	SetFileAttributes( FileName.data(), FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE );
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ofstream File( FileName.data(), ios::binary|ios::out );
	locale::global(loc);//恢复全局locale

	DWORD Res = MAKEFOURCC( 'S', 'K', 'E', 0 );
	DWORD Version = ARA_COMP_VERSION;
	File.write( (TCHAR*)&Res, sizeof(DWORD) );
	File.write( (TCHAR*)&Version, sizeof(DWORD) );

	float FPS;
	CMatrix Glob;
	BYTE uSkeletalNum;
	File.write( (TCHAR*)&FPS, sizeof(float) );
	File.write( (TCHAR*)&Glob, sizeof(CMatrix) );
	File.write( (TCHAR*)&uSkeletalNum, sizeof(uSkeletalNum) );

	((CEditSkeletal*)m_pSkeletalRoot)->BinSave( File );

	int Size;
	int i;
	Size = (int)m_SkeletalList.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );
	for( i = 0; i < (int)m_SkeletalList.size(); i++ )
		((CEditSkeletalUnit*)m_SkeletalList[i])->BinSave( File );

	return true;
}

CEditAnimationGroup::CEditAnimationGroup( const CAnimationGroup& Res )
: CAnimationGroup( Res.GetName(), Res.GetAlias() )
{
	SetContainerAndRecycle(Res.Container(), Res.Recycle());
}

CEditAnimationGroup::CEditAnimationGroup(const string& szName, const wstring& Alias )
: CAnimationGroup( szName, Alias )
{
}

void	CEditAnimationGroup::Destroy()
{
	for ( int i = 0; i < (int)m_AnimationList.size(); i++ )
	{
		if(m_AnimationList[i] && m_AnimationList[i]->GetRootGroup() != this)
			SafeRelease(m_AnimationList[i]);
		else
			SafeDelete(m_AnimationList[i]);
	}

	m_AnimationList.clear();
	m_NameList.clear();
	m_AniList.clear();
	m_pSke = NULL;
}

bool CEditAnimationGroup::BatBinSave( const string& FileName )
{
	SetFileAttributes( FileName.data(), FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE );
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ofstream File( FileName.data(), ios::binary|ios::out );
	locale::global(loc);//恢复全局locale

	DWORD Res = MAKEFOURCC( 'A', 'G', 'P', 0 );
	DWORD Version = AGP_COMP_VERSION;
	File.write( (TCHAR*)&Res, sizeof(DWORD) );
	File.write( (TCHAR*)&Version, sizeof(DWORD) );


	m_SkeName = FileName.substr(0,FileName.size()-4).c_str();
	int pos = m_SkeName.rfind("/");
	if(pos!=string::npos)
		m_SkeName = m_SkeName.substr(pos+1,m_SkeName.size());
	if(m_SkeName.find("rlm")!=string::npos)
		m_SkeName = m_SkeName.substr(0,3);
	if(m_SkeName.find("rlw")!=string::npos)
		m_SkeName = m_SkeName.substr(0,3);
	if(m_SkeName.find("srm")!=string::npos)
		m_SkeName = m_SkeName.substr(0,3);
	m_SkeName += ".ske";

	int dataSize = (int)m_SkeName.size();
	File.write( (TCHAR*)&dataSize, sizeof(dataSize) );
	File.write( m_SkeName.data(), dataSize );
	
	int Size;
	int i;

	Size = (int)m_AnimationList.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );

	TCHAR* szPreName = (TCHAR*)FileName.c_str();
	szPreName[FileName.size() - 4] = 0;

	for( i = 0; i < (int)m_AnimationList.size(); i++ )
	{
		sqr::IDNAME Name = m_AnimationList[i]->GetName();
		File.write( (TCHAR*)(&Name), sizeof(sqr::IDNAME) );
	}
	return true;
}

bool CEditAnimationGroup::BinSave( const string& FileName )
{
	SetFileAttributes( FileName.data(), FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE );
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ofstream File( FileName.data(), ios::binary|ios::out );
	locale::global(loc);//恢复全局locale

	DWORD Res = MAKEFOURCC( 'A', 'G', 'P', 0 );
	DWORD Version = AGP_COMP_VERSION;
	File.write( (TCHAR*)&Res, sizeof(DWORD) );
	File.write( (TCHAR*)&Version, sizeof(DWORD) );

	//m_SkeName = FileName.substr(0,FileName.size()-4).c_str();

	//replace(m_SkeName.begin(), m_SkeName.end(), '\\', '/');
	//int pos = m_SkeName.rfind("/");
	//if(pos!=string::npos)
	//	m_SkeName = m_SkeName.substr(pos+1,m_SkeName.size());

	//if(m_SkeName.find("rlm")!=string::npos)
	//	m_SkeName = m_SkeName.substr(0,3);
	//if(m_SkeName.find("rlw")!=string::npos)
	//	m_SkeName = m_SkeName.substr(0,3);
	//if(m_SkeName.find("srm")!=string::npos)
	//	m_SkeName = m_SkeName.substr(0,3);
	//m_SkeName += ".ske";

	int dataSize = (int)m_SkeName.size();
	File.write( (TCHAR*)&dataSize, sizeof(dataSize) );
	File.write( m_SkeName.data(), dataSize );

	int Size;
	int i;

	Size = (int)m_AnimationList.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );

	TCHAR* szPreName = (TCHAR*)FileName.c_str();
	szPreName[FileName.size() - 4] = 0;

	for( i = 0; i < (int)m_AnimationList.size(); i++ )
	{
		sqr::IDNAME Name = m_AnimationList[i]->GetName();
		File.write( (TCHAR*)(&Name), sizeof(sqr::IDNAME) );

		TCHAR szAniName[256];
		sprintf_s( szAniName, "%s_%s.ani", szPreName, (LPCTSTR)Name );
		((CEditAnimate*)m_AnimationList[i])->BinSave( szAniName );
	}
	return true;
}

bool CEditAnimationGroup::ReLoad( const string& FileName )
{
	return true;
}

const CEditAnimationGroup& CEditAnimationGroup::operator = ( const CEditAnimationGroup& fARes )
{
	if( this != &fARes )
	{
		Destroy();

		m_NameList = fARes.m_NameList;
		m_pSke = fARes.m_pSke;

		m_AnimationList.resize( fARes.m_AnimationList.size(), NULL );
		for ( size_t i = 0; i < m_AnimationList.size(); i++ )
		{
			m_AnimationList[i] = fARes.m_AnimationList[i];
			m_AnimationList[i]->AddRef();
		}
	}
	return *this;
}

BatchState CEditAnimationGroup::AgpResLoad(const string& filename,CRefBufFile& GraphicFile)
{
	if (GraphicFile.GetBufSize() <= GraphicFile.Tell())
		return eTrans;

	m_szName = filename.c_str();
	
	int Size;
	int i;
	GraphicFile.read( &Size, sizeof(Size) );
	m_AnimationList.resize( Size, NULL );

	m_NameList.clear();
	TCHAR szAniName[256];
	TCHAR* szChild = szAniName + m_szName.size() - 3;
	strncpy_s( szAniName, m_szName.c_str(), m_szName.size() - 4 );
	szChild[-1] = '_';

	CAnimate* pAnimate = NULL;
	for ( i = 0; i < (int)m_AnimationList.size(); i++ )
	{
		GraphicFile.read( szChild, sizeof(IDNAME) );
		IDNAME Name = *(IDNAME*)szChild;

		m_NameList[szChild] = i;
		strcat_s( szChild,128,".ani" );

		pAnimate = CDataSources::GetInst()->NewAnimate(this,szAniName, Name);//new CAnimate(this, szAniName, Name );
		pAnimate->AddRef();
		//pAnimate->CreateResFile();
		m_AnimationList[i] = pAnimate;
	}

	return eNoTrans;
}