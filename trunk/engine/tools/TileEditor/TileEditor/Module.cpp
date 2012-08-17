#include "StdAfx.h"
#include "Module.h"

CModule::CModule()
{
	m_hInstance = NULL;
	memset(m_szName, 0, sizeof(m_szName));
}

CModule::~CModule()
{
}

int CModule::Bind(const char * pModuleName)
{
	HINSTANCE hInstance = LoadLibrary(pModuleName);
	
	if ( hInstance == NULL ) 
	{
		return -1;
	}
	
	m_hInstance = hInstance;
	strcpy(m_szName, pModuleName);
	return 0;
}

void CModule::Unbind()
{
	FreeLibrary(m_hInstance);
}

void * CModule::GetFunctionPointer(const char * pFunctionName)
{
	return (void*)GetProcAddress(m_hInstance, pFunctionName);
}