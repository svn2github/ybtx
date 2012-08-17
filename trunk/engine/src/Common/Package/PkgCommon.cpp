#include "stdafx.h"
#include "PkgCommon.h"
#include "StringHelper.h"


namespace sqr
{
	const uint8* GetPackKey()
	{
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4309)
#endif
		static const uint8 ls_PackKey[16] =
		{ 0xb7, 0xbd, 0xd7, 0xdb, 0xb9, 0xd8, 0xd1, 0xf7, 0xa1, 0xd2, 0x62, 0xfc, 0x7a, 0x60, 0xca, 0xa4 };
#ifdef _WIN32
#pragma warning(pop)
#endif

		return ls_PackKey;
	}

	// 为了适应将字符串转化为唯一的数字ID，调整字符串
	// 如果字符大写，就转化为小写，将'\\'转化为'/'
	static char* StringAdjust( char* p )
	{
		for( int i = 0; p[i]; i++ ) 
		{
			if ( p[i] >= 'A' && p[i] <= 'Z' ) 
				p[i] += 'a' - 'A';
			else if ( p[i] == '/' ) 
				p[i] = '\\';
		}
		return p;
	}

	unsigned int FileName2Id( const char *filename )
	{
		char buffer[256];
		buffer[255] = 0;
		strncpy( buffer, filename, 255 );
		return (unsigned int)String2ID( StringAdjust(buffer) );
	} 

	static bool gs_bReadPkgOnly = false;
	void EnableReadPkgOnly(bool bEnable)
	{
		gs_bReadPkgOnly = bEnable;
	}

	bool ReadPkgOnlyEnabled()
	{
		return gs_bReadPkgOnly;
	}

	bool ReadPackageHeadInfo( PackageHead& head, PackFileIndex& head_info, FILE* fp )
	{
		uint32 uBaseSize = sizeof(head.id) + sizeof(head.bXor) + sizeof(head.number) + sizeof(head.offset);
		fread( &head, uBaseSize, 1, fp );
		if ( head.id != 'SQRP' )
			return false;

		fseek( fp, head.offset, SEEK_SET );
		fread( &head_info.uid,		4, 1, fp );
		fread( &head_info.offset,	4, 1, fp );
		fread( &head_info.size,		4, 1, fp );
		fread( &head_info.space,	4, 1, fp );

		fseek( fp, 0, SEEK_SET );
		fread( &head, min(sizeof(PackageHead), size_t(head_info.size)), 1, fp );

		return true;
	}

	void SeekToFileIndexList( FILE* fp, const PackageHead& head )
	{
		fseek( fp, head.offset, SEEK_SET );
	}

	void ReadPackageFileIndex( PackFileIndex& file_index, const PackageHead& head, FILE* fp )
	{
		fread( &file_index.uid,		4, 1, fp );
		fread( &file_index.offset,	4, 1, fp );
		fread( &file_index.size,	4, 1, fp );
		fread( &file_index.space,	4, 1, fp );

		switch ( head.ver )
		{
		case PACKAGE_VER_CRC_CHECK:
			fread( &file_index.check_code, 4, 1, fp);
		}
	}

	void WriteFileIndex( FILE* fp, const PackFileIndex& fileIndex, uint32 ver )
	{
		fwrite( &fileIndex.uid,		4, 1, fp );
		fwrite( &fileIndex.offset,	4, 1, fp );
		fwrite( &fileIndex.size,	4, 1, fp );
		fwrite( &fileIndex.space,	4, 1, fp );

		switch ( ver )
		{
		case PACKAGE_VER_CRC_CHECK:
			fwrite( &fileIndex.check_code, 4, 1, fp );
		}
	}

	void WritePkgHead( FILE* fp, PackageHead& head, uint32 ver )
	{
		fseek( fp, 0, SEEK_SET );
		head.ver = ver;

		fwrite(&head.id,		sizeof(head.id),	1, fp);
		fwrite(&head.bXor,		sizeof(head.bXor),	1, fp);
		fwrite(&head.number,	sizeof(head.number),1, fp);
		fwrite(&head.offset,	sizeof(head.offset),1, fp);
		
		switch ( ver )
		{
		case PACKAGE_VER_CRC_CHECK:
			fwrite(&head.ver,	sizeof(head.ver),	1, fp);
		}
	}

}
