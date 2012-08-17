#include "stdafx.h"
#include "CFastObjCmd.h"


CFastObjCmd::CFastObjCmd(const void* pData,uint32 uSize)
:m_sData(static_cast<const char*>(pData),uSize)
{

}

CFastObjCmd::~CFastObjCmd(void)
{
}