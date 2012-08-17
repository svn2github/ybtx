
#include "StdAfx.h"
#include "cbaseclass.h"


BEGIN_CLASS(CBaseClass)
END_CLASS_NOPARENT(CBaseClass)

CBaseClass::CBaseClass(void)
{
}

CBaseClass::~CBaseClass(void)
{
}

// 对象创建的准备，需要游戏告诉Engine一些信息，填充ObjectCreateStruct
void CBaseClass::OnPrecreate(ObjectCreateStruct& ocs, float fCreateFlag)
{

}

// 是否选中的通告
void CBaseClass::OnSelected(bool bSelected)
{

}