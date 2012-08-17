#include "stdafx.h"
#include "CError.h"
#include "CCallStackInfo.h"
#include "MemoryHelper.h"

CError::CError(const std::string& sErrorType) throw()
: m_sErrorType(sErrorType.c_str())
{
	CCallStackInfo* pInfo=new CCallStackInfo;
	m_sStackInfo.append(pInfo->GetText());
	delete pInfo;

	GetMemInfo();
}

CError::CError(const std::string& sErrorType, const std::string& sStackInfo) throw()
: m_sErrorType(sErrorType.c_str())
, m_sStackInfo(sStackInfo.c_str())
{
	GetMemInfo();
}

CError::CError(const char* sErrorType) throw()
: m_sErrorType(sErrorType)
{
	CCallStackInfo* pInfo=new CCallStackInfo;
	m_sStackInfo.append(pInfo->GetText());
	delete pInfo;

	GetMemInfo();
}

CError::CError(const char* sErrorType, const char* sStackInfo) throw()
: m_sErrorType(sErrorType)
, m_sStackInfo(sStackInfo)
{
	GetMemInfo();
}


CError::CError(const CError& exp) throw()
: m_sErrorType(exp.ErrorTitle())
, m_sErrorMsg(exp.ErrorMsg())
, m_sStackInfo(exp.StackInfo())
, m_sCategory(exp.Category())
, m_sExpandInfo(exp.ExpandInfo())
{
	GetMemInfo();
}

CError::~CError()throw()
{
}

CError& CError::operator =(const CError& exp) throw()
{
	if(this == &exp)
		return *this;

	m_sErrorType = exp.ErrorTitle();
	m_sErrorMsg = exp.ErrorMsg();
	m_sStackInfo = exp.StackInfo();
	m_sCategory = exp.Category();
	m_sExpandInfo = exp.ExpandInfo();

	return *this;
}

const char* CError::ErrorTitle() const throw()
{
	return m_sErrorType.c_str();
}

const char* CError::ErrorMsg()const throw()
{
	return m_sErrorMsg.c_str();
}


const char* CError::StackInfo()const throw()
{
	return m_sStackInfo.c_str();
}

const char* CError::Category() const throw()
{
	return m_sCategory.c_str();
}

const char* CError::ExpandInfo() const throw()
{
	return m_sExpandInfo.c_str();
}

const char* CError::MemInfo() const throw()
{
	return m_sMemInfo.c_str();
}

void CError::GetMemInfo()
{
#ifdef _WIN32
	//windows下面我们记录当前内存的信息
	ProcessMemInfo MemInfo;
	GetProcessMemInfo(&MemInfo);

	char buffer[128];
	const char* szFormat = "剩余物理内存: %lu MB; 剩余页交换文件: %lu MB; 占用的物理内存: %lu MB; 占用的页交换文件: %lu MB\n";

	_snprintf(buffer, sizeof(buffer) - 1, szFormat, MemInfo.stAvailPhysMem,
		MemInfo.stAvailPageMem, MemInfo.stProcessPhys, MemInfo.stProcessPage);

	m_sMemInfo = buffer;
#endif
}

void CError::AppendType(const std::string& sType, bool bAppendEnd) throw()
{
	if(sType.empty())
		return;

	AppendType(sType.c_str(), bAppendEnd);
}

void CError::AppendType(const char* sType, bool bAppendEnd) throw()
{
	if (!sType)
	{
		return;
	}

	if(bAppendEnd)
	{
		m_sErrorType.append("\t").append(sType);
	}
	else
	{
		ErrorStr_t str = sType;
		str.append("\t");
		m_sErrorType.insert(0, str);
	}
}


void CError::AppendMsg(const string& strMsg, bool bAppendEnd) throw()
{
	if(strMsg.empty())
		return;

	AppendMsg(strMsg.c_str(), bAppendEnd);

}

void CError::AppendMsg(const char* strMsg, bool bAppendEnd) throw()
{
	if (!strMsg)
	{
		return;
	}

	if(bAppendEnd)
		m_sErrorMsg.append("\n").append(strMsg);
	else
	{
		ErrorStr_t str = strMsg;
		str.append("\n");

		m_sErrorMsg.insert(0, str);
	}

}

void CError::AppendStackInfo(const string& strStackInfo) throw()
{
	if( strStackInfo.empty() )
		return;

	AppendStackInfo(strStackInfo.c_str());
}

void CError::AppendStackInfo(const char* strStackInfo) throw()
{
	if (!strStackInfo)
	{
		return;
	}

	m_sStackInfo.append("\n").append(strStackInfo);	
}

void CError::SetCategory(const std::string& sCategory) throw()
{
	m_sCategory = sCategory.c_str();	
}

void CError::SetCategory(const char* sCategory) throw()
{
	if (!sCategory)
	{
		return;
	}

	m_sCategory = sCategory;
}

void CError::AppendInfo(const std::string& sInfo) throw()
{
	if(sInfo.empty())
		return;
	AppendInfo(sInfo.c_str());
}

void CError::AppendInfo(const char* sInfo) throw()
{
	if (!sInfo)
	{
		return;
	}

	if(!m_sExpandInfo.empty())
		m_sExpandInfo.append("\n");

	m_sExpandInfo.append(sInfo);
}

