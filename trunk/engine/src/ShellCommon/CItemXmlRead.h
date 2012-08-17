#pragma once

namespace sqr
{
	class CItemTableXml
	{
	public:
		CItemTableXml();
		~CItemTableXml();
		uint32 GetItemXmlNumber(){return m_Number;};
		const char * GetItemXmlAppName(uint32 index){return m_vAppName[index].c_str();};
		const char * GetItemXmlLanName(uint32 index){return m_vLanName[index].c_str();};
		uint32 GetItemXmlBigId(uint32 index){return m_vBigId[index];};
	private:
		vector<string> m_vAppName;
		vector<string> m_vLanName;
		vector<uint32> m_vBigId;
		uint32 m_Number;
	};
}

