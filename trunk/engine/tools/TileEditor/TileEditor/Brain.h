#pragma  once

#include "props.h"

class CClassMgr;

class CBrain
{
	void * m_pHook;
	std::vector<CBaseProp*> m_vecProps;
	int m_nPropCnt;
	
public:
	CBrain(void * p) : m_pHook(p), m_nPropCnt(INVALID) { }

	~CBrain();

	void Init(std::string strClassName, CClassMgr * pClassMgr);
	void AddProps(std::string strClassName, CClassMgr * pClassMgr);

	void * GetHook() { 	return m_pHook;	}

	int GetPropCnt() { return m_nPropCnt;}
	
	CBaseProp * GetProp(int n) 
	{
		assert(!m_vecProps.empty());
		assert(0<=n && n<m_nPropCnt);
		return m_vecProps[n];
	}
};