#include "stdafx.h"
#include "CDbContext.h"

CDbContext::CDbContext()
: m_pValueTable(NULL)
{
}

CDbContext::~CDbContext()
{

}

void CDbContext::Clear()
{
	m_pValueTable = NULL;
	m_strReport = "";
}

void CDbContext::SetValueTable(CValueTable* pValueTable)
{
	m_pValueTable = pValueTable;
}

CValueTable* CDbContext::GetValueTable()
{
	return m_pValueTable;
}

void CDbContext::SetReport(const ReportString_t& strReport)
{
	m_strReport.assign(strReport.data(),strReport.size());
}

const char* CDbContext::GetReport()
{
	return m_strReport.c_str();
}
