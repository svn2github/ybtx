#include "stdafx.h"
#include "GUIXmlResSaver.h"

GUIXmlResSaverClass::GUIXmlResSaverClass()
: m_ResSaver(NULL)
{
}

GUIXmlResSaverClass::~GUIXmlResSaverClass()
{
}

void GUIXmlResSaverClass::AddResData(Res_type rt, string& str)
{
	if (m_ResSaver)
		m_ResSaver->AddXmlResData(rt, str);
}

void GUIXmlResSaverClass::DelResData(Res_type rt, string& str)
{
	if (m_ResSaver)
		m_ResSaver->DelXmlResData(rt, str);
}

void GUIXmlResSaverClass::ClearResData(Res_type rt)
{
	if (m_ResSaver)
		m_ResSaver->ClearXmlResData(rt);
}

void GUIXmlResSaverClass::SetXmlResSaver(GUIXmlResSaver* saver)
{
	m_ResSaver = saver;
}

void GUIXmlResSaverClass::SaveResData()
{
	if (m_ResSaver)
		m_ResSaver->Save();
}