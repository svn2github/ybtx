#pragma once
#include "CommonDefs.h"

namespace sqr
{
	enum FileError
	{
		eFE_SUCCESS	= 0,
		eFE_PKG_OPEN_FAILED,
		eFE_PKG_DAMAGED,
		eFE_PKG_SUB_FILE_OPEN_FAILED,
		eFE_NOT_A_PACKAGE,
		eFE_RAW_FILE_CANNOT_BE_ACCESS,
		eFE_RAW_FILE_NAME_OR_PATH_NOT_FOUND,
		eFE_RAW_FILE_OPEN_INVALID_PARAM,
	};

	enum
	{
		PACKAGE_VER_BASE,
		PACKAGE_VER_CRC_CHECK,
	};
	#define PACKAGE_CUR_VER PACKAGE_VER_CRC_CHECK


	struct PackFileIndex
	{
		PackFileIndex()
			: uid(0)
			, offset(0)
			, size(0)
			, space(0)
			, check_code(0)
		{
		}

		uint32		uid;
		uint		offset;
		uint		size;
		uint		space;
		uint32		check_code;
	};

	struct PackageHead
	{
		PackageHead()
			: id('SQRP')
			, bXor(0)
			, number(0)
			, offset(0)
			, ver(0)
		{
		}

		uint		id;
		uint		bXor;
		int			number;
		uint		offset;
		uint32		ver;
	};

	COMMON_API const uint8* GetPackKey();
	COMMON_API unsigned int FileName2Id( const char *filename );

	COMMON_API void EnableReadPkgOnly(bool bEnabled);
	COMMON_API bool ReadPkgOnlyEnabled();

	bool	ReadPackageHeadInfo( PackageHead& head, PackFileIndex& head_info, FILE* fp );
	void	SeekToFileIndexList( FILE* fp, const PackageHead& head );
	void	ReadPackageFileIndex( PackFileIndex& file_index, const PackageHead& head, FILE* fp );
	void	WriteFileIndex( FILE* fp, const PackFileIndex& fileIndex, uint32 ver );
	void	WritePkgHead( FILE* fp, PackageHead& head, uint32 ver );
}

