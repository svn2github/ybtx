#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


#include "CUIBase.h"

namespace sqr_tools {

	/// <summary>
	/// Summary for CBlockViewCtrl
	/// </summary>
	public ref class CBlockViewCtrl : public CUIBase
	{
	public:
		CBlockViewCtrl(void)
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
		~CBlockViewCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::RadioButton^  normalBlock_radioButton1;
	protected: 

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::RadioButton^  exactBlock_radioButton;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::RadioButton^  eraseBlock_radioButton;

	private: System::Windows::Forms::RadioButton^  addBlock_radioButton;

	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::RadioButton^  highBlock_radioButton;
	private: System::Windows::Forms::RadioButton^  midleBlock_radioButton;
	private: System::Windows::Forms::RadioButton^  lowBlock_radioButton;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::Button^  effect_button;

	private: System::Windows::Forms::Button^  object_button;

	private: System::Windows::Forms::Button^  block_button;
	private: System::Windows::Forms::Button^  sceneLight_button;
	private: System::Windows::Forms::Button^  all_button;
	private: System::Windows::Forms::Button^  clear_button;



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
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->exactBlock_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->normalBlock_radioButton1 = (gcnew System::Windows::Forms::RadioButton());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->eraseBlock_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->addBlock_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->highBlock_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->midleBlock_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->lowBlock_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->all_button = (gcnew System::Windows::Forms::Button());
			this->clear_button = (gcnew System::Windows::Forms::Button());
			this->sceneLight_button = (gcnew System::Windows::Forms::Button());
			this->effect_button = (gcnew System::Windows::Forms::Button());
			this->object_button = (gcnew System::Windows::Forms::Button());
			this->block_button = (gcnew System::Windows::Forms::Button());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->exactBlock_radioButton);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->normalBlock_radioButton1);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Location = System::Drawing::Point(15, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(233, 55);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"精确阻挡";
			// 
			// exactBlock_radioButton
			// 
			this->exactBlock_radioButton->AutoSize = true;
			this->exactBlock_radioButton->Location = System::Drawing::Point(169, 22);
			this->exactBlock_radioButton->Name = L"exactBlock_radioButton";
			this->exactBlock_radioButton->Size = System::Drawing::Size(41, 16);
			this->exactBlock_radioButton->TabIndex = 3;
			this->exactBlock_radioButton->Text = L"1*1";
			this->exactBlock_radioButton->UseVisualStyleBackColor = true;
			this->exactBlock_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CBlockViewCtrl::exactBlock_radioButton_CheckedChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(130, 24);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(29, 12);
			this->label2->TabIndex = 2;
			this->label2->Text = L"精确";
			// 
			// normalBlock_radioButton1
			// 
			this->normalBlock_radioButton1->AutoSize = true;
			this->normalBlock_radioButton1->Checked = true;
			this->normalBlock_radioButton1->Location = System::Drawing::Point(61, 23);
			this->normalBlock_radioButton1->Name = L"normalBlock_radioButton1";
			this->normalBlock_radioButton1->Size = System::Drawing::Size(41, 16);
			this->normalBlock_radioButton1->TabIndex = 1;
			this->normalBlock_radioButton1->TabStop = true;
			this->normalBlock_radioButton1->Text = L"1*1";
			this->normalBlock_radioButton1->UseVisualStyleBackColor = true;
			this->normalBlock_radioButton1->CheckedChanged += gcnew System::EventHandler(this, &CBlockViewCtrl::normalBlock_radioButton1_CheckedChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(22, 25);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(29, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"普通";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->eraseBlock_radioButton);
			this->groupBox2->Controls->Add(this->addBlock_radioButton);
			this->groupBox2->Location = System::Drawing::Point(15, 80);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(233, 49);
			this->groupBox2->TabIndex = 1;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"阻挡操作";
			// 
			// eraseBlock_radioButton
			// 
			this->eraseBlock_radioButton->AutoSize = true;
			this->eraseBlock_radioButton->Location = System::Drawing::Point(139, 23);
			this->eraseBlock_radioButton->Name = L"eraseBlock_radioButton";
			this->eraseBlock_radioButton->Size = System::Drawing::Size(71, 16);
			this->eraseBlock_radioButton->TabIndex = 1;
			this->eraseBlock_radioButton->Text = L"擦除阻挡";
			this->eraseBlock_radioButton->UseVisualStyleBackColor = true;
			this->eraseBlock_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CBlockViewCtrl::eraseBlock_radioButton_CheckedChanged);
			// 
			// addBlock_radioButton
			// 
			this->addBlock_radioButton->AutoSize = true;
			this->addBlock_radioButton->Checked = true;
			this->addBlock_radioButton->Location = System::Drawing::Point(24, 23);
			this->addBlock_radioButton->Name = L"addBlock_radioButton";
			this->addBlock_radioButton->Size = System::Drawing::Size(71, 16);
			this->addBlock_radioButton->TabIndex = 0;
			this->addBlock_radioButton->TabStop = true;
			this->addBlock_radioButton->Text = L"添加阻挡";
			this->addBlock_radioButton->UseVisualStyleBackColor = true;
			this->addBlock_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CBlockViewCtrl::addBlock_radioButton_CheckedChanged);
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->highBlock_radioButton);
			this->groupBox3->Controls->Add(this->midleBlock_radioButton);
			this->groupBox3->Controls->Add(this->lowBlock_radioButton);
			this->groupBox3->Location = System::Drawing::Point(15, 141);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(233, 55);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"阻挡类型";
			// 
			// highBlock_radioButton
			// 
			this->highBlock_radioButton->AutoSize = true;
			this->highBlock_radioButton->Checked = true;
			this->highBlock_radioButton->Location = System::Drawing::Point(163, 25);
			this->highBlock_radioButton->Name = L"highBlock_radioButton";
			this->highBlock_radioButton->Size = System::Drawing::Size(47, 16);
			this->highBlock_radioButton->TabIndex = 2;
			this->highBlock_radioButton->TabStop = true;
			this->highBlock_radioButton->Text = L"高障";
			this->highBlock_radioButton->UseVisualStyleBackColor = true;
			this->highBlock_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CBlockViewCtrl::highBlock_radioButton_CheckedChanged);
			// 
			// midleBlock_radioButton
			// 
			this->midleBlock_radioButton->AutoSize = true;
			this->midleBlock_radioButton->Location = System::Drawing::Point(93, 25);
			this->midleBlock_radioButton->Name = L"midleBlock_radioButton";
			this->midleBlock_radioButton->Size = System::Drawing::Size(47, 16);
			this->midleBlock_radioButton->TabIndex = 1;
			this->midleBlock_radioButton->Text = L"中障";
			this->midleBlock_radioButton->UseVisualStyleBackColor = true;
			this->midleBlock_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CBlockViewCtrl::midleBlock_radioButton_CheckedChanged);
			// 
			// lowBlock_radioButton
			// 
			this->lowBlock_radioButton->AutoSize = true;
			this->lowBlock_radioButton->Location = System::Drawing::Point(24, 25);
			this->lowBlock_radioButton->Name = L"lowBlock_radioButton";
			this->lowBlock_radioButton->Size = System::Drawing::Size(47, 16);
			this->lowBlock_radioButton->TabIndex = 0;
			this->lowBlock_radioButton->Text = L"低障";
			this->lowBlock_radioButton->UseVisualStyleBackColor = true;
			this->lowBlock_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CBlockViewCtrl::lowBlock_radioButton_CheckedChanged);
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->all_button);
			this->groupBox4->Controls->Add(this->clear_button);
			this->groupBox4->Controls->Add(this->sceneLight_button);
			this->groupBox4->Controls->Add(this->effect_button);
			this->groupBox4->Controls->Add(this->object_button);
			this->groupBox4->Controls->Add(this->block_button);
			this->groupBox4->Location = System::Drawing::Point(7, 212);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(254, 86);
			this->groupBox4->TabIndex = 3;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"更新小地图";
			// 
			// all_button
			// 
			this->all_button->Location = System::Drawing::Point(129, 55);
			this->all_button->Name = L"all_button";
			this->all_button->Size = System::Drawing::Size(114, 22);
			this->all_button->TabIndex = 5;
			this->all_button->Text = L"全部";
			this->all_button->UseVisualStyleBackColor = true;
			this->all_button->Click += gcnew System::EventHandler(this, &CBlockViewCtrl::all_button_Click);
			// 
			// clear_button
			// 
			this->clear_button->Location = System::Drawing::Point(9, 55);
			this->clear_button->Name = L"clear_button";
			this->clear_button->Size = System::Drawing::Size(114, 22);
			this->clear_button->TabIndex = 4;
			this->clear_button->Text = L"清除";
			this->clear_button->UseVisualStyleBackColor = true;
			this->clear_button->Click += gcnew System::EventHandler(this, &CBlockViewCtrl::clear_button_Click);
			// 
			// sceneLight_button
			// 
			this->sceneLight_button->Location = System::Drawing::Point(190, 27);
			this->sceneLight_button->Name = L"sceneLight_button";
			this->sceneLight_button->Size = System::Drawing::Size(54, 22);
			this->sceneLight_button->TabIndex = 3;
			this->sceneLight_button->Text = L"场景灯";
			this->sceneLight_button->UseVisualStyleBackColor = true;
			this->sceneLight_button->Click += gcnew System::EventHandler(this, &CBlockViewCtrl::sceneLight_button_Click);
			// 
			// effect_button
			// 
			this->effect_button->Location = System::Drawing::Point(130, 27);
			this->effect_button->Name = L"effect_button";
			this->effect_button->Size = System::Drawing::Size(54, 22);
			this->effect_button->TabIndex = 2;
			this->effect_button->Text = L"特效";
			this->effect_button->UseVisualStyleBackColor = true;
			this->effect_button->Click += gcnew System::EventHandler(this, &CBlockViewCtrl::effect_button_Click);
			// 
			// object_button
			// 
			this->object_button->Location = System::Drawing::Point(69, 27);
			this->object_button->Name = L"object_button";
			this->object_button->Size = System::Drawing::Size(54, 22);
			this->object_button->TabIndex = 1;
			this->object_button->Text = L"物件";
			this->object_button->UseVisualStyleBackColor = true;
			this->object_button->Click += gcnew System::EventHandler(this, &CBlockViewCtrl::object_button_Click);
			// 
			// block_button
			// 
			this->block_button->Location = System::Drawing::Point(9, 27);
			this->block_button->Name = L"block_button";
			this->block_button->Size = System::Drawing::Size(54, 22);
			this->block_button->TabIndex = 0;
			this->block_button->Text = L"阻挡";
			this->block_button->UseVisualStyleBackColor = true;
			this->block_button->Click += gcnew System::EventHandler(this, &CBlockViewCtrl::block_button_Click);
			// 
			// CBlockViewCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Name = L"CBlockViewCtrl";
			this->Size = System::Drawing::Size(272, 554);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->groupBox4->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void normalBlock_radioButton1_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void exactBlock_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void addBlock_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void eraseBlock_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void lowBlock_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void midleBlock_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void highBlock_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void block_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void object_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void effect_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void sceneLight_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void clear_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void all_button_Click(System::Object^  sender, System::EventArgs^  e);

	public:  virtual void   Update(void) override;
	public:  virtual char*	GetImpTypeName(void)	override;
	public:  virtual void*	GetImpParam(void)		override;
};
}
