#include "stdafx.h"
#include "Package.h"
#include "CodeCvs.h"
#include "DebugHelper.h"

enum
{
	OPEN_PKG_SUCCESSED = 0,
};



wstringstream g_VeryResult;

inline static void BufferXor(char* buffer, uint32 uPosition, size_t uBytes)
{
	const uint8* szPackKey=GetPackKey();

	for( unsigned i = 0; i < uBytes; i++ )
		buffer[i] = buffer[i]^szPackKey[( uPosition + i )&0xf];
}

bool CVerifyPackage::CFileInfoComp::operator()(const PackFileIndex& Left, const PackFileIndex& Right) const
{
	return (Left.offset<Right.offset) || (Left.offset==Right.offset && (Left.size+Left.space) > (Right.size+Right.space));
}

CVerifyPackage::CVerifyPackage( const wstring &pkg_file )
	: m_pFile(0)
	, m_szPkgName(pkg_file)
{
	memset(&m_FileHeader, 0, sizeof(CFileHeader));
}

CVerifyPackage::~CVerifyPackage()
{
	fclose(m_pFile);
	m_pFile = 0;
}

bool CVerifyPackage::IsVerify(wstring& szErrMsg)
{
	if( !readPkgData(m_szPkgName) )
		goto VERY_FAILED;

	if( checkOverFlow() )
		goto VERY_FAILED;

	if ( checkCrcCodeWrong() )
		goto VERY_FAILED;

	return true;

VERY_FAILED:
	szErrMsg = m_ErrMsg.str();
	return false;
}


bool CVerifyPackage::readPkgData( const wstring &pkg_file )
{
	if( _wfopen_s(&m_pFile, pkg_file.c_str(), L"rb+") != OPEN_PKG_SUCCESSED )
	{
		m_ErrMsg << L"open package file failed.\n";
		return false;
	}

	fseek( m_pFile, m_FileHeader.offset, SEEK_SET );
	PackFileIndex headIndex;
	if ( !ReadPackageHeadInfo(m_FileHeader, headIndex, m_pFile) )
	{
		m_ErrMsg << L"is not a package file.\n";
		return false;
	}

	PackFileIndex curFileIndex;
	SeekToFileIndexList(m_pFile, m_FileHeader);
	for( int32 i = 0; i < m_FileHeader.number; i++ )
	{
		if ( i==0 && curFileIndex.uid !=0 )
		{
			m_ErrMsg << L" 包格式不正确\n";
			return false;
		}

		ReadPackageFileIndex(curFileIndex, m_FileHeader, m_pFile);
		m_FileList.push_back(curFileIndex);
	}

	m_FileList.sort(CFileInfoComp());

	return true;
}

bool CVerifyPackage::checkOverFlow( void )
{
	FileInfoList::iterator it,eit = m_FileList.end();
	uint32 safeOffset = 0;

	typedef set<uint32> OverlapChecker;
	OverlapChecker overlapChecker;

	bool bPkgCrashed = false;
	for( it = m_FileList.begin(); it!=eit; ++it )
	{
		pair<OverlapChecker::iterator, bool> checker = overlapChecker.insert((*it).offset);
		if ( !checker.second )
		{
			addErrMsg(L"文件重叠", *it);
			bPkgCrashed  = true;
		}
		else if( (*it).offset < safeOffset )
		{
			addErrMsg(L"文件错位", *it);
			bPkgCrashed	 = true;
		}

		if ( safeOffset < (*it).offset + (*it).size + (*it).space )
			safeOffset = (*it).offset + (*it).size + (*it).space;
	}

	return bPkgCrashed;
}

bool CVerifyPackage::checkCrcCodeWrong()
{
	if ( m_FileHeader.ver < PACKAGE_VER_CRC_CHECK )
		return false;

	vector<char> fileBuf;
	fileBuf.resize(10*1024*1024);
	FileInfoList::iterator it, eit = m_FileList.end();
	for ( it = ++m_FileList.begin(); it != eit; ++it )
	{
		uint32 crc_file	= 0;
		uint32 crc_org	= it->check_code;
		uint32 size		= it->size;
		if ( size > fileBuf.size() )
			fileBuf.resize(size);
		
		fseek(m_pFile, it->offset, SEEK_SET);
		fread_s(&fileBuf[0], fileBuf.size(), size, 1, m_pFile);
		
		if ( m_FileHeader.bXor )
			BufferXor(&fileBuf[0], 0, size);
		
		CCrc32::GetInst()->InitCrc32(crc_file);
		CCrc32::GetInst()->MakeCrc32Sub(crc_file, (const uint8*)&fileBuf[0], size);
		CCrc32::GetInst()->MakeCrc32End(crc_file);

		if ( crc_file != crc_org )
		{
			m_ErrMsg << L"crc码验证失败，origin code: 0x" << hex << crc_org
				     << L" current code: 0x" << hex << crc_file << L"\n";
			return true;
		}
	}

	return false;
}

void CVerifyPackage::addErrMsg( const wstring szMsg, const PackFileIndex& fileIndex )
{
	m_ErrMsg << "\t" << szMsg
			 << L" uid: 0x"  << hex << fileIndex.uid  
			 << L" offset: " << dec << fileIndex.offset 
			 << L" size: "   << fileIndex.size	     
			 << L" space: "  << fileIndex.space << L"\n"; 
}