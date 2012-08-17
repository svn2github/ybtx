#pragma once
#include <vector>
#include <map>
//#include <string>
#include "IOriFileInterface.h"

class TiXmlDocument;

class CSvsFile : public IOriFileInterface
{
public:
	CSvsFile(string& strFileName);
	CSvsFile();
	virtual ~CSvsFile();
	string GetDataByRowCol(size_t row,size_t col);
	bool	  BeNumber(size_t row,size_t col);
	void   AddData_toTab(string& strContent,size_t row,size_t col,bool bNum = false);
	void	  InsertData_toTab(string& strContent,size_t row,size_t col,bool bNum = false);
	void   AddRowColPos(size_t row,size_t col);
	void   CheckRowCol(size_t row,size_t col);
	//void   GetFileContent(vector<vector<string>>& vecRef);
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

	void	Resize(size_t row, size_t col);

private:
	enum ETableDataType
	{
		eTDT_None,
		eTDT_String,
		eTDT_Float,
	};

	struct TableData
	{
		TableData() : m_eTDType(eTDT_None), m_sTDValue("") {};
		ETableDataType	m_eTDType;
		string			m_sTDValue;
	};

	typedef vector<TableData>	TableRow;
	typedef vector<TableRow>	TableMatrix;

	void LoadXML();
	void UnloadXML();
	ETableDataType GetTableDataType(const string& sType);
	void SaveXML();
	void XMLEncode(string& sText, bool bIsAttr = false);
	void XMLDecode(string& sText, bool bIsAttr = false);

	string			m_strFile;
	//TiXmlNode*		m_sFileHeadInfo;
	TableMatrix		m_vecTR;
	size_t			m_uTitleLineNo;

	//TiXmlDocument*	m_pXmlDoc;

};

