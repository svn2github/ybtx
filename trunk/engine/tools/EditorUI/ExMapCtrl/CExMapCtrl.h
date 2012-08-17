#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace sqr_tools {

	/// <summary>
	/// Summary for CExMapCtrl
	/// </summary>
	public ref class CExMapCtrl : public System::Windows::Forms::UserControl
	{
	public:
		CExMapCtrl(void)
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
		~CExMapCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::GroupBox^  GroupEx;
	private: System::Windows::Forms::Label^  label0;
	private: System::Windows::Forms::Label^  label1;	
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;	
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  EditExLength;
	private: System::Windows::Forms::TextBox^  EditExWidth;
	private: System::Windows::Forms::Label^  labelCurLength;
	private: System::Windows::Forms::Label^  labelName;
	private: System::Windows::Forms::Label^  labelCurWidth;
	private: System::Windows::Forms::Button^  btCancle;
	private: System::Windows::Forms::Button^  btOK;
	private: System::Windows::Forms::CheckBox^  cBoxLBack;
	private: System::Windows::Forms::CheckBox^  cBoxWLeft;
	private: System::Windows::Forms::CheckBox^  cBoxLFront;
	private: System::Windows::Forms::CheckBox^  cBoxLDouble;
	private: System::Windows::Forms::CheckBox^  cBoxWRight;
	private: System::Windows::Forms::CheckBox^  cBoxWDouble;


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
			this->GroupEx = (gcnew System::Windows::Forms::GroupBox());
			this->cBoxLBack = (gcnew System::Windows::Forms::CheckBox());
			this->cBoxWLeft = (gcnew System::Windows::Forms::CheckBox());
			this->cBoxLFront = (gcnew System::Windows::Forms::CheckBox());
			this->cBoxLDouble = (gcnew System::Windows::Forms::CheckBox());
			this->cBoxWRight = (gcnew System::Windows::Forms::CheckBox());
			this->cBoxWDouble = (gcnew System::Windows::Forms::CheckBox());
			this->btCancle = (gcnew System::Windows::Forms::Button());
			this->btOK = (gcnew System::Windows::Forms::Button());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->EditExLength = (gcnew System::Windows::Forms::TextBox());
			this->EditExWidth = (gcnew System::Windows::Forms::TextBox());
			this->labelCurLength = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->labelName = (gcnew System::Windows::Forms::Label());
			this->labelCurWidth = (gcnew System::Windows::Forms::Label());
			this->label0 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->GroupEx->SuspendLayout();
			this->SuspendLayout();
			// 
			// GroupEx
			// 
			this->GroupEx->Controls->Add(this->cBoxLBack);
			this->GroupEx->Controls->Add(this->cBoxWLeft);
			this->GroupEx->Controls->Add(this->cBoxLFront);
			this->GroupEx->Controls->Add(this->cBoxLDouble);
			this->GroupEx->Controls->Add(this->cBoxWRight);
			this->GroupEx->Controls->Add(this->cBoxWDouble);
			this->GroupEx->Controls->Add(this->btCancle);
			this->GroupEx->Controls->Add(this->btOK);
			this->GroupEx->Controls->Add(this->label4);
			this->GroupEx->Controls->Add(this->label3);
			this->GroupEx->Controls->Add(this->label5);
			this->GroupEx->Controls->Add(this->EditExLength);
			this->GroupEx->Controls->Add(this->EditExWidth);
			this->GroupEx->Controls->Add(this->labelCurLength);
			this->GroupEx->Controls->Add(this->label2);
			this->GroupEx->Controls->Add(this->labelName);
			this->GroupEx->Controls->Add(this->labelCurWidth);
			this->GroupEx->Controls->Add(this->label0);
			this->GroupEx->Controls->Add(this->label1);
			this->GroupEx->Location = System::Drawing::Point(5, 5);
			this->GroupEx->Name = L"GroupEx";
			this->GroupEx->Size = System::Drawing::Size(320, 243);
			this->GroupEx->TabIndex = 0;
			this->GroupEx->TabStop = false;
			this->GroupEx->Text = L"扩建";
			// 
			// cBoxLBack
			// 
			this->cBoxLBack->AutoSize = true;
			this->cBoxLBack->Location = System::Drawing::Point(162, 161);
			this->cBoxLBack->Name = L"cBoxLBack";
			this->cBoxLBack->Size = System::Drawing::Size(72, 16);
			this->cBoxLBack->TabIndex = 10;
			this->cBoxLBack->Text = L"长度后向";
			this->cBoxLBack->UseVisualStyleBackColor = true;
			this->cBoxLBack->CheckedChanged += gcnew System::EventHandler(this, &CExMapCtrl::cBoxLBack_CheckedChanged);
			// 
			// cBoxWLeft
			// 
			this->cBoxWLeft->AutoSize = true;
			this->cBoxWLeft->Location = System::Drawing::Point(10, 161);
			this->cBoxWLeft->Name = L"cBoxWLeft";
			this->cBoxWLeft->Size = System::Drawing::Size(72, 16);
			this->cBoxWLeft->TabIndex = 9;
			this->cBoxWLeft->Text = L"宽度左向";
			this->cBoxWLeft->UseVisualStyleBackColor = true;
			this->cBoxWLeft->CheckedChanged += gcnew System::EventHandler(this, &CExMapCtrl::cBoxWLeft_CheckedChanged);
			// 
			// cBoxLFront
			// 
			this->cBoxLFront->AutoSize = true;
			this->cBoxLFront->Location = System::Drawing::Point(163, 139);
			this->cBoxLFront->Name = L"cBoxLFront";
			this->cBoxLFront->Size = System::Drawing::Size(72, 16);
			this->cBoxLFront->TabIndex = 8;
			this->cBoxLFront->Text = L"长度前向";
			this->cBoxLFront->UseVisualStyleBackColor = true;
			this->cBoxLFront->CheckedChanged += gcnew System::EventHandler(this, &CExMapCtrl::cBoxLFront_CheckedChanged);
			// 
			// cBoxLDouble
			// 
			this->cBoxLDouble->AutoSize = true;
			this->cBoxLDouble->Location = System::Drawing::Point(163, 116);
			this->cBoxLDouble->Name = L"cBoxLDouble";
			this->cBoxLDouble->Size = System::Drawing::Size(72, 16);
			this->cBoxLDouble->TabIndex = 7;
			this->cBoxLDouble->Text = L"长度双向";
			this->cBoxLDouble->UseVisualStyleBackColor = true;
			this->cBoxLDouble->CheckedChanged += gcnew System::EventHandler(this, &CExMapCtrl::cBoxLDouble_CheckedChanged);
			// 
			// cBoxWRight
			// 
			this->cBoxWRight->AutoSize = true;
			this->cBoxWRight->Location = System::Drawing::Point(10, 138);
			this->cBoxWRight->Name = L"cBoxWRight";
			this->cBoxWRight->Size = System::Drawing::Size(72, 16);
			this->cBoxWRight->TabIndex = 6;
			this->cBoxWRight->Text = L"宽度右向";
			this->cBoxWRight->UseVisualStyleBackColor = true;
			this->cBoxWRight->CheckedChanged += gcnew System::EventHandler(this, &CExMapCtrl::cBoxWRight_CheckedChanged);
			// 
			// cBoxWDouble
			// 
			this->cBoxWDouble->AutoSize = true;
			this->cBoxWDouble->Location = System::Drawing::Point(10, 116);
			this->cBoxWDouble->Name = L"cBoxWDouble";
			this->cBoxWDouble->Size = System::Drawing::Size(72, 16);
			this->cBoxWDouble->TabIndex = 5;
			this->cBoxWDouble->Text = L"宽度双向";
			this->cBoxWDouble->UseVisualStyleBackColor = true;
			this->cBoxWDouble->CheckedChanged += gcnew System::EventHandler(this, &CExMapCtrl::cBoxWDouble_CheckedChanged);
			// 
			// btCancle
			// 
			this->btCancle->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->btCancle->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btCancle->Location = System::Drawing::Point(240, 214);
			this->btCancle->Name = L"btCancle";
			this->btCancle->Size = System::Drawing::Size(60, 23);
			this->btCancle->TabIndex = 4;
			this->btCancle->Text = L"取消";
			this->btCancle->UseVisualStyleBackColor = true;
			this->btCancle->Visible = false;
			this->btCancle->Click += gcnew System::EventHandler(this, &CExMapCtrl::btCancle_Click);
			// 
			// btOK
			// 
			this->btOK->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->btOK->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btOK->Location = System::Drawing::Point(174, 214);
			this->btOK->Name = L"btOK";
			this->btOK->Size = System::Drawing::Size(60, 23);
			this->btOK->TabIndex = 4;
			this->btOK->Text = L"确定";
			this->btOK->UseVisualStyleBackColor = true;
			this->btOK->Visible = false;
			this->btOK->Click += gcnew System::EventHandler(this, &CExMapCtrl::btOK_Click);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(159, 83);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(41, 12);
			this->label4->TabIndex = 3;
			this->label4->Text = L"扩建长";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(8, 83);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(41, 12);
			this->label3->TabIndex = 3;
			this->label3->Text = L"扩建宽";
			// 
			// label5
			// 
			this->label5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(8, 189);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(209, 12);
			this->label5->TabIndex = 3;
			this->label5->Text = L"注:扩展宽长为需扩展的尺寸(4的倍数)";
			// 
			// EditExLength
			// 
			this->EditExLength->Location = System::Drawing::Point(210, 80);
			this->EditExLength->Name = L"EditExLength";
			this->EditExLength->Size = System::Drawing::Size(90, 21);
			this->EditExLength->TabIndex = 2;
			this->EditExLength->Text = L"4";
			this->EditExLength->TextChanged += gcnew System::EventHandler(this, &CExMapCtrl::EditExLength_TextChanged);
			// 
			// EditExWidth
			// 
			this->EditExWidth->Location = System::Drawing::Point(57, 80);
			this->EditExWidth->Name = L"EditExWidth";
			this->EditExWidth->Size = System::Drawing::Size(90, 21);
			this->EditExWidth->TabIndex = 2;
			this->EditExWidth->Text = L"4";
			this->EditExWidth->TextChanged += gcnew System::EventHandler(this, &CExMapCtrl::EditExWidth_TextChanged);
			// 
			// labelCurLength
			// 
			this->labelCurLength->AutoSize = true;
			this->labelCurLength->Location = System::Drawing::Point(208, 50);
			this->labelCurLength->Name = L"labelCurLength";
			this->labelCurLength->Size = System::Drawing::Size(11, 12);
			this->labelCurLength->TabIndex = 1;
			this->labelCurLength->Text = L"4";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(161, 50);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(47, 12);
			this->label2->TabIndex = 0;
			this->label2->Text = L"当前长:";
			// 
			// labelName
			// 
			this->labelName->AutoSize = true;
			this->labelName->Location = System::Drawing::Point(69, 23);
			this->labelName->Name = L"labelName";
			this->labelName->Size = System::Drawing::Size(77, 12);
			this->labelName->TabIndex = 1;
			this->labelName->Text = L"DefaultWorld";
			// 
			// labelCurWidth
			// 
			this->labelCurWidth->AutoSize = true;
			this->labelCurWidth->Location = System::Drawing::Point(55, 50);
			this->labelCurWidth->Name = L"labelCurWidth";
			this->labelCurWidth->Size = System::Drawing::Size(11, 12);
			this->labelCurWidth->TabIndex = 1;
			this->labelCurWidth->Text = L"4";
			// 
			// label0
			// 
			this->label0->AutoSize = true;
			this->label0->Location = System::Drawing::Point(8, 23);
			this->label0->Name = L"label0";
			this->label0->Size = System::Drawing::Size(59, 12);
			this->label0->TabIndex = 0;
			this->label0->Text = L"地图名称:";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(8, 50);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(47, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"当前宽:";
			// 
			// CExMapCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->GroupEx);
			this->Name = L"CExMapCtrl";
			this->Size = System::Drawing::Size(330, 258);
			this->GroupEx->ResumeLayout(false);
			this->GroupEx->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		private: System::Void EditExWidth_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void EditExLength_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void btOK_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void btCancle_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void cBoxWDouble_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void cBoxWRight_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void cBoxWLeft_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void cBoxLDouble_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void cBoxLFront_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void cBoxLBack_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		public: void HideBt();
		public: void SetMapName(void* name);
		public: void SetCurMapWidth(int w);
		public: void SetCurMapLength(int l);
		public: int GetExMapWidth();
		public: int GetExMapLength();
		public: int GetExMapWidthDir();
		public: int GetExMapLengthDir();
};
}
