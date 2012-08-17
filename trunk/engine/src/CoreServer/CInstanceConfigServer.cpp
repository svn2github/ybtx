#include "stdafx.h"
#include "CInstanceConfigServer.h"
#include "CXmlConfig.inl"
#include "StringHelper.h"
#include "PkgStream.h"

CInstanceConfigServer* CInstanceConfigServer::ms_pInst = NULL;

CInstanceConfigServer::CInstanceConfigServer(const string& sInstancePath, const string& sInstanceName)
: m_sInstanceName(sInstanceName)
{
	if (ms_pInst)
	{
		GenErr("CInstanceConfigServer不能够重复创建");
	}

	ms_pInst = this;

	string sInstanceConfig = format("%s/%s.xml", sInstancePath.c_str(), sInstanceName.c_str());
	ipkgstream strmInstConfig(L"", sInstanceConfig.c_str());
	if(!strmInstConfig)
		GenErr("Can't open Gas Instance Config file.");

	m_pConfig = new CXmlConfig("instance_config" , strmInstConfig);

	m_uChildVMNum = m_pConfig->Get<uint32>("child_vm_num");	
}

CInstanceConfigServer::~CInstanceConfigServer()
{
	delete m_pConfig;

	Ast(ms_pInst);
	ms_pInst = NULL;
}

CInstanceConfigServer* CInstanceConfigServer::Inst()
{
	return ms_pInst;
}

const char* CInstanceConfigServer::GetInstanceName()
{
	return m_sInstanceName.c_str();
}

uint32 CInstanceConfigServer::GetChildVMNum()
{
	return m_uChildVMNum;
}

