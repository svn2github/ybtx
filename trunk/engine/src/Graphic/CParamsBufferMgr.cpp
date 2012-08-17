#include "stdafx.h"
#include "CParamsBufferMgr.h"
#include "TSqrAllocator.inl"

//----------------------------------------------------------
CParamsBufferMgr::CParamsBufferMgr()
		:m_start(0)
{
	//m_fpBufferFn = &CParamsBufferMgr::_SetParamBufferC;
}

CParamsBufferMgr::~CParamsBufferMgr()
{
	BufferList::iterator  it = m_BufferList.begin();
	BufferList::iterator eit = m_BufferList.end();
	for (;it!=eit;++it)
		delete(*it);
	m_BufferList.clear();
}

void* CParamsBufferMgr::getBuffer(size_t start)
{
	size_t index = start/MinBufferSize;
	size_t si = start%MinBufferSize;
	AstMsg(index<m_BufferList.size(), "非法Buffer参数");
	return &(m_BufferList[index]->data[si]);
}

void*  CParamsBufferMgr::setBuffer(size_t size,size_t& start)
{
	start = size;
	return new char[size];
}

void* CParamsBufferMgr::regBuffer(size_t size,size_t& start)
{
	AstMsg(size<MinBufferSize, "哥们儿！咱申请的空间小点儿行么？");

	size_t index = m_start/MinBufferSize;
	size_t si = m_start%MinBufferSize;
	size_t overplus = MinBufferSize-si;

	if ( overplus<size )
	{
		++index;
		m_start = index*MinBufferSize;
		overplus = MinBufferSize;
		si = 0;
	}

	if (index >= m_BufferList.size())
		m_BufferList.push_back(new MinBuffer);

	start = m_start;
	m_start += size;
	return &(m_BufferList[index]->data[si]);
}

void  CParamsBufferMgr::reset(void)
{
	m_start = 0;
}

//void	CParamsBufferMgr::SetImmediate(bool isImmediate)
//{
//	m_fpBufferFn = isImmediate?&CParamsBufferMgr::_SetParamBufferI:&CParamsBufferMgr::_SetParamBufferC;
//}

//void*  CParamsBufferMgr::_SetParamBufferI(size_t size,size_t& start)
//{
//	RenderParamBuffer rpb;
//	rpb.m_Size = size;
//	void* p = regBuffer(rpb);
//	start = rpb.m_Start;
//	return p;
//}
//void*  CParamsBufferMgr::_SetParamBufferC(size_t size,size_t& start)
//{
//	start = size;
//	return new char[size];
//}


