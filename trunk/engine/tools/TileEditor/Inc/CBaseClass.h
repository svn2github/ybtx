#ifndef _CBASECLASS_H_
#define _CBASECLASS_H_

#include "IBaseClass.h"
struct ObjectCreateStruct;

class CBaseClass : IBaseClass
{
public:
	CBaseClass(void);
	~CBaseClass(void);

	// 对象创建的准备，需要游戏告诉Engine一些信息，填充ObjectCreateStruct
	void OnPrecreate(ObjectCreateStruct& ocs, float fCreateFlag);
	// 是否选中的通告
	void OnSelected(bool bSelected);
	// 发送网络消息
//	void OnSend(CMessage*	pMsg);
	// 接受网络消息
//	void OnReceive(CMessage*	pMsg);

	void SetClassIdx(uint32 nClassIdx)
	{
		m_nClassIdx = nClassIdx;
	}

	void SetObjectIdx(uint32 nObjectIdx)
	{
		m_nObjectIdx = nObjectIdx;
	}

	uint32 GetObjectIdx()
	{ 
		return m_nObjectIdx;
	}
	
	uint32 GetClassIdx()
	{ 
		return m_nClassIdx;
	}

protected:
	// 这个对象属于哪一个类，可以从类管理器中得到相关类的数据
	uint32	m_nClassIdx;

	// 对象自身在Cache中的索引
	uint32	m_nObjectIdx;

};

#endif