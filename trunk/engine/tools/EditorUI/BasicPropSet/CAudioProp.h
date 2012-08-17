#pragma once

#include "CBaseProp.h"

namespace sqr_tools 
{

	public ref class CAudioProp :public CBaseProp
	{
	public:
		CAudioProp(void* ctrl);
		~CAudioProp();
	public:
		[CategoryAttribute("音效属性"), DescriptionAttribute("区域"),ReadOnlyAttribute(true)]
		property int AudioRegion
		{
			int get();
			void set(int value);
		}
		[CategoryAttribute("音效属性"),DescriptionAttribute("音效盒位置X"),ReadOnlyAttribute(false)]
		property  float AudioPosX
		{
			float get();
			void set(float value);
		}
		[CategoryAttribute("音效属性"), DescriptionAttribute("音效盒位置Y"),ReadOnlyAttribute(false)]
		property float AudioPosY
		{
			float get();
			void set(float value);
		}
		[CategoryAttribute("音效属性"), DescriptionAttribute("音效盒位置Z"),ReadOnlyAttribute(false)]
		property float AudioPosZ
		{
			float get();
			void set(float value);
		}
		[CategoryAttribute("音效属性"), DescriptionAttribute("音效名称"),ReadOnlyAttribute(false)]
		property System::String^ AudioName
		{
			System::String^ get();
			void set(System::String^ value);
		}
	protected:
		int	m_AudioRegion;
		float	m_AudioPosX;
		float	m_AudioPosY;
		float	m_AudioPosZ;
		System::String^ m_AudioName;
	
	public:
		virtual	void UpdateProp() override;
		virtual	void UpdateTimerProp() override;
	};

//!namespace
}
