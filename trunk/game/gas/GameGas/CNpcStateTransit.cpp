#include "stdafx.h"
#include "CNpcStateTransit.h"
#include "CNpcStateMetaData.h"
#include "CNpcEventMetaData.h"
#include "ErrLogHelper.h"
#include "PkgStream.h"
#include "CXmlConfig.inl"

CNpcStateTransit::CNpcStateTransit(uint32 uID, const string& strName)
	: CNpcStateMachine::TransitTableType((uint32)-1, NULL)
	, m_uID(uID)
	, m_strName(strName)
{
}

CNpcStateTransit::~CNpcStateTransit()
{
	DeleteAll();
}

bool CNpcStateTransitMgr::LoadConfig(const string& sFileName)
{
	ipkgstream file(L"", sFileName.c_str(), ios::binary);
	if( !file)
	{
		ostringstream strm;
		strm<<"Can't open file \""<<sFileName<<"\"";
		GenErr(strm.str());
	}

	CXmlConfig *pNpcAIServerCfg = new CXmlConfig( "root" , file );
	file.close();

	TiXmlNode *pRoot = pNpcAIServerCfg->GetRootNode();

	map<string, CNpcStateTransit*> mapSubMachine;
	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild=pChild->NextSibling())
	{
		TiXmlElement* pElemet = pChild->ToElement();
		if (!pElemet || pElemet->ValueStr() != "SubMachine")
			continue;

		const string& sName = pElemet->Attribute("Name");
		if (CNpcStateMetaDataMgr::GetInst()->GetEntity(sName) != NULL)
		{
			std::ostringstream oss;
			oss << "Npc状态：" << sName << "已存在，不能注册同名的子状态机";
			GenErr(oss.str());
		}

		CNpcStateTransit* pTransitTable = new CNpcStateTransit(GetSize(), sName); 
		CNpcSubMachineCreator* pCreator = new CNpcSubMachineCreator(pTransitTable);
		CNpcStateMetaData* pMetaData = new CNpcStateMetaData(CNpcStateMetaDataMgr::GetInst()->GetSize(), sName, pCreator);
		if (!CNpcStateMetaDataMgr::GetInst()->AddEntity(pMetaData))
		{
			std::ostringstream oss;
			oss << "状态：" << sName << "已存在，不能注册同名的子状态机";
			GenErr(oss.str());
		}

		if (mapSubMachine[sName] != NULL)
		{
			std::ostringstream oss;
			oss << "Npc子状态机：" << sName << "已存在，不能注册同名的子状态机";
			GenErr(oss.str());
		}

		mapSubMachine[sName] = pTransitTable;
	}

	//组织跳转表
	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild=pChild->NextSibling())
	{
		TiXmlElement* pElement = pChild->ToElement();
		if (!pElement || !(pElement->ValueStr() == "AI" || pElement->ValueStr() == "SubMachine"))
		{
			std::ostringstream oss;
			oss << "错误的结点:" << pElement->ValueStr() << endl;
			GenErr(oss.str());
		}

		if (!(pChild->FirstChild()->ToElement()->ValueStr() == "Item"))
		{
			std::ostringstream oss;
			oss << "错误的结点:" << pChild->FirstChild()->ToElement()->ValueStr() << endl;
			GenErr(oss.str());
		}

		const string& sTransitName = pElement->Attribute("Name");
		CNpcStateTransit* pTransitTable = NULL;

		if (pElement->ValueStr() == "AI")
		{
			pTransitTable = new CNpcStateTransit(GetSize(), sTransitName); 
		}		
		else if(pElement->ValueStr() == "SubMachine")
		{
			pTransitTable = mapSubMachine[sTransitName];
			pTransitTable->SetID(GetSize());
		}
		else
		{
			std::ostringstream oss;
			oss<<" 错误的节点："<<pElement->ValueStr()<<endl;
			GenErr(oss.str());
		}

		if (!CNpcStateTransitMgr::GetInst()->AddEntity(pTransitTable))
		{
			std::ostringstream oss;
			oss << "Name为" << sTransitName << "的AI配置已存在" ;
			GenErr(oss.str());
		}

		//将第一个状态设置成初始的状态
		const string& strEntryName = pChild->FirstChild()->ToElement()->Attribute("Current");
		CNpcStateMetaData *pStateMetaData = CNpcStateMetaDataMgr::GetInst()->GetEntity(strEntryName);
		if (pStateMetaData == NULL)
		{
			std::ostringstream oss;
			oss << "Npc状态：" << strEntryName << "不存在，请检查状态注册函数或NpcAI配置表";
			GenErr(oss.str());
		}

		pTransitTable->SetEntryCreator(pStateMetaData->GetID(), pStateMetaData->GetCreator());

		for (TiXmlNode* pSubChild = pChild->FirstChild(); pSubChild; pSubChild=pSubChild->NextSibling())
		{
			TiXmlElement* pSubElement = pSubChild->ToElement();
			if (!pSubElement)
				continue;
			Ast(pSubElement->ValueStr() == "Item");
			const string& strCurrentName = pSubElement->Attribute("Current");
			if (CNpcStateMetaDataMgr::GetInst()->GetEntity(strCurrentName) == NULL)
			{
				std::ostringstream oss;
				oss << "Name为" << strCurrentName << "的状态不存在" ;
				GenErr(oss.str());
			}
			uint32 uItemID = CNpcStateMetaDataMgr::GetInst()->GetEntity(strCurrentName)->GetID();
			CNpcStateTransit::ItemType* pItem = pTransitTable->GetEntity(uItemID);
			if (pItem == NULL)
			{
				pItem = new CNpcStateTransit::ItemType(uItemID);
				pTransitTable->AddEntity(pItem);
			}
			for (TiXmlNode* pSubSubChild = pSubChild->FirstChild(); pSubSubChild; pSubSubChild=pSubSubChild->NextSibling())
			{
				TiXmlElement* pSubSubElement = pSubSubChild->ToElement();
				if (!pSubSubElement)
					continue;
				Ast(pSubSubElement->ValueStr() == "Transit");
				const string& strEventName = pSubSubElement->Attribute("Event");
				const string& strNextName = pSubSubElement->Attribute("Next");
				CNpcEventMetaData *pEventMetaData = CNpcEventMetaDataMgr::GetInst()->GetEntity(strEventName);
				CNpcStateMetaData *pNextMetaData = CNpcStateMetaDataMgr::GetInst()->GetEntity(strNextName);
				if (pEventMetaData == NULL)
				{
					std::ostringstream oss;
					oss << "Npc事件：" << strEventName << "不存在，请检查事件注册函数或NpcAI配置表";
					GenErr(oss.str());
				}
				if (pNextMetaData == NULL)
				{
					std::ostringstream oss;
					oss << "Npc状态：" << strNextName << "不存在，请检查状态注册函数或NpcAI配置表";
					GenErr(oss.str());
				}
				if (!pItem->AddTransit(pEventMetaData->GetID(), pNextMetaData->GetID(), pNextMetaData->GetCreator()))
				{
					std::ostringstream oss;
					oss << "Name为" << pElement->Attribute("Name") << "的状态" << strCurrentName << "所响应的事件" << strEventName << "已存在";
					GenErr(oss.str());
				}
			}
		}
		try
		{
			pTransitTable->CheckValid();
		}
		catch(uint32& uStateID)
		{
			std::ostringstream oss;
			oss << "在AI配置 " << pElement->Attribute("Name") << " 中Name为 " << CNpcStateMetaDataMgr::GetInst()->GetEntity(uStateID)->GetName() << " 的状态没有相应的配置项";
			LogErr(oss.str().c_str());
			GenErr(oss.str());
		}
	}
	delete pNpcAIServerCfg;
	pNpcAIServerCfg = NULL;

	return true;
}

void CNpcStateTransitMgr::UnloadConfig()
{
	DeleteAll();
}

