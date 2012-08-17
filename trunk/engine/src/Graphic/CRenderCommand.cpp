#include "stdafx.h"
#include "CRenderCommand.h"
#include "CRenderPipeline.h"

void sqr::NullRender(CRenderCommand& Com)
{
}

void sqr::NullRelease(CRenderCommand& Com)
{
}

void sqr::NullUpdate(CRenderCommand& Com,CRenderMatrix& AniInst)
{
	CRenderPipeline::GetInst()->SetTextureMatrixImmediate(0,NULL);
}

CRenderCommand::CRenderCommand()
: param_ptr(NULL)
, param_size(0)
, param_fun(NullUpdate)
, param_render(NullRender)
, param_release(NullRelease)
{
	Create();
}

CRenderCommand::CRenderCommand(const CRenderCommand& Com)
: param_ptr(Com.param_ptr)
, param_size(Com.param_size)
, param_fun(Com.param_fun)
, param_render(Com.param_render)
, param_release(Com.param_release)
{

	Com.Clone(*this);
}

CRenderCommand::~CRenderCommand()
{
	Destroy();
}

CRenderCommand&		CRenderCommand::operator=(const CRenderCommand& Com)
{
	CRenderOperation::operator = (Com);
	param_ptr	= Com.param_ptr;
	param_size	= Com.param_size;
	param_fun	= Com.param_fun;
	param_render	= Com.param_render;
	param_release	= Com.param_release;
	return *this;
}

CRenderOperation&	CRenderCommand::operator = (const CRenderOperation& op)
{
	CRenderOperation::operator = (op);
	return *this;
}