#pragma once
#include "TSingleton.h"
#include "Tinyxml.h"
#include "CRenderGroup.h"
#include "CGraphic.h"

namespace sqr_tools
{
	class CEditorConfig : public TSingleton<CEditorConfig>
	{
	public:
		CEditorConfig();
		~CEditorConfig();

		const wstring&	GetEditorResourcePath(void);
		const wstring&	GetEditorAppPath(void);
		::TiXmlNode*	GetEditorConfig(const string& node_name);
		bool			InitEditorConfig( const char* filename = NULL );
		bool			SaveEditorConfig( const char* filename = NULL );
		void			Initialize(void);

		static TiXmlNode& GetNodeChild( TiXmlNode& node, const string& child_name );

		template <class ValueType>
		void		Set( TiXmlNode& node, const string& elem, const string& attribute, const ValueType& value )
		{
			TiXmlElement* pElem = node.FirstChildElement(elem);
			if ( !pElem )
				pElem = node.InsertEndChild(TiXmlElement(elem));

			string		 value_str;
			stringstream ss;
			ss << value;
			value_str = ss.str();

			pElem->SetAttribute(attribute, value_str);
		}

		template <class ValueType>
		ValueType Get( TiXmlNode& node, const string& elem, const string& attribute )
		{
			TiXmlElement* pElem = node.FirstChildElement(elem);
			if ( !pElem )
			{
				pElem = node.InsertEndChild(TiXmlElement(elem));
				pElem->SetAttribute(attribute, "");
			}

			ValueType	 ret;
			stringstream ss;
			string value_str = pElem->Attribute(attribute);
			ss << value_str;
			ss >> ret;

			return ret;
		}


	protected:
		TiXmlDocument	m_CfgXml;
		wstring			m_strAppPath;
		wstring			m_strResPath;
		wstring			m_CfgFileName;
	};

	inline const wstring&	CEditorConfig::GetEditorResourcePath(void)
	{
		return m_strResPath;
	}

	inline const wstring&	CEditorConfig::GetEditorAppPath(void)
	{
		return m_strAppPath;
	}
}