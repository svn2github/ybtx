#pragma once
#include "iresprocessormgr.h"

class CLuaProcessor :
	public CBaseResProcessor
{
public:
	CLuaProcessor(void);
	virtual ~CLuaProcessor(void);

	virtual bool			RunProcess( const char* szFilename );
	virtual size_t			GetCountCanBeProcess(){ return 1; };
	virtual unsigned long	GetTypeCanBeProcess( size_t nNum ){ return 'lua'; }
};
