#pragma once

#include <string>
#include <cstring>
#include "CStaticObject.h"

namespace sqr
{

struct CAddress
			:public virtual sqr::CStaticObject
{
private:
	std::string	m_sAddress;
	uint32		m_uPort;

public:

	CAddress()
			:m_uPort(0)
	{
	}

	bool operator==(const CAddress& ano)const
	{
		return (strcmp(m_sAddress.c_str(),ano.m_sAddress.c_str())==0 ) && (m_uPort==ano.m_uPort);
	}

	const CAddress& operator=(const CAddress& ano)
	{
		m_sAddress=ano.m_sAddress;
		m_uPort=ano.m_uPort;
		return *this;
	}

	void SetAddress(const char* szAddress)
	{
		m_sAddress=szAddress;
	}

	const char* GetAddress()const
	{
		return m_sAddress.c_str();
	}

	const std::string& GetAddressByString()const
	{
		return m_sAddress;
	}

	void GetAddressAndPort(string& str)
	{
		char buf[32];
		sprintf(buf, "%s %u", m_sAddress.c_str(), m_uPort);
		str.append(buf);
	}

	void SetPort(uint32 uArgPort)
	{
		m_uPort=uArgPort;
	}

	uint32 GetPort()const
	{
		return m_uPort;
	}
};

}
