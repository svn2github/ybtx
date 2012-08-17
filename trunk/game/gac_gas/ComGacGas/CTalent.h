#pragma once
#include <set>
#include "TFighterAllocator.h"

class ITalentHandler : public CFighterMallocObject
{
public:
	virtual ~ITalentHandler(){}
	virtual void OnTalentChanged(const string& szName, int32 iPoint){}
	virtual void OnTalentAdded(const string& szName, uint32 uPoint){}
	virtual void OnTalentRemove(const string& szName,uint32 uPoint){}
	virtual void OnAllTalentCleared(){}
};

class CExpression;
class CTalentHolder : public CFighterMallocObject
{
friend class  CExpression;

public:
	typedef map<string, uint32, less<string>, TFighterAllocator<pair<const string, int32> > > MapPoint;
	typedef set<string, less<string>, TFighterAllocator<string> > SetGlobalName;
	CTalentHolder();
	virtual ~CTalentHolder(){}
	uint32				GetPoint(const string& szName)	const;
	void				ChangeTalent(const string& szName, int32 uPoint);
	virtual void		RemoveTalent(const string& szName);
	static void			InsertName(const TCHAR* sName);
	static bool			ExistName(const string& szName);
	void				SetHandler(ITalentHandler* pHander){m_pHandler=pHander;}
	ITalentHandler*		GetHandler(){return m_pHandler;}
protected:
	virtual void		InsertTalent(const string& szName, uint32 uPoint);
private:
	MapPoint				m_mapPoint;
	static SetGlobalName	m_setGlobalName;
	ITalentHandler*			m_pHandler; 
};
