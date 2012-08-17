#pragma once
#include "stdafx.h"
#include "SQRGuiMallocObject.h"

enum Res_type
{
	eRes_T_GUI = 0,
	eRes_T_Style,
	eRes_T_Flash,
	eRes_T_Emt,
	eRes_T_TexFt,
	eRes_T_Pro,
	eRes_T_count
};

// GUI资源保存器
class GUIXmlResSaver : public SQRGuiMallocObject
{
public:
	GUIXmlResSaver(){};
	~GUIXmlResSaver(){};

	virtual void AddXmlResData(Res_type rt, string& str) = 0;
	virtual void DelXmlResData(Res_type rt, string& str) = 0;
	virtual void ClearXmlResData(Res_type rt) = 0;
	virtual	void Save() = 0;
};


// 带GUI资源保存功能的类
class GUIXmlResSaverClass
{
public:
	GUIXmlResSaverClass();
	~GUIXmlResSaverClass();

	void	AddResData(Res_type rt, string& str);
	void	DelResData(Res_type rt, string& str);
	void	ClearResData(Res_type rt);
	void	SetXmlResSaver(GUIXmlResSaver* saver);
	void	SaveResData();
protected:
	GUIXmlResSaver*	m_ResSaver;
};