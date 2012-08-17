#ifndef		_IBASECLASS_H_
#define		_IBASECLASS_H_

struct ObjectCreateStruct;

class IBaseClass
{
public:
	// 对象创建的准备，需要游戏告诉Engine一些信息，填充ObjectCreateStruct
	virtual void OnPrecreate(ObjectCreateStruct& ocs, float fCreateFlag) = 0;
	// 是否选中的通告
	virtual void OnSelected(bool bSelected) = 0;
};
#endif