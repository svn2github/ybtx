#pragma once

#include "CBaseProp.h"

namespace sqr_tools {

	enum struct LIGHTTYPE
	{
		DIR_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT
	};
	/// <summary>
	/// Summary for CLightCom
	/// </summary>
	public ref class CLightProp :public CBaseProp
	{
	public:
		CLightProp(void* ctrl);
		~CLightProp();

		public:
			[CategoryAttribute("灯光属性"), DescriptionAttribute("灯光ID")]
			property int LightID 
			{
				int get();
				void set(int value);
			}

			[CategoryAttribute("灯光属性"),DescriptionAttribute("灯光类型")]
			property LIGHTTYPE LightType 
			{
				LIGHTTYPE get();
				void set(LIGHTTYPE value);
			}

			[CategoryAttribute("灯光属性"), DescriptionAttribute("灯光颜色")]
			property System::Drawing::Color LightColor
			{
				System::Drawing::Color get();
				void set(System::Drawing::Color value);
			}

			[CategoryAttribute("灯光属性"), DescriptionAttribute("灯光位置X")]
			property float LightPosX
			{
				float get();
				void set(float value);
			}

			[CategoryAttribute("灯光属性"), DescriptionAttribute("灯光位置Y")]
			property float LightPosY
			{
				float get();
				void set(float value);
			}

			[CategoryAttribute("灯光属性"), DescriptionAttribute("灯光位置Z")]
			property float LightPosZ
			{
				float get();
				void set(float value);
			}

			[CategoryAttribute("灯光属性"), DescriptionAttribute("灯光方向X")]
			property float LightDirX
			{
				float get();
				void set(float value);
			}

			[CategoryAttribute("灯光属性"), DescriptionAttribute("灯光方向Y")]
			property float LightDirY
			{
				float get();
				void set(float value);
			}

			[CategoryAttribute("灯光属性"), DescriptionAttribute("灯光方向Z")]
			property float LightDirZ
			{
				float get();
				void set(float value);
			}

			[CategoryAttribute("灯光属性"), DescriptionAttribute("灯光最大距离")]
			property float LightMaxDis
			{
				float get();
				void set(float value);
			}

			[CategoryAttribute("聚光灯属性"), DescriptionAttribute("聚光灯内外衰减")]
			property float LightFalloff
			{
				float get();
				void set(float value);
			}

			[CategoryAttribute("聚光灯属性"), DescriptionAttribute("距离衰减系数1")]
			property float LightAtten0
			{
				float get();
				void set(float value);
			}

			[CategoryAttribute("聚光灯属性"), DescriptionAttribute("距离衰减系数2")]
			property float LightAtten1
			{
				float get();
				void set(float value);
			}

			[CategoryAttribute("聚光灯属性"), DescriptionAttribute("距离衰减系数3")]
			property float LightAtten2
			{
				float get();
				void set(float value);
			}

			[CategoryAttribute("聚光灯属性"), DescriptionAttribute("聚光灯内径")]
			property float LightInnerR
			{
				float get();
				void set(float value);
			}

			[CategoryAttribute("聚光灯属性"), DescriptionAttribute("聚光灯外径")]
			property float LightOuterR
			{
				float get();
				void set(float value);
			}

	protected: 
			int			m_LightID;
			LIGHTTYPE				m_LightType;
			System::Drawing::Color	m_LightColor;
			float			m_LightPosX;
			float			m_LightPosY;
			float			m_LightPosZ;		
			float			m_LightDirX;
			float			m_LightDirY;
			float			m_LightDirZ;
			float			m_LightMaxDis;
			float			m_LightFalloff;
			float			m_LightAtten0;
			float			m_LightAtten1;
			float			m_LightAtten2;
			float			m_LightInnerR;
			float			m_LightOuterR;
	
	public:
		virtual	void UpdateProp() override;
		virtual	void UpdateTimerProp() override;
	};
}
