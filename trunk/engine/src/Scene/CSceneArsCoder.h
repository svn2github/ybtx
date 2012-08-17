#pragma once
#include "CCoder.h"
#include "CSceneClassDefine.h"
#include "CDataScene.h"

namespace sqr
{
	class CSceneArsCoder : public CCoder
		, public CCoderHelper
		, public CSceneMallocObject
	{
		DECLARE_VERSION_MAP();
	public:
		bool SceneDeCode( CDataScene* pSceneManager,CPkgFile& File );
	protected:
		bool DeCode_Ver_C( const CDataChunk& inData,OBJ_HANDLE handle );
		bool DeCode_Ver_D( const CDataChunk& inData,OBJ_HANDLE handle );
		bool DeCode_Ver_E( const CDataChunk& inData,OBJ_HANDLE handle );
	};
}
