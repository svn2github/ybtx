#include "stdafx.h"
#include "CSceneClientCoder.h"
#include "CPkgFile.h"
#include "CSceneManagerClient.h"
#include "MemoryHelper.h"
#include "ExpHelper.h"
#include "CGraphic.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

const SWString PATH_ALIAS_SCENE = L"res";
const SString CSceneClientCoder::FileName_Add[ESFT_COUNT] = 
{
	"_c.ars",
	"_c.arg",
	".amp"
};
const SString FileName_Pre[ESFT_COUNT]=
{ 
	"",
	"",
	"common/" 
};


bool CSceneClientCoder::SceneDeCode( CDataScene* pSceneManager )
{
	CSceneManagerClient* pScene = (CSceneManagerClient*)pSceneManager;
	SString strSceneName = pSceneManager->GetSceneName();
	CPkgFile sceneFileList[ESFT_COUNT];

	for( int i = 0;	i<ESFT_COUNT;	++i )
	{
		CPkgFile PkgFile;
		SString sSceneFileName = FileName_Pre[i] + strSceneName + FileName_Add[i];

		int ret = sceneFileList[i].Open(PATH_ALIAS_SCENE.c_str(),sSceneFileName.c_str());
		if ( eFE_SUCCESS != ret )
		{
			GraphicErrType gfkErrType;
			string msg = sceneFileList[i].GetCurPkgName(sSceneFileName.c_str()) + ".pkg";
			if ( eFE_RAW_FILE_NAME_OR_PATH_NOT_FOUND == ret )
			{
				msg += "文件缺失。请重新下载并安装客户端。";
				ShowErrorMsgAndExit(msg.c_str());
			}
			else if ( eFE_RAW_FILE_CANNOT_BE_ACCESS == ret )
			{
				msg += "文件访问被拒绝，可能文件被锁定，或没有权限。请尝试以管理员权限运行程序，或重新下载并安装客户端。";
				ShowErrorMsgAndExit(msg.c_str());
			}
			else if ( eFE_PKG_DAMAGED == ret || eFE_PKG_SUB_FILE_OPEN_FAILED == ret )
			{
				msg += "文件已损坏，可能由网络传输错误或玩家误操作引起。请重新下载并安装客户端。";
				ShowErrorMsgAndExit(msg.c_str());
			}
			else if ( eFE_NOT_A_PACKAGE == ret )
			{
				msg = "客户端文件已损坏，可能由网络传输错误或玩家误操作引起。请重新下载并安装客户端。";
				ShowErrorMsgAndExit(msg.c_str());
			}
			else 
			{
				msg += "文件打开发生未知错误。";
				gfkErrType = FileRead_Err;
			}

			LogErr(msg.c_str());
			GenErr(GraphicErr::GetErrTypeStr(gfkErrType).c_str(), msg.c_str());
		}
	}	

	pScene->DoProcess( 10.0f );
	bool ret = true;
	ESceneFileType curCoder = ESFT_AMP;
	SQR_TRY
	{
		ret &= m_AmpCoder.SceneDeCode(pSceneManager,sceneFileList[ESFT_AMP]);
		pScene->DoProcess( 15.0f );
		
		curCoder = ESFT_ARG;
		ret &= m_ArgCoder.SceneDeCode(pSceneManager,sceneFileList[ESFT_ARG]);
		pScene->DoProcess( 20.0f );

		curCoder = ESFT_ARS;
		ret &= m_ArsCoder.SceneDeCode(pSceneManager,sceneFileList[ESFT_ARS]);
		pScene->DoProcess( 25.0f );
	}
	SQR_CATCH(exp)
	{
		GfkLogPkgErr(exp, "", sceneFileList[curCoder]);
		for ( int i=0; i<ESFT_COUNT; ++i )
			sceneFileList[i].Close();

		TerminateProcess(GetCurrentProcess(), 1);
	}
	SQR_TRY_END
	
	for ( int i=0; i<ESFT_COUNT; ++i )
		sceneFileList[i].Close();

	return ret;
}
