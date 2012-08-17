#pragma once
#include "ExpHelper.h"
#include "CGraphicExp.h"

namespace sqr_tools
{

#define EDITOR_TRY { CJmpBuf jmpBuf; \
	if(!setjmp(*(jmpBuf.GetBuf()))) \
	{  \
	try{


#define EDITOR_CATCH(sqr_exp)\
	}\
	catch(std::exception& exp) \
	{\
		GenErr(exp.what()); \
	}\
	catch(...) \
	{\
		GenErr("WIN APP ERR"); \
	}\
	jmpBuf.Reset(); } \
else \
	try {\
		jmpBuf.Reset(); \
		CError error((CJmpStack::Inst().GetError())); \
		SQR_CATCH_APPEND_ERROR_INFO(error);\
		throw(error);\
	}\
	catch(CError& sqr_exp)


#define EDITOR_TRY_END }

#define EDITOR_THROW CJmpStack::Inst().LongJmp
}