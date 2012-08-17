#include "stdafx.h"
#include "CAnimationGroup.h"
#include "GraphicCommon.h"
#include "BaseHelper.h"
#include "Algorithm.h"
#include "CAnimation.h"
#include "GraphicCommon.h"
#include "SyncSystem.h"
#include "CSkeletalsMgr.h"
#include "TSqrAllocator.inl"
#include "CDataSources.h"

#pragma warning(disable:4996)


CAnimationGroup::CAnimationGroup(const string& szName, const wstring& Alias)
: CResUnitGroup( eRT_eAgp, szName, Alias )
{
}

//得到动画标号
int CAnimationGroup::GetAniIDbyName( IDNAME Name ) const
{
	GMap<GString,int32>::const_iterator it = m_NameList.find( (const char*)Name );
	return it == m_NameList.end() ? -1 : it->second;
}

CAnimate* CAnimationGroup::GetAniById(uint32 id)
{
	return id < GetAnimateNum() ? m_AnimationList[id] : NULL;
}
CAnimate* CAnimationGroup::GetAniByName(IDNAME name)
{
	return (*this)[GetAniIDbyName(name)];
}

void CAnimationGroup::Destroy()
{
	if ( NULL == Container() )
	{
		for ( int i = 0; i < (int)m_AnimationList.size(); i++ )
			SafeRelease(m_AnimationList[i]);
	}
	else
	{
		for ( int i = 0; i < (int)m_AnimationList.size(); i++ )
			GfkSafeDelete(m_AnimationList[i]);
	}

	m_AnimationList.clear();
	m_NameList.clear();
	m_AniList.clear();
	m_pSke = NULL;
}

int CAnimationGroup::GetAnimationNum( IDNAME AnimationName )
{
	return GetAniIDbyName( AnimationName );
}

int CAnimationGroup::GetAnimationNum()
{
	return GetAnimateNum();
}

IDNAME CAnimationGroup::GetAnimationName( int Num ) const
{
	return (uint32)Num < GetAnimateNum() && NULL!= m_AnimationList[Num] ? m_AnimationList[Num]->GetName() : IDNAME();
};

int CAnimationGroup::GetAnimationMaxFrame( int nAni )
{
	return (uint32)nAni < GetAnimateNum() && NULL!= m_AnimationList[nAni] ? m_AnimationList[nAni]->GetFrameNum() - 1 : 0;
}

float CAnimationGroup::GetGroundSpeed( int nAni )
{
	return (uint32)nAni < GetAnimateNum() && NULL!= m_AnimationList[nAni] ? m_AnimationList[nAni]->GetGroundSpeed() : 0;
}

int CAnimationGroup::GetFrameCountToFrameStr( int nAni, const char* szFrameStr, int nFrameStart, int nFrameEnd, int nStr )
{
	if ( (uint32)nAni >= GetAnimateNum() || !szFrameStr )
		return -1;

	const GMap<WORD,IDNAME>* FrameStr = m_AnimationList[nAni]->GetFrameString();
	GMap<WORD,IDNAME>::const_iterator it = FrameStr->lower_bound( (WORD)nFrameStart );
	for ( int nCurStrFrame = 0; it != FrameStr->end() && it->first <= (WORD)nFrameEnd; ++it )
	{
		if ( !strcmp( it->second, szFrameStr ) )
		{
			if ( nCurStrFrame == nStr )
				return it->first;
			nCurStrFrame++;
		}
	}

	return -1;
}

bool CAnimationGroup::Merge( CAnimationGroup* pGroup )
{
	if(pGroup==NULL)
		return false;

	if(pGroup==this)
		return true;

	GMap<GString,int32>::iterator it,eit = pGroup->m_NameList.end();
	for( it = pGroup->m_NameList.begin(); it != eit; ++it )
	{
		GMap<GString,int32>::iterator mapIt = m_NameList.find(it->first);
		if( mapIt != m_NameList.end() )
		{
			pGroup->m_AnimationList[it->second]->AddRef();
			SafeRelease(m_AnimationList[mapIt->second]);
			m_AnimationList[mapIt->second] = pGroup->m_AnimationList[it->second];
		}
		else
		{
			m_NameList.insert(GMap<GString,int32>::value_type(it->first,m_AnimationList.size()));
			m_AnimationList.push_back(pGroup->m_AnimationList[it->second]);
			pGroup->m_AnimationList[it->second]->AddRef();
		}
	}
	return true;
}

DoAniIsValidResult CAnimationGroup::IsAniValid(const char* szAniName)
{
	GMap<GString,int32>::iterator iter = m_NameList.find(szAniName);
	if( iter != m_NameList.end() )
	{
		int32 id = iter->second;
		//////////////////////////////////////////////////////////////////////////

		if(!m_AnimationList[id]->IsLoaded())
		{
			CAnimate* pAni = m_AnimationList[id];
			pAni->CreateResFile();
		}
		//////////////////////////////////////////////////////////////////////////
		
		if(m_AnimationList[id]->IsValid())
			return eAni_Valid;
		else
			return eAni_InValid;
	}
	else
		return eAni_NoExist;
}

void CAnimationGroup::InsertAni(CAnimate* pAni)
{
	m_AniList.push_back(pAni);
}

bool	CAnimationGroup::IsDefaultAniValid(int id)
{
	if(!m_AnimationList[id]->IsLoaded())
	{	
		CAnimate* pAni = m_AnimationList[id];
		pAni->CreateResFile();
	}	

	return m_AnimationList[id]->IsValid();
}

int CAnimationGroup::BinLoad(void)
{
	int bSucced = 0;
	AddRef();
	if( FAILED(bSucced = CreateResFile()) )
		Release();
	return bSucced;
}

void CAnimationGroup::OnRead( CRefBufFile& GraphicFile )
{
	uint32 Res;
	uint32 Version;
	GraphicFile.read( &Res, sizeof(uint32) );
	GraphicFile.read( &Version, sizeof(uint32) );
	bool bARPMark = true;

	if ( Res != MAKEFOURCC( 'A', 'G', 'P', 0 ) || Version != AGP_COMP_VERSION )
		return;

	if(Version>=10002)
	{
		int dataSize;
		GraphicFile.read((TCHAR*)&dataSize,sizeof(int));
		m_SkeName.resize(dataSize);
		GraphicFile.read((TCHAR*)m_SkeName.data(), dataSize );
	}
	
	CSkeletalsMgr::GetInst()->GetSkeInPool( m_SkeName.c_str(), &m_pSke );

	int Size;

	GraphicFile.read( &Size, sizeof(Size) );
	m_AnimationList.resize( Size, NULL );

	m_NameList.clear();
	TCHAR szAniName[256];
	TCHAR* szChild = szAniName + m_szName.size() - 3;
	strncpy( szAniName, m_szName.c_str(), m_szName.size() - 4 );
	szChild[-1] = '_';

	CAnimate* pAnimate = NULL;
	GString strAniname;
	int i;
	for ( i = 0; i < (int)m_AnimationList.size(); i++ )
	{
		GraphicFile.read( szChild, sizeof(IDNAME) );
		strAniname = szChild;
		IDNAME Name = *(IDNAME*)szChild;

		m_NameList[szChild] = i;
		strcat( szChild, ".ani" );

		pAnimate = CDataSources::GetInst()->NewAnimate(this, szAniName, Name);//new CAnimate(this, szAniName, Name );
		m_AnimationList[i] = pAnimate;

		if(strAniname.find("stand")!=GString::npos)//这个地方以后要改,以后会在插件到处调整骨骼动画的加载顺序和批量处理老资源,现在临时在这里过滤下把stand在最先加载
			InsertAni(m_AnimationList[i]);
	}
	LoadNextAni();
	Release();
}

void CAnimationGroup::LoadNextAni()
{
	CSyncSystem::TryToStop();
	if( !m_AniList.empty())
	{
		CAnimate* pAni = m_AniList.front();
		m_AniList.erase( m_AniList.begin() );
		if(pAni&&!pAni->IsValid()&&!pAni->IsLoaded())
		{
			pAni->CreateResFile();
		}
		return;
	}

	for (int i = 0; i < (int)m_AnimationList.size(); i++ )
	{
		if(m_AnimationList[i]&&!m_AnimationList[i]->IsValid()&&!m_AnimationList[i]->IsLoaded())
		{			
			m_AnimationList[i]->CreateResFile();
			return;
		}
	}
}

bool CAnimationGroup::IsValid()
{
	return CResUnit::IsValid(); 
}

CSkeletalFrame*	CAnimationGroup::GetSkeletalFrame()
{
	return m_pSke;
}
