#include "stdafx.h"
#include "CGuiProcessor.h"
#include "CCoderNameHelper.h"
#include "CGuiCoder.h"
#include "TSqrAllocator.inl"

CGuiProcessor::CGuiProcessor(void)
: CCoderProcessor(CCoderNameHelper::GUI_CODER, 0)
{
	RegProcessTrunk(CCoderNameHelper::TEX_REF_CODER);
}

CGuiProcessor::~CGuiProcessor(void)
{
}
