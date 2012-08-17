#include "StdAfx.h"
#include <stack>
#include "IoSystem.h"
#include "StringHelper.h"
#include "CPkgFile.h"
#include "CodeCvs.h"

namespace sqr
{

//------------------------------------------------------------------------------
IoSystem::IoSystem()
{
	TCHAR buf[MAX_PATH] = { 0 };
	GetTempPath(sizeof(buf), buf);
	this->RegisterDirMapping("temp", buf);
	GetModuleFileName(NULL, buf, sizeof(buf));
	string binPath = buf;
	binPath = binPath.substr(0, binPath.rfind('\\'));
	this->RegisterDirMapping("bin", binPath);
	GetCurrentDirectory(sizeof(buf), buf);
	this->RegisterDirMapping("worksrc", buf);
}

//------------------------------------------------------------------------------
IoSystem::~IoSystem()
{
	this->dirMappings.clear();
}

//------------------------------------------------------------------------------
bool
IoSystem::CreateDirectory( const string& d ) const
{
	Ast(!d.empty());
	string dir = this->GetFullPath(d);
	if (endwith(dir, '\\'))
		dir = dir.substr(0, dir.length() - 1);

	stack<string> paths;
	while (!this->DirectoryExists(dir))
	{
		paths.push(dir);
		string nextPath = dir.substr(0, dir.rfind('\\'));
		if (nextPath == dir || nextPath.length() <= 2)
			break;
		dir = nextPath;
	}
	while (!paths.empty())
	{
		const string& path = paths.top();
		if (0 == ::CreateDirectory(path.c_str(), NULL))
			return false;
		paths.pop();
	}

	return true;
}

//------------------------------------------------------------------------------
bool
IoSystem::DeleteDirectory( const string& d ) const
{
	Ast(!d.empty());
	string dir = this->GetFullPath(d);
	return (0 != ::RemoveDirectory(dir.c_str()));
}

//------------------------------------------------------------------------------
bool
IoSystem::DirectoryExists( const string& d ) const
{
	Ast(!d.empty());
	string dir = this->GetFullPath(d);
	DWORD fileAttrs = GetFileAttributes(dir.c_str());
	return ((INVALID_FILE_ATTRIBUTES != fileAttrs) && (0 != (FILE_ATTRIBUTE_DIRECTORY & fileAttrs)));
}

//------------------------------------------------------------------------------
/**
	if the new file already exists, the function overwrites the existing file
 */
bool
IoSystem::CopyFile( const string& f, const string& t ) const
{
	Ast(!f.empty() && !t.empty());
	string from = this->GetFullPath(f);
	string to = this->GetFullPath(t);
	return (0 != ::CopyFile(from.c_str(), to.c_str(), FALSE));
}

//------------------------------------------------------------------------------
bool
IoSystem::DeleteFile( const string& s ) const
{
	Ast(!s.empty());
	string path = this->GetFullPath(s);
	return (0 != ::DeleteFile(path.c_str()));
}

//------------------------------------------------------------------------------
bool
IoSystem::FileExists( const string& s ) const
{
	Ast(!s.empty());
	string path = this->GetFullPath(s);
	DWORD fileAttrs = GetFileAttributes(path.c_str());
	return ((INVALID_FILE_ATTRIBUTES != fileAttrs) && (0 == (FILE_ATTRIBUTE_DIRECTORY & fileAttrs)));
}

//------------------------------------------------------------------------------
void
IoSystem::SetReadOnly( const string& s, bool b ) const
{
	Ast(!s.empty());
	string path = this->GetFullPath(s);
	DWORD fileAttrs = GetFileAttributes(path.c_str());
	if (b)
		fileAttrs |= FILE_ATTRIBUTE_READONLY;
	else
		fileAttrs &= ~FILE_ATTRIBUTE_READONLY;
	SetFileAttributes(path.c_str(), fileAttrs);
}

//------------------------------------------------------------------------------
bool
IoSystem::IsReadOnly( const string& s ) const
{
	Ast(!s.empty());
	string path = this->GetFullPath(s);
	DWORD fileAttrs = GetFileAttributes(path.c_str());
	return (INVALID_FILE_ATTRIBUTES != fileAttrs && (fileAttrs & FILE_ATTRIBUTE_READONLY));
}

//------------------------------------------------------------------------------
vector<string>
IoSystem::ListFiles( const string& d, const string& pattern ) const
{
	Ast(!d.empty() && !pattern.empty());

	string dir = this->GetFullPath(d);
	string pathWithPattern = dir + "\\" + pattern;

	vector<string> result;
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;
	hFind = FindFirstFile(pathWithPattern.c_str(), &findFileData);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		do
		{
			if (0 == (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				result.push_back(findFileData.cFileName);
			}
		}
		while (FindNextFile(hFind, &findFileData) != 0);
		FindClose(hFind);
	}
	return result;
}

//------------------------------------------------------------------------------
vector<string>
IoSystem::ListDirectories( const string& d, const string& pattern ) const
{
	Ast(!d.empty() && !pattern.empty());

	string dir = this->GetFullPath(d);
	string pathWithPattern = dir + "\\" + pattern;

	vector<string> result;
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;
	hFind = FindFirstFile(pathWithPattern.c_str(), &findFileData);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		do
		{
			if ((0 != (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) &&
			        (0 != strcmp(findFileData.cFileName, "..")) &&
			        (0 != strcmp(findFileData.cFileName, ".")))
			{
				result.push_back(findFileData.cFileName);
			}
		}
		while (FindNextFile(hFind, &findFileData) != 0);
		FindClose(hFind);
	}
	return result;
}

//------------------------------------------------------------------------------
void
IoSystem::RegisterDirMapping( const string& shortName, const string& d, const bool bFull )
{
	Ast(!d.empty() && !shortName.empty());

	string path = d;
	if( bFull ) 
		path = this->GetFullPath(d);

	if (bFull && !endwith(path, '\\'))
		path += '\\';
	
	this->dirMappings[shortName] = path;
}

//------------------------------------------------------------------------------
string
IoSystem::GetFullPath( const string& shortPath ) const
{
	Ast(!shortPath.empty());

	string shortName = shortPath.substr(0, shortPath.find(':'));
	string path = shortPath;

	if (1 != shortName.length() && shortPath.length() != shortName.length())
	{
		// replace short name
		map<string, string>::const_iterator it = this->dirMappings.find(shortName);
		Ast(this->dirMappings.end() != it);
		path.replace(0, shortName.length() + 1, it->second);
	}

	TCHAR buffer[MAX_PATH];
	GetFullPathName(path.c_str(), MAX_PATH, buffer, NULL);
	return buffer;
}

//------------------------------------------------------------------------------
string
IoSystem::GetRegisterDir(const string& shortName) const
{
	map<string, string>::const_iterator it = this->dirMappings.find(shortName);
	Ast(this->dirMappings.end() != it);
	return it->second;
}

//------------------------------------------------------------------------------
void
IoSystem::SetWorkDirectory( const string& dir )
{
	this->RegisterDirMapping("work", dir);
	CPkgFile::AddLoadPath(L"res", gbk_to_utf16(dir).c_str());
	SetCurrentDirectory(dir.c_str());
}
//------------------------------------------------------------------------------
void
IoSystem::SetWorkSrcDirectory( const string& dir )
{
	this->RegisterDirMapping("worksrc", dir);
}

//------------------------------------------------------------------------------
void
IoSystem::SetWorkTgtDirectory( const string& dir )
{
	this->RegisterDirMapping("worktgt", dir);
}
}// namespace 
