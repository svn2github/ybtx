#include "stdafx.h"
#include "SQRWndDef.h"
#include "TSqrAllocator.inl"
#include "CSqrAllocPool.h"
#include "CGraphic.h"

#define EMPTY_STR ""

WndResParam::~WndResParam()
{
	UninitTChar();

	for ( UIMap<UIString, WndResParam*>::iterator it  = m_Child.begin(); it != m_Child.end(); it++ )
		delete it->second;
}

void WndResParam::InitTChar()
{
	m_WindowTitle	= EMPTY_STR;
	m_TransTexture	= EMPTY_STR;
	m_szDesc		= EMPTY_STR;
	for (int i = 0; i < eMF_Count; i++)
	{
		m_MsgFx[i]	= EMPTY_STR;
	}
	m_UseClassName	= EMPTY_STR;
	m_UseInfo		= EMPTY_STR;
	m_SourceName    = EMPTY_STR;//add by guo
	m_GroupID		= -1;
}

void WndResParam::UninitTChar()
{
	if ( m_beAucRes )
	{
		if (m_ClassName)
		{
			SqrDelete(m_ClassName, SQRGuiMallocObject::GetPool());
			m_ClassName = NULL;
			SqrDelete(m_TransTexture, SQRGuiMallocObject::GetPool());
			m_TransTexture = NULL;
			SqrDelete(m_WindowName, SQRGuiMallocObject::GetPool());
			m_WindowName = NULL;
		}
	}
	else
	{
		clearTChar(m_ClassName);
		clearTChar(m_WindowTitle);
		clearTChar(m_TransTexture);
		clearTChar(m_SourceName);//add by guo
		clearTChar(m_szDesc);
		for (int i = 0; i < eMF_Count; i++)
		{
			clearTChar(m_MsgFx[i]);
		}
		clearTChar(m_UseClassName);
		clearTChar(m_UseInfo);
	}

}

void WndResParam::clearTChar( char* str)
{
	if (str != "")
	{
		delete[] str;
	}
}

WindowImage::WindowImage() 
: dwLockFlage(0)
, dwColor(0) 
{}

ChildImageList::ChildImageList() 
: uTextColor(0)
, uRimColor(0)
{}

ChildWndInfo::ChildWndInfo() 
: m_uStyleIndex(0)
, m_uLockFlag(LOCK_LEFT|LOCK_TOP )
{};

FlashWndInfo::FlashWndInfo()
: m_count(0)
, m_time(DWORD(1000))
, m_Amin(0.0f)
, m_Amax(1.0f)
, m_iRef(0)
{}

FlashWndInfo::~FlashWndInfo()
{
	Destroy();
}

bool FlashWndInfo::Init(IGraphic* pGraphic, UIString flashname, UIString texname, CFRect Texpos, uint32 time, uint32 ct, float ato, float afrom, CFRect repos)
{
	m_FlashName			= flashname;
	m_image.pTexture	= NULL;
	m_image.texName		= texname;
	m_image.rtTexture	= Texpos;

	m_time				= time;
	m_count				= ct;
	if (ato > afrom)
	{
		m_bAdd = false;			// 透明度减小
		m_Amax = ato;
		m_Amin = afrom;
	}
	else
	{
		m_bAdd = true;			// 透明度增加
		m_Amax = afrom;
		m_Amin = ato;
	}
	m_repos				= repos;
	return true;
}

void FlashWndInfo::Destroy()
{
	SafeRelease( m_image.pTexture );
}

void FlashWndInfo::CreateTexture()
{
	if (m_image.pTexture)
	{
		++m_iRef;
		return;
	}

	ITexture* pTexture = NULL;
	if ( m_image.texName.c_str() && m_image.texName[0] )
	{
		bool bEnableASynRead = CPkgFile::IsEnableAsyncRead();
		CPkgFile::EnableAsyncRead( false );
		HRESULT hr = CGraphic::GetInst()->CreateTexture( PATH_ALIAS_GUITEX.c_str(), m_image.texName.c_str(), &pTexture );
		CPkgFile::EnableAsyncRead( bEnableASynRead );
		if(FAILED(hr))
			OutputDebugString(m_FlashName.c_str());
		if ( !pTexture )
			return;
	}
	m_image.pTexture	= pTexture;
	++m_iRef;
}

void FlashWndInfo::ReleaseTexture()
{
	--m_iRef;
	if (m_iRef <= 0)
		SafeRelease( m_image.pTexture );
}