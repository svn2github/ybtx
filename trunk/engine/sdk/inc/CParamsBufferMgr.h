#ifndef _CParams_Buffer_Mgr_
#define _CParams_Buffer_Mgr_
#include "TSingleton.h"
#include "CGraphicMallocObject.h"
#define MinBufferSize 1048576 //1MB

namespace sqr
{
//class RenderParamBuffer : public CGraphicMallocObject
//{
//public:
//	size_t		m_Size;
//	size_t		m_Start;
//};

class CParamsBufferMgr 
	: public Singleton<CParamsBufferMgr>
	, public CGraphicMallocObject
{
public:
	CParamsBufferMgr();
	~CParamsBufferMgr();
public:
	void*	getBuffer(size_t start);
	void*	setBuffer(size_t size,size_t& start);
	void*	regBuffer(size_t size,size_t& start/*in&out*/);
	void	reset(void);
	//void	SetImmediate(bool isImmediate);
public:
	//void*	_SetParamBufferI(size_t size,size_t& start);
	//void*	_SetParamBufferC(size_t size,size_t& start);
protected:
	
	//void* (__thiscall CParamsBufferMgr:: *m_fpBufferFn)(size_t size,size_t& start);

	struct MinBuffer
	{
		char data[MinBufferSize];
	};
	size_t	m_start;
	typedef GVector<MinBuffer*> BufferList;
	BufferList	m_BufferList;
};

//CParamsBufferMgr st_BufMgr;
}
#endif