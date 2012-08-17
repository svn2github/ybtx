
// AutoSearchFile.cpp: implementation of the helper_coffs::AutoSearchFile class.
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
#include "stdafx.h"	//--i want remove it
#include "AutoSearchFile.h"
#include <windows.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AutoSearchFile::AutoSearchFile()
	: _limit(def_limit), _dirs(1), m_strPrePath("")
{
}

AutoSearchFile::~AutoSearchFile()
{
}

//////////////////////////////////////////////////////////////////////
string AutoSearchFile::_uppercase_(string s)
{
	const TCHAR aazz = 'z' - 'Z';
	string rs;
	for (string::iterator it = s.begin();
	s.end() != it; it++)
	{
		if ('a' <= *it && *it <= 'z') rs.append(1, *it - aazz);
		else rs.append(1, *it);
	}
	return rs;
}
//////////////////////////////////////////////////////////////////////

int AutoSearchFile::match(string fext_, string file_)
{
	string fext = _uppercase_(fext_);
	string file = _uppercase_(file_);
	
	int pos = file.find_last_of('.');
	if (string::npos != pos) file = file.substr(pos);
	
	return (string::npos != fext.find(file));
}

void AutoSearchFile::limit(int limit_)
{
	if (limit_ < 1) _limit = def_limit;
	if (limit_ < max_limit) _limit = limit_;
}

int AutoSearchFile::dir(string s)
{
	return (FILE_ATTRIBUTE_DIRECTORY == GetFileAttributes(s.c_str()));
}

void AutoSearchFile::SearchFile(string path_, string fext_)
{
	_co_file.clear();
	_co_dir.clear();
	_co.clear();
	
	string path = path_;
	if (path.size() > 0 && c_pathSeparator != path[path.size() - 1])
		path.append(s_pathSeparator);
	
	string fext = fext_;
	if (0 == fext.compare("*") || 0 == fext.compare("*.*")) 
		fext = "";

	//string file = fext;
	string file = "*";
	
	string s = path + file;
	
	WIN32_FIND_DATA fileinfo = {0};
	HANDLE handle = FindFirstFile( s.c_str(), &fileinfo );
	
	if (NULL != handle && INVALID_HANDLE_VALUE != handle)
	{
		
		do {
			
			if (_limit_()) break;	//--limit test
			
			if ('.' != fileinfo.cFileName[0])	//--skip./..
				if ((FILE_ATTRIBUTE_DIRECTORY & fileinfo.dwFileAttributes)
					== FILE_ATTRIBUTE_DIRECTORY)	//--目录
				{
					if (!_limit_()) 
						_co_dir.push_back( path + fileinfo.cFileName + s_pathSeparator );
					
					if (0 != dirs())
					{
						AutoSearchFile o;
						o.dirs(dirs());
						o.SearchFile( path + fileinfo.cFileName, fext);
						//--dir
						typeT o_dir = o.co_dir();
						for (typeT::iterator it_dir = o_dir.begin();o_dir.end() != it_dir && !_limit_();it_dir ++) 
							_co_dir.push_back(*it_dir);
						//--file
						typeT o_file = o.co_file();
						for (typeT::iterator it_file = o_file.begin();o_file.end() != it_file && !_limit_();it_file ++)
							_co_file.push_back(*it_file);
					}
				}
				else
				{
					if (!_limit_()&& (0 == fext.size() || match(fext, fileinfo.cFileName))) 
						_co_file.push_back( path + fileinfo.cFileName );
				}
				
		} while (FindNextFile( handle, &fileinfo ));
		
		FindClose(handle);
	}
	
	//--dir
	for (typeT::iterator it_dir = _co_dir.begin();_co_dir.end() != it_dir;it_dir ++) 
		_co.push_back(*it_dir);

	//--file
	for (typeT::iterator it_file = _co_file.begin();_co_file.end() != it_file;it_file ++) 
		_co.push_back(*it_file);
}

//搜索到一个文件后，立马返回值
string AutoSearchFile::FindFileName(string path_, string matchname)
{
	string fext = "*.bmp";

	if( m_strPrePath != path_ )
	{
		m_strPrePath = path_;
		SearchFile(path_, fext);
	}

	for(size_t i = 0; i < _co_file.size(); ++i)
	{
		if( _co_file[i].find(matchname) != -1 )
		{
			return _co_file[i];
		}
	}

	return "";
}

//搜索到一个后，立马返回值
string AutoSearchFile::FindCatalogName(string path_, string matchname)
{
	string fext = "*.bmp";

	if( m_strPrePath != path_ )
	{
		m_strPrePath = path_;
		SearchFile(path_, fext);
	}

	for(size_t i = 0; i < _co_dir.size(); ++i)
	{
		if( _co_dir[i].find(matchname) != -1 )
		{
			return _co_dir[i];
		}
	}

	return "";
}

//根据后缀名，返回一个文件夹下的所有名称
vector<string> & AutoSearchFile::GetAllFiles(const string& path_, const string& postfix)
{
	if( m_strPrePath != path_ )
	{
		m_strPrePath = path_;
		SearchFile(path_, postfix);
	}

	return _co_file;
}

