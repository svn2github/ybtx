#pragma once
#include "CCoder.h"
#include "CSceneClassDefine.h"
#include "CDataScene.h"

namespace sqr
{
	class CPkgFile;
	class CSceneManagerClient;
	class CSceneArgCoder 	
		: public CCoder
		, public CCoderHelper
		, public CSceneMallocObject
	{
		DECLARE_VERSION_MAP();
	public:
		bool SceneDeCode( CDataScene* pSceneManager,CPkgFile& File );
	protected:
		bool DeCode_Ver_C( const CDataChunk& inData,OBJ_HANDLE handle );
	private: //重构临时代码
		void LoadOneRegion( CPkgFile& File, uint32 nChunk, uint32 uRegionId );
		void LoadOneChunk( CPkgFile& File, uint32 uRegionId, uint32 uMagicID );
		void LoadSubRegionInfo( CPkgFile& File, uint32 uRegionId );
		void LoadTerranHeight( CPkgFile& File, uint32 uRegionId );
		void LoadDiffuse( CPkgFile& File, uint32 uRegionId );
		void LoadSpecular( CPkgFile& File, uint32 uRegionId );
		void LoadTexture( CPkgFile& File, uint32 uRegionId );
		void LoadModel( CPkgFile& File, uint32 uRegionId );
		void LoadOffsetHeight( CPkgFile& File, uint32 uRegionId );
		void LoadWater( CPkgFile& File, uint32 uRegionId );
		void LoadSceneSpotLight( CPkgFile& File, uint32 uRegionId );
		void LoadScenePointLight( CPkgFile& File, uint32 uRegionId );
		void LoadSceneTrap( CPkgFile& File, uint32 uRegionId );
		void LoadRegionBlock( CPkgFile& File, uint32 uRegionId );
		void LoadRegionExactBlock( CPkgFile& File, uint32 uRegionId );
		void LoadRegionSceneAudio( CPkgFile& File, uint32 uRegionId );
		void LoadRegionLoginModel( CPkgFile& File, uint32 uRegionId );
		void LoadRegionMaterialIndex( CPkgFile& File, uint32 uRegionId );
		SString m_strMapName;
		uint32 m_uModelVersion;
		CSceneManagerClient* m_pScene;
	};
}
