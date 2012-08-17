#ifndef _CMap_Tex_Coder_H_
#define _CMap_Tex_Coder_H_
#include "CMapFileMgr.h"
#include "CCoder.h"
#include "CMetaSceneClient.h"

namespace sqr
{
class CMapTexCoder : public CCoder
{
	//被屏蔽的三行和 编码相关 暂时不需要
	//static const DWORD New_Version;
	DECLARE_VERSION_MAP();
public:
	CMapTexCoder(CMetaSceneClient* pScene);
	~CMapTexCoder();

	//bool Code();
	bool DeCode_Ver_1(const CDataChunk& inData);

	DWORD	getChunkName(void)
	{
		return CMapFileMgr::ID_MAP_TEXTURE.AsDWORD();
	};
	//virtual	DWORD	getCodeVersion(){return New_Version;};
protected:
	CMetaSceneClient* m_pScene;
};
}

#endif //_CMap_Tex_Coder_H_