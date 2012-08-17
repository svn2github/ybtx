#include "stdafx.h"
#include "CResourceContainer.h"
#include "BaseHelper.h"
#include "ErrLogHelper.h"
#include "CGraphicExp.h"
#include "CodeCvs.h"
#include "TSqrAllocator.inl"
#include "CBufFile.h"
#include "StringHelper.h"
#include "DebugHelper.h"
#include "GraphicBase.h"

#define VEDIO_MEM_POOL_SIZE		0x6000000
#define SYSTEM_MEM_POOL_SIZE	0x2000000

const GString g_szType[eRT_eCount] = {	"0", "1", "2", "3", "4", "5", "6" };
GString Strlwr( const GString& str, EResType eType )
{
	if(eType == eRT_eCount)
		return str;

	GString NewStr = g_szType[eType] + str;
	for ( int i = 0; i < (int)NewStr.length(); i++ )
		if ( NewStr[i] >= 'A' && NewStr[i] <= 'Z' )
			NewStr[i] = NewStr[i] - 'A' + 'a';
	return NewStr;
}

#pragma warning(disable:4101)

CBaseGraphicRead::CBaseGraphicRead( const string& szName, const wstring& szAlias ) 
	: m_szName( szName.c_str() )
	, m_szAlias( szAlias.c_str() )
{
	m_ppMyself  = new CBaseGraphicRead*;
	*m_ppMyself = NULL;
}

CBaseGraphicRead::~CBaseGraphicRead()
{
	if ( m_ppMyself && *m_ppMyself )
		*m_ppMyself = NULL;
	else
		GfkSafeDelete( m_ppMyself );
}

void CBaseGraphicRead::OnReadAsyn( void* pBaseRead, CRefBufFile& GraphicFile )
{
	CBaseGraphicRead** ppReader = (CBaseGraphicRead**)pBaseRead;
	if ( *ppReader )
	{		
		GString szFileName = (*ppReader)->m_szName;
		SQR_TRY	
		{
			(*ppReader)->OnRead( GraphicFile );
		}
		SQR_CATCH(exp)
		{
			GfkLogPkgErr(exp, szFileName.c_str(), GraphicFile);
		}
		SQR_TRY_END;

		if( *ppReader )
			(*ppReader)->m_ppMyself = NULL;
	}
	GfkSafeDelete( ppReader );
}

int32 CBaseGraphicRead::CreateResFile()
{
	SQR_TRY	
	{
		*m_ppMyself = this;
		int32 n = CPkgFile::ReadWholeFile( m_szAlias.c_str(), m_szName.c_str(), m_ppMyself, OnReadAsyn );

		if ( n < 0 )
		{	
			*m_ppMyself = NULL;

			ostringstream strm;
			strm<<"CreateResFile 打开文件错误"<<m_szName;
			ArtErr(strm.str());
		}

		return n;
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("图形资源创建异常");
		GfkLogExp(exp);
	}
	SQR_TRY_END;
	return -1;
}

void CBaseGraphicRead::SetValid()
{
	GfkSafeDelete( m_ppMyself );
}

CResMgr::CResMgr()
		: m_SystemMemRecycle( SYSTEM_MEM_POOL_SIZE )
		, m_VideoMemRecycle( VEDIO_MEM_POOL_SIZE )
{
}

CResUnit::CResUnit( const string& szName, const wstring& szAlias, EResType eType )
		: CBaseGraphicRead( szName,szAlias )
		, m_pContainer( NULL )
		, m_pRecycle( NULL )
		, m_uSize(0)
		, m_eType(eType)
		, m_IsRecycle(true)
		, m_bInRecycle(false)
{
};

CResUnit::~CResUnit()
{
	if ( m_pContainer )
		m_pContainer->Erase( m_szName.c_str(), m_eType );
	if ( m_pRecycle )
		m_pRecycle->Erase( m_szName.c_str(), m_eType );
}

void CResUnit::FreeRes()
{
	if ( !m_pContainer || !m_pRecycle || !m_IsRecycle )
	{
		delete this;
		return;
	}
	m_pContainer->Erase( m_szName.c_str(), m_eType );
	m_pRecycle->AddResToContainer( m_szName.c_str(), this, m_eType );
	m_bInRecycle = true;
	_FreeRes();
}

void CResUnit::Reclaim()
{
	bool isRecy = m_pRecycle->Erase( m_szName.c_str(), m_eType );
	m_pContainer->AddResToContainer( m_szName.c_str(), this, m_eType );
	m_bInRecycle = false;
	if(isRecy)
		_Reclaim();
	AddRef();
}

int32 CResUnit::CreateResFile()
{
	int32 n = CBaseGraphicRead::CreateResFile();
	if(GetBufSize()==0)
		SetBufSize( max( n, 0 ) );
	return n;
}

void CResUnit::SetBufSize( uint32 uSize )
{
	if( m_pRecycle )
		m_pRecycle->ChangeSize( int32(uSize)-int32(m_uSize) );
	m_uSize = uSize;
}
//--------------------------------------------------------------------------------------------------------------
CResourceContainer::~CResourceContainer()
{
	if ( !m_ResContainer.empty() )
	{
		OutputDebugStringA( "以下资源没有释放\n" );
	}

	GMap< GString, CResUnit* >::iterator Res = m_ResContainer.begin();
	for ( ; Res != m_ResContainer.end(); Res++ )
		OutputDebugStringA( ( Res->first + "\n" ).c_str() );
}

CResUnit* CResourceContainer::GetResFromContainer( const string& Name, EResType eType )
{
	GMap< GString, CResUnit* >::iterator Res = m_ResContainer.find( Strlwr( Name.c_str(), eType ) );
	if ( Res == m_ResContainer.end() )
		return NULL;

	return Res->second;
}

void CResourceContainer::AddResToContainer( const string& Name, CResUnit* Res, EResType eType )
{
	m_ResContainer[ Strlwr( Name.c_str(), eType ) ] = Res;
}

void CResourceContainer::Erase( const string& Name, EResType eType )
{
	m_ResContainer.erase( Strlwr( Name.c_str(), eType ) );
}

//--------------------------------------------------------------------------------------------------------------
void CResourceRecycle::Clear()
{
	while(m_ResPool.size() != 0)
	{
		for ( RecyclePool::iterator it = m_ResPool.begin(); it != m_ResPool.end(); )
		{
			delete *(it++);
		}
	}
}

void CResourceRecycle::OptimizeRecycle(void)
{
	RecyclePool::iterator it = m_ResPool.begin();
	CResUnit* tmpDelRes = NULL;

	size_t CheckSize = 0;
	string	ResName;
	for(;;)
	{
		SQR_TRY
		{
			for ( ; it != m_ResPool.end() && m_uCurBufSize > m_uMaxBufSize; )
			{
				tmpDelRes = (*it++);
				if(tmpDelRes->IsValid())
				{
					tmpDelRes->SetBufSize(0);

					CResUnit* tRes = tmpDelRes;
					ResName = tRes->GetName();

					tmpDelRes  = NULL;
					CheckSize = m_ResPool.size();

					Erase( tRes->m_szName.c_str(), tRes->m_eType );
					tRes->m_pRecycle = NULL;

					if( m_ResPool.size() < CheckSize)
						delete tRes;
					else
						GfkLogErr("回收站异常","没能正常删除节点");
				}	
			}
			break;
		}
		SQR_CATCH(exp)
		{
			if(tmpDelRes==NULL)
				exp.AppendType("OptimizeRecycle删除时异常");
			exp.AppendMsg(ResName.c_str());		
			GfkLogExp(exp);
		}
		SQR_TRY_END;
	}
}

CResourceRecycle::~CResourceRecycle()
{
}

CResUnit* CResourceRecycle::GetResFromContainer( const string& Name, EResType eType )
{
	ResourceContainer::iterator Res = m_ResContainer.find( Strlwr( Name.c_str(), eType ) );
	if ( Res == m_ResContainer.end() )
		return NULL;

	return *(Res->second);
}

void CResourceRecycle::AddResToContainer( const string& Name, CResUnit* Res, EResType eType )
{
	GString StrL = Strlwr( Name.c_str(), eType );
	ResourceContainer::iterator It = m_ResContainer.find( StrL );
	if ( It == m_ResContainer.end() )
	{
		m_ResPool.push_back(Res);
		m_ResContainer[StrL] = --m_ResPool.end();
	}
}

bool CResourceRecycle::Erase( const string& Name, EResType eType )
{
	GString StrL = Strlwr( Name.c_str(), eType );
	ResourceContainer::iterator Res = m_ResContainer.find( StrL );
	if ( Res != m_ResContainer.end() )
	{
		m_ResPool.erase(Res->second);
		m_ResContainer.erase(Res);
		return true;
	}
	return false;
}

void CResourceRecycle::ChangeSize( int32 nSize )
{
	m_uCurBufSize += nSize;
}

uint32 CResMgr::GetVedioMemSize(void)
{
	return VEDIO_MEM_POOL_SIZE;
}

void CResUnit::SetContainerAndRecycle( CResourceContainer* pContainer, CResourceRecycle* pRecycle )
{
	Ast(m_pRecycle==NULL&&m_pContainer==NULL);
	m_pContainer	= pContainer;
	m_pRecycle		= pRecycle;

	if ( m_pContainer )
		m_pContainer->AddResToContainer( m_szName.c_str(), this, m_eType );
}