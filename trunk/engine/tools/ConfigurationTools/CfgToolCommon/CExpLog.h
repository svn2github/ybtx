#pragma once
#include "TSingleton.h"

class CExpLog : public TSingleton<CExpLog>
{
public:
	CExpLog();
	~CExpLog();
	void BeginToPrint(const string& strProjectName);
	void EndToPrint();
	void PrintLog(const string & strMsg, bool bIsWarning = false);
	void PrintLogNoLf(const string & strMsg, bool bIsWarning = false);

private:
	string m_strProjectName;
	ofstream* m_pLogFile;
	uint32 m_uCount;
};

bool GetLogToFile();
void BeginToPrint(const string& strProjectName);
void EndToPrint();
void PrintLog(const string & strMsg, bool bIsWarning = false);
void PrintLogNoLf(const string & strMsg, bool bIsWarning = false);
