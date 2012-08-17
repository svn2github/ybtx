#include "stdafx.h"
#include "TestProp.h"

TestProp::TestProp(void)
{
}

//区域
System::Int32^  TestProp::Region::get()
{
	return m_Region;
}
void TestProp::Region::set(System::Int32^ value)
{
}

//颜色
System::Drawing::Color  TestProp::FogColor::get()
{
	return m_FogColor;
}
void TestProp::FogColor::set(System::Drawing::Color value)
{}

//开始
System::Single^  TestProp::Begin::get()
{
	return m_Begin;
}
void TestProp::Begin::set(System::Single^ value)
{}

//结束
System::Single^  TestProp::End::get()
{
	return m_End;
}

//密度
void TestProp::End::set(System::Single^ value)
{}

System::Single^  TestProp::Density::get()
{
	return m_Density;
}

void TestProp::Density::set(System::Single^ value)
{}
