#include "stdafx.h"
#include "Package.h"
#include "PkgCommon.h"

CFixPackage::CFixPackage( const string &pkg_file )
	: m_pFile(0)
	, m_szPkgName(pkg_file)
{
}

CFixPackage::~CFixPackage()
{
	fclose(m_pFile);
	m_pFile = 0;
}

void CFixPackage::DoFix()
{
	if( !readOldData(m_szPkgName) )
		return;

	fixSpaces();
	writeList();
}

void CFixPackage::writeList()
{
	fseek(m_pFile, m_FileHeader.offset, SEEK_SET);
	for( list<PackFileIndex>::iterator it = m_vecOldFiles.begin(); it != m_vecOldFiles.end(); it++ )
		WriteFileIndex(m_pFile, *it, m_FileHeader.ver);
}

bool CFixPackage::readOldData( const string &pkg_file )
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

	PackFileIndex temp;
	SeekToFileIndexList(m_pFile, m_FileHeader);
	m_BegOffsetList.resize(m_FileHeader.number);
	for( int32 i = 0; i < m_FileHeader.number; i++ )
	{
		ReadPackageFileIndex(temp, m_FileHeader, m_pFile);
		m_vecOldFiles.push_back(temp);
		m_BegOffsetList[i] = temp.offset;
	}

	std::sort(m_BegOffsetList.begin(), m_BegOffsetList.end());

	return true;
}

void CFixPackage::fixSpaces()
{
	FileInfoList::iterator it, eit = m_vecOldFiles.end();
	for ( it = m_vecOldFiles.begin(); it != eit; ++it )
	{
		uint32	uEndOffset = it->offset + it->size + it->space;
		uint32	uNearestOffset = findNearstNextOffset(uEndOffset);	
	
		assert(uEndOffset <= uNearestOffset);

		uint32	uFixSpace = uNearestOffset - uEndOffset;
		if ( uFixSpace > 0 )
		{
			stringstream ss;
			ss << m_szPkgName << ": fix " << uFixSpace << " bytes.\n";
			cout << ss.str();
			it->space += uFixSpace;
		}
	}
}

uint32 CFixPackage::findNearstNextOffset(uint32 uNodeEndOffset)
{
	if ( uNodeEndOffset > m_BegOffsetList[m_FileHeader.number-1] )
		return uNodeEndOffset;

	int32 beg = 0;
	int32 end = m_FileHeader.number-1;
	while ( beg < end-1 )
	{
		uint32 mid = (beg + end)/2;
		if ( uNodeEndOffset == m_BegOffsetList[mid] )
		{
			end = mid;
			break;
		}
		if ( uNodeEndOffset < m_BegOffsetList[mid] )
			end = mid;
		else
			beg = mid;
	}

	return m_BegOffsetList[end];
}