#pragma once
#include "TSingleton.h"
#include <set>
#include <vector>
#include <iterator>
#include "tinyxml/tinyxml.h"
#include <stack>
class CSplitInfo;
class CDif2Txt : public TSingleton<CDif2Txt>
{
public:
	CDif2Txt();
	~CDif2Txt();

	void StartProcess();
	static FTW_RESULT Dif2Txt( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void Change2Txt(string& strFileName);
	bool BeRequireToTxt(string& strFileName);
	//检测多行表多重索引重复情况
	bool CheckMultiTable(string& strFileName);
	//解析多行表xml文件，最终结果保存在map中
	void MultiTableXmlTree(TiXmlNode * node);

private:
	CSplitInfo* m_pSplitInfo;
	set<string> FIRSTROW;
	string m_strSrc;
	string m_strTgt;
	int m_iRecord;
	//用于解析多行表xml文件时临时存储节点名
	vector<string> m_vString;
	//用于解析多行表xml文件时临时存储节点名，并将值传递给m_vString
	stack<string> m_sString;
	//解析多行表xml文件时临时将m_sString的内容借过来，是树某一叶子的最大长度，是m_sString的最长状态
	stack<string> m_sMax;
	//存储多行表xml文件的解析结果
	map<vector<string>,int> m_mVectorInt;
	//在判断是否重复时用于记录某一行的内容和行号
	map<vector<string>,int> m_mMapVecBuf;
	//记录多行表形成唯一索引的最小列数
	int m_iMax;


};
