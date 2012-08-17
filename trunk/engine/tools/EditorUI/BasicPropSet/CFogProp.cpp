#include "StdAfx.h"
#include "CFogProp.h"

CFogProp::CFogProp(void)
{
}

//区域
System::Int32^  CFogProp::Region::get()
{
	return m_Region;
}
void CFogProp::Region::set(System::Int32^ value)
{
	m_Region=value;
}

//类型
CFogProp::FOGMODE  CFogProp::FogType::get()
{
	return m_FogType;
}
void CFogProp::FogType::set(FOGMODE value)
{
	m_FogType=value;
}

//颜色
System::Drawing::Color  CFogProp::FogColor::get()
{
	return m_FogColor;
}
void CFogProp::FogColor::set(System::Drawing::Color value)
{
	m_FogColor=value;
}

//开始
System::Single^  CFogProp::Begin::get()
{
	return m_Begin;
}
void CFogProp::Begin::set(System::Single^ value)
{
	m_Begin=value;
}

//结束
System::Single^  CFogProp::End::get()
{
	return m_End;
}
void CFogProp::End::set(System::Single^ value)
{
	m_End=value;
}

//密度
System::Single^  CFogProp::Density::get()
{
	return m_Density;
}
void CFogProp::Density::set(System::Single^ value)
{
	m_Density=value;
}
