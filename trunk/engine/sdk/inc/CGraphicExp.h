#pragma once
#include <string>
namespace sqr
{
	class CError;
	class CPkgFile;
	class CRefBufFile;
	struct PkgDebugInfo;

	enum GraphicErrType
	{
		Create_Err			= 0,
		FileVer_Err			= 1,
		FileFormat_Err		= 2,
		FileRead_Err		= 3,
		OutUse_Err			= 4,
		BufferUse_Err		= 5,
		Font_Err			= 6,
		Memory_Err			= 7,
		TextureCreate_Err	= 8,
		TextureLock_Err		= 9,
		ShaderInit_Err		= 10,
		ShaderRuntime_Err	= 11,
		RenderRuntime_Err	= 12,
		InvalidIndex_Err	= 13,
		AllocMemory_Err		= 14,
		CallBack_Err		= 15,
		PkgOpen_Err			= 16,
		PkgDamaged_Err		= 17,
		ErrCount,
	};

	namespace GraphicErr
	{
		void  InitializeGraphicErr(void);
		const string&	GetErrTypeStr(GraphicErrType et);
		const string&	GetErrInfo(GraphicErrType et);
		const string&	GetErrInfo(const string& ErrTypeStr);
		void		SetErrState(const string& ErrState);
		void		SetErrAudioState(const string& ErrAudioState);

		extern const string strGreateErr;
		extern const string strFileVerErr;
		extern const string strFileFormatErr;
		extern const string strFileReadErr;
		extern const string strOutUseErr;
		extern const string strBufferUseErr;
		extern const string strFontErr;
		extern const string strMemoryErr;
		extern const string strTextureCreateErr;
		extern const string strShaderInitErr;
		extern const string strShaderRuntimeErr;
		extern const string strRenderRuntimeErr;
		extern const string	strCallBackErr;
		extern const string strPkgOpenErr;
		extern const string strPkgDamagedErr;
	};

	void GfkLogExpOnce(CError& exp);
	void GfkLogAudioExpOnce(CError& exp);
	void GfkLogExp(CError& exp);
	void GfkAudioLogExp(CError& exp);
	void GfkLogErrOnce(const string& strType, const string& strMsg = "");
	void GfkLogAudioErrOnce(const string& strType, const string& strMsg = "");
	void GfkLogErr(const string& strType, const string& strMsg = "");
	void GfkDrvLog(const string& strType, const string& strMsg = "");
	
	void GfkLogPkgErr( CError& exp, const char* szFileName, CPkgFile& pkgFile );
	void GfkLogPkgErr( CError& exp, const char* szFileName, const CRefBufFile& refBuf );
	void GfkVerifyPkg( CError& inExp, const char* szFileName );

}
