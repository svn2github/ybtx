#pragma once
#include "CLogicDataMgr.h"
#include "CSceneManagerClient.h"
namespace sqr
{
	class CLogicDataCoder : public CElementCoderEx
	{
	public:
		CLogicDataCoder(CSceneManagerClient* pScene);
		bool CElementDeCode( CElementNode* pNode, bool bSync );
		bool _CElementDeCode( const DeCodeParams& Params );
	protected:				
		CSceneManagerClient* m_pScene;

		DECLARE_VERSION_MAP();
		bool DeCode_Ver_0( const CDataChunk& inData,OBJ_HANDLE handle );	
	};
}