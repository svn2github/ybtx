#include "stdafx.h"
#include "CLackFileCopy.h"
#include "CSplitInfo.h"
#include "common.h"
#include <set>
#include "ErrLogHelper.h"
#include "conio.h"
#include "CExpLog.h"

void CLackFileCopy::StartProcess()
{
	DeleteFile("outLogFileName.txt");
	string strPath = CSplitInfo::GetInst()->GetLangTablePath();
	wstring wstrPath = utf8_to_utf16(strPath) + L"/..";
	//wstring wstrPath = L"E:\\ybtx\\designer\\cfg\\lang\\german\\table";

	string strSave = strPath;
	const string& strChinese = CSplitInfo::GetInst()->GetOriLang();
	const string& strForeign = CSplitInfo::GetInst()->GetForeLang();
	replace_all_distinct(strSave, strChinese, strForeign, 0);
	const string& strGui = CSplitInfo::GetInst()->GetGuiFolder();
	const string& strGuiStaticText = CSplitInfo::GetInst()->GetGuiStaticText();
	string strFrom = strSave + "/" + strGui + "/" + strGuiStaticText;
	string strTo = strSave + "/../" + strGui + "/" + strGuiStaticText;
	CreateFileDirectory(strTo);
	CopyFile(strFrom.c_str(), strTo.c_str(), false);

	FileTreeWalk(wstrPath.c_str(),WalkCopyFile,NULL);

	wstring wstrPath2 = utf8_to_utf16(strSave);
	FileTreeWalk(wstrPath2.c_str(),WalkCheckFileIndependently,NULL);

	FileTreeWalk(wstrPath.c_str(),WalkCheckFileContrastively,NULL);

	//cout << "检测结束！\n";
	//_getch();
}

FTW_RESULT CLackFileCopy::WalkCopyFile( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	//if( _wcsicmp( szFileName + nLen - 4, L".txt" ) && _wcsicmp( szFileName + nLen - 4, L".lua" ) && _wcsicmp( szFileName + nLen - 4, L".xml" ))
	//	return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	if (strFileName.find(CSplitInfo::GetInst()->GetOriLang()) == -1)
	{
		return _FTW_CONTINUNE;
	}
	CLackFileCopy* pManager = CLackFileCopy::GetInst();
	bool bSuccess = pManager->StartCopy(strFileName);
	if( bSuccess && _wcsicmp( szFileName + nLen - 4, L".txt" ) == 0)
	{
		stringstream strMsg;
		strMsg << "国际化语言包缺少文件：" << strFileName;
		if(CSplitInfo::GetInst()->GetLogToFile())
		{
			PrintLog(strMsg.str());
		}
		cout << strMsg.str() << endl;
	}
	//pManager->CheckGUIStaticText();
	return _FTW_CONTINUNE;
}

FTW_RESULT CLackFileCopy::WalkCheckFileIndependently( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".txt" ))
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CLackFileCopy* pManager = CLackFileCopy::GetInst();
	pManager->CheckFileIndependently(strFileName);
	return _FTW_CONTINUNE;
}

FTW_RESULT CLackFileCopy::WalkCheckFileContrastively( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".txt" ))
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CLackFileCopy* pManager = CLackFileCopy::GetInst();
	pManager->CheckFileContrastively(strFileName);
	return _FTW_CONTINUNE;
}

bool CLackFileCopy::StartCopy(string& strFileName)
{
	string strSave = strFileName;
	const string& strChinese = CSplitInfo::GetInst()->GetOriLang();
	const string& strForeign = CSplitInfo::GetInst()->GetForeLang();
	replace_all_distinct(strSave, strChinese, strForeign, 0);
	ifstream inFile(strSave.c_str());
	if (inFile)
	{	inFile.close();
		return false;
	}
	CreateFileDirectory(strSave);
	CopyFile(strFileName.c_str(), strSave.c_str(), false);
	return true;
}

void CLackFileCopy::CheckFileIndependently(string& strFileName)
{
	vector<vector<string> > vecDdata;
	ReadUtf8Txt(strFileName,vecDdata);
	for (int i=1;i<(int)vecDdata.size();++i)
	{
		for (int j=0;j<(int)vecDdata[i].size();++j)
		{
			string& strRef = vecDdata[i][j];
			size_t pos;
			//size_t pos = strRef.find("\"");
			//while(pos != string::npos)
			//{
			//	if(!(pos > 0 && strRef[pos - 1] == '{' || pos < strRef.size() - 1 && strRef[pos + 1] == '}'))
			//	{
			//		stringstream strMsg;
			//		strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos;
			//		GenErr("返回语言表内容有单独的双引号",strMsg.str().c_str());
			//	}
			//	pos = strRef.find("\"", pos + 1);
			//}
			size_t pos1 = strRef.find("{\"");
			size_t pos2 = strRef.find("\"}");
			if(pos2 < pos1)
			{
				stringstream strMsg;
				strMsg << "返回语言表内容缺少第一个左括号\t配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos2;
				//GenErr("返回语言表内容缺少第一个左括号",strMsg.str().c_str());
				printf("%s\n",strMsg.str().c_str());
				if(GetLogToFile())
				{
					PrintLog(strMsg.str());
				}


			}
			pos = pos1;
			bool bIsLeftBra = true;
			while(pos != string::npos)
			{
				size_t pos1 = strRef.find("{\"", pos + 1);
				size_t pos2 = strRef.find("\"}", pos + 1);
				if(bIsLeftBra)
				{
					if(pos2 == string::npos)
					{
						stringstream strMsg;
						strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos;
						if(GetLogToFile())
						{
							stringstream strExpLog;
							strExpLog << "返回语言表内容缺少最后一个右括号：" << strMsg.str();
							PrintLog(strExpLog.str());
						}
						else
						{
							GenErr("返回语言表内容缺少最后一个右括号",strMsg.str().c_str());
						}
					}
					if(pos1 != string::npos && pos2 > pos1)
					{
						stringstream strMsg;
						strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos1;
						if(GetLogToFile())
						{
							stringstream strExpLog;
							strExpLog << "返回语言表内容多了一个左括号：" << strMsg.str();
							PrintLog(strExpLog.str());
						}
						else
						{
							GenErr("返回语言表内容多了一个左括号",strMsg.str().c_str());
						}
					}
					pos = pos2;
				}
				else if(pos1 != string::npos || pos2 != string::npos)
				{
					if(pos2 == string::npos)
					{
						stringstream strMsg;
						strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos;
						if(GetLogToFile())
						{
							stringstream strExpLog;
							strExpLog << "返回语言表内容缺少最后一个右括号：" << strMsg.str();
							PrintLog(strExpLog.str());
						}
						else
						{
							GenErr("返回语言表内容缺少最后一个右括号",strMsg.str().c_str());
						}
					}
					else if(pos1 == string::npos)
					{
						stringstream strMsg;
						strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos;
						if(GetLogToFile())
						{
							stringstream strExpLog;
							strExpLog << "返回语言表内容多了最后一个右括号：" << strMsg.str();
							PrintLog(strExpLog.str());
						}
						else
						{
							GenErr("返回语言表内容多了最后一个右括号",strMsg.str().c_str());
						}
					}
					else if(pos1 > pos2)
					{
						stringstream strMsg;
						strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos1;
						if(GetLogToFile())
						{
							stringstream strExpLog;
							strExpLog << "返回语言表内容左右括号不匹配：" << strMsg.str();
							PrintLog(strExpLog.str());
						}
						else
						{
							GenErr("返回语言表内容左右括号不匹配",strMsg.str().c_str());
						}
					}
					pos = pos1;
				}
				else
				{
					pos = string::npos;
				}
				bIsLeftBra = !bIsLeftBra;
			}
		}
	}
}

void CLackFileCopy::CheckFileContrastively(string& strFileName)
{
	//if(strFileName.find("GUIStaticText.txt") != string::npos)
	//{
	//	int i = 0;
	//}
	string strFileToCheck = strFileName;
	const string& strChinese = CSplitInfo::GetInst()->GetOriLang();
	const string& strForeign = CSplitInfo::GetInst()->GetForeLang();
	replace_all_distinct(strFileToCheck, strChinese, strForeign, 0);

	vector<vector<string> > vecDataFrom;
	ReadUtf8Txt(strFileName,vecDataFrom);
	vector<vector<string> > vecDataTo;
	ReadUtf8Txt(strFileToCheck,vecDataTo);


	//标题检测
	typedef set<string> MapDataFromForTitle;
	MapDataFromForTitle setDataFromForTitle;
	for(uint32 i = 0; i < vecDataFrom[0].size(); ++i)
	{
		setDataFromForTitle.insert(vecDataFrom[0][i]);
	}

	for(uint32 i = 0; i < vecDataTo[0].size(); ++i)
	{
		setDataFromForTitle.erase(vecDataTo[0][i]);
	}

	if(!setDataFromForTitle.empty())
	{
		//报错
		ofstream strOutLogFile("outLogFileName.txt", ios_base::app);
		for(MapDataFromForTitle::iterator itr = setDataFromForTitle.begin(); itr != setDataFromForTitle.end(); ++itr)
		{
			strOutLogFile << (*itr) << "\n";
		}

		strOutLogFile << "标题缺失" << setDataFromForTitle.size() << "列，在文件" << strFileToCheck << "\n\n";
		strOutLogFile.close();

		if(GetLogToFile())
		{
			stringstream strExpLog;
			for (MapDataFromForTitle::iterator itr = setDataFromForTitle.begin(); itr != setDataFromForTitle.end(); ++itr)
			{
				strExpLog << (*itr) << "\n";
			}

			strExpLog << "标题缺失" << setDataFromForTitle.size() << "列，在文件" << strFileToCheck << "\n\n";

			PrintLog(strExpLog.str());
		}
	}

	//索引检测
	typedef map<string, string*> MapDataFrom;
	MapDataFrom setDataFrom;
	for (int i=1;i<(int)vecDataFrom.size();++i)
	{
		if(vecDataFrom[i].size() == 1 && vecDataFrom[i][0].empty())
			continue;
		setDataFrom.insert(make_pair(vecDataFrom[i][0], &vecDataFrom[i][1]));
	}

	for (int i=1;i<(int)vecDataTo.size();++i)
	{
		setDataFrom.erase(vecDataTo[i][0]);
	}

	if(setDataFrom.empty())
		return;

	ofstream strOutLogFile("outLogFileName.txt", ios_base::app);
	for (MapDataFrom::iterator itr = setDataFrom.begin(); itr != setDataFrom.end(); ++itr)
	{
		strOutLogFile << itr->first << "\t" << utf8_to_gbk(*(itr->second)) << "\n";
	}
		
	strOutLogFile << "索引缺失" << setDataFrom.size() << "行，在文件" << strFileToCheck << "\n\n";
	
	strOutLogFile.close();

	if(GetLogToFile())
	{
		stringstream strExpLog;
		for (MapDataFrom::iterator itr = setDataFrom.begin(); itr != setDataFrom.end(); ++itr)
		{
			strExpLog << itr->first << "\t" << utf8_to_gbk(*(itr->second)) << "\n";
		}

		strExpLog << "索引缺失" << setDataFrom.size() << "行，在文件" << strFileToCheck << "\n\n";

		PrintLog(strExpLog.str());
	}

	//	for (int j=0;j<(int)vecDdata[i].size();++j)
	//	{
	//		string& strRef = vecDdata[i][j];
	//		size_t pos = strRef.find("\"");
	//		while(pos != string::npos)
	//		{
	//			if(!(pos > 0 && strRef[pos - 1] == '{' || pos < strRef.size() - 1 && strRef[pos + 1] == '}'))
	//			{
	//				stringstream strMsg;
	//				strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos;
	//				GenErr("返回语言表内容有单独的双引号",strMsg.str().c_str());
	//			}
	//			pos = strRef.find("\"", pos + 1);
	//		}
	//		size_t pos1 = strRef.find("{\"");
	//		size_t pos2 = strRef.find("\"}");
	//		if(pos2 < pos1)
	//		{
	//			stringstream strMsg;
	//			strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos2;
	//			GenErr("返回语言表内容缺少第一个左括号",strMsg.str().c_str());
	//		}
	//		pos = pos1;
	//		bool bIsLeftBra = true;
	//		while(pos != string::npos)
	//		{
	//			size_t pos1 = strRef.find("{\"", pos + 1);
	//			size_t pos2 = strRef.find("\"}", pos + 1);
	//			if(bIsLeftBra)
	//			{
	//				if(pos2 == string::npos)
	//				{
	//					stringstream strMsg;
	//					strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos;
	//					GenErr("返回语言表内容缺少最后一个右括号",strMsg.str().c_str());
	//				}
	//				if(pos1 != string::npos && pos2 > pos1)
	//				{
	//					stringstream strMsg;
	//					strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos1;
	//					GenErr("返回语言表内容多了一个左括号",strMsg.str().c_str());
	//				}
	//				pos = pos2;
	//			}
	//			else if(pos1 != string::npos || pos2 != string::npos)
	//			{
	//				if(pos2 == string::npos)
	//				{
	//					stringstream strMsg;
	//					strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos;
	//					GenErr("返回语言表内容缺少最后一个右括号",strMsg.str().c_str());
	//				}
	//				else if(pos1 == string::npos)
	//				{
	//					stringstream strMsg;
	//					strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos;
	//					GenErr("返回语言表内容多了最后一个右括号",strMsg.str().c_str());
	//				}
	//				else if(pos1 > pos2)
	//				{
	//					stringstream strMsg;
	//					strMsg << "配置表名为:" << strFileName << " 索引=" << vecDdata[i][0] << " 列=" << j << " 位置=" << pos1;
	//					GenErr("返回语言表内容左右括号不匹配",strMsg.str().c_str());
	//				}
	//				pos = pos1;
	//			}
	//			else
	//			{
	//				pos = string::npos;
	//			}
	//			bIsLeftBra = !bIsLeftBra;
	//		}
	//	}
	//}
}

void CLackFileCopy::ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef)
{
	ifstream inFile(strFileName.c_str());
	if (!inFile)
		return;
	//inFile.seekg(3);
	string str;
	while(getline(inFile,str))
	{
		vecRef.push_back(vector<string>());
		int pos = -1;
		while ((pos = str.find('\t')) != -1)
		{
			string strTemp = str.substr(0,pos);
			vecRef[vecRef.size()-1].push_back(strTemp);
			str = str.substr(pos+1,str.length()-pos-1);
		}
		vecRef[vecRef.size()-1].push_back(str);
	}
	inFile.close();
}
