#pragma once
#include "CWaterMgr.h"
#include "CSceneManagerClient.h"

namespace sqr
{
	class CWaterCoder : public CElementCoderEx
	{
	public:
		CWaterCoder(CSceneManagerClient* pScene);

	protected:
		CSceneManagerClient* m_pScene;
		bool	_CElementDeCode( const DeCodeParams& Params );
	private:
		DECLARE_VERSION_MAP();
		bool DeCode_Ver_0( const CDataChunk& inData,OBJ_HANDLE handle );	
	};
}