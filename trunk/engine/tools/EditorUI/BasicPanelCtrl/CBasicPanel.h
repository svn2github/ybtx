#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"
#include "SceneStateCtrl\CSceneStateCtrl.h"

namespace sqr_tools {

	ref class CSceneStateCtrl;
	/// <summary>
	/// Summary for CBasicPanel
	/// </summary>
	public ref class CBasicPanel : public CUIBase
	{
	public:
		CBasicPanel(void)
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
		~CBasicPanel()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  WaveGroup;
	protected: 
	private: System::Windows::Forms::GroupBox^  roleGroup;
	private: System::Windows::Forms::GroupBox^  bgPicGroup;
	private: System::Windows::Forms::GroupBox^  bgMusicGroup;
	private: System::Windows::Forms::CheckBox^  checkBox1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button1;
	private: sqr_tools::CSceneStateCtrl^  cSceneStateCtrl1;

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
			this->WaveGroup = (gcnew System::Windows::Forms::GroupBox());
			this->roleGroup = (gcnew System::Windows::Forms::GroupBox());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->bgPicGroup = (gcnew System::Windows::Forms::GroupBox());
			this->bgMusicGroup = (gcnew System::Windows::Forms::GroupBox());
			this->cSceneStateCtrl1 = (gcnew sqr_tools::CSceneStateCtrl());
			this->roleGroup->SuspendLayout();
			this->SuspendLayout();
			// 
			// WaveGroup
			// 
			this->WaveGroup->Location = System::Drawing::Point(23, 24);
			this->WaveGroup->Name = L"WaveGroup";
			this->WaveGroup->Size = System::Drawing::Size(240, 164);
			this->WaveGroup->TabIndex = 0;
			this->WaveGroup->TabStop = false;
			this->WaveGroup->Text = L"²¨ÀËÉèÖÃ";
			// 
			// roleGroup
			// 
			this->roleGroup->Controls->Add(this->checkBox1);
			this->roleGroup->Controls->Add(this->button2);
			this->roleGroup->Controls->Add(this->button1);
			this->roleGroup->Location = System::Drawing::Point(23, 216);
			this->roleGroup->Name = L"roleGroup";
			this->roleGroup->Size = System::Drawing::Size(240, 54);
			this->roleGroup->TabIndex = 1;
			this->roleGroup->TabStop = false;
			this->roleGroup->Text = L"Ö÷½Ç";
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(164, 28);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(72, 16);
			this->checkBox1->TabIndex = 1;
			this->checkBox1->Text = L"ÐÐ×ß×èµ²";
			this->checkBox1->UseVisualStyleBackColor = true;
			// 
			// button2
			// 
			this->button2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button2->Location = System::Drawing::Point(86, 20);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(65, 23);
			this->button2->TabIndex = 0;
			this->button2->Text = L"Ö÷½Ç¶¯»­";
			this->button2->UseVisualStyleBackColor = true;
			// 
			// button1
			// 
			this->button1->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button1->Location = System::Drawing::Point(12, 20);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(65, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"Ö÷½ÇÄ£ÐÍ";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// bgPicGroup
			// 
			this->bgPicGroup->Location = System::Drawing::Point(24, 305);
			this->bgPicGroup->Name = L"bgPicGroup";
			this->bgPicGroup->Size = System::Drawing::Size(240, 118);
			this->bgPicGroup->TabIndex = 2;
			this->bgPicGroup->TabStop = false;
			this->bgPicGroup->Text = L"±³¾°Í¼ÉèÖÃ";
			// 
			// bgMusicGroup
			// 
			this->bgMusicGroup->Location = System::Drawing::Point(24, 453);
			this->bgMusicGroup->Name = L"bgMusicGroup";
			this->bgMusicGroup->Size = System::Drawing::Size(240, 136);
			this->bgMusicGroup->TabIndex = 3;
			this->bgMusicGroup->TabStop = false;
			this->bgMusicGroup->Text = L"±³¾°ÒôÀÖÉèÖÃ";
			// 
			// cSceneStateCtrl1
			// 
			this->cSceneStateCtrl1->ContextComponent = nullptr;
			this->cSceneStateCtrl1->Location = System::Drawing::Point(48, 605);
			this->cSceneStateCtrl1->Name = L"cSceneStateCtrl1";
			this->cSceneStateCtrl1->Size = System::Drawing::Size(182, 334);
			this->cSceneStateCtrl1->TabIndex = 4;
			// 
			// CBasicPanel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->Controls->Add(this->cSceneStateCtrl1);
			this->Controls->Add(this->bgMusicGroup);
			this->Controls->Add(this->bgPicGroup);
			this->Controls->Add(this->roleGroup);
			this->Controls->Add(this->WaveGroup);
			this->Name = L"CBasicPanel";
			this->Size = System::Drawing::Size(291, 983);
			this->roleGroup->ResumeLayout(false);
			this->roleGroup->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

public:  virtual void Update(void) override;
public:  virtual char*	GetImpTypeName(void)	override;

	};
}
