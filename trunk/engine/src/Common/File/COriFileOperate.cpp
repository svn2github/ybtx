#include "stdafx.h"
#include "COriFileOperate.h"

#define DIFFILE

#ifdef DIFFILE
	#include "CDifFile.h"
#else
	#include "CSvsFile.h"
#endif

COriFileOperate::COriFileOperate()
{
#ifdef DIFFILE
	m_pFileOper = new CDifFile;
#else
	m_pFileOper = new CSvsFile;
#endif
}

COriFileOperate::COriFileOperate(string& strFileName)
{
#ifdef DIFFILE
	m_pFileOper = new CDifFile(strFileName);
#else
	m_pFileOper = new CSvsFile(strFileName);
#endif
}

COriFileOperate::~COriFileOperate()
{
	delete m_pFileOper;
}

string COriFileOperate::GetDataByRowCol(size_t row,size_t col)
{
	return m_pFileOper->GetDataByRowCol(row,col);
}

size_t COriFileOperate::GetRowCount()
{
	return m_pFileOper->GetRowCount();
}

size_t COriFileOperate::GetColCountByRow(size_t row)
{
	return m_pFileOper->GetColCountByRow(row);
}

void COriFileOperate::LoadFile()
{
	m_pFileOper->LoadFile();
}

void	COriFileOperate::AddData_toTab(string& strContent,size_t row,size_t col,bool bNum)
{
	m_pFileOper->AddData_toTab(strContent,row,col,bNum);
}

void COriFileOperate::InsertData_toTab(string& strContent,size_t row,size_t col,bool bNum)
{
	m_pFileOper->InsertData_toTab(strContent,row,col,bNum);
}

void	COriFileOperate::SaveFile()
{
	m_pFileOper->SaveFile();
}

void	COriFileOperate::SetName(string& str)
{
	m_pFileOper->SetName(str);
}

void	COriFileOperate::DeleteData(size_t row,size_t col)
{
	m_pFileOper->DeleteData(row,col);
}

void	COriFileOperate::DeleteRow(size_t row)
{
	m_pFileOper->DeleteRow(row);
}

void	COriFileOperate::DeleteCol(size_t col)
{
	m_pFileOper->DeleteCol(col);
}

void	COriFileOperate::DeleteColByRow(size_t row,size_t col)
{
	m_pFileOper->DeleteColByRow(row,col);
}

void	COriFileOperate::CleanBlanRow()
{
	if (!GetRowCount() || !GetColCountByRow(0))
		return;
	for (size_t i = GetRowCount();i-->0;)
	{
		if (GetDataByRowCol(i,0) == "")
		{
			m_pFileOper->DeleteRow(i);
		}
	}
}

void	COriFileOperate::RetrenchBlanRow()
{
	if (!GetRowCount() || !GetColCountByRow(0))
		return;
	bool bBlanStarted = false;
	for (size_t i = GetRowCount();i-->0;)
	{
		if (GetDataByRowCol(i,0) == "")
		{
			if(bBlanStarted)
				m_pFileOper->DeleteRow(i);
			else
				bBlanStarted = true;
		}
		else
		{
			bBlanStarted = false;
		}
	}
}

void	COriFileOperate::CleanLastBlankRow()
{
	if (!GetRowCount() || !GetColCountByRow(0))
		return;
	for (size_t i = GetRowCount();i-->0;)
	{
		size_t col = GetColCountByRow(i);
		if (GetDataByRowCol(i,0) == "")
		{
			bool bDelete = true;
			for (size_t j = col ; j-- >0;)
			{
				if (GetDataByRowCol(i,j) != "")
				{
					bDelete = false;
					break;
				}
			}
			if (bDelete)
				DeleteRow(i);
		}
		else
			break;
	}
}

void	COriFileOperate::CleanNoteRow()
{
	if (!GetRowCount() || !GetColCountByRow(0))
		return;
	if (m_pFileOper->GetDataByRowCol(0,0) == "NOTE:")
	{
		DeleteRow(0);
	}
	size_t rowCount = GetRowCount();
	if (!rowCount)
		return;
	for (size_t i = GetColCountByRow(0);i-->0;)
	{
		if (GetDataByRowCol(0,i) == "")
		{
			DeleteCol(i);
		}
	}
}

void	COriFileOperate::CleanBlankCol()
{
	CleanNoteRow();
	vector<string> vecTemp;
	GetOneRow(vecTemp,0);
	for (size_t i = vecTemp.size();i-->0;)
	{
		if (GetDataByRowCol(0,i) == "")
		{
			DeleteCol(i);
		}
	}
}

void	COriFileOperate::GetOneRow(vector<string>& vecRef,size_t row)
{
	if (row>m_pFileOper->GetRowCount()-1 || !m_pFileOper->GetColCountByRow(row))
		return;
	for (size_t i = 0;i < m_pFileOper->GetColCountByRow(row);++i)
	{
		vecRef.push_back(m_pFileOper->GetDataByRowCol(row,i));
	}
}

void	COriFileOperate::AddRow_Back()
{
	m_pFileOper->AddRow_Back();
}

bool	COriFileOperate::BeNumber(size_t row,size_t col)
{
	return m_pFileOper->BeNumber(row,col);
}

void	COriFileOperate::GetAllData(vector<vector<string> >& vecRef)
{
	vecRef.clear();
	size_t row = GetRowCount();
	if (!row)
		return;
	size_t col = GetColCountByRow(0);
	if (!col)
		return;
	for (size_t i = 0;i<row;++i)
	{
		vecRef.push_back(vector<string>());
		for (size_t j=0;j<col;++j)
		{
			vecRef.back().push_back(GetDataByRowCol(i,j));
		}
	}
}
