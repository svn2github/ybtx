#pragma once

#include "IResProcessorMgr.h"

class SMapProcessor : public CBaseResProcessor
{
public:
	SMapProcessor(void){}
	virtual ~SMapProcessor(void){}

	virtual bool            RunProcess( const char* szFilename );
	virtual size_t          GetCountCanBeProcess(){ return 1; };
	virtual unsigned long   GetTypeCanBeProcess( size_t nNum ){ return 'amp'; }
};