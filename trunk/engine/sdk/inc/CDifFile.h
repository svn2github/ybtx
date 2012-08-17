#pragma once
#include <vector>
#include <map>
#include <string>
#include "IOriFileInterface.h"

class CDifFile : public IOriFileInterface
{
public:
	CDifFile(string& strFileName);
	CDifFile();
	~CDifFile();
	string GetDataByRowCol(size_t row,size_t col);
	bool	  BeNumber(size_t row,size_t col);
	void   AddData_toTab(string& strContent,size_t row,size_t col,bool bNum = false);
	void	  InsertData_toTab(string& strContent,size_t row,size_t col,bool bNum = false);
	void   AddRowColPos(size_t row,size_t col);
	void   CheckRowCol(size_t row,size_t col);
	void   GetFileContent(vector<vector<string> >& vecRef);
	void   SetName(string& str) { m_strFile = str;}
	void   LoadFile();
	size_t	  GetRowCount();
	size_t	  GetColCountByRow(size_t row);
	void		SaveFile();

	void			DeleteData(size_t row,size_t col);
	void			DeleteRow(size_t row);
	void			DeleteCol(size_t col);
	void			DeleteColByRow(size_t row,size_t col);
	void			AddRow_Back();

private:
	enum EFieldType										//Dif文件中字段类型
	{
		eFT_None,
		eFT_Table,										//TABLE
		eFT_Vectors,									//VECTORS
		eFT_Tuples,										//TUPLES
		eFT_Data,										//DATA
		eFT_BotBegin,									//BOT
		eFT_String,										//1,0
		eFT_Number,										//0,X
		eFT_BotEnd,										//-1,0
		eFT_Eod,										//EOD
		eFT_Context,									//字段内容

		eFT_All,
	};

	struct  MetaElement
	{
		EFieldType eFileType;
		vector<string> vecElement;
		bool operator == (MetaElement& other)
		{
			if(eFileType == other.eFileType && vecElement.size() == other.vecElement.size())
			{
				bool bReturn = true;
				for (int i=0;i<(int)vecElement.size();++i)
				{
					if (vecElement[i] != other.vecElement[i])
					{
						bReturn = false;
						break;
					}
				}
				return bReturn;
			}
			else
			{
				return false;
			}
		}
	};

	typedef vector<MetaElement> VecElement;
	typedef vector<VecElement> VecElementCol;
	
	struct MetaData
	{
		EFieldType m_eDataType;
		string m_strData;
	};

	void ReadFileToMap(string strFileName,map<EFieldType,VecElementCol>& mapRef);
	void ReadFileToVec(string strFileName,vector<vector<string> >& vecRef);
	void ReadFileToVec(string strFileName,vector<vector<MetaData> >& vecRef);
	void WriteSectionToDifFile(ofstream& outStream,EFieldType eFileHeadType,map<EFieldType,VecElementCol>& mapRef);
	void WriteDataToDifFile(string strFileName,map<EFieldType,VecElementCol>& mapRef);
	void AdjustVECTORSAndTUPLES(map<EFieldType,VecElementCol>& mapRef);
	string ProduceNumStr(string& str);
	string ProduceCommonStr(vector<string> &vecStr);
	EFieldType GetFileType(string str,string& strReturn,stringstream& strStream,bool &bReturn);
	void CleanBlankRow(VecElementCol& vecSource);
	bool VerdictFirstRowNote(VecElementCol& vecEleCol);
	void CleanOnePairQuotation(string& str);
	void CleanEndSpace(string& str);
	MetaElement GetBlankElement();
	MetaElement GetBotBeginElement();
	MetaElement	GetBotEndElement();

private:
	string	m_strFile;
	map<EFieldType,VecElementCol> m_mDif;

};
