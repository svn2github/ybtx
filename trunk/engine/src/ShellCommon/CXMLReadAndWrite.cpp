#include "stdafx.h"
#include "CXMLReadAndWrite.h"
#include "tinyxml/tinyxml.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CBaseScriptApp.h"
CXmlReadAndWrite::CXmlReadAndWrite()
{
}
CXmlReadAndWrite::~CXmlReadAndWrite()
{
}
void CXmlReadAndWrite::XmlOpen(const char * FileName)
{
	m_tDoc = new TiXmlDocument();
	SQR_TRY
	{
		if(!m_tDoc->LoadFile(FileName))
		{
			m_tDoc->InsertEndChild(TiXmlElement("root"));
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("没有对应的节点");
		LogExp(exp);
	}
	SQR_TRY_END;
}
bool CXmlReadAndWrite::XmlNodeIsExist()
{
	SQR_TRY
	{
		TiXmlNode * pRoot = m_tDoc->RootElement();
		if(pRoot->NoChildren())
		{
			return false;
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("没有对应的节点");
		LogExp(exp);
	}
	SQR_TRY_END;
	return true;
}

uint32 CXmlReadAndWrite::XmlNodeNumber()
{
	uint32 i = 0;
	SQR_TRY
	{
		TiXmlNode * pRoot = m_tDoc->RootElement();
		TiXmlNode * pNode;
		if(pRoot->NoChildren())
			return 0;
		for(pNode = pRoot->FirstChild();pNode;pNode = pNode->NextSibling())
		{
			i++;
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("没有对应的节点");
		LogExp(exp);
	}
	SQR_TRY_END;
	return i;
}

const char * CXmlReadAndWrite::XmlReadByNumber(uint32 NodeNumber)
{
	string *pString = CBaseScriptApp::GetStrIndexTlsData();
	SQR_TRY
	{
		TiXmlNode * pRoot = m_tDoc->RootElement();
		TiXmlNode * pNode = pRoot->FirstChild();
		for(uint32 i = 1;i<NodeNumber;i++)
		{
			pNode = pNode->NextSibling();
		}
		*pString = pNode->GetValue();
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("没有对应的节点");
		LogExp(exp);
	}
	SQR_TRY_END;
	return (*pString).c_str();
}
const char * CXmlReadAndWrite::XmlReadContentByNumber(uint32 NodeNumber)
{
	string *pString = CBaseScriptApp::GetStrIndexTlsData();
	SQR_TRY
	{
		TiXmlNode * pRoot = m_tDoc->RootElement();
		TiXmlNode * pNode = pRoot->FirstChild();
		for(uint32 i = 1;i<NodeNumber;i++)
		{
			pNode = pNode->NextSibling();
		}
		*pString = pNode->ToElement()->Attribute("Content");
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("没有对应的节点");
		LogExp(exp);
	}
	SQR_TRY_END;
	return (*pString).c_str();
}
const char * CXmlReadAndWrite::XmlReadLastNode()
{
	string *pString = CBaseScriptApp::GetStrIndexTlsData();
	SQR_TRY
	{
		TiXmlNode * pRoot = m_tDoc->RootElement();
		TiXmlNode * pNode = pRoot->LastChild();
		*pString = pNode->GetValue();
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("没有对应的节点");
		LogExp(exp);
	}
	SQR_TRY_END;
	return (*pString).c_str();

}
const char * CXmlReadAndWrite::XmlRead(const char * NodeName)
{
	string *pString = CBaseScriptApp::GetStrIndexTlsData();
	SQR_TRY
	{
		TiXmlNode * pRoot = m_tDoc->RootElement();
		TiXmlNode * pNode = pRoot->FirstChild(NodeName);
		*pString = pNode->ToElement()->Attribute("Content");
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("没有对应的节点");
		LogExp(exp);
	}
	SQR_TRY_END;
	return (*pString).c_str();
	
}
const char * CXmlReadAndWrite::XmlReadNext(const char * NodeName)
{
	string *pString = CBaseScriptApp::GetStrIndexTlsData();
	SQR_TRY
	{
		TiXmlNode * pRoot = m_tDoc->RootElement();
		TiXmlNode * pNode = pRoot->FirstChild(NodeName);
		string str = pRoot->LastChild()->GetValue();
		if(str!=NodeName)
		{
			*pString = pNode->ToElement()->Attribute("Next");
		}
		else
		{
			*pString = "";
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("没有对应的节点");
		LogExp(exp);
	}
	SQR_TRY_END;
	return (*pString).c_str();
}
const char * CXmlReadAndWrite::XmlReadLast(const char * NodeName)
{
	string *pString = CBaseScriptApp::GetStrIndexTlsData();
	SQR_TRY
	{
		TiXmlNode * pRoot = m_tDoc->RootElement();
		TiXmlNode * pNode = pRoot->FirstChild(NodeName);
		string str = pRoot->FirstChild()->GetValue();
		if(str!=NodeName)
		{
			*pString = pNode->ToElement()->Attribute("Last");
		}
		else
		{
			*pString = "";
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("没有对应的节点");
		LogExp(exp);
	}
	SQR_TRY_END;
	return (*pString).c_str();
}
void CXmlReadAndWrite::XmlWrite(const char *NodeName)
{
	TiXmlNode * pRoot;
	string strLastName;
	SQR_TRY
	{
		pRoot = m_tDoc->RootElement();
		bool NoChild = pRoot->NoChildren();
		if(!NoChild)
		{
			TiXmlElement * pPreNode = pRoot->LastChild()->ToElement();
			pPreNode->SetAttribute("Next",NodeName);
			strLastName = pPreNode->GetValue();
		}
		TiXmlElement * pNode = new TiXmlElement(NodeName);
		pRoot->LinkEndChild(pNode);
		if(!NoChild)
		{
			pNode->SetAttribute("Last",strLastName.c_str());
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("写入XML文件失败");
		LogExp(exp);
	}
	SQR_TRY_END;
}
void CXmlReadAndWrite::XmlWrite(const char *NodeName, const char *NodeContent)
{
	TiXmlNode * pRoot;
	SQR_TRY
	{
		pRoot = m_tDoc->RootElement();
		TiXmlElement * pNode = new TiXmlElement(NodeName);
		pRoot->LinkEndChild(pNode);
		pNode->SetAttribute("Content",NodeContent);
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("写入XML文件失败");
		LogExp(exp);
	}
	SQR_TRY_END;
}
void CXmlReadAndWrite::XmlClose(const char * FileName)
{
	m_tDoc->SaveFile(FileName);
	delete m_tDoc;
	m_tDoc = NULL;
}

