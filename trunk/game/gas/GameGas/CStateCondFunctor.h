#pragma once

#include "FightDef.h"
#include "CMagicStateMallocObject.h"

class CBaseStateServer;

class CStateCondBase
	:public CMagicStateMallocObject
{
public:
	virtual bool operator()(CBaseStateServer* pState) const = 0;
	virtual ~CStateCondBase(){}
};

class CStateCondForAll
	: public CStateCondBase
{
public:
	virtual bool operator()(CBaseStateServer* pState) const
	{
		return true;
	}
};


class CStateCondForIcon
	: public CStateCondBase
{
public:
	virtual bool operator()(CBaseStateServer* pState) const;
};

class CStateCondForDecreaseType
	: public CStateCondBase
{
public:
	explicit CStateCondForDecreaseType(EDecreaseStateType eDecreaseType, int32 iCount = -1, bool bDelEqual = true)
		: m_eDecreaseType(eDecreaseType), m_bDelEqual(bDelEqual), m_iCount(iCount)
	{

	}
	virtual bool operator()(CBaseStateServer* pState) const;
protected:
	EDecreaseStateType	m_eDecreaseType;
	bool				m_bDelEqual;
	mutable int32		m_iCount;
};

class CStateCondForDispellableDecreaseType
	: public CStateCondForDecreaseType
{
public:
	explicit CStateCondForDispellableDecreaseType(EDecreaseStateType eDecreaseType, int32 iCount = -1, bool bDelEqual = true)
		: CStateCondForDecreaseType(eDecreaseType, iCount, m_bDelEqual)
	{

	}
	virtual bool operator()(CBaseStateServer* pState) const;
};

class CStateCondForAllDecrease
	: public CStateCondBase
{
public:
	explicit CStateCondForAllDecrease(int32 iCount = -1)
		: m_iCount(iCount)
	{

	}
	virtual bool operator()(CBaseStateServer* pState) const;
protected:
	mutable int32		m_iCount;
};

class CStateCondForDispellableAllDecrease
	: public CStateCondForAllDecrease
{
public:
	explicit CStateCondForDispellableAllDecrease(int32 iCount = -1)
		: CStateCondForAllDecrease(iCount)
	{

	}
	virtual bool operator()(CBaseStateServer* pState) const;
};

class CStateCondForNonPersistent
	: public CStateCondBase
{
public:
	virtual bool operator()(CBaseStateServer* pState) const;
};

class CFighterDictator;

class CStateCondForSelfState
	: public CStateCondBase
{
public:
	explicit CStateCondForSelfState(CFighterDictator* pOwner)
		: m_pOwner(pOwner)
	{

	}
	virtual bool operator()(CBaseStateServer* pState) const;
private:
	CFighterDictator * m_pOwner;
};

class CStateCondForCountDispellableAllDecrease
	: public CStateCondForDispellableAllDecrease
{
public:
	explicit CStateCondForCountDispellableAllDecrease(int32 iCount = -1)
		: CStateCondForDispellableAllDecrease(iCount)
		, m_iAllCount(0)
	{

	}
	virtual bool operator()(CBaseStateServer* pState) const;
	int32 GetAllCount()		{return m_iAllCount;}

protected:
	mutable int32		m_iAllCount;
};

class CStateCondForRandDispellableAllDecrease
	: public CStateCondForDispellableAllDecrease
{
public:
	explicit CStateCondForRandDispellableAllDecrease(int32 iAllCount, int32 iCount = -1)
		: CStateCondForDispellableAllDecrease(iCount)
		, m_iAllCount(iAllCount)
	{

	}
	virtual bool operator()(CBaseStateServer* pState) const;

protected:
	mutable int32		m_iAllCount;
};

class CStateCondForCountDispellableDecreaseType
	: public CStateCondForDispellableDecreaseType
{
public:
	explicit CStateCondForCountDispellableDecreaseType(EDecreaseStateType eDecreaseType, int32 iCount = -1, bool bDelEqual = true)
		: CStateCondForDispellableDecreaseType(eDecreaseType, iCount, m_bDelEqual)
		, m_iAllCount(0)
	{

	}
	virtual bool operator()(CBaseStateServer* pState) const;
	int32 GetAllCount()		{return m_iAllCount;}
protected:
	mutable int32		m_iAllCount;
};

class CStateCondForRandDispellableDecreaseType
	: public CStateCondForDispellableDecreaseType
{
public:
	explicit CStateCondForRandDispellableDecreaseType(int32 iAllCount, EDecreaseStateType eDecreaseType, int32 iCount = -1, bool bDelEqual = true)
		: CStateCondForDispellableDecreaseType(eDecreaseType, iCount, m_bDelEqual)
		, m_iAllCount(iAllCount)
	{

	}
	virtual bool operator()(CBaseStateServer* pState) const;
protected:
	mutable int32		m_iAllCount;
};

