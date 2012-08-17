#pragma once
#include "CTerrainMgr.h"
#include "CSceneManagerClient.h"
namespace sqr
{
	class CTerrainCoder : public CElementCoderEx
	{
	protected:
		CTerrainCoder(CSceneManagerClient* pScene);
		CSceneManagerClient*		m_pDataScene;
		bool	_CElementDeCode( const DeCodeParams& Params );

		//void	SetDeCodeVersion(DWORD version) = 0;//不允许直接创建 CTerrainCoder
	};

	class CNormalTerrainCoder : public CTerrainCoder
	{
	public:
		CNormalTerrainCoder( CSceneManagerClient* pScene )
			: CTerrainCoder(pScene){}

	protected:
		DECLARE_VERSION_MAP();
		bool DeCode_Ver_0( const CDataChunk& inData,OBJ_HANDLE handle );	
	};

	class CShaderTerrainCoder : public CTerrainCoder
	{
	public:
		CShaderTerrainCoder( CSceneManagerClient* pScene )
			: CTerrainCoder(pScene){}

	protected:
		DECLARE_VERSION_MAP();
		bool DeCode_Ver_0( const CDataChunk& inData,OBJ_HANDLE handle );	
	};
}