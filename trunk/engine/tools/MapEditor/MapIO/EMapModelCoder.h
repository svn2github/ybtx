#ifndef _EMap_Model_Coder_H_
#define _EMap_Model_Coder_H_

#include "CMapFileMgr.h"
#include "CCoder.h"
#include "TerrainMesh.h"

namespace sqr
{
//用于拼接纹理 并保存于文件
class EMapModelCoder : public CCoder
{
	static const DWORD New_Version;
	DECLARE_VERSION_MAP();
public:
	EMapModelCoder(CTerrainMesh* pTerrainMesh);
	~EMapModelCoder();

	bool Code();
	bool DeCode_Ver_1(const CDataChunk& inData);

	DWORD	getChunkName(void)
	{
		return CMapFileMgr::ID_MAP_MODEL.AsDWORD();
	};
	virtual	DWORD	getCodeVersion()
	{
		return New_Version;
	};
protected:
	CTerrainMesh* m_pTerrain;
};
}

#endif //_EMap_Model_Coder_H_