#pragma once
#include "TSingleton.h"
#include "COriFileOperate.h"

class CSplitInfo;

class CGermanLang :public TSingleton<CGermanLang>
{
public:
	CGermanLang();
	~CGermanLang();
	static FTW_RESULT FormatUtf2Utf16( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	static FTW_RESULT MakeDifList( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void FormatUtf8Vec(string& strFileName);
	bool ReadMappedXML();
	void ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef);
	void AddNoteLevel(string& strFileName,vector<vector<string> >& vecRef);
	void RecordDifPath(string& str);
	
	COriFileOperate& GetOriFile(string& strFile);

	string GetFilePath(string& strName);
	void Write2Utf16Txt(string& strFileName,vector<vector<string> >& vecRef);

	//生成物品任务NPC等级信息=========================
	bool AdjustBeQuestTbl(string& strFileName);
	void BeginCreateQuestGrade();

	void CreateQuestTableGradeInfor(string& strFile);
	void BulitSprintTypeGrade(string& strFile,int row);
	void BulitItemRequireGrade(string& strFile,int row);
	void BulitMonsterRequireGrade(string& strFile,int row);
	void BulitIntObjRequireGrade(string& strFile,int row);
	void BulitDropGrade(string& strFile,int row);
	void BulitItemAwardGrade(string& strFile,int row);
	void BulitReceiveItemGrade(string& strFile,int row);
	void BuiltIntObjUseRequireGrade(string& strFile,int row);

	int	GetQuestLevel(string& strQuest);
	void RecordQuestLevel(string& strQuest,int level);
	string GetReferFileName(string& strType,string& strContent,int itemType = 0);
	void RecordGrade(string strFileName,string strIndex,int grade);
	void RecordGrade(string strFileName,int index,int grade);
	int	GetItemTypeFromString(string str);
	void ReadItemTable();
	string GetItemNameFromStr(string str);
	bool BeInNpc_Common(string strName);
	int	QueryQuestItemType(string& strItem);
	void CreateNpcGrade();
	int GetColPosByName(string strCol,vector<string>& vecRef);

	void PostProcessWithLangTbl();
	//
	void CreateNpcShopGrade();
	void CreateNpcShowContent_CommonGrade(vector<vector<string> >& vecTemp);
	string GetFirstIndexFromBase64(string& str);
	void CreateNpcDropItemObj();
	void CreateNpcDropItem();
	void CreateNpcTheaterGrade();
	void CreateArmorPieceEnactment();
	void RelationObjAndItem();
	void CreateAlcazarNpcGrade();
	void CreateAlcazarObjGrade();
	static FTW_RESULT StartEvaluateObj( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void EvaluateObjGrade(string& strFileName);
	void GetLuaStartEndRow(string& strFileName,string start,string end,vector<int>& vecResult);
	int	GetNearestNpcGrade(string& strFileName,int x,int y);
	void GetXYCoord(string str,vector<int>& vecResult);
	void AddGradeColToLangData(string& strName,vector<vector<string> >& vecRef);
	void SortLangDataByGrade(vector<vector<string> >& vecRef);
	void GetSearchColPos(vector<string>& vecRef,string strName,vector<int>& vecResult);
	int	GetComGrade(string strFileName,string str);
	void GetSearchContent(string& strFileName,string& strFind,string &strGroup,vector<string>& vecResult);
	void GetDropItemObjName(string& strDropGroup,vector<string>& vecResult);
	void GetDropItemNameType(string& strDropGroup,vector<string>& vecName,vector<int>& vecType);
	void ObjRandomDropItem_Server(string& strDropGroup,int grade);
	void GetObjDropNameType(string& strDropGroup,vector<string>& vecName,vector<int>& vecType);


	//临时功能，拆出baseitem描述里的物品类型列
	void SplitBaseItemDescripType(string strFileName);
	string PickItemDisType(string& str);
	void BatchItemType();

	struct GradeData 
	{
	public:
		enum IndexType
		{
			eIT_Number,
			eIT_String,
		};
		bool operator == (const GradeData& data)
		{
			if (m_etype == data.m_etype && m_grade == data.m_grade)
			{
				if (m_etype == eIT_Number && m_index == data.m_index)
				{
					return true;
				}
				else if (m_etype == eIT_String && m_strIndex == data.m_strIndex)
				{
					return true;
				}
				else
					return false;
			}
			else
				return false;
		}
		IndexType m_etype;
		int m_index;
		string m_strIndex;
		int m_grade;
	};
	//===========================================
	//处理任务翻译表索引列加内容描述
	void AddDescriptInfoToQuestTxt(vector<vector<string> >& vecRef);

	//===========================================
	//针对老外新需求读取引用表及前缀名功能名,实现双向索引功能
	void ReadReferDescriptTxt();
	string GetQuestNameFromMulitKey(string str);
	static FTW_RESULT SearchNeedFile( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );

	//任务翻译表添加level,sceneName,coordinate
	void AddLeveToQuest(string& strFileName,vector<vector<string> >& vecRef);
	void AddSceneNameToQuest(string& strFileName,vector<vector<string> >& vecRef);
	void AddCoordinateToQuest(string& strFileName,vector<vector<string> >& vecRef);
	string GetQuestSecen(string& strFileName,string& strQuest);
	string GetQuestCoordinate(string& strFileName,string& strQuest);
	string GetPatchName(string strName);
private:
	struct QuestReferData;
	string GetSourceDifFile(string strLangName);
	//===========================================
private:
	CSplitInfo* m_pSplitInfo;
	map<string,string> m_mapName;
	map<string,string> m_mapFileList;

	map<string,COriFileOperate*> m_mapOriFile;

	map<string,int>	m_mapQuestLevel;
	map<int,string> m_ItemTable;
	map<string,vector<GradeData> > m_mapGrade;
	bool	m_bEnablePatch;
};
