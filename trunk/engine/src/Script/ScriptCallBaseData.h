#pragma once
#include "CScriptAllocator.h"

namespace sqr
{

	//保存参数类型信息的结构
	struct CallBaseData : public CScriptMallocObject
	{
		CallBaseData() : m_pResult(NULL), m_uBufSize(0), m_nReturnNum(0), m_bHasVariant(false), m_bHasArgList(false), m_bRetArgList(false){}

		typedef vector<CBaseType*, CScriptAllocator<CBaseType*> >	VecType_t;

		VecType_t			m_vecParam;
		CBaseType*			m_pResult;
		size_t				m_uBufSize;
		int32				m_nReturnNum;
		bool                m_bHasVariant;     //参数列表中是否存在CVariant类型
		bool                m_bHasArgList;     //参数列表中是否存在可变参数
		bool                m_bRetArgList;     //返回值是否为可变参数 不能用于callback
	};

}

