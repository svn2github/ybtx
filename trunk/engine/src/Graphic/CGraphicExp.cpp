#include "stdafx.h"
#include <hash_set>
#include "CGraphicExp.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "CComputerInfo.h"
#include "StringHelper.h"
#include "DebugHelper.h"
#define  GraphicFlag "图形组:"
#include "CPkgFile.h"
#include "CGraphic.h"

namespace sqr
{
	static stdext::hash_set<string> ExpPool;
	static string st_ErrState = "";
	static string st_ErrAudioState = "";
	
	void GfkLogExpOnce(CError& exp)
	{
		string Temp = exp.ErrorTitle();
		Temp += exp.ErrorMsg();
		if(ExpPool.find(Temp)==ExpPool.end())
		{
			ExpPool.insert(Temp);
			GfkLogExp(exp);
		}
	}

	void GfkLogAudioExpOnce(CError& exp)
	{
		string Temp = exp.ErrorTitle();
		if(ExpPool.find(Temp)==ExpPool.end())
		{
			ExpPool.insert(Temp);

			exp.AppendType(GraphicFlag,false);
			exp.AppendMsg(st_ErrAudioState + ":",false);

			LogExp(exp);
		}
	}

	void GfkLogErrOnce(const string& strType, const string& strMsg)
	{
		CError error(strType);
		error.AppendMsg(strMsg);
		GfkLogExpOnce(error);
	}

	void GfkLogAudioErrOnce(const string& strType, const string& strMsg)
	{
		CError error(strType);
		error.AppendMsg(strMsg);
		GfkLogAudioExpOnce(error);
	}

	void GfkLogErr(const string& strType, const string& strMsg)
	{
		CError error(strType);
		error.AppendMsg(strMsg);
		GfkLogExp(error);
	}

	void GfkLogExp(CError& exp)
	{
		exp.AppendType(GraphicFlag,false);
		exp.AppendMsg(st_ErrState + ":",false);

		LogExp(exp);
	}

	void GfkAudioLogExp(CError& exp)
	{
		exp.AppendType(GraphicFlag,false);
		exp.AppendMsg(st_ErrAudioState + ":",false);

		LogExp(exp);
	}

	void GfkDrvLog(const string& strType, const string& strMsg)
	{
		CError error(strType);
		error.AppendMsg(strMsg);
		error.SetCategory("兼容性错误");
		GfkLogExp(error);
	}

	static void ShowPkgUnknownErr( CError& exp, const char* szFileName )
	{
		string msg = format("加载包文件：%s 时发生未知错误", szFileName);
		exp.AppendMsg(msg);
		GfkLogExpOnce(exp);
	}
	
	void GfkVerifyPkg( CError& inExp, const char* szFileName )
	{
		bool bAsync = CPkgFile::IsEnableAsyncRead();
		SQR_TRY
		{
			CPkgFile::EnableAsyncRead(false);
			CPkgFile tmpFile(PATH_ALIAS_RES.c_str(), szFileName);
			if ( !CheckPkgCrash(tmpFile) )
				GfkLogExp(inExp);
		}
		SQR_CATCH(exp)
		{
			exp.AppendType("校验文件失败");
			GfkLogExp(exp);
		}
		SQR_TRY_END;
		CPkgFile::EnableAsyncRead(bAsync);
	}

	void GfkLogPkgErr( CError& exp, const char* szFileName, const CRefBufFile& refBuf )
	{
		if ( !CheckPkgCrash(refBuf) )
			ShowPkgUnknownErr(exp, szFileName);
	}

	void GfkLogPkgErr( CError& exp, const char* szFileName, CPkgFile& pkgFile )
	{
		if ( !CheckPkgCrash(pkgFile) )
			ShowPkgUnknownErr(exp, szFileName);
	}

	namespace GraphicErr
	{
		const string strGreateErr		= "库初始化失败";
		const string strFileVerErr		= "文件版本不匹配";
		const string strFileFormatErr	= "无法识别文件格式";
		const string strFileReadErr		= "文件读取失败";
		const string strOutUseErr		= "图形库外部使用错误";
		const string strBufferUseErr	= "Buffer使用错误";
		const string strFontErr			= "字体相关错误";	
		const string strMemoryErr		= "内存不足";
		const string strTextureCreateErr= "纹理创建错误";
		const string strTextureLockErr	= "纹理锁定失败";
		const string strShaderInitErr	= "Shader初始化错误";
		const string strShaderRuntimeErr= "Shader运行期相关错误";
		const string strRenderRuntimeErr= "图形库运行期错误";
		const string strInvalidIndexErr	= "地图索引错误";
		const string strAllocMemFailed	= "内存分配失败";
		const string strCallBackErr		= "回调函数错误";
		const string strPkgOpenErr		= "包打开失败";
		const string strPkgDamagedErr	= "包损坏";

		const string*			g_ErrMap[ErrCount];
		map<string , string*>	g_ErrInfoMap;

		const string strNullInfo = "无附加信息"; 

		void  InitializeGraphicErr(void)
		{
			g_ErrMap[Create_Err]		= &strGreateErr;
			g_ErrMap[FileVer_Err]		= &strFileVerErr;
			g_ErrMap[FileFormat_Err]	= &strFileFormatErr;
			g_ErrMap[FileRead_Err]		= &strFileReadErr;
			g_ErrMap[OutUse_Err]		= &strOutUseErr;
			g_ErrMap[BufferUse_Err]		= &strBufferUseErr;
			g_ErrMap[Font_Err]			= &strFontErr;
			g_ErrMap[Memory_Err]		= &strMemoryErr;
			g_ErrMap[TextureCreate_Err] = &strTextureCreateErr;
			g_ErrMap[TextureLock_Err]	= &strTextureLockErr;
			g_ErrMap[ShaderInit_Err]	= &strShaderInitErr;
			g_ErrMap[ShaderRuntime_Err] = &strShaderRuntimeErr;
			g_ErrMap[RenderRuntime_Err] = &strRenderRuntimeErr;
			g_ErrMap[InvalidIndex_Err]	= &strInvalidIndexErr;
			g_ErrMap[AllocMemory_Err]	= &strAllocMemFailed;
			g_ErrMap[CallBack_Err]		= &strCallBackErr;
			g_ErrMap[PkgOpen_Err]		= &strPkgOpenErr;
			g_ErrMap[PkgDamaged_Err]	= &strPkgDamagedErr;
		}

		const string& GetErrTypeStr(GraphicErrType et)
		{
			return *g_ErrMap[et];
		}
		const string& GetErrInfo(GraphicErrType et)
		{
			return GetErrInfo(GetErrTypeStr(et));
		}

		const string& GetErrInfo(const string& ErrTypeStr)
		{
			return strNullInfo;
		}

		void SetErrState(const string& ErrState)
		{
			CGraphic::GetInst()->ClearFrameCount();
			st_ErrState = ErrState;
		}

		void SetErrAudioState(const string& ErrAudioState)
		{
			st_ErrAudioState = ErrAudioState;
		}
	}

}

