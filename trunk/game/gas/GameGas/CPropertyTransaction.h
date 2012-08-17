#pragma once
#include "TFighterAllocator.h"
#include "PropertyType.h"

class CFighterMediator;
class CFighterDictator;
enum ETransactionState
{
	eTS_Ready,
	eTS_Enable,
	eTS_Disable,
};

enum ETransactionLockState
{
	eTLS_UnLock,
	eTLS_Locked,
};

class CBaseValueHolder : public CBasePropertyHolder
{
public:
	virtual ~CBaseValueHolder(){}
	virtual float GetBaseValue(CFighterDictator* pFighter) = 0;
};

class  CRegistPropertyHolder : public CFighterMallocObject
{
public:
	CRegistPropertyHolder(const string& strPropertyName, float fValue):m_strPropertyName(strPropertyName),m_fValue(fValue){}
	const string m_strPropertyName;
	const float	m_fValue; 
};

class CPropertyTransaction;
class CPropertyTransactionMgr
{;
public:
	typedef map<uint32,CPropertyTransaction*,less<uint32>, TFighterAllocator<pair<uint32, CPropertyTransaction* > > > MapTransaction;
	typedef map<string , float,less<string>,TFighterAllocator<pair<string, float > > > MapPropertyChange;
	typedef map<string , CBaseValueHolder*,less<string>,TFighterAllocator<pair<string, CBaseValueHolder* > > > MapBaseProperty;
	CPropertyTransactionMgr(CFighterMediator* pFighter);
	~CPropertyTransactionMgr();
	static void BuildBasePropertyTable();
	static void ReleaseBasePropertyTable();
	uint32 CreateTransactionID();
	void DestroyTransaction(uint32 uTransactionID);
	void EnableTransaction(uint32 uTransactionID);
	void DisableTransaction(uint32 uTransactionID);
	void CommitTransaction(uint32 uTransactionID);
	void ChangeTransactionPropertyValue(uint32 uTransactionID, const string& strPropertyName, float fValue);
	float GetPropertyValue(const string& strPropertyName);
	MapPropertyChange& GetPropertyChangeMap(){return m_mapPropertyChange;}
	CFighterMediator* GetOwner()const {return m_pFighter;}
private:
	static MapBaseProperty ms_mapBaseProperty;
	MapPropertyChange		m_mapPropertyChange;
	MapTransaction m_mapTransaction;
	CFighterMediator* m_pFighter;
	static uint32 ms_uCount;
};

class CPropertyTransaction	 : public CFighterMallocObject
{
public:
	typedef set<CRegistPropertyHolder*,less<CRegistPropertyHolder*>, TFighterAllocator<CRegistPropertyHolder*> > SetProperty;
	CPropertyTransaction(CPropertyTransactionMgr* pMgr);
	~CPropertyTransaction();
	void ChangePropertyValue(const string& strPropertyName, float fValue);
	bool IsLock(){return eTLS_Locked==m_eLockState;}
	void Lock();
	void ChangeState(ETransactionState eState);
	string MakeLog();
private:
	void ChangeValue(const string& strPropertyName, float fValue);
	SetProperty m_setProperty;
	ETransactionLockState m_eLockState;
	ETransactionState m_eState;
	CPropertyTransactionMgr* m_pMgr;
};

