#pragma once

#include "TraceLog.h"

namespace sqr
{
	struct FileHead;
}

namespace sqr
{

class IDiff
{
public:
	virtual bool ExecuteModify(FileHead* pPatchHead)=0;
};

}


