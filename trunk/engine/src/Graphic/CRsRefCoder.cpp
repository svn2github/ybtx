#include "stdafx.h"
#include "CRsRefCoder.h"
#include "CCoderNameHelper.h"

namespace sqr
{
	FourCC CRsRefCoder::GetChunkName()
	{
		return CCoderNameHelper::RS_REF_CODER;
	}
}