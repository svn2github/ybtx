#include "stdafx.h"
#include "CTexRefCoder.h"
#include "CCoderNameHelper.h"

namespace sqr
{
	FourCC CTexRefCoder::GetChunkName()
	{
		return CCoderNameHelper::TEX_REF_CODER;
	}
}
