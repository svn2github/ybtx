#pragma once 

#include "TEntityManager.h"
#include "TSingleton.h"
#include "CStaticObject.h"
#include "CNpcAIDataMallocObject.h"

class CNpcResServerData
	: public virtual CStaticObject
	, public CNpcAIDataMallocObject
{
	friend class CNpcServerBaseDataMgr;
public:
	CNpcResServerData(const string& sNpcName, uint32 uID);
	~CNpcResServerData();
	inline uint32 GetID() const { return m_uID; }
	inline const string& GetName() const{ return m_sNpcName; }
	static CNpcResServerData* Create(const TCHAR* sNpcName, float fBaseSize, float fBottomSize, bool bAdhereTerrain, const TCHAR* sAniFileName);
	static bool CheckNpcAniKeyFrameCfg();		//检查Npc动作关键帧信息，这个必须放在加载完所有动作关键帧之后
	float	m_fBaseSize;
	float	m_fBottomSize;
	bool	m_bAdhereTerrain;
private:
	string	m_sNpcName;
	uint32	m_uID;
};

class CNpcResServerDataMgr
	: public TEntityManager< const CNpcResServerData, TIDIndex< const CNpcResServerData >, TNameIndex< const CNpcResServerData > >
	, public TSingleton< CNpcResServerDataMgr >
	, public CNpcAIDataMallocObject
{
	friend class TSingleton< CNpcResServerDataMgr>;
private:
	CNpcResServerDataMgr();
	~CNpcResServerDataMgr();
};

