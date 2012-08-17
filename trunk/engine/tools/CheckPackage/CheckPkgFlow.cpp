#include "stdafx.h"
#include "CheckPkgFlow.h"

CheckPkgFlow::CheckPkgFlow()
{

}

CheckPkgFlow::~CheckPkgFlow()
{

}

void CheckPkgFlow::ProcessFlow( int argc, char* argv[] )
{
	_ProcessParameter( argc, argv );
	_RegisterPrcessor();
	_ProcessPath();
	_Process();
	_UnRegisterPrcessor();
	if ( !m_beProcessOk )
		return;				// 失败

	_BuildErrorFile();		// 输出缺失的文件
	_BuildPkgFile();		// 成功生成PKG LOG 
	_BuildSerPkgFile();
}

void CheckPkgFlow::_ProcessParameter( int argc, char *argv[] )
{
	m_PathMgr.SetCurPath( gbk_to_utf16(argv[1]).c_str() );
	m_ResMgr.SetSrcDir( utf16_to_gbk(m_PathMgr.GetCurPath()).c_str() );

	CPkgFile::AddLoadPath(L"", gbk_to_utf16(m_ResMgr.GetSrcDir()).c_str());
	CPkgFile::AddLoadPath(L"res", gbk_to_utf16(m_ResMgr.GetSrcDir()).c_str());

	m_PathMgr.SetCurPath( gbk_to_utf16(argv[2]).c_str() );
	m_ResMgr.SetDstDir( utf16_to_gbk(m_PathMgr.GetCurPath()).c_str() );

	m_PackType = 2;					// 1代表打包server，2代表打包client
	m_ResMgr.SetCompileLua(true);		// lua文件是否编译
	for( int32 i = 3; argv[i]; i += 2 )
	{
		if( !strcmp( argv[i], "-d" ) )
			m_ResMgr.AddPrcDir( argv[ i + 1 ] );
		if ( !strcmp( argv[i], "-s" ) )
			m_ResMgr.AddSerDir( argv[ i + 1 ] );
		if ( !strcmp( argv[i], "-n" ) )
			m_ResMgr.SetNewSerFile( argv[ i + 1 ] );
		if( !strcmp( argv[i], "-t" ) )
			m_ResMgr.AddExType( argv[ i + 1 ] );
		if( !strcmp( argv[i], "-f" ) )
			m_ResMgr.AddExFile( argv[ i + 1 ] );
		if( !strcmp( argv[i], "-u" ) )
		{
			string szUpdateFile = m_ResMgr.GetDstDir() + "\\" + "update.ini";
			SureCreateFileDirectory( szUpdateFile.c_str() );
			FILE* fp = fopen( szUpdateFile.c_str(), "w" );
			if( fp )
			{
				fprintf( fp, "[Setting]\n" );
				fprintf( fp, "Version = %s\n", argv[ i + 1 ] );
				fclose( fp );
			}
		}

		// 命令行参数 -p server/client
		if (!strcmp( argv[i], "-p"))
		{
			if (!strcmpi( argv[i+1], "server"))
				m_PackType = 1;
		}

		// 命令行参数 -c no(lua编译专用)
		if (!strcmp( argv[i], "-c"))
		{
			if (!strcmpi( argv[i+1], "no"))
				m_ResMgr.SetCompileLua(false);
		}
	}
}

void CheckPkgFlow::_RegisterPrcessor()
{
	// 根据打包的是server或client初始化不同的处理类，数组最后一个为NULL
	memset(m_pProcessor, 0, MAX_PRIORITY_LEVEL * sizeof(IResProcessor*));
	if (m_PackType == 1)
		m_pProcessor[0] = new SMapProcessor;
	else
		m_pProcessor[0] = new CMapProcessor;

	m_pProcessor[1] = new CArsProcessor;
	m_pProcessor[2] = new CAreProcessor;
	m_pProcessor[3] = new CArpProcessor;
	m_pProcessor[4] = new CAraProcessor;
	m_pProcessor[5] = new CLuaProcessor;
	m_pProcessor[6] = new CDftProcessor;
	m_pProcessor[7] = new CAgpProcessor;
	//m_pProcessor[8] = new CGuiProcessor;


	for( int i = 0; i < MAX_PRIORITY_LEVEL && m_pProcessor[i]; i++ )
	{
		m_ResMgr.RegisterProcessor( m_pProcessor[i], i );
	}
}

void CheckPkgFlow::_ProcessPath()
{
	_chdir( m_ResMgr.GetSrcDir().c_str() );
	m_ResMgr.ProcessPath();
}

void CheckPkgFlow::_Process()
{
	m_beProcessOk = true;

	// 把资源从SrcDir复制到DstDir，需要转换的资源进行转换。
	SQR_TRY
	{
		CDataSources::Create();
		CSkeletalsMgr::Create();

		setlocale(LC_CTYPE, ".936");
		locale::global(locale(".936"));
		m_ResMgr.Process( &m_Handler );
	}
	SQR_CATCH(exp)
	{
		m_beProcessOk=false;
		clog << exp.ErrorMsg() << endl;;
	}
	SQR_TRY_END;
}

void CheckPkgFlow::_UnRegisterPrcessor()
{
	for( int i = 0; i < MAX_PRIORITY_LEVEL; i++ )
	{
		delete m_pProcessor[i];
	}
}

void CheckPkgFlow::_BuildErrorFile()
{
	// 处理完毕后，这里存放了所有转换过程中发现的缺少的文件。
	string szErrLogFile = m_ResMgr.GetDstDir() + "/ErrLog.log";

	if( m_Handler.m_mapMissFile.size() )
	{
		ofstream err_file( szErrLogFile.c_str() );
		if ( err_file.good() )
		{
			err_file << "the follow file can't found:\n";

			map<string,string>::iterator it = m_Handler.m_mapMissFile.begin();
			for( ; it != m_Handler.m_mapMissFile.end(); it++ )
			{
				err_file << it->first << "所属父文件：" << it->second << endl;
			}
		}
	}
}

void CheckPkgFlow::_BuildPkgFile()
{
	string		szSpacer	 = "###"; 
	string		szPkgLogFile = m_ResMgr.GetDstDir() + "/PkgLog.log";
	ofstream	pkg_file( szPkgLogFile.c_str() );

	if(pkg_file.is_open())
		m_ResMgr.ProcessDir(&m_Handler);	// 把收集的文件塞到m_mapProcessFiles中去

	map< string, list<string> >::iterator it = m_Handler.m_mapProcessFiles.begin(); 
	for( ; it != m_Handler.m_mapProcessFiles.end(); it++ )
	{
		string szSourceFile;
		string szPkgName = it->first;
		list<string>::iterator list_iter = it->second.begin();
		list<string>::iterator list_end  = it->second.end();

		for ( ; list_iter != list_end; ++list_iter )
		{
			if ( (*list_iter).rfind(".lua") != string::npos && m_ResMgr.GetCompileLua() )
			{
				szSourceFile = m_ResMgr.GetDstDir() + "/" + *list_iter;
			}
			else
			{
				szSourceFile = m_ResMgr.GetSrcDir() + "/" + *list_iter;
			}
			pkg_file << szPkgName << szSpacer << szSourceFile << endl;
		}
	}
	cout << szPkgLogFile;
}

void CheckPkgFlow::_BuildSerPkgFile()
{
	m_ResMgr.BuildSerDir();
	m_ResMgr.BuildSerLogFile();
}