//#include "AutoSearchFile.h"

//////////////////////////////////////////////////////////////////////
// AutoSearchFile.h: interface for the helper_coffs::AutoSearchFile class.
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

const int max_limit = 0x100000;	//--1048576
const int def_limit = 0x10000;	//--65536

const char c_pathSeparator = '\\';
const string s_pathSeparator = "\\";

class AutoSearchFile  
{
public:
	AutoSearchFile();
	virtual ~AutoSearchFile();

	static AutoSearchFile * GetInstance() 
	{ 
		if ( !one ) 
		{
			one = new AutoSearchFile;
		}
		return one;
	} 

public:
	typedef vector<string>	typeT;
	virtual int match(string fext_, string file_);	//--文件匹配检测
	
public:
	void SearchFile(string path_, string fext_ = "");
	string AutoSearchFile::FindFileName(string path_, string matchname);
	string AutoSearchFile::FindCatalogName(string path_, string matchname);
public:
	string get(int i = 0) { return _co.at(i); }
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
	int dir(string s);	//--判断s是否为目录

	vector<string> & GetAllFiles(const string& path_, const string& postfix);
	
private:
	int _limit;	//--limit
	int _dirs;	//--dirs-是否查找子目录(默认0-不查找)
	typeT _co_file;
	typeT _co_dir;
	typeT _co;
	
private:
	inline int _limit_() { return count() == limit(); }	//--limit test
	inline string _uppercase_(string s);	//--转换s到大写字母

	static AutoSearchFile * one;
	string m_strPrePath;
};

#endif	//--__FFSCO__
