#pragma once

#include "iresprocessormgr.h"

class CArsProcessor : public CBaseResProcessor
{
public:
	CArsProcessor(void){}
	virtual ~CArsProcessor(void){}

	virtual bool			RunProcess( const char* szFilename );
	virtual size_t			GetCountCanBeProcess(){ return 1; };
	virtual unsigned long	GetTypeCanBeProcess( size_t nNum ){ return 'ars'; }
};