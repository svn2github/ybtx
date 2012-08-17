#pragma once
#include "CCoder.h"
#include "CSceneClassDefine.h"
#include "CDataScene.h"

namespace sqr
{
	class CPkgFile;
	class CSceneManagerClient;
	class CSceneAmpCoder		
		: public CCoder
		, public CCoderHelper
		, public CSceneMallocObject
	{
		DECLARE_VERSION_MAP();
	public:
		bool SceneDeCode( CDataScene* pSceneManager, CPkgFile& File );
	protected:
		
		bool DeCode_Ver_B( const CDataChunk& inData,OBJ_HANDLE handle );
		bool DeCode_Ver_C( const CDataChunk& inData,OBJ_HANDLE handle );
		bool DeCode_Ver_D( const CDataChunk& inData,OBJ_HANDLE handle );
		bool DeCode_Ver_E( const CDataChunk& inData,OBJ_HANDLE handle );
		bool DeCode_Ver_F( const CDataChunk& inData,OBJ_HANDLE handle );
		bool DeCode_Ver_G( const CDataChunk& inData,OBJ_HANDLE handle );
		bool DeCode_Ver_H( const CDataChunk& inData,OBJ_HANDLE handle );

	private: // 0xB - 0xE 版本公用代码
		void InitScene(CSceneManagerClient* pScene);	
		bool DeCoder_Com_B_D( CPkgFile& File,OBJ_HANDLE handle );
	};
}
