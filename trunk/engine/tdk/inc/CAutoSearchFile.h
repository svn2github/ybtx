//#include "CAutoSearchFile.h"

//////////////////////////////////////////////////////////////////////
// CAutoSearchFile.h: interface for the helper_coffs::CAutoSearchFile class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Copyright 2004, ??? (www.???.com). All rights reserved.
 * Copyright 1990-2004, ???.
 *
 * See the COPYING file for the terms of usage and distribution.
*/
//////////////////////////////////////////////////////////////////////
// ...
// create@2004-03-17
// by ???
//////////////////////////////////////////////////////////////////////

#ifndef	__AUTOSEARCHFile__
//#if !defined(__AUTOSEARCHFile__)
#define __AUTOSEARCHFile__

#ifndef	WIN32
#error now only for WIN32(platform).
#endif
#include <string>
const int max_limit = 0x100000;	//--1048576
const int def_limit = 0x10000;	//--65536

const TCHAR c_pathSeparator = '\\';
const std::string s_pathSeparator = "\\";

class CAutoSearchFile  
{
public:
	CAutoSearchFile();
	virtual ~CAutoSearchFile();

	static CAutoSearchFile * GetInstance() 
	{ 
		if ( !one ) 
		{
			one = new CAutoSearchFile;
		}
		return one;
	} 

public:
	typedef vector<std::string>	typeT;
	virtual int match(std::string fext_, std::string file_);	//--文件匹配检测
	
public:
	void SearchFile(std::string path_, std::string fext_ = "");
	std::string CAutoSearchFile::FindFileName(std::string path_, std::string matchname);
	std::string CAutoSearchFile::FindCatalogName(std::string path_, std::string matchname);
public:
	std::string get(int i = 0) { return _co.at(i); }
	int count() { return _co_dir.size() + _co_file.size(); }
	//int count() { return _co.size(); }
	typeT co() { return _co; }
	typeT co_dir() { return _co_dir; }
	typeT co_file() { return _co_file; }
	
public:
	int limit() { return _limit; }
	//void limit(int limit_) { _limit = limit_; }
	void limit(int limit_);	//--设置结果文件数目上限
	
public:
	int dirs() { return _dirs; }	//get
	void dirs(int dirs) { _dirs = dirs; }	//--set
	int dir(std::string s);	//--判断s是否为目录

	vector<std::string> & GetAllFiles(const std::string& path_, const std::string& postfix);
	
private:
	int _limit;	//--limit
	int _dirs;	//--dirs-是否查找子目录(默认0-不查找)
	typeT _co_file;
	typeT _co_dir;
	typeT _co;
	
private:
	inline int _limit_() { return count() == limit(); }	//--limit test
	inline std::string _uppercase_(std::string s);	//--转换s到大写字母

	static CAutoSearchFile * one;
	std::string m_strPrePath;
};

#endif	//--__FFSCO__
