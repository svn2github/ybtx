#pragma once

#include "CBaseProp.h"

namespace sqr_tools 
{

	public ref class CModelProp :public CBaseProp
	{
	public:
		CModelProp(void* ctrl);
		~CModelProp();
	public:
		[CategoryAttribute("模型属性"), DescriptionAttribute("模型名称")]
		property System::String^ ModelName
		{
			System::String^ get();
			void set(System::String^ value);
		}
		[CategoryAttribute("模型属性"), DescriptionAttribute("模型位置X")]
		property  float ModelPosX
		{
			float get();
			void set(float value);
		}
		[CategoryAttribute("模型属性"), DescriptionAttribute("模型位置Y")]
		property float ModelPosY
		{
			float get();
			void set(float value);
		}
		[CategoryAttribute("模型属性"), DescriptionAttribute("模型位置Z")]
		property float ModelPosZ
		{
			float get();
			void set(float value);
		}
		[CategoryAttribute("模型属性"), DescriptionAttribute("模型绕X旋转")]
		property  float ModelRotX
		{
			float get();
			void set(float value);
		}
		[CategoryAttribute("模型属性"), DescriptionAttribute("模型绕Y旋转")]
		property float ModelRotY
		{
			float get();
			void set(float value);
		}
		[CategoryAttribute("模型属性"), DescriptionAttribute("模型绕Z旋转")]
		property float ModelRotZ
		{
			float get();
			void set(float value);
		}
		[CategoryAttribute("模型属性"), DescriptionAttribute("模型缩放X")]
		property  float ModelScaleX
		{
			float get();
			void set(float value);
		}
		[CategoryAttribute("模型属性"), DescriptionAttribute("模型缩放Y")]
		property float ModelScaleY
		{
			float get();
			void set(float value);
		}
		[CategoryAttribute("模型属性"), DescriptionAttribute("模型缩放Z")]
		property float ModelScaleZ
		{
			float get();
			void set(float value);
		}

	protected:
		System::String^ m_ModelName;
		float m_ModelPosX;
		float m_ModelPosY;
		float m_ModelPosZ;
		float m_ModelRotX;
		float m_ModelRotY;
		float m_ModelRotZ;
		float m_ModelScaleX;
		float m_ModelScaleY;
		float m_ModelScaleZ;
	
	public:
		virtual	void UpdateProp() override;
		virtual	void UpdateTimerProp() override;
	};

//!namespace
}