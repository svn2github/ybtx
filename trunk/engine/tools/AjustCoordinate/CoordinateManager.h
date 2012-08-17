#pragma once

class CoordinateManager
{
public:
	void ReadXml(string strXML);
	void ReadCoordinateData();
	void TranslateCoordinate();
	int   GetColPosByName(const string& strCol,const vector<string>& vecFirstRow);
private:
	struct CoordinateData
	{
		string m_strScene;
		string m_strX;
		string m_strY;
	};
	string m_strReferFile;
	map<string,vector<CoordinateData> > m_mapFileList;
	map<string,map<int,int> > m_mapCoordinate;
};