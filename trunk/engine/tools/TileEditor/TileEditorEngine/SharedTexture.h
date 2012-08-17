//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Render.h"

using namespace std;

class CSharedTexture
{
public:
	CSharedTexture() : m_nCount(1),m_dwTextureHandle(INVALID) { }
	~CSharedTexture() {}

private:
	int m_nCount;
	string m_strFilename;
	DWORD m_dwTextureHandle;
	static map<string, CSharedTexture*>	m_map;

	DWORD LoadTexture(const char * strFilename)
	{
		return m_dwTextureHandle = bly::rd::CRenderDevice::GetInstance()->CreateTexture(strFilename);
	}

public:
	void Grab() { ++m_nCount; }
	void Drop() 
	{ 
		--m_nCount; 
		if ( m_nCount == 0 ) 
		{ 
			bly::rd::CRenderDevice::GetInstance()->DestroyTexture(m_dwTextureHandle);
			m_map.erase(m_map.find(m_strFilename)); delete this;
		} 
	}

	DWORD GetTextureHandle() { return m_dwTextureHandle; }
	
	static const char * GetTextureFileNameByTextureHandle(int n)
	{
		map<string, CSharedTexture*>::iterator iter = m_map.begin();

		for ( ; iter != m_map.end(); ++iter )
		{
			if ( iter->second->m_dwTextureHandle == n )
				return iter->first.c_str();
		}

		return NULL;
	}

	static CSharedTexture * GetTexture(std::string strFilename)
	{
		map<string,CSharedTexture *>::iterator iter;

		iter = m_map.find(strFilename);

		if ( iter != m_map.end() )
		{
			iter->second->Grab();
			return iter->second;
		}
		else
		{
			CSharedTexture * t = new CSharedTexture;
			m_map[strFilename] = t;
			t->SetFilename(strFilename);
			t->LoadTexture(strFilename.c_str());
			return t;
		}
	}

	static void CSharedTexture::Clear()
	{
		map<string, CSharedTexture*>::iterator iter = m_map.begin();
		
		for ( ; iter != m_map.end(); ++iter )
		{
			if ( iter->second->m_dwTextureHandle != INVALID )
			{
				bly::rd::CRenderDevice::GetInstance()->DestroyTexture(iter->second->m_dwTextureHandle);
				delete iter->second;
			}
		}

		m_map.clear();
	}

	void SetFilename(string strFilename) { m_strFilename = strFilename; }
};
