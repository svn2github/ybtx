#include "stdafx.h"
#include "CPkgFile.h"
#include "CodeCvs.h"
#include <time.h>
#include "CFlashTmpFile.h"

const wstring PATH_ALIAS_AUD = L"res";

CFlashTmpFile::CFlashTmpFile(wstring fname)
:m_FileName(fname)
{
	m_IsCreateFileFolder=false;

	::srand((int)time(0));
	int randNum=::rand();
	char* randStr=new char[1024];
	itoa(randNum,randStr,10);
	string m_TmpFileName;
	m_TmpFileName="var/";
	m_TmpFileName+=randStr;
	m_TmpFileName+=".tmp";
	utf8_to_utf16(m_wTmpFileName,m_TmpFileName);
	m_wTmpFileName=CPkgFile::GetLoadPath(L"")+m_wTmpFileName;

	m_tmpFolderName=m_wTmpFileName.substr(0,m_wTmpFileName.rfind(L"/"));
	m_IsCreateFileFolder=::CreateDirectoryW(m_tmpFolderName.c_str(),NULL);

	delete[] randStr;
}

CFlashTmpFile::~CFlashTmpFile(void)
{
	Clear();
}

bool CFlashTmpFile::CreateTmpFlash()
{
	string file;
	utf16_to_utf8(file,m_FileName);
	m_PkgFile.Open(PATH_ALIAS_AUD.c_str(),file.c_str());
	int length = m_PkgFile.Size();
	int readLength=0;
	if(length>0)
	{
		int bufSize=1024;
		char* buf=new char[bufSize];
		FILE* fp=_wfopen(m_wTmpFileName.c_str(),L"wb");
		if(fp)
		{
			while(readLength<length)
			{
				memset(buf,0,bufSize);
				int lastLength=length-readLength;
				if(lastLength>bufSize)
					lastLength=bufSize;
				m_PkgFile.read(buf,sizeof(char)*lastLength);
				fwrite(buf,sizeof(char),lastLength,fp);
				readLength+=lastLength;
			}
			fclose(fp);
			::SetFileAttributesW(m_wTmpFileName.c_str(),FILE_ATTRIBUTE_HIDDEN);
		}
		delete[] buf;
		return true;
	}
	return false;
}

wstring	CFlashTmpFile::GetTmpFileName() const
{
	return m_wTmpFileName;
}

void CFlashTmpFile::Clear()
{
	::DeleteFileW(m_wTmpFileName.c_str());
	if(m_IsCreateFileFolder)
		::RemoveDirectoryW(m_tmpFolderName.c_str());
}
