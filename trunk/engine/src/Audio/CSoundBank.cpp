#include "stdafx.h"
#include "CSoundBank.h"
#include "ErrLogHelper.h"
#include "CGraphicExp.h"
#include "TSqrAllocator.inl"

namespace sqr
{

//------------------------------------------------------------------------------
CSoundBank::CSoundBank()
: isValid(false)
, isStreaming(false)
{
	// empty
}

//------------------------------------------------------------------------------
CSoundBank::~CSoundBank()
{
	if(this->IsValid())
	{
		//GfkLogErr("CSoundBank is not destroyed before wave is deleted!");
		GfkLogErrOnce("CSoundBank deleted failed");
	}
}

//------------------------------------------------------------------------------
bool CSoundBank::Setup( const AString& file, bool streaming )
{
	this->filename = file;
	this->isStreaming = streaming;
	this->isValid = true;
	return true;
}

//------------------------------------------------------------------------------
void CSoundBank::Destroy()
{
	if(!this->IsValid())
		return;

	this->filename.clear();
	this->cueProperties.clear();
	this->waveProperties.clear();
	this->cueIndexMap.clear();
	this->waveIndexMap.clear();
	this->isValid = false;
}

//------------------------------------------------------------------------------
AString CSoundBank::GetSoundBankFileName() const
{
	return this->filename;
}

//------------------------------------------------------------------------------
sqr::index_t CSoundBank::FindCueIndexByName( const AString& cueName ) const
{
	AMap<AString, index_t>::const_iterator it = this->cueIndexMap.find(cueName);
	if (this->cueIndexMap.end() != it)
		return it->second;
	else
		return InvalidIndex;
}
}// namespace sqr
