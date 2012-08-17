#pragma once
#include "CUIBase.h"

namespace sqr_tools {

	/// <summary>
	/// Summary for CEditViewCtrl
	/// </summary>
	[ToolboxBitmap(CViewCtrl::typeid,"CViewCtrl")]
	public ref class CViewCtrl : public CUIBase
	{
	public:
		CViewCtrl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CViewCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	public:
		virtual char*	GetImpTypeName(void)	override;
		virtual void*	GetImpParam(void)		override;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			// 
			// CViewCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::Gray;
			this->Name = L"CViewCtrl";
			this->Size = System::Drawing::Size(368, 318);
			this->MouseEnter += gcnew System::EventHandler(this, &CViewCtrl::CViewCtrl_MouseEnter);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void CViewCtrl_MouseEnter(System::Object^  sender, System::EventArgs^  e);
	};
}
