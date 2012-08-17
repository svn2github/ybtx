#include "stdafx.h"
#include "BaseHelper.h"
#include "CGraphicExp.h"
#include "CSkeletalsMgr.h"
#include "TSqrAllocator.inl"
#include "CPkgDirectory.h"
#include "CDataSources.h"
#include "StringHelper.h"

const string CSkeletalsMgr::st_SkePathConfig = "ske/";
CSkeletalsMgr::CSkeletalsMgr(bool bInit)
:m_bInit(bInit)
{
	m_PoolSize = 0;
	//‘ÿ»Îπ«º‹≥ÿ
	if(m_bInit)SkesPoolLoad();
}

CSkeletalsMgr::~CSkeletalsMgr()
{
	for ( GMap< GString, CSkeletalFrame* >::iterator it = m_SkesContainer.begin(); it != m_SkesContainer.end(); )
		delete (it++)->second;
	m_SkesContainer.clear();
}

//return KB
int CSkeletalsMgr::GetPoolSize()
{
	return (int)(m_PoolSize/1024);
}

bool CSkeletalsMgr::SkesPoolLoad()
{
	CPkgFile PkgFile;

	CPkgDir pkgDir;
	bool bAsyn = CPkgFile::IsEnableAsyncRead();
	CPkgFile::EnableAsyncRead(false);
	pkgDir.Open(PATH_ALIAS_RES.c_str(), st_SkePathConfig.c_str(),"ske");
	for(size_t i=0;i<pkgDir.GetFileNum();i++)
	{
		string filename;
		pkgDir.GetFullFileName(i,filename);
		CSkeletalFrame* pSkeFrame = CDataSources::GetInst()->NewSkeletalFrame( filename.c_str(), PATH_ALIAS_RES );

		if ( !pSkeFrame )
			continue;
		
		int filesize = pSkeFrame->BinLoad();

		if ( SUCCEEDED( filesize ) )
		{
			int pos = filename.rfind("/");
			filename = filename.substr(pos+1,filename.size()-pos);
			AddSkeToContainer(filename.c_str(),pSkeFrame);
			ChangePoolSize(filesize);
			continue;
		}

		delete pSkeFrame;
	}
	CPkgFile::EnableAsyncRead(bAsyn);

	return true;
}


HRESULT CSkeletalsMgr::GetSkeInPool( const TCHAR* szName , CSkeletalFrame** pSkeletalFrame )
{
	if ( szName == 0 || szName[0] == 0 )
	{
		*pSkeletalFrame = NULL;
		return GERROR;
	}

	string result = szName;
	backslash_to_slash(result);
	tolower(result);

	int pos = result.rfind("/");
	result = result.substr(pos+1,result.size()-pos);

	*pSkeletalFrame = GetSkeFromContainer( result );

	if(*pSkeletalFrame)
		return 0;

	//CSkeletalFrame* Temp = new CSkeletalFrame( result );
	//if ( !Temp )
	//	return GERROR;

	//CPkgFile::EnableAsyncRead(false);
	//int filesize = Temp->BinLoad();
	//CPkgFile::EnableAsyncRead(true);

	//if ( SUCCEEDED( filesize ) )
	//{
	//	*pSkeletalFrame = Temp;
	//	AddSkeToContainer(result,Temp);
	//	ChangePoolSize(filesize);
	//	return 0;
	//}

	//delete Temp;
	if(!m_bInit)
	{
		string fileName = st_SkePathConfig + result;
		CSkeletalFrame* pSkeFrame = CDataSources::GetInst()->NewSkeletalFrame( fileName.c_str(),PATH_ALIAS_RES );

		if ( pSkeFrame )
		{
			bool bAsyn = CPkgFile::IsEnableAsyncRead();
			CPkgFile::EnableAsyncRead(false);
			int filesize = pSkeFrame->BinLoad();
			CPkgFile::EnableAsyncRead(bAsyn);
			if ( SUCCEEDED( filesize ) )
			{
				AddSkeToContainer(result.c_str(),pSkeFrame);
				ChangePoolSize(filesize);
				*pSkeletalFrame = pSkeFrame;
				return 0;
			}

			delete pSkeFrame;
		}
	}

	ArtErr("π«º‹Œƒº˛»± ß",result);
	return GERROR;
}

CSkeletalFrame* CSkeletalsMgr::GetSkeFromContainer( const string& Name)
{
	GMap< GString, CSkeletalFrame* >::iterator iter = m_SkesContainer.find( Name.c_str() );
	if ( iter == m_SkesContainer.end() )
	{
		return NULL;
	}
	return iter->second;
}

void CSkeletalsMgr::AddSkeToContainer( const string& Name, CSkeletalFrame* pSke)
{
	m_SkesContainer[ Name.c_str() ] = pSke;
}

void CSkeletalsMgr::ChangePoolSize(const int& size)
{
	m_PoolSize += size;
}