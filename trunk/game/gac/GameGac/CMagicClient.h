#pragma once
#include "ICoreObjectFollowerHandler.h"
#include "ICypherVariantHolderHandler.h"
#include "PatternRef.h"
#include "TMagicAllocator.h"

namespace sqr
{
	class CRenderObject;
	class CCoreObjectFollower;
}

class CMagicFX;
class CMagicClient
	:public ICoreObjectFollowerHandler
	,public ICypherVariantHolderHandler
	,public CMagicMallocObject
{
public:
	CMagicClient(CCoreObjectFollower* pCoreObj);
	virtual ~CMagicClient();
	CRenderObject* GetRenderObject()const;
	CMagicFX* GetMagicFX()const;
	virtual void PlayMagicFx(const string& strMagicFx);
	void OnDestroy();
	//virtual void OnPosChanged(){}
	virtual void OnRefreshRenderObjPos(const CFPos& PixelPos);
	virtual void OnMemberAdded(CCypherVariant *pVariant){}
	virtual void OnDeleteMember(CCypherVariant *pVariant){}
	virtual void OnClearTable(CCypherVariant *pVariant){}
	virtual void OnValueChanged(CCypherVariant *pVariant){}
protected:
	CCoreObjectFollower*	m_pCoreObj;
	TPtRefer<CMagicClient, CMagicFX>	m_MagicFX;
};
