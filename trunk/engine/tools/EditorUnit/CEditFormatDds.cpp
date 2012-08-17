#include "stdafx.h"
#include "CEditFormatDds.h"

namespace sqr_tools
{
	CEditFormatDds::CEditFormatDds(const string& base_file, const string& file_name )
		: CEditFormat(base_file, file_name, 'EDDS', 1)
	{

	}

	CEditFormatDds::~CEditFormatDds()
	{

	}
}