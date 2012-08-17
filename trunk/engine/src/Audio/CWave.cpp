#include "stdafx.h"
#include "CWave.h"
#include "CSoundBank.h"
#include "ErrLogHelper.h"
#include "StringHelper.h"
#include "TSqrAllocator.inl"

namespace sqr
{

//------------------------------------------------------------------------------
CWave::CWave()
: waveIndex(InvalidIndex)
{
	// empty
}

//------------------------------------------------------------------------------
CWave::~CWave()
{
	if(this->IsValid())
	{
		string msg = format("%d", this->waveIndex);
		GenErr("Wave deleted failed", msg);
	}
}

//------------------------------------------------------------------------------
void CWave::Setup( const CSoundBankPtr& sb, index_t index, bool start, size_t loopCount )
{
	if(this->IsValid())
	{
		string msg = format("%d", this->waveIndex);
		GenErr("Wave has been setup", msg);
	}

	Ast(sb.isvalid());
	Ast(InvalidIndex != index);
	this->soundBank = sb;
	this->waveIndex = index;
}

//------------------------------------------------------------------------------
void CWave::Destroy()
{
	if(!this->IsValid())
	{
		string msg = format("%d", this->waveIndex);
		GenErr("Wave destroyed failed", msg);
		return;
	}

	this->soundBank = NULL;
	this->waveIndex = InvalidIndex;
}

//------------------------------------------------------------------------------
const CWaveProperty& CWave::GetWaveProperty() const
{
	if(!this->IsValid())
	{
		string msg = format("%d", this->waveIndex);
		GenErr("Wave get propert failed", msg);
	}

	return this->soundBank->GetWaveProperty(this->waveIndex);
}

}// namespace sqr
