#include "stdafx.h"
#include "OperationLog.h"
#include "Operator.h"
#include "Operator.inl"
#include "CMapEditApp.h"

namespace sqr
{
COpearationLog::COpearationLog() 
	:m_fdOperationInfo(NULL), m_bStart(false)
{
}

COpearationLog::~COpearationLog()
{
	this->Realese();
}

void COpearationLog::Realese()
{
	if (m_fdOperationInfo)
	{
		fclose(m_fdOperationInfo);
		m_fdOperationInfo = NULL;
	}
}

void COpearationLog::SetLogFileName(const char* szMapName, const char* szFilePath)
{
	if (m_fdOperationInfo)
		return;

	m_bStart = true;

	sprintf(m_szOperationFilePath, "%s/%s.info", szFilePath, szMapName);
	sprintf(m_szMapName, "%s", szMapName);
	sprintf(m_szLogPath, "%s", szFilePath);

	m_szOperationFilePath[ sizeof(m_szOperationFilePath)-1 ] = '\0';

	char szStartBuf[128];
	sprintf(szStartBuf, "开始记录地图: %s 操作信息", szMapName);
	WriteOperationLog(szStartBuf);
}

const char* COpearationLog::GetLogFilePath() const
{
	return m_szOperationFilePath;
}

void COpearationLog::WriteInfoLog(const char* szOperationInfo)
{
	fprintf(m_fdOperationInfo, "%s", szOperationInfo);

	fflush(m_fdOperationInfo);
}

void COpearationLog::WriteOperationLog(const char* szOperationLog)
{
	if( !m_bStart )
		return;

	if (!m_fdOperationInfo)
	{
		m_fdOperationInfo = fopen(m_szOperationFilePath, "wb");

		if (!m_fdOperationInfo)
			return;

	}
		
	time_t T;
	struct tm * timenow;
	time ( &T );
	timenow = localtime ( &T ); 
	char szTimeBuf[64];

	sprintf(szTimeBuf, "%d_%02d_%02d_%02d_%02d_%02d", timenow->tm_year + 1900, timenow->tm_mon + 1, 
		timenow->tm_mday, timenow->tm_hour, timenow->tm_min, timenow->tm_sec);

	ostringstream str;
	str << "记录: " << szOperationLog << "  时间: " << szTimeBuf << endl << endl;
	
	WriteInfoLog(str.str().c_str());
}

void COpearationLog::WriteOperationLog(const int nGridIndx, const char* szOperationLog)
{
	if( !m_bStart )
		return;

	if (!m_fdOperationInfo)
	{
		m_fdOperationInfo = fopen(m_szOperationFilePath, "wb");

		if (!m_fdOperationInfo)
			return;

	}

	int nX = 0, nZ = 0;
	CMapEditApp::GetInst()->GetOperator()->GetMapCoordinate(nGridIndx, nX, nZ);
	time_t T;
	struct tm * timenow;
	time ( &T );
	timenow = localtime ( &T ); 
	char szTimeBuf[64];

	sprintf(szTimeBuf, "%d_%02d_%02d_%02d_%02d_%02d", timenow->tm_year + 1900, timenow->tm_mon + 1, 
		timenow->tm_mday, timenow->tm_hour, timenow->tm_min, timenow->tm_sec);

	ostringstream str;
	str << "记录: " << szOperationLog << "  时间: " << szTimeBuf << endl;
	str << "格子索引: " << nGridIndx << " ，行: " << nZ << " ，列: " << nX << endl << endl;

	WriteInfoLog(str.str().c_str());
}

void COpearationLog::Save(const string& path)
{
	size_t npos = 0;
	string strMapName = "";
	npos = path.rfind("\\");
	if( npos != -1 )
		strMapName = path.substr(npos+1, path.length());
	npos = strMapName.rfind(".");
	if( npos != -1 )
		strMapName = strMapName.substr(0, npos);

	char cc1[500],cc2[500],cc3[500],cc4[500];
	sprintf(cc1, "%s/%s%s", m_szLogPath, strMapName.c_str(), "[1].info");
	sprintf(cc2, "%s/%s%s", m_szLogPath, strMapName.c_str(), "[2].info");
	sprintf(cc3, "%s/%s%s", m_szLogPath, strMapName.c_str(), "[3].info");
	sprintf(cc4, "%s/%s%s", m_szLogPath, strMapName.c_str(), "[4].info");

	::DeleteFile( cc4);
	::MoveFile( cc3, cc4 );
	::MoveFile( cc2, cc3 );
	::MoveFile( cc1, cc2 );	

	CopyFile(m_szOperationFilePath, cc1, false);

	this->Realese();
}

}





