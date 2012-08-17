#pragma once
#include "TSingleton.h"

namespace sqr
{

struct MoldeSaveProperty;

class CreateSceneObjectMgr: public Singleton<CreateSceneObjectMgr>
{
public:
	map<string, sqr::uint16> sceneRgnIndexs;
public:
	CreateSceneObjectMgr(void);
	~CreateSceneObjectMgr(void);

	bool Rebirth(MoldeSaveProperty* pModelSavePro, const string& strItemName, const string strResSetName, const string& strSkeName, const string& strEffectName, 
				int nAffectRatio, const string strCurName, bool bCameraOriented, bool bEffectBase, bool bAudio, bool bLogin );
};

};
