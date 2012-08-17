#include "stdafx.h"
#include "CoordinateManager.h"
#include "COriFileOperate.h"
#include "CXmlConfig.inl"

void CoordinateManager::ReadXml(string strXML)
{
	ifstream config(strXML.c_str());
	if(!config)
		return;

	CXmlConfig *pXmlCfg = new CXmlConfig();
	pXmlCfg->InitConfig( "root" , config );
	config.close();

	TiXmlNode* pFileTeam = pXmlCfg->Get<TiXmlNode*>("File_Team");
	if( pFileTeam == NULL )
	{
		delete pXmlCfg;
		return; 
	}
	TiXmlNode* pReferFile = pFileTeam->FirstChild("ReferCoorFile");
	m_strReferFile = pReferFile->ToElement()->Attribute("Name");
	for (TiXmlNode* pFileNode = pFileTeam->FirstChild("Section");pFileNode;pFileNode = pFileNode->NextSibling("Section"))
	{
		string strFileName = pFileNode->ToElement()->Attribute("FileName");
		if(m_mapFileList.count(strFileName) == 0)
		{
			m_mapFileList[strFileName] = vector<CoordinateData>();
		}
		for (TiXmlNode* pCoorCol = pFileNode->FirstChild("CoordinateCol");pCoorCol;pCoorCol = pCoorCol->NextSibling("CoordinateCol"))
		{
			CoordinateData data;
			data.m_strScene = pCoorCol->ToElement()->Attribute("Name");
			TiXmlNode* pXYCol = pCoorCol->FirstChild("XYCol");
			data.m_strX = pXYCol->ToElement()->Attribute("X");
			data.m_strY = pXYCol->ToElement()->Attribute("Y");
			m_mapFileList[strFileName].push_back(data);
		}
	}
}

void CoordinateManager::ReadCoordinateData()
{
	COriFileOperate oriFile(m_strReferFile);
	oriFile.CleanNoteRow();
	oriFile.CleanBlanRow();
	oriFile.CleanBlankCol();
	int row = (int)oriFile.GetRowCount();
	vector<string> vecFirst;
	oriFile.GetOneRow(vecFirst,0);
	int searchPos = GetColPosByName("SceneName",vecFirst);
	if (searchPos == -1)
		return;
	for (int i=1;i<row;++i)
	{
		string strSceneName = oriFile.GetDataByRowCol(i,searchPos);
		string strPosX = oriFile.GetDataByRowCol(i,searchPos+1);
		string strPosY = oriFile.GetDataByRowCol(i,searchPos+2);
		int posX = atoi(strPosX.c_str());
		int posY = atoi(strPosY.c_str());
		if (m_mapCoordinate.count(strSceneName) == 0)
		{
			m_mapCoordinate[strSceneName] = map<int,int>();
		}
		m_mapCoordinate[strSceneName].insert(make_pair(posX,posY));
	}
}

void CoordinateManager::TranslateCoordinate()
{
	map<string,vector<CoordinateData> >::iterator iter = m_mapFileList.begin();
	for (;iter != m_mapFileList.end();++iter)
	{
		string strFileName = iter->first;
		vector<CoordinateData>& vecCol = iter->second;
		COriFileOperate oriFile(strFileName);
		int row = (int)oriFile.GetRowCount();
		int startRow = 1;
		if (oriFile.GetDataByRowCol(0,0) == "NOTE:")
			startRow = 2;
		vector<string> vecFirst;
		oriFile.GetOneRow(vecFirst,startRow-1);
		vector<vector<int> > vecScene;
		for (int i=0;i<(int)vecCol.size();++i)
		{
			CoordinateData& dataRef = vecCol[i];
			int firstPos = GetColPosByName(dataRef.m_strScene,vecFirst);
			int secondPos = GetColPosByName(dataRef.m_strX,vecFirst);
			int thirdPos = GetColPosByName(dataRef.m_strY,vecFirst);
			vecScene.push_back(vector<int>());
			vecScene.back().push_back(firstPos);
			vecScene.back().push_back(secondPos);
			vecScene.back().push_back(thirdPos);
		}
		for (int j=startRow;j<row;++j)
		{
			for (int k=0;k<(int)vecScene.size();++k)
			{
				string strScene = oriFile.GetDataByRowCol(j,vecScene[k][0]);
				if (m_mapCoordinate.count(strScene))
				{
					string strPosX = oriFile.GetDataByRowCol(j,vecScene[k][1]);
					string strPosY = oriFile.GetDataByRowCol(j,vecScene[k][2]);
					int posX = atoi(strPosX.c_str());
					int posY =  atoi(strPosY.c_str());
					posX += m_mapCoordinate[strScene].begin()->first;
					posY += m_mapCoordinate[strScene].begin()->second;
					char numArry[10];
					sprintf(numArry,"%d",posX);
					strPosX = numArry;
					sprintf(numArry,"%d",posY);
					strPosY = numArry;
					oriFile.AddData_toTab(strPosX,j,vecScene[k][1],true);
					oriFile.AddData_toTab(strPosY,j,vecScene[k][2],true);
				}
			}
		}
		oriFile.SaveFile();
	}
	MessageBox(NULL,"坐标调整已经完成","完成",MB_OK);
}

int CoordinateManager::GetColPosByName(const string& strCol,const vector<string>& vecFirstRow)
{
	int pos = -1;
	for (int i=0;i<(int)vecFirstRow.size();++i)
	{
		if (strCol == vecFirstRow[i])
		{
			pos = i;
			break;
		}
	}
	return pos;
}
