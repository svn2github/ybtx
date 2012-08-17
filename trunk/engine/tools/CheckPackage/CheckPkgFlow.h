/*	checkpkgflow  生成打包文件的整个流程类
	从 checkpackage 中抽离出来
*/
#pragma once
#include "CResProcessorMgr.h"
#include "CAraProcessor.h"
#include "CAreProcessor.h"
#include "CArpProcessor.h"
#include "CMapProcessor.h"
#include "CArsProcessor.h"
#include "CLuaProcessor.h"
#include "CDftProcessor.h"
#include "CAgpProcessor.h"
#include "CSkeletalsMgr.h"
#include "CGuiProcessor.h"
#include "SMapProcessor.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CDataSources.h"
#include "CodeCvs.h"

struct CProcessHandler : public IProcessHandler
{
	map< string, list<string> > m_mapProcessFiles;
	map< string, string >		m_mapMissFile;

	void OnProcessed( const char* szFileName, const char* szParentFileName, bool bMiss )
	{
		if( !bMiss )
		{
			string sName = szParentFileName;
			size_t i = 0;
			while( i < sName.size() && sName[i] != '/' )
			{
				i++;
			}
			sName[i] = 0;

			m_mapProcessFiles[sName.c_str()].push_back( szFileName );
		}
		else
		{
			// 搜索路径也没有
			m_mapMissFile[szFileName] = szParentFileName;
		}
	}
};

class CheckPkgFlow
{
public:
	CheckPkgFlow();
	~CheckPkgFlow();

	// 传递所有参数过来
	void	ProcessFlow( int argc, char* argv[] );
private:
	void	_ProcessParameter( int argc, char* argv[] );	// 处理参数，初始化系统
	void	_RegisterPrcessor();							// 注册Processor
	void	_ProcessPath();									// 扫描处理路径，生成dir
	void	_Process();										// 处理文件，扫描文件
	void	_UnRegisterPrcessor();							// 注消Processor
	void	_BuildErrorFile();								// 生成确实文件路径
	void	_BuildPkgFile();								// 生成打包文件路径
	void	_BuildSerPkgFile();								// 生成ser文件路径
private:
	CResProcessorMgr	m_ResMgr;
	CProcessHandler		m_Handler;
	CPathMgr			m_PathMgr;
	IResProcessor*		m_pProcessor[MAX_PRIORITY_LEVEL];
	int					m_PackType;
	bool				m_beProcessOk;
};