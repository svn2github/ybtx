#pragma once
#include "stdafx.h"
#include "tinyxml/tinyxml.h"
namespace sqr
{
	class CXmlReadAndWrite
	{
	public:
		CXmlReadAndWrite();
		~CXmlReadAndWrite();
		void XmlOpen(const char * FileName);
		void XmlClose(const char * FileName);
		const char * XmlRead(const char *NodeName);
		const char * XmlReadLast(const char *NodeName);
		const char * XmlReadNext(const char *NodeName);
		const char * XmlReadByNumber(uint32 NodeNumber);
		const char * XmlReadContentByNumber(uint32 NodeNumber);
		uint32 XmlNodeNumber();
		void XmlWrite(const char *NodeName);
		void XmlWrite(const char *NodeName, const char *NodeContent);
		bool XmlNodeIsExist();
		const char * XmlReadLastNode();
	private:
		TiXmlDocument * m_tDoc;
	};
}

