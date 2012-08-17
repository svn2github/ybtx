#pragma once
#include "TSingleton.h"
#include "COriFileOperate.h"

class CBatchBaseItemDescript : public TSingleton<CBatchBaseItemDescript>
{
public:
	CBatchBaseItemDescript();
	void SplitBaseItemDescripType(string strFileName);
	string PickItemDisType(string& str);
	void BatchItemType();
	int GetColPosByName(string strCol,vector<string>& vecRef);
	bool ReadXml();

private:
	string m_strDestSave;
	string m_strItemPath;
	COriFileOperate m_destFile;
	vector<string> m_vecFile;
	string m_strXmlFile;
};