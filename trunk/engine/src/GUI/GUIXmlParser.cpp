#include "stdafx.h"
#include "GUIXmlParser.h"

namespace sqr
{
	
	XMLParser::XMLParser() : m_idtfString("Unknown XML parser"),m_beInit(false)
	{
		// ... 构造函数
	}

	XMLParser::~XMLParser()
	{
		// ... 析构函数
	}

	bool XMLParser::Initialise()
	{
		if ( !m_beInit )
		{
			m_beInit = InitilaliseImpl();
		}
		return m_beInit;
	}

	void XMLParser::Cleanup()
	{
		if ( m_beInit )
		{
			CleanupImpl();
			m_beInit = false;
		}
	}

	const char* XMLParser::GetIdentifierString() const
	{
		return m_idtfString.c_str();
	}

}