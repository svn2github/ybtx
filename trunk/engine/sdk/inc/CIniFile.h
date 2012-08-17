/*
*	与操作系统无关的读写Ini文件的实现
*/

#ifndef _VFS_INIFILE_H_
#define _VFS_INIFILE_H_

#include "CommonDefs.h"
#include <sstream>

namespace sqr
{
//---------------------------------------------------------------------------
class COMMON_API CIniFile
{
private:
	typedef map<string, string> KeyMap;
	typedef map<string, KeyMap> SectionMap;
	wstring fileName;
	SectionMap valueMap;
	bool    Prepare(char* lpBuffer);

public:
	CIniFile();
	~CIniFile();

	// 创建一个新的 .Ini文件，为写(保存)做准备
	bool	Create(const wchar_t* szFileName);

	// 打开文件，扫描一遍，为以后的读操作做好准备(Get...)
	bool	Open(const wchar_t* alias, const wchar_t* szFileName);

	// 将链表中的内容保存到磁盘文件
	bool	Save(void);
	bool	Save(const wstring& FileName);

	// 关闭文件,释放内存
	void	Close();

	// 改为泛型实现, 方便使用 [2/26/2009 xoyojank]
	template <typename TYPE> TYPE GetValue( const string& section, const string& key,
	                                        const TYPE& defaultValue) const;
	template <typename TYPE> void SetValue( const string& section, const string& key,
	                                        const TYPE& value);
};

//------------------------------------------------------------------------------
template <typename TYPE>
void
CIniFile::SetValue( const string& section, const string& key, const TYPE& value )
{
	stringstream ss;
	ss << value;
	this->valueMap[section][key] = ss.str();
}
//------------------------------------------------------------------------------
template <typename TYPE>
TYPE
CIniFile::GetValue( const string& section, const string& key, const TYPE& defaultValue ) const
{
	SectionMap::const_iterator itSection = this->valueMap.find(section);
	if (this->valueMap.end() != itSection)
	{
		KeyMap::const_iterator itKey = itSection->second.find(key);
		if (itSection->second.end() != itKey)
		{
			stringstream ss;
			ss << itKey->second;
			TYPE returnValue;
			ss >> returnValue;
			return returnValue;
		}
	}
	// not exist
	return defaultValue;
}

}

#endif

