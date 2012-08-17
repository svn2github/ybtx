#include "stdafx.h"
#include "FixOverflowPkg.h"

bool CFixOverflowPkg::CFileInfoComp::operator()(const PackFileIndex& Left, const PackFileIndex& Right) const
{
	return (Left.offset<Right.offset) || (Left.offset==Right.offset && (Left.size+Left.space) > (Right.size+Right.space));
}

CFixOverflowPkg::CFixOverflowPkg( const string &pkg_file )
	: m_pFile(0)
	, m_szPkgName(pkg_file)
{
	m_ErrMsg << "package crashed: " << m_szPkgName << "\n";
}

CFixOverflowPkg::~CFixOverflowPkg()
{
	fclose(m_pFile);
	m_pFile = 0;
}

bool CFixOverflowPkg::DoFix()
{
	// 读取包信息, 如果包已经被秀不过则直接推出
	if( !readPkgData(m_szPkgName) )
		return false;

	if( isPkgCrash() )
		if( !fixPkgData() )
			return false;

	return true;
}

inline static uint32 EndOf( CFixOverflowPkg::FileInfoSet::iterator it )
{
	return (*it).offset + (*it).size + (*it).space;
}

bool CFixOverflowPkg::fixPkgData( void )
{
	uint Num = 0;
	uint32 safeOffset = 0;
	FileInfoSet::iterator eit = m_fileList.end();
	FileInfoSet::iterator A   = m_fileList.begin();
	FileInfoSet::iterator B   = ++m_fileList.begin();
	
	for ( ; B != eit; )
	{
		if ( safeOffset < EndOf(A) )
			safeOffset = EndOf(A);

		if ( (*B).offset < safeOffset )
		{
			if ( safeOffset < EndOf(B) )
				safeOffset = EndOf(B);

			(*A).space = safeOffset - (*A).offset - (*A).size;

			++Num;
			B = m_fileList.erase(B);
			continue;
		}

		A = B;
		++B;
	}

	m_FileHeader.number -= Num;
	writeList();
	WritePkgHead(m_pFile, m_FileHeader, m_FileHeader.ver);

	cout << "修复完毕 删除文件数量：" << Num << "\n\n";
	return true;
}

void CFixOverflowPkg::writeList()
{
	assert((size_t)m_FileHeader.number == m_fileList.size());
	fseek(m_pFile, m_FileHeader.offset, SEEK_SET);
	for( FileInfoSet::iterator it = m_fileList.begin(); it != m_fileList.end(); it++ )
	{
		WriteFileIndex(m_pFile, *it, m_FileHeader.ver);
	}
}

bool CFixOverflowPkg::isPkgCrash( void )
{	
	typedef set<uint32> OverlapChecker;
	
	bool			bCrash = false;
	stringstream	errMsg;
	OverlapChecker	checkOverlap;
	
	uint32 safeOffset = 0;
	FileInfoSet::iterator it, eit = m_fileList.end();
	for ( it = m_fileList.begin(); it!=eit; ++it )
	{
		PackFileIndex& curFile = *it;
		pair<OverlapChecker::iterator, bool> ret = checkOverlap.insert(curFile.offset);
		if ( !ret.second )
		{
			bCrash = true;
			outErrFile("文件重叠", curFile);
			continue;
		}

		if( curFile.offset < safeOffset )
		{
			bCrash = true;
			outErrFile("文件错位", curFile);
		}

		safeOffset = curFile.offset + curFile.size + curFile.space;
	}
	m_ErrMsg << "\n";

	if ( bCrash )
		cout << m_ErrMsg.str();
	
	return bCrash;
}

bool CFixOverflowPkg::readPkgData( const string &pkg_file )
{
	m_pFile = fopen( pkg_file.c_str(), "rb+" );
	if( !m_pFile )
	{
		string szErr;
		szErr = szErr + "Can't open " + pkg_file + ".\n";
		throw szErr;
	}

	PackFileIndex headIndex;
	if ( !ReadPackageHeadInfo(m_FileHeader, headIndex, m_pFile) )
	{
		string szErr;
		szErr = pkg_file + "is not a package file.\n";
		throw szErr;
	}

	SeekToFileIndexList(m_pFile, m_FileHeader);
	
	PackFileIndex temp;
	for( int32 i = 0; i < m_FileHeader.number; i++ )
	{
		ReadPackageFileIndex(temp, m_FileHeader, m_pFile);
		m_fileList.push_back(temp);
	}

	m_fileList.sort(CFileInfoComp());
	return true;
}

void CFixOverflowPkg::outErrFile( const string& msg, const PackFileIndex& curIndex )
{
	m_ErrMsg << "\t" << msg;
	m_ErrMsg << hex 
			 << " uid:0x" << curIndex.uid << dec << " off:" << curIndex.offset 
			 << " size:"  << curIndex.size << " space:" << curIndex.space << "\n";
}
