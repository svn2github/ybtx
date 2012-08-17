#pragma once
#include <string>

class IOriFileInterface
{
public:
	virtual ~IOriFileInterface() {};
	virtual string GetDataByRowCol(size_t row,size_t col) = 0;
	virtual size_t GetRowCount() = 0;
	virtual size_t GetColCountByRow(size_t row) = 0;
	virtual void LoadFile() = 0;
	virtual	void AddData_toTab(string& strContent,size_t row,size_t col,bool bNum = false) = 0;
	virtual void InsertData_toTab(string& strContent,size_t row,size_t col,bool bNum = false) = 0;
	virtual void AddRow_Back() = 0;
	virtual void DeleteData(size_t row,size_t col) = 0;
	virtual void DeleteRow(size_t row) = 0;
	virtual void DeleteCol(size_t col) = 0;
	virtual void DeleteColByRow(size_t row,size_t col) = 0;
	virtual void SaveFile() = 0;
	virtual void SetName(string& str) = 0;
	virtual bool BeNumber(size_t row,size_t col) = 0;
};
