#pragma once
#include "CDynamicObject.h"

namespace sqr
{

	class CFileWatcherData
		: public virtual CDynamicObject
	{
	public:
		CFileWatcherData();
		CFileWatcherData(const CFileWatcherData& data);
		~CFileWatcherData();
		
		const char* GetFileName(uint32 uIndex);					
		
		uint32 GetCount();								
		
		void PushBack(const string& sFileName);

		void Pop(const string& sFileName);

		bool Empty();									
		
		void SetIsLuaFile(bool bIsLuaFile);

		bool GetIsLuaFile();
		
		void Release();

	private:
		vector<string>		m_vecFileData;
		bool				m_bLuaFile;
	};

}

