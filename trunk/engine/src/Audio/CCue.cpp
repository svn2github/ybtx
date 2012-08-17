#include "stdafx.h"
#include "CCue.h"
#include "StringHelper.h"
#include "ErrLogHelper.h"
#include "CGraphicExp.h"
#include "TSqrAllocator.inl"

namespace sqr
{

//------------------------------------------------------------------------------
CCue::CCue()
: cueIndex(InvalidIndex)
{
	// 这数学库用着很不爽!
	this->position.Init();
	this->velocity.Init();
}

//------------------------------------------------------------------------------
CCue::~CCue()
{
	this->Destroy();
}

//------------------------------------------------------------------------------
void CCue::Init( const AString& strCueName, index_t index, bool start, bool enable3D )
{
	if(this->IsValid())
	{
		string msg = format("%d", this->cueIndex);
		GfkLogAudioErrOnce("CueSetUpFailed", msg);
	}

//	Ast(InvalidIndex != index);
	this->cueIndex    = index;
	this->strCueName  = strCueName;
	this->is3DEnabled = enable3D;
}

//------------------------------------------------------------------------------
void CCue::Destroy()
{
	if(!this->IsValid())
	{
		///如果失败，有可能是CCue::Init都没有执行到
		string msg = format("%d", this->cueIndex);
		GfkLogAudioErrOnce("CueDestroyFailed", msg);
	}

	this->cueIndex    = InvalidIndex;
	this->strCueName  = "";
}

}// namespace sqr

