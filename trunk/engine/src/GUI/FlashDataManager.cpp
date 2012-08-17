#include "stdafx.h"
#include "FlashDataManager.h"
#include "tinyxml/tinyxml.h"
#include "PkgStream.h"
#include "StringHelper.h"
#include "CodeCvs.h"
#include <iostream>

namespace sqr
{
	const UIString FlashDataManager::Flash_Root	= "root";				// 根
	const UIString FlashDataManager::Flash_Info	= "FlashInfo";			// 节点
	const UIString FlashDataManager::Flash_Tex	= "Tex";				// 材质
	const UIString FlashDataManager::Flash_Name	= "Name";				// 名字
	const UIString FlashDataManager::Flash_Ato	= "Ato";				// alpha
	const UIString FlashDataManager::Flash_Afrom= "Afrom";				// alpha
	const UIString FlashDataManager::Flash_Pos	= "Pos";				// 位置
	const UIString FlashDataManager::Flash_V	= "V";					// 单程时间
	const UIString FlashDataManager::Flash_Count= "Count";				// 闪烁次数
	const UIString FlashDataManager::Flash_RePos= "RePos";

	const int32 FlashDataManager::Flash_Scale	= 1000;					// 缩放比1000

	FlashDataManager::FlashDataManager()
		: m_isLoad(false)
		, m_pGraphic(NULL)
	{
		m_FlashData.clear();
		IntInst() = this;
	}

	FlashDataManager::~FlashDataManager()
	{
		IntInst() = NULL;
		Release();
	}

	FlashDataManager* FlashDataManager::Inst()
	{
		return IntInst();
	}

	FlashDataManager*& FlashDataManager::IntInst()
	{
		static FlashDataManager*	ls_pFlashMgr=NULL;
		return ls_pFlashMgr;
	}

	CFRect FlashDataManager::StringToCFRect(std::string& str)
	{
		int pos, tempInt;
		string sub, subend;
		CFRect tempRt;

		pos = str.find_first_of(',');
		sub = str.substr(0,pos);
		subend = str.substr(pos + 1, str.length() - pos - 1);
		integer(sub, tempInt);
		tempRt.left = (float)tempInt;

		pos = subend.find_first_of(',');
		sub = subend.substr(0,pos);
		subend = subend.substr(pos + 1, subend.length() - pos - 1);
		integer(sub, tempInt);
		tempRt.top = (float)tempInt;

		pos = subend.find_first_of(',');
		sub = subend.substr(0,pos);
		subend = subend.substr(pos + 1, subend.length() - pos - 1);
		integer(sub, tempInt);
		tempRt.right = (float)tempInt;

		integer(subend, tempInt);
		tempRt.bottom = (float)tempInt;

		return tempRt;
	}

	// 初始化表情系统
	bool FlashDataManager::Initialize(const string& Path, IGraphic* pIGraphic)
	{
		if(!m_isLoad)
		{
			m_pGraphic = pIGraphic;

			ipkgstream iXML;
			TiXmlDocument XmlDoc;

			UIString Str = Path.c_str();
			Str = Str.substr(0, Str.size() - 15);
			Str += "FlashInfo.xml";

			if (iXML.open(PATH_ALIAS_EMT.c_str(), Str.c_str()))
				return false;

			iXML>>XmlDoc;
			TiXmlNode* EmtRoot = XmlDoc.FirstChild( Flash_Root.c_str() );

			for( TiXmlNode* pNode = EmtRoot->FirstChild( Flash_Info.c_str() );pNode; pNode = pNode->NextSibling( Flash_Info.c_str() ) )
			{
				TiXmlElement* pElement = pNode->ToElement();
				if (!pElement)
					continue;

				UIString	temp_name;
				UIString	temp_texname;

				int32		temp_V;
				int32		temp_count;
				float		temp_Amin;
				float		temp_Amax;
				CFRect		temp_pos;
				CFRect		temp_repos;
				int32		temp_int;

				// ID
				std::string tempStr = pElement->Attribute( Flash_Name.c_str() );
				temp_name = tempStr.c_str();
				if (temp_name == "NULL")
					continue;

				// POS
				tempStr = pElement->Attribute( Flash_Pos.c_str() );
				temp_pos =  StringToCFRect(tempStr);

				// REPOS
				tempStr = pElement->Attribute( Flash_RePos.c_str() );
				temp_repos = StringToCFRect(tempStr);

				// V
				tempStr = pElement->Attribute( Flash_V.c_str() );
				integer(tempStr, temp_V);

				// Count
				tempStr = pElement->Attribute( Flash_Count.c_str() );
				integer(tempStr, temp_count);

				// Amin
				tempStr = pElement->Attribute( Flash_Ato.c_str() );
				integer(tempStr, temp_int);
				temp_Amin = temp_int * 1.0f / Flash_Scale;

				// Amax
				tempStr = pElement->Attribute( Flash_Afrom.c_str() );
				integer( tempStr, temp_int );
				temp_Amax = temp_int * 1.0f / Flash_Scale;

				// TEXTURE NAME
				TiXmlNode* pElemNode = pNode->FirstChild( Flash_Tex.c_str() );
				TiXmlElement* pElem  = pElemNode->ToElement();
				if(!pElem)
					continue;
				temp_texname = pElem->GetText();

				FlashWndInfo* pFlash = new FlashWndInfo();
				pFlash->Init(pIGraphic, temp_name, temp_texname, temp_pos, temp_V, temp_count, temp_Amin, temp_Amax, temp_repos);

				// emt map
				m_FlashData.insert(pair<UIString, FlashWndInfo*>(temp_name, pFlash));
			}
		}
		m_isLoad = true;
		return m_isLoad;
	}

	// 释放申请资源
	void FlashDataManager::Release(void)
	{
		FlashInfoMap::iterator itr = m_FlashData.begin();
		FlashInfoMap::iterator itr_end = m_FlashData.end();

		while(itr != itr_end)
		{
			GfkSafeDelete(itr->second);
			itr++;
		}
		m_FlashData.clear();
	}

	void FlashDataManager::EraserFlashData(FlashInfoMap::iterator it)
	{
		GfkSafeDelete(it->second);
		m_FlashData.erase(it);
	}

	FlashWndInfo* FlashDataManager::GetFlashInfo(UIString name)
	{
		FlashInfoMap::iterator itr = m_FlashData.begin();
		FlashInfoMap::iterator itr_end = m_FlashData.end();

		while(itr != itr_end)
		{
			if (itr->first == name)
				return itr->second;
			++itr;
		}
		return NULL;
	}

}// end of namespace
