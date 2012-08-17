#pragma once

#include "CBaseProp.h"
#include "CPropChinese.h"

namespace sqr_tools {

	ref class CSceneProp :public CBaseProp
	{
	public:
		CSceneProp(void* ctrl);
		~CSceneProp();

	public:
		#pragma region mapProp
			[CategoryAttribute("场景属性"), DescriptionAttribute("场景名称")]
			property System::String^ SceneName 
			{
				System::String^ get();
				void set(System::String^ value);
			}
			[CategoryAttribute("场景属性"), DescriptionAttribute("场景宽度")]
			property int SceneWidth 
			{
				int get();
				void set(int value);
			}
			[CategoryAttribute("场景属性"), DescriptionAttribute("场景高度")]
			property int SceneHeight 
			{
				int get();
				void set(int value);
			}
			[CategoryAttribute("场景属性"), DescriptionAttribute("当前区域ID")]
			property int SceneRegion 
			{
				int get();
				void set(int value);
			}
	#pragma endregion

	#pragma region camProp
			[CategoryAttribute("相机属性"), DescriptionAttribute("相机类型")]
			property int CamType 
			{
				int get();
				void set(int value);
			}
			[CategoryAttribute("相机属性"), DescriptionAttribute("相机位置X")]
			property  float CamPosX
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("相机属性"), DescriptionAttribute("相机位置Y")]
			property float CamPosY
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("相机属性"), DescriptionAttribute("相机位置Z")]
			property float CamPosZ
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("相机属性"), DescriptionAttribute("相机方向X")]
			property float CamDirX
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("相机属性"), DescriptionAttribute("相机方向Y")]
			property float CamDirY
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("相机属性"), DescriptionAttribute("相机方向Z")]
			property float CamDirZ
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("相机属性"), DescriptionAttribute("相机目标X")]
			property float CamTargetX
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("相机属性"), DescriptionAttribute("相机目标Y")]
			property float CamTargetY
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("相机属性"), DescriptionAttribute("相机目标Z")]
			property float CamTargetZ
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("相机属性"), DescriptionAttribute("相机远截面")]
			property float CamFarDis
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("相机属性"), DescriptionAttribute("相机近截面")]
			property float CamNearDis
			{
				float get();
				void set(float value);
			}
	#pragma endregion

	#pragma region GlightProp
			[CategoryAttribute("全局光参"), DescriptionAttribute("环境光颜色")]
			property System::Drawing::Color AmbientColor
			{
				System::Drawing::Color get();
				void set(System::Drawing::Color value);
			}
			[CategoryAttribute("全局光参"), DescriptionAttribute("全局太阳光颜色")]
			property System::Drawing::Color SunColor 
			{
				System::Drawing::Color get();
				void set(System::Drawing::Color value);
			}
			[CategoryAttribute("全局光参"), DescriptionAttribute("人物环境光颜色")]
			property System::Drawing::Color PlayerAmbientColor 
			{
				System::Drawing::Color get();
				void set(System::Drawing::Color value);
			}
			[CategoryAttribute("全局光参"), DescriptionAttribute("人物太阳光颜色")]
			property System::Drawing::Color PlayerSunColor 
			{
				System::Drawing::Color get();
				void set(System::Drawing::Color value);
			}
			[CategoryAttribute("全局光参"), DescriptionAttribute("太阳光向X")]
			property float SunDirX
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("全局光参"), DescriptionAttribute("太阳光向Y")]
			property float SunDirY
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("全局光参"), DescriptionAttribute("太阳光向Z")]
			property float SunDirZ
			{
				float get();
				void set(float value);
			}
	#pragma endregion
			
	#pragma region FogProp
	public:
			enum struct FOGMODE
			{
				FOG_LINEAR		=1,
				FOG_EXPONENT
			};

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
			property float FogBegin
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("雾属性"), DescriptionAttribute("结束位置")]
			property float FogEnd
			{
				float get();
				void set(float value);
			}
			[CategoryAttribute("雾属性"), DescriptionAttribute("密度")]
			property float FogDensity
			{
				float get();
				void set(float value);
			}
	#pragma endregion

	protected:
		//区域
		System::String^ m_SceneName;
		int		m_Width;
		int		m_Height;
		int		m_SceneRegion;
		int		m_LastSceneRegion;
		//Cam
		int		m_CamType;
		float	m_CamPosX;
		float	m_CamPosY;
		float	m_CamPosZ;
		float	m_CamDirX;
		float	m_CamDirY;
		float	m_CamDirZ;
		float	m_CamTargetX;
		float	m_CamTargetY;
		float	m_CamTargetZ;
		float	m_CamFarDis;
		float	m_CamNearDis;
		//glight
		System::Drawing::Color m_AmbientColor;
		System::Drawing::Color m_SunColor;
		System::Drawing::Color m_PlayerAmbientColor;
		System::Drawing::Color m_PlayerSunColor;
		float	m_SunDirX;
		float	m_SunDirY;
		float	m_SunDirZ;
		//fog
		FOGMODE					m_FogType;
		System::Drawing::Color	m_FogColor;
		float			m_Begin;
		float			m_End;
		float			m_Density;

	public:
		virtual	void UpdateProp() override;
		virtual	void UpdateTimerProp() override;
	};


//!namespace
}