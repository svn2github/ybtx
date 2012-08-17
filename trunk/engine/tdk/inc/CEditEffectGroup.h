#pragma once
#include "CEffectGroup.h"

class CEditEffectGroup : public CEffectGroup
{
	friend class CEditEffectManager;
protected:

	CEditEffectGroup( IEffectManager* pEffectMgr, const char* szFileName );
	virtual CEffectProp*			CreateEffectPropFun(const char* szEffect);
public:
	virtual ~CEditEffectGroup();
	EMap< EString, CEffectProp* >&	GetEffectPropMap() { return m_EffectProps;}
	
	bool			CreateEffectProp(string GroupName, string Name);
	virtual int32	AddRef();
	void			ReNameGroup(string strOld,string strNew);
	void			DeleteGroup(string strName);
	void			InitEffectProps();
	void			SetName(EString szName);
};
