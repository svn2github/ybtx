#pragma once
#include "stdafx.h"
#include "tinyxml/tinyxml.h"
#include "CRenderGroup.h"
#include "CGraphic.h"
#include "CSceneConfig.h"
#include "CXmlConfig.h"

namespace sqr_tools
{
	extern const wstring PATH_ALIAS_EDITOR;

	class CEditorConfig : public sqr::TSingleton<CEditorConfig>
	{
	public:
		CEditorConfig();
		~CEditorConfig();

		const string&	GetEditorResourcePath(void);
		const string&	GetEditorAppPath(void);
		::TiXmlNode*	GetEditorConfig(const string& node_name);
		bool			InitEditorConfig( const char* filename = NULL );
		bool			SaveEditorConfig( const char* filename = NULL );
		void			GetEditorConfigValue(const char* Attri0, const char* Attri1, const char* key, string& strValue);
		void			Initialize(void);
		
	protected:
		TiXmlDocument	m_CfgXml;
		string			m_strAppPath;
		string			m_strResPath;
		string			m_CfgFileName;
	private:
		//±£¥Ê≥°æ∞≈‰÷√
		void SaveSceneConfig();
		bool IsOpen;
		bool			m_bInitialized;
	};

	inline const string&	CEditorConfig::GetEditorResourcePath(void)
	{
		return m_strResPath;
	}

	inline const string&	CEditorConfig::GetEditorAppPath(void)
	{
		return m_strAppPath;
	}

	extern LPCTSTR GetRegistKeyStr( LPCTSTR Path, LPCTSTR Name );

	class CXmlHelper
	{	
	public:
		static TiXmlNode& GetNodeChild( TiXmlNode& node, const string& child_name );

		template <class ValueType>
		static void		Set( TiXmlNode& node, const string& elem, const string& attribute, const ValueType& value )
		{
			TiXmlElement* pElem = node.FirstChildElement(elem);
			if ( !pElem )
				pElem = (TiXmlElement*)node.InsertEndChild(TiXmlElement(elem));

			string		 value_str;
			stringstream ss;
			ss << value;
			value_str = ss.str();

			pElem->SetAttribute(attribute, value_str);
		}

		template <class ValueType>
		static ValueType	Get( TiXmlNode& node, const string& elem, const string& attribute )
		{
			TiXmlElement* pElem = node.FirstChildElement(elem);
			if ( !pElem )
			{
				pElem = (TiXmlElement*)node.InsertEndChild(TiXmlElement(elem));
				pElem->SetAttribute(attribute, "");
			}

			ValueType	 ret;
			stringstream ss;
			string value_str = *(pElem->Attribute(attribute));
			ss << value_str;
			ss >> ret;

			return ret;
		}

		template <class ValueType>
		static ValueType	Get( TiXmlNode& node, const string& elem )
		{
			TiXmlElement* pElem = node.FirstChildElement(elem);
			if ( !pElem )
			{
				TiXmlText xmlText("");
				pElem = (TiXmlElement*)node.InsertEndChild(TiXmlElement(elem));
				pElem->InsertEndChild(xmlText);
			}

			ValueType	 ret;
			stringstream ss;
			const char* value_str = pElem->GetText();
			if ( value_str == 0  )
			{
				value_str = "";
			}
			ss << value_str;
			ss >> ret;

			return ret;
		}
	};
}
