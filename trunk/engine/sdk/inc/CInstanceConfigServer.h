#pragma once 

namespace sqr
{
	class CXmlConfig;

	class CInstanceConfigServer
	{
	public:
		CInstanceConfigServer(const string& sInstancePath, const string& sInstanceName);
		~CInstanceConfigServer();

		static CInstanceConfigServer* Inst();

		const char* GetInstanceName();
		uint32 GetChildVMNum();

	private:
		static CInstanceConfigServer*	ms_pInst;

		CXmlConfig*	m_pConfig;

		string	m_sInstanceName;
		uint32 m_uChildVMNum;
	};

}
