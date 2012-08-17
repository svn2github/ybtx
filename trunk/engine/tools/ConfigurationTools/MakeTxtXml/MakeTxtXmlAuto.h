#include "stdafx.h"
#include "CXmlConfig.inl"
#include "tinyxml/tinyxml.h"
#include "TSingleton.h"
#include "COriFileOperate.h"

	
	void WriteToXml(string& strXml, vector<bool>& vecType,string& FileName);
	void GetColType(string& strFileName,vector<bool>& vecType);
	void ForceDeleteFile(const string& strFile);