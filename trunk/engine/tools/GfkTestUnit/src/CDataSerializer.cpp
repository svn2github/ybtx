#include "stdafx.h"
#include "CDataSerializer.h"
#include "tinyxml/tinyxml.h"
#include "CEffectMallocObject.h"

bool CXMLDataSerializer::FromXml( AutoDataHelper* pAutoData,TiXmlNode* Node )
{
	vector<EAutoCtrlData*>& AutoVec = *pAutoData->GetKeyUintDatas();
	TiXmlElement* pElement = Node->ToElement();
	if(pElement==NULL)
		return false;

	for( size_t i = 0; i<AutoVec.size(); ++i )
	{
		TiXmlElement* pChildNode = pElement->FirstChildElement(AutoVec[i]->m_Title.c_str());

		if(pChildNode == NULL)
			continue;

		switch(AutoVec[i]->m_pCtrlData->m_eDataType)
		{
		case E_INTEGER:
		case E_BYTE:
		case E_BOOLValue:
		case E_DBWORD:
			pChildNode->QueryIntAttribute("value",(int*)AutoVec[i]->m_pCtrlData->m_pData);
			break;
		case E_FLOAT:
			pChildNode->QueryFloatAttribute("value",(float*)AutoVec[i]->m_pCtrlData->m_pData);
			break;
		case E_STRING:
		case E_STRINGBUTTON:
			*(string*)AutoVec[i]->m_pCtrlData->m_pData = pChildNode->Attribute("value");
			break;
		case E_ESTRING:
		case E_ESTRINGBUTTON:
			*(EString*)AutoVec[i]->m_pCtrlData->m_pData = pChildNode->Attribute("value");
			break;			
		case E_VECTOR:
		case E_VECTOR2F:
		case E_ESETESTRING:
		case E_EVECESTRING:
		case E_COLORSTRING:
		case E_CHARPTR:
		case E_CUEESTRING:
		case E_COLOR4Map:
		case E_COLORMap:
		case E_UINT32CLMap:
		case E_FLOATCLMap:
		case E_ECOLORMap:
		case E_COMBO:
		case E_FloatMap:
		case E_EFloatMap:
		case E_BYTEMap:
		case E_EBYTEMap:
		case E_VectorMap:
		case E_EVectorMap:
		case E_EEStrMap:
		default:
			break;
		}

		if(AutoVec[i]->m_pCtrlData->m_pFunc)
			AutoVec[i]->m_pCtrlData->m_pFunc(AutoVec[i]->m_pCtrlData->m_pObj);
	}
	pAutoData->UIShowUpdate();
	return true;
}

bool CXMLDataSerializer::ToXml( AutoDataHelper* pAutoData,TiXmlNode* Node )
{
	Node->Clear();
	vector<EAutoCtrlData*>& AutoVec = *pAutoData->GetKeyUintDatas();

	for( size_t i = 0; i<AutoVec.size(); ++i )
	{
		TiXmlElement* childNode = Node->InsertEndChild( TiXmlElement(AutoVec[i]->m_Title) )->ToElement();
		switch(AutoVec[i]->m_pCtrlData->m_eDataType)
		{
		case E_INTEGER:
		case E_BYTE:
		case E_DBWORD:
			childNode->SetAttribute("value",*(int*)AutoVec[i]->m_pCtrlData->m_pData);
			break;
		case E_BOOLValue:
			childNode->SetAttribute("value",(*(bool*)AutoVec[i]->m_pCtrlData->m_pData)?1:0);
			break;
		case E_FLOAT:
			childNode->SetDoubleAttribute("value",*(float*)AutoVec[i]->m_pCtrlData->m_pData);
			break;
		case E_STRING:
		case E_STRINGBUTTON:
			childNode->SetAttribute("value",*(string*)AutoVec[i]->m_pCtrlData->m_pData);
			break;
		case E_ESTRING:
		case E_ESTRINGBUTTON:
			childNode->SetAttribute("value",((EString*)AutoVec[i]->m_pCtrlData->m_pData)->c_str());
			break;			
		case E_VECTOR:
		case E_VECTOR2F:
		case E_ESETESTRING:
		case E_EVECESTRING:
		case E_COLORSTRING:
		case E_CHARPTR:
		case E_CUEESTRING:
		case E_COLOR4Map:
		case E_COLORMap:
		case E_UINT32CLMap:
		case E_FLOATCLMap:
		case E_ECOLORMap:
		case E_COMBO:
		case E_FloatMap:
		case E_EFloatMap:
		case E_BYTEMap:
		case E_EBYTEMap:
		case E_VectorMap:
		case E_EVectorMap:
		case E_EEStrMap:
		default:
			break;
		}
	}
	return true;
}