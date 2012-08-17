#include "stdafx.h"
#include "CChangeSceneUnit.h"
#include "GraphicBase.h"
#include "CEditContext.h"
#include "CSceneBase.h"
#include "PathManager/CPathManager.h"
#include <io.h>

namespace sqr_tools
{
	IMP_TEST_UNIT(CChangeSceneUnit, "切换场景");

	const SString SceneNameExt[ESFT_COUNT] = 
	{
		"_c.ars",
		"_c.arg",
		".amp"
	};

	const SString SceneNamePre[ESFT_COUNT]=
	{ 
		"",
		"",
		"common/" 
	};


	static void Update_Work_Dir(AutoDataHelper* pUnit)
	{	
		((CChangeSceneUnit*)pUnit)->UpdateWorkDir();
	}

	CChangeSceneUnit::CChangeSceneUnit( CEditContext* pContext )
		: CBaseUnit(pContext)
		, m_strWorkPath("")
		, m_uPreIndex(0)
		, m_uCurIndex(0)
	{
		_RegCtrlData(E_STRINGBUTTON, Update_Work_Dir, this, "场景路径", "文件夹|*.", &m_strWorkPath);
	}

	CChangeSceneUnit::~CChangeSceneUnit()
	{
	}

	void CChangeSceneUnit::UpdateWorkDir( void )
	{ 
		if(m_strWorkPath.empty())
			return;

		string tmp = m_strWorkPath;
		replace(tmp.begin(),tmp.end(), '/','\\');
		m_strWorkPath  = tmp.substr( 0,tmp.find_last_of('\\') + 1);

		m_WorkDir.Open(PATH_ALIAS_RES.c_str(), m_strWorkPath.c_str(), "arg");
	}

	void CChangeSceneUnit::_OnTick( void )
	{
		if ( m_WorkDir.GetFileNum() > 0 )
		{
			string szSceneName;
			m_WorkDir.GetFullFileName(m_uCurIndex, szSceneName);

			for ( int i=0; i<ESFT_COUNT; ++i )
			{
				string szTmp; 
				szTmp = szSceneName.substr(0, szSceneName.find("_c.arg"));
				szTmp = string(SceneNamePre[i].c_str()) + szTmp + SceneNameExt[i].c_str();

				CPkgFile PkgFile;
				int ret = PkgFile.Open(PATH_ALIAS_RES.c_str(), szTmp.c_str());
				if ( eFE_SUCCESS != ret )
				{
					goto ERROR_LABEL;
				}
			}

			m_pContext->LoadScene(szSceneName);

ERROR_LABEL:
			m_uPreIndex = m_uCurIndex;
			m_uCurIndex = (++m_uCurIndex)%m_WorkDir.GetFileNum();
			if ( m_uCurIndex == 0 && m_uPreIndex != 0 )
				MessageBox(NULL, L"已经跑完所有地图", L"提示", MB_OK);
		}
	}
}