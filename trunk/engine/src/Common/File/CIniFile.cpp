//---------------------------------------------------------------------------
// Comment      : 与语言无关的读写Ini文件的实现
//              : 顺序读写可提高速度.
//              : 对于同一个实例，读写不可穿插进行
//---------------------------------------------------------------------------
// NOTE         : Use std libc function str*** instead of ms ext function 
//                lstr*** for platform compatibility reason.
//---------------------------------------------------------------------------
#include "stdafx.h"
#include "CIniFile.h"
#include "CPkgFile.h"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "CodeCvs.h"
//---------------------------------------------------------------------------

#ifdef _WIN32
#pragma warning(disable:4127)
#endif

static bool			IsBlank(char    cChar);
static void			NextLine(char*& lpBuffer);
static string		BuildSection(char*& lpBuffer);
static bool			BuildKey(char*& lpBuffer, map<string, string>& Section);



CIniFile::CIniFile()
{	
}

CIniFile::~CIniFile()
{
	Close();
}

// 创建一个新的 .Ini文件，为写(保存)做准备
bool CIniFile::Create(const wchar_t* szFileName)
{
	Ast( NULL != szFileName);
	// 如果链表不为空，清空链表.
	Close();

	this->fileName = szFileName;
	return true;
}

// 打开文件，扫描一遍，为以后的读操作做好准备(Get...)
bool CIniFile::Open(const wchar_t* alias, const wchar_t* szFileName)
{
	CPkgFile	File;
	char*	    pFileBuff;
	int32		nSize;
	bool        IsSuccess;

	if( !szFileName || !szFileName[0] )
		return false;

	// comp file name  strcmp
	if( this->fileName == szFileName )
		return true;

	// 先释放
	Close();
	// open Ini file  , 在内部形成全路径
	if (File.Open(alias, utf16_to_utf8(szFileName).c_str(), true, eRO_File) != eFE_SUCCESS)
		return false;

	// get file size
	if ((nSize = File.Size()) == 0)
		return false;

	// alloc memory
	pFileBuff = new char[nSize+1];

	// read data from file
	if (File.read(pFileBuff, nSize) == false)
	{
		SafeDelAry(pFileBuff);
		return false;
	}

	pFileBuff[nSize] = 0;

	// close the file
	File.Close();

	// save file name
	this->fileName = szFileName;

	// 建立内部链表
	IsSuccess = Prepare(pFileBuff);
	if( !IsSuccess )
		Close();

	SafeDelAry(pFileBuff);

	return IsSuccess;
}

/*
*	建立内部链表
*/
bool CIniFile::Prepare(char* lpBuffer)
{
	char*   lpCursor = lpBuffer;
	char    cCursor = *lpCursor;

	string sSection;
	while( 0 != cCursor)
	{
		if( IsBlank(cCursor))
		{
			lpBuffer++;
			lpCursor = lpBuffer;
			cCursor = *lpCursor;
			continue;
		}
		if( '/' == cCursor)
		{
			NextLine(lpBuffer);
			lpCursor    = lpBuffer;
			cCursor     = *lpCursor;
			continue;
		}
		if( '[' == cCursor )
		{
			lpBuffer++;
			sSection = BuildSection(lpBuffer);
			if( sSection.empty() )
				return false;

			// 这句Release Ybtx.exe竟然报错? [2/27/2009 xoyojank]
			//this->valueMap.insert(make_pair(sSection, KeyMap()));
			lpCursor    = ++lpBuffer;
			cCursor     = *lpCursor;
			continue;
		}

		//--------read key and value-----------------
		if( !BuildKey(lpBuffer, this->valueMap[sSection]))
			return false;
		lpCursor    = lpBuffer;
		cCursor     = *lpCursor;
		//-------------------------------------------

	}

	return true;  
}

// 按顺序将链表中的内容保存到磁盘文件
bool CIniFile::Save()
{	
	wstring szSavePath = CPkgFile::GetLoadPath(L"");
	return Save(szSavePath + this->fileName);
}

bool CIniFile::Save(const wstring& FileName)
{
#ifdef _WIN32 
	FILE* pFile = _wfopen( FileName.c_str(), L"wb" );
#else
	FILE* pFile = fopen( utf16_to_utf8(FileName).c_str(), "wb" );
#endif

	if( !pFile )
		return false;

	for( SectionMap::iterator itSection = this->valueMap.begin();
		itSection != this->valueMap.end(); ++itSection )
	{
		string sSection = "[" + itSection->first + "]\r\n";
		fwrite((void*)sSection.c_str(), sSection.size(), 1, pFile );
		for( KeyMap::iterator itKey = itSection->second.begin();
			itKey != itSection->second.end(); itKey++ )
		{
			string sKey = itKey->first + " = " + itKey->second + "\r\n";
			fwrite((void*)sKey.c_str(), sKey.size(), 1, pFile );
		}
	}
	fclose( pFile );
	return true;
}

// 关闭文件,释放内存
void CIniFile::Close()
{
	this->fileName.clear();
	this->valueMap.clear();
}

// Comment      :   忽略 空格(0x20)，tab(0x09), 回车(0x0d)，换行符(0x0a)
//              :   可以做一些优化，如传入指针，一次做多个处理，以减少函数调用
bool IsBlank(char    cChar)
{
	if( 0x20 == cChar )
		return true;
	if( 0x09 == cChar )
		return true;
	if( 0x0d == cChar )
		return true;
	if( 0x0a == cChar )
		return true;
	return false;
}

//---------------------------------------------------------------------------
// Function     : 把Buffer指针移到下一行, 比如处理注释时需要.
// Comment      : 传入的Buffer指针是一个引用，内部修改后可影响外部.
//              : 当碰到Buffer结尾时（0），返回的lpBuffer 指向 0
//              : 当碰到行结束时
//---------------------------------------------------------------------------
void NextLine(char*& lpBuffer)
{
	Ast(NULL != lpBuffer);

	char    cChar = *lpBuffer;

	while( 0 != cChar )
	{
		if( 0x0d == cChar )
		{
			lpBuffer += 2;
			return;
		}
		cChar = *++lpBuffer;
	}
}

//---------------------------------------------------------------------------
// Function     :  读取Section的名字，传给AddtoSectionList
//---------------------------------------------------------------------------
string BuildSection(char*& lpBuffer)
{
	Ast(NULL != lpBuffer);

	char*   lpCursor = lpBuffer;
	char    cChar = *lpBuffer;

	while( 1 )
	{
		if( 0 == cChar )
			return "";
		if( ']' == cChar)
		{
			*lpBuffer = 0;
			string sSection = lpCursor;
			*lpBuffer = ']';
			return sSection;
		}
		lpBuffer++;
		cChar = *lpBuffer;
	}
}

// 形成KeyName, 及KeyValue
bool BuildKey(char*& lpBuffer, map<string, string>& sectionMap )
{
	Ast(NULL != lpBuffer);

	char    cChar = *lpBuffer;
	char*   lpCursor = lpBuffer;
	string  sKey;
	string  sValue;
	size_t     nLen;

	// Read Key
	while( 1 )
	{
		if( '=' == cChar)
		{
			*lpBuffer = 0;
			nLen = strlen( lpCursor );
			string sTemp = lpCursor;
			//modified by weck to eat space .
			for( size_t i = sTemp.size() - 1; i > 0 && IsBlank( sTemp[i] ); i-- )
				sTemp[i] = 0;

			sKey = sTemp.c_str();

			*lpBuffer = '=';
			//modified by weck to eat space .
			while ( *lpBuffer == '\t' || *lpBuffer == ' ' || *lpBuffer == '=')
			{
				lpBuffer++;
			}
			lpCursor = lpBuffer;
			cChar = *lpBuffer;
			break;
		}
		if( 0 == cChar )
			return false;
		// modified by weck
		// just linebreak or return indicate thtat the format is incorrect.
		// some people like to add space between key and '=' and value.
		if ( cChar == 0x0d || cChar ==0x0a )
			return false;
		/*
		if( IsBlank(cChar))
		return false;
		*/
		lpBuffer++;
		cChar = *lpBuffer;
	}

	// Read Value
	int Len = 0;
	while( 1 )
	{
		if( ( 0 == cChar ) || (0x0d == cChar) || (0x0a == cChar))
		{
			if(Len==0)
				return false;
			// 回退到非空格
			while(1)
			{
				lpBuffer--;
				if( !IsBlank(*lpBuffer))
					break;
			}
			cChar = *++lpBuffer;

			*lpBuffer = 0;
			sValue = lpCursor;
			*lpBuffer = cChar;
			break;
		}
		++Len;
		lpBuffer++;
		cChar = *lpBuffer;
	}

	sectionMap.insert(make_pair(sKey, sValue));

	return true;
}

