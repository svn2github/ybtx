#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;

namespace sqr_tools {

public ref class CFogProp
{
public:
	CFogProp(void);

	public:
		enum struct FOGMODE
		{
			FOG_LINEAR,
			FOG_EXPONENT
		};

	public:
		[CategoryAttribute("雾属性"), DescriptionAttribute("雾ID")]
		property System::Int32^ Region 
		{
			System::Int32^ get();
			void set(System::Int32^ value);
		}

		[CategoryAttribute("雾属性"), DescriptionAttribute("雾类型")]
		property FOGMODE FogType 
		{
			FOGMODE get();
			void set(FOGMODE value);
		}

		[CategoryAttribute("雾属性"), DescriptionAttribute("雾颜色")]
		property System::Drawing::Color FogColor
		{
			System::Drawing::Color get();
			void set(System::Drawing::Color value);
		}

		[CategoryAttribute("雾属性"), DescriptionAttribute("开始位置")]
		property System::Single^ Begin
		{
			System::Single^ get();
			void set(System::Single^ value);
		}

		[CategoryAttribute("雾属性"), DescriptionAttribute("结束位置")]
		property System::Single^ End
		{
			System::Single^ get();
			void set(System::Single^ value);
		}

		[CategoryAttribute("雾属性"), DescriptionAttribute("密度")]
		property System::Single^ Density
		{
			System::Single^ get();
			void set(System::Single^ value);
		}

	protected: 
		System::Int32^				m_Region;
		FOGMODE						m_FogType;
		System::Drawing::Color		m_FogColor;
		System::Single^				m_Begin;
		System::Single^				m_End;
		System::Single^				m_Density;
};

//!namespace
}