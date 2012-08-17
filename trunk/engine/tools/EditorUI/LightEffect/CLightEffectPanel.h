#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"
#include "LightViewCtrl\CLightView.h"

namespace sqr_tools {

	/// <summary>
	/// Summary for CLightEffectPanel
	/// </summary>
	public ref class CLightEffectPanel : public CUIBase
	{
	public:
		CLightEffectPanel(void)
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
		~CLightEffectPanel()
		{
			if (components)
			{
				delete components;
			}
		}
	private: sqr_tools::CLightView^  cLightView1;
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->cLightView1 = (gcnew sqr_tools::CLightView());
			this->SuspendLayout();
			// 
			// cLightView1
			// 
			this->cLightView1->ContextComponent = nullptr;
			this->cLightView1->Location = System::Drawing::Point(3, 3);
			this->cLightView1->Name = L"cLightView1";
			this->cLightView1->Size = System::Drawing::Size(283, 366);
			this->cLightView1->TabIndex = 2;
			// 
			// CLightEffectPanel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->cLightView1);
			this->Name = L"CLightEffectPanel";
			this->Size = System::Drawing::Size(288, 378);
			this->ResumeLayout(false);

		}
#pragma endregion

		public:  virtual void Update(void) override;
		public:  virtual char*	GetImpTypeName(void)	override;

	};
}
