#include "stdafx.h"
#include "GUIStuffParser.h"
#include "GUIXmlHandler.h"


namespace sqr
{
	// GUI XML  解析
	StuffParser::StuffParser()
	{
		m_idtfString = "GUI Stuff Parser";
	}

	StuffParser::~StuffParser()
	{
		// ...  析构函数
	}

	// All stuff goes here
	void StuffParser::ParseXMLFile(XMLHandler& handler, TiXmlDocument& doc)
	{
		// 数据处理，读取根数据
		TiXmlElement* Root = doc.RootElement();
		if (Root)
		{
			// 遍历
			if ( !ParXMLElement(&handler,Root) )
			{
				// 解析失败，抛出异常 
				return;
			}
		}
		
	}

	bool StuffParser::ParXMLElement(XMLHandler* handler,TiXmlElement* ElementPtr)
	{
		// 解析
		if ( ElementPtr )
		{
			// 开始一个元素节点
			std::string element = ElementPtr->ValueStr();
			StartElement(handler, element.c_str(), ElementPtr);

			{
				// 递归调用
				TiXmlElement* ChildElement = ElementPtr->FirstChildElement();
				while(ChildElement)
				{
					ParXMLElement(handler, ChildElement);
					ChildElement = ChildElement->NextSiblingElement();
				}
			}

			// 结束该元素节点
			EndElement(handler, element.c_str());

			return true;
		}
		return false;
	}

	inline void StuffParser::StartElement(XMLHandler* handler, const char* element, TiXmlElement* attr)
	{
		handler->ElementStart(element, attr);
	}

	inline void StuffParser::EndElement(XMLHandler* handler, const char* element)
	{
		handler->ElementEnd(element);
	}

}