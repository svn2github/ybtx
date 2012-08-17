#pragma once
#include "CBaseComponent.h"

namespace sqr_tools
{
	public ref class CBoundingBoxPropCom : public CBaseComponent
	{
	public: 
		CBoundingBoxPropCom();
		~CBoundingBoxPropCom();

		virtual char* GetImpTypeName(void) override;
		virtual void  Update(void) override;

		[CategoryAttribute("偏移"), DescriptionAttribute("X偏移")]
		property float xDelta
		{
			float get();
			void set(float value);
		}

		[CategoryAttribute("偏移"), DescriptionAttribute("Y偏移")]
		property float yDelta 
		{
			float get();
			void set(float value);
		}

		[CategoryAttribute("偏移"), DescriptionAttribute("Z偏移")]
		property float zDelta 
		{
			float get();
			void set(float value);
		}

		[CategoryAttribute("大小"), DescriptionAttribute("X大小")]
		property float xSize
		{
			float get();
			void set(float value);
		}

		[CategoryAttribute("大小"), DescriptionAttribute("Y大小")]
		property float ySize 
		{
			float get();
			void set(float value);
		}


		[CategoryAttribute("大小"), DescriptionAttribute("Z大小")]
		property float zSize 
		{
			float get();
			void set(float value);
		}

		[CategoryAttribute("显示"), DescriptionAttribute("显示大包围盒")]
		property bool showBigBox 
		{
			bool get();
			void set(bool value);
		}

		[CategoryAttribute("显示"), DescriptionAttribute("显示小包围盒")]
		property bool showSmaBox 
		{
			bool get();
			void set(bool value);
		}

		[CategoryAttribute("控件集属性"), DescriptionAttribute("数据集拥有者")]
		property System::Windows::Forms::ContainerControl^ ControlParent 
		{
			System::Windows::Forms::ContainerControl^ get();
			void set(System::Windows::Forms::ContainerControl^ value);
		}

	protected: virtual System::Void _InitCtrl(void) override;
	protected: float	m_fXDelta;
	protected: float	m_fYDelta;
	protected: float	m_fZDelta;
	protected: float	m_fXSize;
	protected: float	m_fYSize;
	protected: float	m_fZSize;
	protected: bool	m_bShowBigBox;
	protected: bool	m_bShowSmaBox;
	protected: System::Windows::Forms::ContainerControl^ m_pUserCtrl;
	};
}
