#pragma once

#include "PropCtrl\CBasePropCtrlImp.h"

namespace sqr_tools
{

class CModelPropCtrlImp :public CBasePropCtrlImp
{
public:
	CModelPropCtrlImp(void);
	~CModelPropCtrlImp(void);

public:
	//获取选中模型的属性
	virtual std::string GetSelModelName();
	virtual float GetSelModelPosX();
	virtual float GetSelModelPosY();
	virtual float GetSelModelPosZ();
	virtual float GetSelModelRotateX();
	virtual float GetSelModelRotateY();
	virtual float GetSelModelRotateZ();
	virtual float GetSelModelScaleX();
	virtual float GetSelModelScaleY();
	virtual float GetSelModelScaleZ();
	//设置选中模型的属性
	virtual void SetSelModelName(std::string name);
	virtual void SetSelModelPos(int px,int py,int pz);
	virtual void SetSelModelScale(int sx,int sy,int sz);
	virtual void SetSelModelRotate(float rx,float ry,float rz);
};

//!namespace
}
