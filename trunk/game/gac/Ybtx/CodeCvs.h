/*
* 启动器用到的字符串处理函数
*/
#ifndef _CODECVS_H
#define _CODECVS_H

#include <string>
#include <winnls.h>
using namespace std;

// utf16格式的字符串转换为utf8格式
string		utf16_to_utf8( const wstring& strSrc );
// utf8格式的字符串转换为utf16格式
wstring		utf8_to_utf16( const string& strSrc );
// gbk格式的字符串转换为utf16格式
wstring		gbk_to_utf16( const string& strSrc );
// strBig中的所有strSrc子串用strDst代替
void		string_replace( wstring& strBig, const wstring& strSrc, const wstring& strDst );
// 获得配置表字符串参数
wstring		GetSetting( const wchar_t* file, wchar_t* strName, wchar_t* strDefault, wchar_t* dst_path );
// 设置当前目录
void		FixCurrentDir( wchar_t* CurPath );
// 删除路径为szFilePath的文件
bool		TryDeleteFile( const wchar_t* szFilePath, DWORD& dwResult );

#endif
