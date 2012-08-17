#include "stdafx.h"
#include "CRecordManager.h"

#pragma warning(push)
#pragma warning(disable:4018)

string CRecordManager::m_strChunk;

void CRecordManager::RecordData::AddFile_toChildMuster(vector<string>& vecRef)
{
	for (int i=0;i<vecRef.size();++i)
	{
		if (find(m_vecChild.begin(),m_vecChild.end(),vecRef[i]) == m_vecChild.end())
		{
			m_vecChild.push_back(vecRef[i]);
		}
	}
}

CRecordManager::CRecordManager()
{
	m_bAllChange = false;
	ReadRecordDoc();
}

CRecordManager::~CRecordManager()
{
	if (!m_bAllChange)
		SaveRecordEntry();
	CleanRecordData();
}

bool CRecordManager::GetFileAmendTime(string strFile,FILETIME* time)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(strFile.c_str(),&fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	*time = fd.ftLastWriteTime;
	FindClose(hFind);
	return true;
}

//void CRecordManager::SaveRecordEntry()
//{
//	ofstream oFile("Record.data",ios_base::binary | ios_base::out);
//	if (!oFile)
//	{
//		string error = "Record.data 打开失败";
//		MessageBox(NULL,error.c_str(),NULL,0);
//		return;
//	}
//	Add_DisinterestRecord();
//	uint32 uVersion = RD_VERSION;
//	oFile.write((char*)&uVersion,sizeof(uint32));
//	int fileNum = m_mapNewRecord.size();
//	oFile.write((char*)&fileNum,sizeof(int));
//	for (map<string,RecordData*>::iterator iter = m_mapNewRecord.begin();iter != m_mapNewRecord.end();++iter)
//	{
//		string strName = iter->first;
//		RecordData* pData = iter->second;
//		int nameNum = strName.length();
//		oFile.write((char*)&nameNum,sizeof(int));
//		oFile.write(strName.c_str(),strName.length());
//		oFile.write((char*)&pData->ftLastWriteTime,sizeof(FILETIME));
//		int parentNum = pData->m_vecParent.size();
//		oFile.write((char*)&parentNum,sizeof(int));
//		for (int i = 0;i<parentNum;++i)
//		{
//			string& strParent = pData->m_vecParent[i];
//			int pNum = strParent.length();
//			oFile.write((char*)&pNum,sizeof(int));
//			oFile.write(strParent.c_str(),strParent.length());
//		}
//		int childNum = pData->m_vecChild.size();
//		oFile.write((char*)&childNum,sizeof(int));
//		for (int i = 0;i<childNum;++i)
//		{
//			string& strChild = pData->m_vecChild[i];
//			int pNum = strChild.length();
//			oFile.write((char*)&pNum,sizeof(int));
//			oFile.write(strChild.c_str(),strChild.length());
//		}
//	}
//	oFile.close();
//}

void CRecordManager::SaveChunkSubRD(ofstream& outFile,const string& chunkName,ChunkType& subRD)
{
	int nameNum = chunkName.length();
	outFile.write((char*)&nameNum,sizeof(int));
	outFile.write(chunkName.c_str(),nameNum);
	int fileNum = subRD.size();
	outFile.write((char*)&fileNum,sizeof(int));
	for (map<string,RecordData*>::iterator iter = subRD.begin();iter != subRD.end();++iter)
	{
		string strName = iter->first;
		RecordData* pData = iter->second;
		int nameNum = strName.length();
		outFile.write((char*)&nameNum,sizeof(int));
		outFile.write(strName.c_str(),strName.length());
		outFile.write((char*)&pData->ftLastWriteTime,sizeof(FILETIME));
		int parentNum = pData->m_vecParent.size();
		outFile.write((char*)&parentNum,sizeof(int));
		for (int i = 0;i<parentNum;++i)
		{
			string& strParent = pData->m_vecParent[i];
			int pNum = strParent.length();
			outFile.write((char*)&pNum,sizeof(int));
			outFile.write(strParent.c_str(),strParent.length());
		}
		int childNum = pData->m_vecChild.size();
		outFile.write((char*)&childNum,sizeof(int));
		for (int i = 0;i<childNum;++i)
		{
			string& strChild = pData->m_vecChild[i];
			int pNum = strChild.length();
			outFile.write((char*)&pNum,sizeof(int));
			outFile.write(strChild.c_str(),strChild.length());
		}
	}
}

void CRecordManager::SaveRecordEntry()
{
	if (m_strChunk == "")
		return;
	ofstream oFile("Record.data",ios_base::binary | ios_base::out);
	if (!oFile)
	{
		string error = "Record.data 打开失败";
		MessageBox(NULL,error.c_str(),NULL,0);
		return;
	}
	if (!m_mapNewRecord.empty())
	{
		m_WholeRD[m_strChunk] = m_mapNewRecord;
	}
	uint32 uVersion = RD_VERSION;
	oFile.write((char*)&uVersion,sizeof(uint32));
	int chunkNum = m_WholeRD.size();
	oFile.write((char*)&chunkNum,sizeof(int));
	for (map<string,ChunkType>::iterator iter = m_WholeRD.begin();iter != m_WholeRD.end();++iter)
	{
		string strName = iter->first;
		SaveChunkSubRD(oFile,strName,iter->second);
	}
	oFile.close();
}

//void CRecordManager::ReadRecordDoc()
//{
//	ifstream iFile("Record.data",ios_base::binary | ios_base::in);
//	if (!iFile)
//		return;
//	SQR_TRY
//	uint32 uVersion = 0;
//	iFile.read((char*)&uVersion,sizeof(uint32));
//	if (uVersion != RD_VERSION)
//	{
//		iFile.close();
//		DeleteFile("Record.data");
//		return;
//	}
//	int recordNum=0;
//	iFile.read((char*)&recordNum,sizeof(int));
//	for (int i=0;i<recordNum;++i)
//	{
//		int nameNum;
//		iFile.read((char*)&nameNum,sizeof(int));
//		string strFileName;
//		strFileName.resize(nameNum);
//		iFile.read((char*)strFileName.c_str(),nameNum);
//		m_mapRecord[strFileName] = new RecordData;
//		RecordData* pData = m_mapRecord[strFileName];
//		iFile.read((char*)&pData->ftLastWriteTime,sizeof(FILETIME));
//		int parentNum;
//		iFile.read((char*)&parentNum,sizeof(int));
//		for (int i=0;i<parentNum;++i)
//		{
//			int strLength;
//			iFile.read((char*)&strLength,sizeof(int));
//			string str;
//			str.resize(strLength);
//			iFile.read((char*)str.c_str(),strLength);
//			pData->m_vecParent.push_back(str);
//		}
//		int childNum;
//		iFile.read((char*)&childNum,sizeof(int));
//		for (int i=0;i<childNum;++i)
//		{
//			int strLength;
//			iFile.read((char*)&strLength,sizeof(int));
//			string str;
//			str.resize(strLength);
//			iFile.read((char*)str.c_str(),strLength);
//			pData->m_vecChild.push_back(str);
//		}
//	}
//	SQR_CATCH(exp)
//	{
//		string strMsg = "读取转换文档信息出错";
//		strMsg = strMsg + exp.ErrorTitle();
//		CleanRecordData();
//		iFile.close();
//	}
//	SQR_TRY_END
//	iFile.close();
//}

void CRecordManager::ReadRecordDoc()
{
	ifstream iFile("Record.data",ios_base::binary | ios_base::in);
	if (!iFile)
		return;
	SQR_TRY
		uint32 uVersion = 0;
	iFile.read((char*)&uVersion,sizeof(uint32));
	if (uVersion != RD_VERSION)
	{
		iFile.close();
		DeleteFile("Record.data");
		return;
	}
	int chunkNum=0;
	iFile.read((char*)&chunkNum,sizeof(int));
	for (int i=0;i<chunkNum;++i)
	{
		int nameNum;
		iFile.read((char*)&nameNum,sizeof(int));
		string strChunkName;
		strChunkName.resize(nameNum);
		iFile.read((char*)strChunkName.c_str(),nameNum);
		m_WholeRD[strChunkName] = ChunkType();
		ReadChunkSubRD(iFile,m_WholeRD[strChunkName]);
	}
	if (m_strChunk != "" && m_WholeRD.count(m_strChunk))
	{
		m_mapRecord = m_WholeRD[m_strChunk];
	}
	SQR_CATCH(exp)
	{
		string strMsg = "读取转换文档信息出错";
		strMsg = strMsg + exp.ErrorTitle();
		CleanRecordData();
		iFile.close();
	}
	SQR_TRY_END
	iFile.close();
}

void CRecordManager::ReadChunkSubRD(ifstream& iFile,ChunkType& subRD)
{
	int recordNum=0;
	iFile.read((char*)&recordNum,sizeof(int));
	for (int i=0;i<recordNum;++i)
	{
		int nameNum;
		iFile.read((char*)&nameNum,sizeof(int));
		string strFileName;
		strFileName.resize(nameNum);
		iFile.read((char*)strFileName.c_str(),nameNum);
		subRD[strFileName] = new RecordData;
		RecordData* pData = subRD[strFileName];
		iFile.read((char*)&pData->ftLastWriteTime,sizeof(FILETIME));
		int parentNum;
		iFile.read((char*)&parentNum,sizeof(int));
		for (int i=0;i<parentNum;++i)
		{
			int strLength;
			iFile.read((char*)&strLength,sizeof(int));
			string str;
			str.resize(strLength);
			iFile.read((char*)str.c_str(),strLength);
			pData->m_vecParent.push_back(str);
		}
		int childNum;
		iFile.read((char*)&childNum,sizeof(int));
		for (int i=0;i<childNum;++i)
		{
			int strLength;
			iFile.read((char*)&strLength,sizeof(int));
			string str;
			str.resize(strLength);
			iFile.read((char*)str.c_str(),strLength);
			pData->m_vecChild.push_back(str);
		}
	}
}

bool CRecordManager::BeChanged(string& strFileName)
{
	if (m_bAllChange)
		return true;

	if (!m_mapRecord.count(strFileName))
		return BeInOtherChunkChange(strFileName);
	RecordData* pdata = m_mapRecord[strFileName];
	FILETIME time;
	if(!GetFileAmendTime(strFileName,&time))
		return true;
	if (time.dwHighDateTime == pdata->ftLastWriteTime.dwHighDateTime && time.dwLowDateTime == pdata->ftLastWriteTime.dwLowDateTime)
	{
		Copy_toNewRecord(strFileName);
		for (int i = 0;i<pdata->m_vecChild.size();++i)
		{
			if (BeChanged(pdata->m_vecChild[i]))
				return true;
		}
		return false;
	}
	else
	{
		return true;
	}
}

bool CRecordManager::BeInOtherChunkChange(string& strFileName)
{
	if (m_strChunk != "")
		return true;
	map<string,ChunkType>::iterator iter = m_WholeRD.begin();
	for (;iter != m_WholeRD.end();++iter)
	{
		if (iter->first == m_strChunk)
			continue;
		ChunkType& subRD = iter->second;
		if (!subRD.count(strFileName))
			continue;
		RecordData* pdata = subRD[strFileName];
		FILETIME time;
		if(!GetFileAmendTime(strFileName,&time))
			return true;
		if (time.dwHighDateTime == pdata->ftLastWriteTime.dwHighDateTime && time.dwLowDateTime == pdata->ftLastWriteTime.dwLowDateTime)
		{
			for (int i = 0;i<pdata->m_vecChild.size();++i)
			{
				if (BeChanged(pdata->m_vecChild[i]))
					return true;
			}
			return false;
		}
		else
		{
			return true;
		}
	}
	return true;
}

bool CRecordManager::BeInRecordList(string& strFileName)
{
	return m_mapRecord.count(strFileName) ? true : false;
}

void CRecordManager::AddFileRecord(string& strFileName)
{
	if (!m_mapNewRecord.count(strFileName))
		m_mapNewRecord[strFileName] = new RecordData();
	RecordData* pdata = m_mapNewRecord[strFileName];
	FILETIME time;
	GetFileAmendTime(strFileName,&time);
	if (time.dwLowDateTime == pdata->ftLastWriteTime.dwLowDateTime 
		&& time.dwHighDateTime == pdata->ftLastWriteTime.dwHighDateTime)
		return;
	else
	{
		pdata->m_vecChild.clear();
		pdata->m_vecParent.clear();
		pdata->ftLastWriteTime = time;
	}
}

void CRecordManager::AddFileChildRecord(string& strFileName,vector<string>& vecRef)
{
	if (!m_mapNewRecord.count(strFileName))
		m_mapNewRecord[strFileName] = new RecordData();
	RecordData* pdata = m_mapNewRecord[strFileName];
	FILETIME time;
	GetFileAmendTime(strFileName,&time);
	if (time.dwLowDateTime == pdata->ftLastWriteTime.dwLowDateTime 
		&& time.dwHighDateTime == pdata->ftLastWriteTime.dwHighDateTime)
	{
		pdata->AddFile_toChildMuster(vecRef);
	}
	else
	{
		pdata->ftLastWriteTime = time;
		pdata->SetChildFileMuster(vecRef);
	}
}

void CRecordManager::AddFileParentRecord(string& strFileName,vector<string>& vecRef)
{
	if (!m_mapNewRecord.count(strFileName))
		m_mapNewRecord[strFileName] = new RecordData();
	RecordData* pdata = m_mapNewRecord[strFileName];
	GetFileAmendTime(strFileName,&pdata->ftLastWriteTime);
	pdata->SetParentFileMuster(vecRef);
}

void CRecordManager::CleanRecordData()
{
	for (map<string,RecordData*>::iterator iter = m_mapRecord.begin();iter != m_mapRecord.end();++iter)
	{
		delete iter->second;
	}
	for (map<string,RecordData*>::iterator iter = m_mapNewRecord.begin();iter != m_mapNewRecord.end();++iter)
	{
		delete iter->second;
	}
}

void CRecordManager::Copy_toNewRecord(string& strFileName)
{
	if (!m_mapRecord.count(strFileName) || m_mapNewRecord.count(strFileName))
		return;
	RecordData* pdata = m_mapRecord[strFileName];
	RecordData* pNewData = new RecordData;
	pNewData->ftLastWriteTime = pdata->ftLastWriteTime;
	pNewData->m_vecParent = pdata->m_vecParent;
	pNewData->m_vecChild = pdata->m_vecChild;
	m_mapNewRecord[strFileName] = pNewData;
}

void CRecordManager::Add_DisinterestRecord()
{
	map<string,RecordData*>::iterator iter = m_mapRecord.begin();
	for (;iter != m_mapRecord.end();++iter)
	{
		string strName = iter->first;
		Copy_toNewRecord(strName);
	}
}


#pragma warning(pop)
