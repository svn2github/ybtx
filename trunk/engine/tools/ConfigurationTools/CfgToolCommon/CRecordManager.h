#pragma once
#include "windows.h"
#include "TSingleton.h"

#define RD_VERSION 5

class CRecordManager : public TSingleton<CRecordManager>
{
public:
	CRecordManager();
	~CRecordManager();
	struct RecordData
	{
		RecordData() {
			ftLastWriteTime.dwHighDateTime=0;
			ftLastWriteTime.dwLowDateTime=0;
		}
		void SetParentFileMuster(vector<string>& vecRef) { m_vecParent = vecRef;}
		void SetChildFileMuster(vector<string>& vecRef)	  { m_vecChild = vecRef;}
		void AddFile_toChildMuster(vector<string>& vecRef);
		FILETIME ftLastWriteTime;
		vector<string> m_vecChild;
		vector<string> m_vecParent;
	};
	typedef map<string,RecordData*> ChunkType;
	void ReadRecordDoc();
	bool GetFileAmendTime(string strFile,FILETIME* time);
	void SaveRecordEntry();
	bool BeChanged(string& strFileName);
	bool BeInRecordList(string& strFileName);
	void AddFileRecord(string& strFileName);
	void AddFileChildRecord(string& strFileName,vector<string>& vecRef);
	void AddFileParentRecord(string& strFileName,vector<string>& vecRef);
	void SetAllFileChange() { m_bAllChange = true;}

	void CleanRecordData();
	void Copy_toNewRecord(string& strFileName);
	void Add_DisinterestRecord();

	static void SetChunkName(const string& str) { m_strChunk = str;}
	void SaveChunkSubRD(ofstream& outFile,const string& chunkName,ChunkType& subRD);
	void ReadChunkSubRD(ifstream& iFile,ChunkType& subRD);

	bool BeInOtherChunkChange(string& strFileName);

private:
	ChunkType m_mapRecord;
	ChunkType m_mapNewRecord;
	map<string,ChunkType> m_WholeRD;
	bool m_bAllChange;
	static string m_strChunk;
};
