#pragma once
#include "TSingleton.h"

class CSplitInfo;
class COriFileOperate;

class CMakeReferRelation :public TSingleton<CMakeReferRelation>
{
public:
	CMakeReferRelation();
	//针对老外新需求读取引用表及前缀名功能名,实现双向索引功能
	static FTW_RESULT MakeDifList( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	static FTW_RESULT CreateRefer( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	static void WriteReferInfoToTxt();
	void OnMakeRefer(string& strFileName);

	void ReadItemTable();

	void CreateQuestReference(string& strFile);
	void BulitSprintTypeRefer(string& strFile,int row,string strFun = "");
	void BulitItemRequireRefer(string& strFile,int row,string strFun = "");
	void BulitMonsterRequireRefer(string& strFile,int row,string strFun = "");
	void BulitIntObjRequireeRefer(string& strFile,int row,string strFun = "");
	void BulitDropRefer(string& strFile,int row,string strFun = "");
	void BulitItemAwardRefer(string& strFile,int row,string strFun = "");
	void BulitReceiveItemRefer(string& strFile,int row,string strFun = "");
	void BuiltIntObjUseRequireRefer(string& strFile,int row,string strFun = "");


	void CreateQuestReferMap(string& strQuest,string& strFileName,string& strName,string& strDescript);
	void RelateTwoWayRefer(string& strQuest,string& strQuestIndex,string& strLangContent);
	string GetQuestNameFromMulitKey(string str);
	void BuiltQuestTwoWayRefer(string strFileName,vector<vector<string> >& vecRef);
	void JudgeAndCreateRefer(string strFileName);
	void BeginCreateQuestReferData();
	static FTW_RESULT SearchNeedFile( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void QueryNpcIDByRealOrDisplay(string& npc,vector<string>& result);

	void CreateItemWithDesc();
	void CreateQuestDescRefer();
	void CreateObjAndItemTwoWay();

	void BulitNewDescRefer(string& strFile,int row,string strFun = "");

	struct QuestReferData 
	{
		bool operator == (const QuestReferData& data)
		{
			if (m_strFile == data.m_strFile && m_strName == data.m_strName 
				&& m_strQuestName == data.m_strQuestName)
			{
				if (m_strReferRelation != data.m_strReferRelation && m_strReferRelation.find(data.m_strReferRelation.c_str()) == -1)
					m_strReferRelation = m_strReferRelation + "_" + data.m_strReferRelation;
				return true;
			}
			else
				return false;
		}
		string m_strFile;
		string m_strName;
		string m_strReferRelation;
		string m_strQuestName;
	};

protected:
	string	GetCurQuestFile() { return m_curQuestFile;}
	void		SetCurQuestFile(string str) { m_curQuestFile = str;}
	void ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef);
	string GetReferDataDisplayName(QuestReferData& data);
	void InitTwoReferVecTab(string& strFileName,bool bQuest = false);
	string GetReferFileName(string& strType,string& strContent,int itemType = 0);
	int	GetItemTypeFromString(string str);
	string GetItemNameFromStr(string str);
	int	QueryQuestItemType(string& strItem);
	COriFileOperate& GetDifData(string& strFile);
	string GetFilePath(string& strName);
	int GetColPosByName(string strCol,vector<string>& vecRef);
	bool AdjustBeQuestTbl(string& strFileName);
	void RecordDifPath(string& str);
	void WriteReferTxt();
	string GetFileSavePath(string& strFileName);
	bool BeInDescQuestLst(string strQuest,int num);
	void AddQuestDescNum(string strQuest,int num);

	void CheckItemType(int itemType);

private:
	string	m_curQuestFile;
	map<string,vector<QuestReferData> > m_mapQuestRefer;
	map<int,string> m_ItemTable;
	map<string,vector<vector<string> > > m_MapTwoRefer;
	map<string,COriFileOperate*> m_mapOri;
	CSplitInfo* m_pSplitInfo;
	map<string,string> m_mapFileList;
	string m_strReferPath;
	string m_strQuestDesc;
	map<int,vector<string> > m_mapDesc;
};
