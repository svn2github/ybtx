#include "stdafx.h"
#include "CLuaProcessor.h"
#include "CScript.h"
#include "CodeCvs.h"


CLuaProcessor::CLuaProcessor(void)
{
}

CLuaProcessor::~CLuaProcessor(void)
{
}

bool CLuaProcessor::RunProcess( const char* szFilename )
{
	string szSrcFilename = m_pMgr->GetSrcDir() + "/" + szFilename;
	string szDstFilename = m_pMgr->GetDstDir() + "/" + szFilename;

	// 编译lua
	if ( m_pMgr->GetCompileLua() )
	{
		// 创建lua源文件路径在目标路径下的相对路径
		string szDstDir = szFilename;
		szDstDir = szDstDir.substr( 0, szDstDir.rfind('/') );
		_chdir( m_pMgr->GetDstDir().c_str() );
		if ( !MakeDirectory(gbk_to_utf16(szDstDir).c_str()) )
		{
			ostringstream strm;
			strm<<"create directory \""<< szDstDir << "\" failed.";
			GenErr(strm.str());
		}

		if ( !CopyFile(szSrcFilename.c_str(), szDstFilename.c_str(), FALSE ) )
			printf("copy %s to %s failed!!", szSrcFilename.c_str(), szDstFilename.c_str());

		//// 将lua源文件copy到目标路径下已经创建好的lua文件夹中
		//ifstream is( szSrcFilename.c_str(), ios::binary );
		//ofstream os( szDstFilename.c_str(), ios::binary );

		//if( !is.good() )
		//{
		//	ostringstream strm;
		//	strm<<"open file \""<< szSrcFilename << "\" failed.";
		//	GenErr(strm.str());
		//}	
		//if ( !os.good() )
		//{
		//	ostringstream strm;
		//	strm<<"open file \""<< szDstFilename << "\" failed.";
		//	GenErr(strm.str());
		//}
		//os << is.rdbuf();
		//os.close();
		//is.close();

		char* szArgv[4] = { "CLuaProcessor", "-o", (char*)szDstFilename.c_str(), (char*)szSrcFilename.c_str() };
		if ( CScript::Compile( 4, szArgv ) )
		{
			ostringstream strm;
			strm<<"Compiling lua file \""<<szSrcFilename<<"\" failed.";
			GenErr(strm.str());
		}
	}

	m_pMgr->PostProcessed(szFilename);
	return true;
}
