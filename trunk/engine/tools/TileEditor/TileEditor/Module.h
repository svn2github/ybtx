#pragma once

class CModule
{
public:
	CModule();
	~CModule();

	int		Bind(const char *pModuleName);
	void	Unbind();

	void * GetFunctionPointer(const char *pFunctionName);

protected:
	HINSTANCE	m_hInstance;
	char		m_szName[64];
};