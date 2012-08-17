#pragma once

#include "IResProcessorMgr.h"

class CMapProcessor : public CBaseResProcessor
{
public:
	CMapProcessor(void){}
	virtual ~CMapProcessor(void){}

	virtual bool			RunProcess( const char* szFilename );
	virtual size_t			GetCountCanBeProcess(){ return 1; };
	virtual unsigned long	GetTypeCanBeProcess( size_t nNum ){ return 'amp'; }
};