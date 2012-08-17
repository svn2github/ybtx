#include "stdafx.h"
#include "CExpLog.h"
#include <time.h>
#include "CSplitInfo.h"

CExpLog::CExpLog()
: m_pLogFile(NULL)
, m_uCount(0)
{

}

CExpLog::~CExpLog()
{
	delete m_pLogFile;
	m_pLogFile = NULL;
}

void CExpLog::BeginToPrint(const string& strProjectName)
{
	m_strProjectName = strProjectName;
	string strFileName = "AllExpInfoLog.txt";
	m_pLogFile = new ofstream(strFileName.c_str(), ios_base::out | ios_base::app);
	ofstream& logFile = *m_pLogFile;
	CSplitInfo* pSplitInfo = CSplitInfo::GetInst();
	string strLanguage;
	if(pSplitInfo->GetBeInternational())
	{
		strLanguage = pSplitInfo->GetForeLang();
	}
	else
	{
		strLanguage = pSplitInfo->GetOriLang();
	}
	logFile << "[" << m_strProjectName << "](" << strLanguage << ") at ";

	time_t lTime = time(NULL);
	logFile << (ctime(&lTime));
	//logFile << (asctime(localtime(&lTime))) << "\n";


}

void CExpLog::EndToPrint()
{
	ofstream& logFile = *m_pLogFile;
	logFile << "[/" << m_strProjectName << "]\n\n";
	logFile.close();
}

void CExpLog::PrintLog(const string & strMsg, bool bIsWarning)
{
	ofstream& logFile = *m_pLogFile;
	if(bIsWarning)
	{
		logFile << "£¨¾¯¸æ" << m_uCount++ << "£©";
	}
	else
	{
		logFile << "£¨´íÎó" << m_uCount++ << " £©";
	}
	logFile << strMsg << "\n";
}

void CExpLog::PrintLogNoLf(const string & strMsg, bool bIsWarning)
{
	ofstream& logFile = *m_pLogFile;
	if(bIsWarning)
	{
		logFile << "£¨¾¯¸æ" << m_uCount++ << "£©";
	}
	else
	{
		logFile << "£¨´íÎó" << m_uCount++ << " £©";
	}
	logFile << strMsg;
}

bool GetLogToFile()
{
	return CSplitInfo::GetInst()->GetLogToFile();
}

void BeginToPrint(const string& strProjectName)
{
	CExpLog::GetInst()->BeginToPrint(strProjectName);
}

void EndToPrint()
{
	CExpLog::GetInst()->EndToPrint();
}

void PrintLog(const string & strMsg, bool bIsWarning)
{
	CExpLog::GetInst()->PrintLog(strMsg, bIsWarning);
}

void PrintLogNoLf(const string & strMsg, bool bIsWarning)
{
	CExpLog::GetInst()->PrintLogNoLf(strMsg, bIsWarning);
}

