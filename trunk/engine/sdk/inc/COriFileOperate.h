#pragma once
#include "IOriFileInterface.h"

class COriFileOperate : public IOriFileInterface
{
public:
	COriFileOperate();
	~COriFileOperate();
	COriFileOperate(string& strFileName);
	string		GetDataByRowCol(size_t row,size_t col);
	size_t		GetRowCount();
	size_t		GetColCountByRow(size_t row);
	void			LoadFile();
	void			AddData_toTab(string& strContent,size_t row,size_t col,bool bNum = false);
	void			InsertData_toTab(string& strContent,size_t row,size_t col,bool bNum = false);
	void			SaveFile();
	void			SetName(string& str);
	void			DeleteData(size_t row,size_t col);
	void			DeleteRow(size_t row);
	void			DeleteCol(size_t col);
	void			DeleteColByRow(size_t row,size_t col);
	void			AddRow_Back();

	void			GetAllData(vector<vector<string> >& vecRef);
	void			CleanLastBlankRow();
	void			CleanBlanRow();
	void			RetrenchBlanRow();
	void			CleanNoteRow();
	void			CleanBlankCol();
	void			GetOneRow(vector<string>& vecRef,size_t row);
	bool	  BeNumber(size_t row,size_t col);

private:
	IOriFileInterface*	m_pFileOper;
};
