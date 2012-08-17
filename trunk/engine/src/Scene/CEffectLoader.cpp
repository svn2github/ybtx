#include "stdafx.h"
#include "CEffectLoader.h"
#include "IEffect.h"
#include "IEffectManager.h"
#include "TSqrAllocator.inl"

CEffectLoader::CEffectLoader()
{
	IntInst() = this;
	m_hWnd = NULL;
}

CEffectLoader::~CEffectLoader()
{
	string Info = "";
	EGroupMap::iterator itr = m_MEffectGroup.begin();
	EGroupMap::iterator eit = m_MEffectGroup.end();
	while(itr != eit)
	{
		Info = Info + itr->first.c_str();
		Info += "\n";
		++itr;
	}

	if(Info.size()>0 && m_hWnd!=NULL)
	{
		Info += "is not released!!";
		::MessageBox(m_hWnd,Info.c_str(),"ÌבÊ¾",0);
	}

	IntInst() = NULL;
}

CEffectLoader* CEffectLoader::Inst()
{
	return IntInst();
}

CEffectLoader*& CEffectLoader::IntInst()
{
	static CEffectLoader*	ls_pEffectLoader=NULL;
	return ls_pEffectLoader;
}

void CEffectLoader::SetGroupHandle(HWND hwnd)
{
	m_hWnd = hwnd;
}

void CEffectLoader::AddEftGroup( const char* szFileName )
{
	if (!szFileName)
		return;
	if (m_MEffectGroup.find(szFileName) != m_MEffectGroup.end())
		return;
	
	IEffectGroup*		eftGroup = NULL;
	if( FAILED( IEffectManager::GetInst()->CreateEffectGroup( szFileName, &eftGroup ) ) )
	{
		SafeRelease( eftGroup );
		return;
	}
	else
		m_MEffectGroup[szFileName] = eftGroup;

}

void CEffectLoader::Clear()
{
	EGroupMap::iterator itr = m_MEffectGroup.begin();
	EGroupMap::iterator eit = m_MEffectGroup.end();
	while(itr != eit)
	{
		SafeRelease( itr->second );
		++itr;
	}
	m_MEffectGroup.clear();
}

void CEffectLoader::FreeEftGroup( const char* szFileName )
{
	EGroupMap::iterator it = m_MEffectGroup.find( szFileName );

	if( it != m_MEffectGroup.end() )
	{
		SafeRelease( it->second );
		m_MEffectGroup.erase(it);
	}
}