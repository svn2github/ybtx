
#include "stdafx.h"

#include <io.h>
#include <direct.h>

#include "CPatcher_Imp.h"
#include "bzip2/bzlib.h"
#include "CExp.h"
#include "IAsyncUpdateHandler.h"
#include "Patcher.h"



//static void string_replace(string & strBig, const string & strsrc, const string &strdst)
//{
//	string::size_type pos=0;
//	string::size_type srclen=strsrc.size();
//	string::size_type dstlen=strdst.size();
//	while( (pos=strBig.find(strsrc, pos)) != string::npos)
//	{
//		strBig.replace(pos, srclen, strdst);
//		pos += dstlen;
//	}
//}
//
//static int CheckLong(char* x, const char* y)
//{
//	int i = 0;
//	for (i=0; i<4; ++i)
//	{
//		if (*x != *y)
//			return 0;
//
//		++x;
//		++y;
//	}
//
//	return 1;
//}

CPatcher_Imp::CPatcher_Imp()
{
}

CPatcher_Imp::~CPatcher_Imp()
{
}

int CPatcher_Imp::YbtxPatchExecute(IAsyncUpdateHandler* pAsyncUpdateHandler, const char *PatchFile)
{
	Patcher patch;
	patch.CheckHeadInfo(PatchFile);

	return 0;
}

