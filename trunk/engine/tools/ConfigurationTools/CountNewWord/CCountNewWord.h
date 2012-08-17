#pragma once
#include "stdafx.h"
#include "TSingleton.h"
#include "common.h"
using namespace std;
class CCount:public TSingleton<CCount>
{
public:
	CCount();
	~CCount(){};
	static FTW_RESULT CountWord_MakeOldFileList( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	static FTW_RESULT CountWord_MakeNewFileList( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	//生成老txt文件列表
	void MakeOldFileList(string strFileName);
	void MakeNewFileList(string strFileName);
	//统计新增文件和减少文件
	void FileNumber();
	//载入文件开始计算
	void FileLoad();
	void start();
	//处理新增的文件
	void WordCountNew(vector<vector<string>> vTemp);
	//处理一般的文件
	void WordCount(vector<string> vNewTemp,vector<string> vOldTemp);
	//获得txt文件的第一列
	vector<string> GetIndex(vector<string> vec);
	//处理某一非新增的行
	void CountWord(vector<string> vecNewIndex,vector<string> vecNew,vector<string> vecOldIndex,vector<string> vecOld);
	//读取UTF16txt文件
	void ReadUtf16Txt(string& strFileName,vector<vector<wstring> >& vecRead);
	void ReadUtf16Txt(string& strFileName,vector<vector<string> >& vecRef);
	//统计新增标点
	uint32 SignNumNew(string str);
	//统计变化的标点
	uint32 SignNumCha(string str);
	//判断是否是非文字的中文符号，根据区位码进行判断
	bool isChineseSign(const string& str);
private:
	//老语言包文件列表
	set<string> m_OldFileList;
	//新语言包文件列表
	set<string> m_NewFileList;
	//新语言包相当老语言包增加文件列表
	set<string> m_NewFileListInc;
	//新语言包相对老语言包减少的文件数
	uint32 m_FileNumberDes;
	//新语言包相对老语言包增加的文件数
	uint32 m_FileNumberInc;
	//新增字节数
	uint32 m_WordCountNew;
	//变化字节数
	uint32 m_WordCountCha;
	//变化的符号数
	uint32 m_SignNumCha;
	//变化的中文符号数
	uint32 m_SignChNumCha;
	//变化的Ansi符号数
	uint32 m_SignAnNumCha;
	//新增符号数
	uint32 m_SignNumNew;
	//新增的中文符号数
	uint32 m_SignChNumNew;
	//新增的Ansi符号数
	uint32 m_SignAnNumNew;
	//新增英语和数字数
	uint32 m_EnglishNewNum;
	//变化的英语和数字数
	uint32 m_EnglishChNum;
	//原语言包文件夹
	string m_OldLanFolder;
	//新语言包文件夹
	string m_NewLanFolder;
	//中文符号集合
	set<string> m_sChineseSign;
	//Ansi符号集合
	set<string> m_sAnsiSign;
	//英文字母和数字集合
	set<string> m_sEnglish;
};