#pragma once 
#include "CElementManager.h"
#include "CDataScene.h"

namespace sqr_tools
{
	class CMapTerrainCoder : public CElementCoderEx
	{
	public:
		CMapTerrainCoder(CDataScene* pScene);

	protected:
		CDataScene* m_pDataScene;
		bool		_CElementDeCode( const DeCodeParams& Params );
	private:
		DECLARE_VERSION_MAP();
		bool DeCode_Ver_0( const CDataChunk& inData,OBJ_HANDLE handle );	
	};
}