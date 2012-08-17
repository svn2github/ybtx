#include "StdAfx.h"
#include "AudioViewCtrl/AmbientAudioCtrlImp.h"
#include "CXmlConfig.inl"
#include <fstream>

DEF_UI_IMP(CAmbientAudioCtrlImp);
CAmbientAudioCtrlImp::CAmbientAudioCtrlImp(void* param)
{
	m_nAudioRange = 12;
}

CAmbientAudioCtrlImp::~CAmbientAudioCtrlImp(void)
{
}

void CAmbientAudioCtrlImp::Update()
{
	CCtrlBase::Update();
}


bool CAmbientAudioCtrlImp::_ProcMsg( const CMsg& msg )
{
	return false;
}

void CAmbientAudioCtrlImp::SetContext( CEditContext* pContext )
{
	m_pOwnContext = pContext;
}

void CAmbientAudioCtrlImp::LoadCueTimeConfig()
{
	locale loc = locale::global(locale(""));
	string strFileName = "音效配置文件\\AudioConfig.xml";
	ifstream iXml(strFileName.c_str());
	locale::global(loc);
	if ( !iXml.good() )
	{
		MessageBox(NULL, "请检查是否有音效配置文件, 请检查或者全部更新", "提示", NULL);
		return;
	}

	string TempValue = "";
	int    nValue = 0;
	int	   id = 0;

	CXmlConfig *pAudioCueTimeConfig = new CXmlConfig( "AudioConfig", iXml );
	TiXmlNode  *pRootNode = pAudioCueTimeConfig->GetRootNode();

	///lift property
	TiXmlNode  *pIntervalAttributeNode = pRootNode->FirstChild("Cue_Play_Interval_Attribute");
	while( pIntervalAttributeNode )
	{
		TiXmlElement *pElement = pIntervalAttributeNode->ToElement();
		if( pElement )
		{
			TempValue = pElement->Attribute("id");
			sscanf(TempValue.c_str(), "%d", &id);

			SCueTimePro sTempPro;

			TiXmlNode *pNode = pIntervalAttributeNode->FirstChild("PlayTime");
			if( pNode )
			{
				TiXmlElement *pElement = pNode->ToElement();
				if (pElement)
				{
					TempValue = pElement->Attribute("Min");
					sscanf(TempValue.c_str(), "%d", &nValue);
					sTempPro.nPlayTimeLenMin = nValue;

					TempValue = pElement->Attribute("Max");
					sscanf(TempValue.c_str(), "%d", &nValue);
					sTempPro.nPlayTimeLenMax = nValue;
				}
			}

			pElement = NULL;
			pElement = pIntervalAttributeNode->FirstChildElement("IntervalTime");
			if( pElement )
			{
				TempValue = pElement->Attribute("Min");
				sscanf(TempValue.c_str(), "%d", &nValue);
				sTempPro.nPlayIntervalMin = nValue;

				TempValue = pElement->Attribute("Max");
				sscanf(TempValue.c_str(), "%d", &nValue);
				sTempPro.nPlayIntervalMax = nValue;
			}

			m_cueAttributeList.push_back(sTempPro);
		}

		pIntervalAttributeNode = pIntervalAttributeNode->NextSibling("Cue_Play_Interval_Attribute");
	}

	///播放范围（控制是否播放，也即包围盒控制）
	TiXmlNode  *pPlayRangeNode = pRootNode->FirstChild("Cue_Play_Range");
	TiXmlElement *pRangeValueElement = pPlayRangeNode->ToElement();
	if( pRangeValueElement )
	{
		TempValue = pRangeValueElement->Attribute("default");

		int nRange = 0;
		sscanf(TempValue.c_str(), "%d", &nRange);
		//CSceneAudioMgr::GetInst()->SetSceneCueBoxRange(nRange);
		m_nAudioRangeList.push_back(nRange);

		TiXmlElement *pRangeUnitValueElement = NULL;
		pRangeUnitValueElement = pRangeValueElement->FirstChildElement("Range");
		while(pRangeUnitValueElement)
		{
			TempValue = pRangeUnitValueElement->Attribute("value");
			sscanf(TempValue.c_str(), "%d", &nRange);
			m_nAudioRangeList.push_back(nRange);

			//移动到下一个
			pRangeUnitValueElement = pRangeUnitValueElement->NextSiblingElement( "Range" );
		}
	}

	delete pAudioCueTimeConfig;
	pAudioCueTimeConfig = NULL;

	iXml.close();
}

size_t CAmbientAudioCtrlImp::GetCueAttributeCount()
{
	return m_cueAttributeList.size();
}

void CAmbientAudioCtrlImp::SetCuePlayTimeLenMin(const size_t index, const int time)
{

}

int CAmbientAudioCtrlImp::GetCuePlayTimeLenMin( const size_t index )
{
	if( index > m_cueAttributeList.size() )
		return 0;

	return m_cueAttributeList[index].nPlayTimeLenMin;
}

void CAmbientAudioCtrlImp::SetCuePlayTimeLenMax(const size_t index, const int time)
{
	if( index < m_cueAttributeList.size() )
		m_cueAttributeList[index].nPlayTimeLenMax = time;
}

int CAmbientAudioCtrlImp::GetCuePlayTimeLenMax( const size_t index )
{
	if( index > m_cueAttributeList.size() )
		return 0;

	return m_cueAttributeList[index].nPlayTimeLenMax;
}

void CAmbientAudioCtrlImp::SetCueIntervalTimeLenMin(const size_t index, const int time)
{
	if( index < m_cueAttributeList.size() )
		m_cueAttributeList[index].nPlayIntervalMin = time;
}

int CAmbientAudioCtrlImp::GetCueIntervalTimeLenMin( const size_t index )
{
	if( index > m_cueAttributeList.size() )
		return 0;

	return m_cueAttributeList[index].nPlayIntervalMin;
}

void CAmbientAudioCtrlImp::SetCueIntervalTimeLenMax(const size_t index, const int time)
{
	if( index < m_cueAttributeList.size() )
		m_cueAttributeList[index].nPlayIntervalMax = time;
}

int CAmbientAudioCtrlImp::GetCueIntervalTimeLenMax( const size_t index )
{
	if( index < m_cueAttributeList.size() )
		return m_cueAttributeList[index].nPlayIntervalMax;

	return 0;
}

size_t CAmbientAudioCtrlImp::GetAudioRangeCount()
{
	return m_nAudioRangeList.size();
}

int CAmbientAudioCtrlImp::GetAudioRange( const size_t index )
{
	if( index < m_nAudioRangeList.size() )
		return m_nAudioRangeList[index];

	return 12;
}