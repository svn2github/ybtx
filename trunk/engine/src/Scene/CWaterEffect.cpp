//------------------------------------------------------------------------------
//  CWaterEffect.cpp
//  (C) 2008 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "CWaterEffect.h"
#include "CGpuProgram.h"
#include "CGpuProgramParameters.h"
#include "CGpuProgramMgr.h"
#include "CRenderPipeline.h"
#include <MMSystem.h>

namespace sqr
{
	static inline uint GetWaterVSID( uint64 id )
	{
		return (uint)(id >> 32 & 0xffffffff);
	}

	static inline uint GetWaterPSID( uint64 id )
	{
		return (uint)(id & 0xffffffff);
	}

//------------------------------------------------------------------------------
CWaterEffect::CWaterEffect()
		: absoluteTime(0)
		, lastFrameTime(0)
{
	// empty
}

//------------------------------------------------------------------------------
CWaterEffect::~CWaterEffect()
{
	// empty
}

//------------------------------------------------------------------------------
void
CWaterEffect::SetEffectID(SHADER_HANDLE id)
{
	this->effectId = id;
}

//------------------------------------------------------------------------------
void
CWaterEffect::OnFrameUpdate()
{
	this->lastFrameTime = this->absoluteTime;
	this->absoluteTime = timeGetTime();
}

//------------------------------------------------------------------------------
void
CWaterEffect::OnRenderBegin()
{
	//È¥µôÒ»¸öD3D Error
	CRenderPipeline::GetInst()->_SetTextureStageState( 0, TSS_TEXCOORDINDEX,  0 );
	CRenderPipeline::GetInst()->_SetTextureStageState( 1, TSS_TEXCOORDINDEX,  1 );
	CRenderPipeline::GetInst()->_SetTextureStageState( 2, TSS_TEXCOORDINDEX,  2 );
	CRenderPipeline::GetInst()->_SetTextureStageState( 3, TSS_TEXCOORDINDEX,  3 );
}

//------------------------------------------------------------------------------
void
CWaterEffect::OnRenderEnd()
{
}

}// namespace sqr
