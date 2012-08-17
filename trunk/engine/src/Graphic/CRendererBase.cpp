#include "stdafx.h"
#include "CRendererBase.h"
#include "CWindowTarget.h"

namespace sqr
{

//------------------------------------------------------------------------------
CRendererBase::CRendererBase()
{
	/// CRendererBase 不一定要绑到CMainWindowTarget上
	//if(CMainWindowTarget::HasInst())
	//	CMainWindowTarget::GetInst()->InsertRenderer(this);
}

//------------------------------------------------------------------------------
CRendererBase::~CRendererBase()
{
	//if(CMainWindowTarget::HasInst())
	//	CMainWindowTarget::GetInst()->DeleteRenderer(this);
}


}//namespace sqr
