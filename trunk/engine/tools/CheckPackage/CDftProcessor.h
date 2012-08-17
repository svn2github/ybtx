#pragma once

#include "iresprocessormgr.h"

class CDftProcessor : public CBaseResProcessor
{
public:
	CDftProcessor(void){};
	virtual ~CDftProcessor(void){};
	
	virtual size_t			GetCountCanBeProcess(){ return -1; };
	virtual unsigned long	GetTypeCanBeProcess( size_t nNum ){ return 'dft'; };
};

