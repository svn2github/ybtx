#include "stdafx.h"
#include "CDifFile.h"
#include "ExpHelper.h"
#include "StringHelper.h"

void CDifFile::ReadFileToMap(string strFileName,map<EFieldType,VecElementCol>& mapRef)
{
	mapRef.clear();
	ifstream Infile(strFileName.c_str());
	if (!Infile)
	{
		string strMsg = strFileName + "打开失败！";
		GenErr(strMsg);
	}
	stringstream backupStream;
	backupStream<<Infile.rdbuf();
	Infile.close();
	string strTemp;
	bool bReturn = false;
	string strReturn;
	EFieldType eCurType = eFT_None;
	MetaElement element;
	while (!backupStream.eof())
	{
		if(bReturn)
			strTemp = strReturn;
		else
			getline(backupStream,strTemp);
		switch(GetFileType(strTemp,strReturn,backupStream,bReturn))
		{
		case eFT_Table:
			{
				mapRef[eFT_Table] = VecElementCol();
				mapRef[eFT_Table].push_back(vector<MetaElement>());
				eCurType = eFT_Table;
				element.eFileType = eFT_Table;
			}
			break;
		case eFT_Vectors:
			{
				mapRef[eFT_Vectors] = VecElementCol();
				mapRef[eFT_Vectors].push_back(vector<MetaElement>());
				mapRef[eCurType][0].push_back(element);
				element.vecElement.clear();
				eCurType = eFT_Vectors;
				element.eFileType = eFT_Vectors;
			}
			break;
		case eFT_Tuples:
			{
				mapRef[eFT_Tuples] = VecElementCol();
				mapRef[eFT_Tuples].push_back(vector<MetaElement>());
				mapRef[eCurType][0].push_back(element);
				element.vecElement.clear();
				eCurType = eFT_Tuples;
				element.eFileType = eFT_Tuples;
				break;
			}
		case eFT_Data:
			{
				mapRef[eFT_Data] = VecElementCol();
				mapRef[eFT_Data].push_back(vector<MetaElement>());
				mapRef[eCurType][0].push_back(element);
				element.vecElement.clear();
				eCurType = eFT_Data;
				element.eFileType = eFT_Data;
				break;
			}
		case eFT_BotBegin:
			{
				if (mapRef.find(eFT_BotBegin) == mapRef.end())
				{
					mapRef[eCurType][0].push_back(element);
					mapRef[eFT_BotBegin] = VecElementCol();
					mapRef[eFT_BotBegin].push_back(vector<MetaElement>());
					eCurType = eFT_BotBegin;
					element.eFileType = eFT_BotBegin;
				}
				else
				{
					int size = (int)mapRef[eCurType].size();
					VecElement& vecElementTemp = mapRef[eCurType][size-1];
					vecElementTemp.push_back(element);
					mapRef[eFT_BotBegin].push_back(vector<MetaElement>());
				}
				element.vecElement.clear();
				element.eFileType = eFT_BotBegin;
			}
			break;
		case eFT_String:
			{
				int size = (int)mapRef[eCurType].size();
				VecElement& vecElementTemp = mapRef[eCurType][size-1];
				vecElementTemp.push_back(element);
				element.vecElement.clear();
				element.eFileType = eFT_String;
			}
			break;
		case eFT_Number:
			{
				int size = (int)mapRef[eCurType].size();
				VecElement& vecElementTemp = mapRef[eCurType][size-1];
				vecElementTemp.push_back(element);
				element.vecElement.clear();
				element.eFileType = eFT_Number;
			}
			break;
		case eFT_BotEnd:
			{
				int size = (int)mapRef[eCurType].size();
				VecElement& vecElementTemp = mapRef[eCurType][size-1];
				vecElementTemp.push_back(element);
				element.vecElement.clear();
				element.eFileType = eFT_BotEnd;
			}
			break;
		case eFT_Eod:
			{
				mapRef[eFT_Eod] = VecElementCol();
				mapRef[eFT_Eod].push_back(vector<MetaElement>());
				mapRef[eFT_Eod][0] = vector<MetaElement>();
				int size = (int)mapRef[eCurType].size();
				VecElement& vecElementTemp = mapRef[eFT_BotBegin][size-1];
				vecElementTemp.push_back(element);
				element.vecElement.clear();
				element.eFileType = eFT_Eod;
			}
			break;
		default:
			break;
		}
		element.vecElement.push_back(strTemp);
	}
	int size = (int)mapRef[eFT_Eod].size();
	VecElement& vecElementTemp = mapRef[eFT_Eod][size-1];
	vecElementTemp.push_back(element);
}

void CDifFile::WriteSectionToDifFile(ofstream& outStream,EFieldType eFileHeadType,map<EFieldType,VecElementCol>& mapRef)
{
	VecElementCol& vecTableCol = mapRef[eFileHeadType];
	for (VecElementCol::iterator iter = vecTableCol.begin();iter != vecTableCol.end();++iter)
	{
		for (VecElement::iterator iterElement = iter->begin();iterElement!=iter->end();++iterElement)
		{
			for (vector<string>::iterator iterStr = iterElement->vecElement.begin();iterStr!= iterElement->vecElement.end();++iterStr)
			{
				if (eFileHeadType == eFT_Eod && iterStr+1 == iterElement->vecElement.end())
				{
					outStream<<(*iterStr);
				} 
				else
				{
					outStream<<(*iterStr)<<endl;
				}
			}
		}
	}
}

void CDifFile::WriteDataToDifFile(string strFileName,map<EFieldType,VecElementCol>& mapRef)
{
	ofstream outputFile(strFileName.c_str());
	if (!outputFile)
	{
		string errorStr = strFileName;
		errorStr = errorStr + "打开时候发生错误，请检查是否有其他程序正在使用中！";
		GenErr(errorStr);
	}
	WriteSectionToDifFile(outputFile,eFT_Table,mapRef);
	WriteSectionToDifFile(outputFile,eFT_Vectors,mapRef);
	WriteSectionToDifFile(outputFile,eFT_Tuples,mapRef);
	WriteSectionToDifFile(outputFile,eFT_Data,mapRef);
	WriteSectionToDifFile(outputFile,eFT_BotBegin,mapRef);
	WriteSectionToDifFile(outputFile,eFT_Eod,mapRef);
	outputFile.close();
}

void CDifFile::AdjustVECTORSAndTUPLES(map<EFieldType,VecElementCol>& mapRef)
{
	string& strVECTORS = mapRef[eFT_Vectors][0][0].vecElement[1];
	int posVECTORS = (int)strVECTORS.find(',');
	strVECTORS = strVECTORS.substr(0,posVECTORS);
	int nowNum = (int)mapRef[eFT_BotBegin].size();
	char numChar[10];
	numChar[9] = '\0';
	sprintf(numChar,"%d",nowNum);
	strVECTORS = strVECTORS + "," + numChar;
	string& strTUPLES = mapRef[eFT_Tuples][0][0].vecElement[1];
	int posTUPLES = (int)strTUPLES.find(',');
	strTUPLES = strTUPLES.substr(0,posTUPLES);
	int colNum = (int)mapRef[eFT_BotBegin][0].size() - 2;
	sprintf(numChar,"%d",colNum);
	strTUPLES = strTUPLES + "," + numChar;
}

string CDifFile::ProduceNumStr(string& str)
{
	string strTemp = str;
	int pos = (int)strTemp.find(',',0);
	strTemp = strTemp.substr(pos+1,strTemp.length()-pos-1);
	return strTemp;
}

string CDifFile::ProduceCommonStr(vector<string> &vecStr)
{
	string strTemp;
	for (vector<string>::iterator iter = vecStr.begin()+1;iter != vecStr.end();++iter)
	{
		string strBlank = (*iter);
		if (iter>vecStr.begin()+1)
			strBlank = " " + strBlank;
		strTemp = strTemp + strBlank;
	}
	return strTemp;
}

CDifFile::EFieldType CDifFile::GetFileType(string str,string& strReturn,stringstream& strStream,bool &bReturn)
{
	bReturn = false;
	strReturn = "";
	if (str == "TABLE")
	{
		return eFT_Table;
	} 
	else if(str == "VECTORS")
	{
		return eFT_Vectors;
	}
	else if (str == "TUPLES")
	{
		return eFT_Tuples;
	} 
	else if(str == "DATA")
	{
		return eFT_Data;
	}
	else if (str == "BOT")
	{
		return eFT_BotBegin;
	} 
	else if(str == "1,0")
	{
		return eFT_String;
	}
	else if (str == "-1,0")
	{
		return eFT_BotEnd;
	}
	else if (str == "EOD")
	{
		return eFT_Eod;
	}
	else
	{
		if(str[0]=='0' && str[1]==',')
		{
			getline(strStream,strReturn);
			bReturn = true;
			if (strReturn == "V")
			{
				return eFT_Number;
			}
			else
			{
				return eFT_Context;
			}
		}
		else
		{
			return eFT_Context;
		}

	}

}

void CDifFile::CleanBlankRow(VecElementCol& vecSource)
{
	for(int i=(int)vecSource.size()-1;i>=0;--i)
	{
		if (vecSource[i][1].vecElement.size() ==2 && vecSource[i][1].vecElement[1] == "\"\"")
		{
			vecSource.erase(vecSource.begin()+i);
		}
	}
}

bool CDifFile::VerdictFirstRowNote(VecElementCol& vecEleCol)
{
	if (vecEleCol[0][1].eFileType == eFT_String && vecEleCol[0][1].vecElement[1] == "\"NOTE:\"")
	{
		return true;
	}
	return false;
}

void CDifFile::ReadFileToVec(string strFileName,vector<vector<string> >& vecRef)
{
	map<EFieldType,VecElementCol> mapSourceFileData;
	ReadFileToMap(strFileName,mapSourceFileData);
	CleanBlankRow(mapSourceFileData[eFT_BotBegin]);
	VecElementCol& vecSourceCol = mapSourceFileData[eFT_BotBegin];
	vecRef.clear();
	if (VerdictFirstRowNote(vecSourceCol))
	{
		vecSourceCol.erase(vecSourceCol.begin());
	}
	for (int i=0;i<(int)vecSourceCol.size();++i)
	{
		vector<string> vecTempStr;
		for (int j=1;j<(int)vecSourceCol[0].size()-1;++j)
		{
			string strContent;
			if (vecSourceCol[i][j].eFileType == eFT_String)
			{
				strContent = ProduceCommonStr(vecSourceCol[i][j].vecElement);
				if (0==i)
				{
					CleanOnePairQuotation(strContent);
				}
			}
			else if (vecSourceCol[i][j].eFileType == eFT_Number)
			{
				strContent = ProduceNumStr(vecSourceCol[i][j].vecElement[0]);
			}
			if (0 == i && strContent == "")
				continue;
			vecTempStr.push_back(strContent);
		}
		vecRef.push_back(vecTempStr);
	}
}

void CDifFile::ReadFileToVec(string strFileName,vector<vector<MetaData> >& vecRef)
{
	map<EFieldType, VecElementCol> mapSourceFileData;
	ReadFileToMap(strFileName,mapSourceFileData);
	CleanBlankRow(mapSourceFileData[eFT_BotBegin]);
	VecElementCol& vecSourceCol = mapSourceFileData[eFT_BotBegin];
	vecRef.clear();
	if (VerdictFirstRowNote(vecSourceCol))
	{
		vecSourceCol.erase(vecSourceCol.begin());
	}
	MetaData dataTemp;
	for (int i=0;i<(int)vecSourceCol.size();++i)
	{
		vector<MetaData> vecTempStr;
		for (int j=1;j<(int)vecSourceCol[0].size()-1;++j)
		{
			string strContent;
			if (vecSourceCol[i][j].eFileType == eFT_String)
			{
				dataTemp.m_eDataType = eFT_String;
				strContent = ProduceCommonStr(vecSourceCol[i][j].vecElement);
				if (0==i)
				{
					CleanOnePairQuotation(strContent);
				}
			}
			else if (vecSourceCol[i][j].eFileType == eFT_Number)
			{
				dataTemp.m_eDataType = eFT_Number;
				strContent = ProduceNumStr(vecSourceCol[i][j].vecElement[0]);
			}
			if (0 == i && strContent == "")
				continue;
			dataTemp.m_strData = strContent;
			vecTempStr.push_back(dataTemp);
		}
		vecRef.push_back(vecTempStr);
	}
}

void CDifFile::CleanOnePairQuotation(string& str)
{
	if(str != "" && str[0] == '\"' && str[str.length()-1] == '\"')
	{
		str.erase(str.length()-1,1);
		str.erase(0,1);
	}
}

void CDifFile::CleanEndSpace(string& str)
{
	while (str != "")
	{
		if (str[str.length()-1] == '\"')
		{
			if (str.length()>1 && str[str.length()-2] == ' ')
			{
				str.erase(str.length()-2,1);
			}
			else
				break;
		}
		else
		{
			if (str[str.length()-1] == ' ')
				str.erase(str.length()-1);
			else
				break;
		}
	}
}

CDifFile::MetaElement CDifFile::GetBlankElement()
{
	MetaElement eleBlank;
	eleBlank.eFileType = eFT_String;
	eleBlank.vecElement.push_back("1,0");
	eleBlank.vecElement.push_back("\"\"");
	return eleBlank;
}

CDifFile::MetaElement CDifFile::GetBotBeginElement()
{
	MetaElement ele;
	ele.eFileType = eFT_BotBegin;
	ele.vecElement.push_back("BOT");
	return ele;
}

CDifFile::MetaElement	CDifFile::GetBotEndElement()
{
	MetaElement ele;
	ele.eFileType = eFT_BotEnd;
	ele.vecElement.push_back("-1,0");
	return ele;
}

CDifFile::CDifFile(string& strFileName)
:m_strFile(strFileName)
{
	LoadFile();
}

void CDifFile::LoadFile()
{
	ReadFileToMap(m_strFile,m_mDif);
}

CDifFile::~CDifFile()
{
	m_mDif.clear();
}

CDifFile::CDifFile()
{
	MetaElement element;
	m_mDif[eFT_Table] = VecElementCol();
	m_mDif[eFT_Table].push_back(VecElement());
	element.eFileType = eFT_Table;
	element.vecElement.push_back("TABLE");
	element.vecElement.push_back("0,1");
	element.vecElement.push_back("\"EXCEL\"");
	m_mDif[eFT_Table].back().push_back(element);

	m_mDif[eFT_Vectors] = VecElementCol();
	m_mDif[eFT_Vectors].push_back(VecElement());
	element.eFileType = eFT_Vectors;
	element.vecElement.clear();
	element.vecElement.push_back("VECTORS");
	element.vecElement.push_back("0,0");
	element.vecElement.push_back("\"\"");
	m_mDif[eFT_Vectors].back().push_back(element);

	m_mDif[eFT_Tuples] = VecElementCol();
	m_mDif[eFT_Tuples].push_back(VecElement());
	element.eFileType = eFT_Tuples;
	element.vecElement.clear();
	element.vecElement.push_back("TUPLES");
	element.vecElement.push_back("0,0");
	element.vecElement.push_back("\"\"");
	m_mDif[eFT_Tuples].back().push_back(element);

	m_mDif[eFT_BotBegin] = VecElementCol();

	m_mDif[eFT_Data] = VecElementCol();
	m_mDif[eFT_Data].push_back(VecElement());
	element.eFileType = eFT_Data;
	element.vecElement.clear();
	element.vecElement.push_back("DATA");
	element.vecElement.push_back("0,0");
	element.vecElement.push_back("\"\"");
	element.vecElement.push_back("-1,0");
	m_mDif[eFT_Data].back().push_back(element);

	m_mDif[eFT_Eod] = VecElementCol();
	m_mDif[eFT_Eod].push_back(VecElement());
	element.vecElement.clear();
	element.eFileType = eFT_Eod;
	element.vecElement.push_back("EOD");
	element.vecElement.push_back("");
	m_mDif[eFT_Eod].back().push_back(element);

}

void CDifFile::CheckRowCol(size_t row,size_t col)
{
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	if (row >= vecSourceCol.size() || vecSourceCol[row].empty() || col+1 >= vecSourceCol[row].size()-1)
	{
		stringstream str;
		str<<m_strFile<<":"<<"row = "<<row<<"col = "<<col
			<<"out of range"<<endl;
		GenErr(str.str());
	}
}

string CDifFile::GetDataByRowCol(size_t row,size_t col)
{
	CheckRowCol(row,col);
	string strResult;
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	if (vecSourceCol[row][col+1].eFileType == eFT_String)
	{
		strResult = ProduceCommonStr(vecSourceCol[row][col+1].vecElement);
		CleanEndSpace(strResult);
		CleanOnePairQuotation(strResult);
		replace(strResult,"\"\"","\"");
		return strResult;
	}
	else if (vecSourceCol[row][col+1].eFileType == eFT_Number)
	{
		strResult = ProduceNumStr(vecSourceCol[row][col+1].vecElement[0]);
		return strResult;
	}
	else
	{
		stringstream str;
		str<< m_strFile << ":row = " << row << " col = " << col << " unknow error"<<endl;
		GenErr(str.str());
		return strResult;
	}
}

bool	CDifFile::BeNumber(size_t row,size_t col)
{
	CheckRowCol(row,col);
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	if (vecSourceCol[row][col+1].eFileType == eFT_String)
		return false;
	else
		return true;
}

void CDifFile::AddRowColPos(size_t row,size_t col)
{
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	if (row >= vecSourceCol.size())
	{
		size_t rowAdd = row+1 - vecSourceCol.size();
		while (rowAdd--)
		{
			vecSourceCol.push_back(VecElement());
			vecSourceCol.back().push_back(GetBotBeginElement());
			vecSourceCol.back().push_back(GetBotEndElement());
		}
	}

	if (col + 1 >= vecSourceCol[row].size()-1)
	{
		size_t colAdd = col+1-vecSourceCol[row].size()+2;
		while (colAdd--)
		{
			vecSourceCol[row].insert(vecSourceCol[row].end()-1,GetBlankElement());
		}
	}
}

void CDifFile::AddData_toTab(string& strContent,size_t row,size_t col,bool bNum)
{
	AddRowColPos(row,col);
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	MetaElement eleInsert;
	if (bNum)
	{
		string strTemp = "0,";
		strTemp = strTemp + strContent;
		eleInsert.vecElement.push_back(strTemp);
		eleInsert.vecElement.push_back("V");
	}
	else
	{
		string strTemp = strContent;
		replace(strTemp,"\"","\"\"");
		strTemp = "\"" + strTemp + "\"";
		eleInsert.eFileType = eFT_String;
		eleInsert.vecElement.push_back("1,0");
		eleInsert.vecElement.push_back(strTemp);
	}
	vecSourceCol[row][col+1] = eleInsert;
}

void	CDifFile::InsertData_toTab(string& strContent,size_t row,size_t col,bool bNum)
{
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	if (vecSourceCol.empty() || row > vecSourceCol.size()-1)
		return;
	MetaElement eleInsert;
	if (bNum)
	{
		string strTemp = "0,";
		strTemp = strTemp + strContent;
		eleInsert.vecElement.push_back(strTemp);
		eleInsert.vecElement.push_back("V");
	}
	else
	{
		string strTemp = strContent;
		replace(strTemp,"\"","\"\"");
		strTemp = "\"" + strTemp + "\"";
		eleInsert.eFileType = eFT_String;
		eleInsert.vecElement.push_back("1,0");
		eleInsert.vecElement.push_back(strTemp);
	}
	vecSourceCol[row].insert(vecSourceCol[row].begin()+col+1,eleInsert);

}

void CDifFile::GetFileContent(vector<vector<string> >& vecRef)
{
	vecRef.clear();
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	for (size_t i=0;i<vecSourceCol.size();++i)
	{
		vecRef.push_back(vector<string>());
		for (size_t j=0;j<vecSourceCol[i].size()-2;++j)
		{
			string strTemp = GetDataByRowCol(i,j);
			vecRef.back().push_back(strTemp);
		}
	}
}

size_t CDifFile::GetRowCount()
{
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	return vecSourceCol.size();
}

size_t CDifFile::GetColCountByRow(size_t row)
{
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	if (vecSourceCol[row].size()<3)
		return 0;
	else
		return vecSourceCol[row].size()-2;
}

void	CDifFile::SaveFile()
{
	AdjustVECTORSAndTUPLES(m_mDif);
	WriteDataToDifFile(m_strFile,m_mDif);
}

void	CDifFile::DeleteData(size_t row,size_t col)
{
	CheckRowCol(row,col);
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	vecSourceCol[row][col+1] = GetBlankElement();
}

void	CDifFile::DeleteRow(size_t row)
{
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	if (row < vecSourceCol.size())
	{
		vecSourceCol.erase(vecSourceCol.begin()+row);
	}
}

void	CDifFile::DeleteCol(size_t col)
{
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	if (vecSourceCol.empty() || col+1 >= vecSourceCol[0].size()-1)
		return;
	for (size_t i=vecSourceCol.size();i-->0;)
	{
		//今天发现dif文件保存可能有些行会少列，所以这里删除列的时候要加上判断，以免少列的行被删除错误的列，导致格式不对
		if(col+1 < vecSourceCol[i].size()-1)
			vecSourceCol[i].erase(vecSourceCol[i].begin()+col+1);
	}
}

void	CDifFile::DeleteColByRow(size_t row,size_t col)
{
	CheckRowCol(row,col);
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	vecSourceCol[row].erase(vecSourceCol[row].begin()+col+1);
}

void	CDifFile::AddRow_Back()
{
	VecElementCol& vecSourceCol = m_mDif[eFT_BotBegin];
	vecSourceCol.push_back(VecElement());
	vecSourceCol.back().push_back(GetBotBeginElement());
	vecSourceCol.back().push_back(GetBotEndElement());
}
