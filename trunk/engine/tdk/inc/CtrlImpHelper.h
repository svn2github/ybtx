#pragma once
#include "CCtrlBase.h"

namespace sqr_tools
{
	template <class T>
	T* CtrlImpPtr(void* pCtrl)
	{
		Ast(((CCtrlBase*)pCtrl)->GetTypeName() == T::st_TypeName && "CtrlImp类型转换不正确");
		T* CtrlImp = static_cast<T*>(pCtrl);
		return CtrlImp;
	}
}