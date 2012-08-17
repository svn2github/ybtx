#pragma once


class ILogWriter
{
public:
	virtual void WriteLog(const char* szContent)=0;
};