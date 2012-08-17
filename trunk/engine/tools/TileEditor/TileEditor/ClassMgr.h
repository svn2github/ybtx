#pragma once

#include "Module.h"

struct ClassDef;
struct PropDef;


#define CM_NOERROR			-1
#define CM_CANTFINDMODULE	0
#define CM_NOTCLASSMODULE	1
#define CM_VERSIONMISMATCH	2

#define _INVALID_CLASSIDX	0xffffffff

class CClassMgr
{
public:
	CClassMgr(void);
	~CClassMgr(void);

	void		Load(std::string strFileName);
	
	int			GetNumClassDefs();
	int			GetNumClassBind2Map();
	ClassDef **	GetClassDefs();

	ClassDef *	FindClass(const char * pClassName);
	int			FindClassIdx(const char * pClassName);
	PropDef *	FindVar(ClassDef * pClass, const char * pVarName);
	bool		IsClassFlagSet(ClassDef * pClass, const uint32 dwClassFlag );
	
protected:
	int				m_nClassTotalCount;	
	int				m_nClassBind2MapCount;		
	ClassDef**		m_pClassDefs;
	int				m_nVersion;
	
	CModule			m_module;
	
	void StatNumClassBind2Map();
	void VerifyClassDefProperties();
};