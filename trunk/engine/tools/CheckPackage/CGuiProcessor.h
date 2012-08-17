#pragma once
#include "iresprocessormgr.h"
#include "CCoderProcessor.h"

class CGuiProcessor : public CCoderProcessor
{
public:
	CGuiProcessor(void);
	virtual ~CGuiProcessor(void);

	virtual size_t			GetCountCanBeProcess(){ return 1; };
	virtual unsigned long	GetTypeCanBeProcess( size_t nNum ){ return 'aud'; }
};