#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace sqr_tools {

	/// <summary>
	/// Summary for CMapFileCtrl
	/// </summary>
	public ref class CNewMapCtrl : public System::Windows::Forms::UserControl
	{
	public:
		CNewMapCtrl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			HideBt();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CNewMapCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::GroupBox^  GroupNew;

	private: System::Windows::Forms::Label^  labelTitle;
	private: System::Windows::Forms::Label^  labelHighMap;	
	private: System::Windows::Forms::Label^  labelTip;
	private: System::Windows::Forms::Label^  labelLength;
	private: System::Windows::Forms::Label^  labelWidth;

	private: System::Windows::Forms::TextBox^  m_editTitle;
	private: System::Windows::Forms::TextBox^  m_editHighMap;
	private: System::Windows::Forms::TextBox^  m_editLength;
	private: System::Windows::Forms::TextBox^  m_editWidth;

	private: System::Windows::Forms::Button^  m_btHighMap;
	private: System::Windows::Forms::Button^  m_btCancle;
	private: System::Windows::Forms::Button^  m_btOK;
	
	private: System::Windows::Forms::CheckBox^  m_checkHighMap;

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
			this->GroupNew = (gcnew System::Windows::Forms::GroupBox());
			this->labelTip = (gcnew System::Windows::Forms::Label());
			this->labelTitle = (gcnew System::Windows::Forms::Label());
			this->m_btCancle = (gcnew System::Windows::Forms::Button());
			this->m_btOK = (gcnew System::Windows::Forms::Button());
			this->m_btHighMap = (gcnew System::Windows::Forms::Button());
			this->m_checkHighMap = (gcnew System::Windows::Forms::CheckBox());
			this->labelHighMap = (gcnew System::Windows::Forms::Label());
			this->m_editTitle = (gcnew System::Windows::Forms::TextBox());
			this->m_editHighMap = (gcnew System::Windows::Forms::TextBox());
			this->labelLength = (gcnew System::Windows::Forms::Label());
			this->labelWidth = (gcnew System::Windows::Forms::Label());
			this->m_editLength = (gcnew System::Windows::Forms::TextBox());
			this->m_editWidth = (gcnew System::Windows::Forms::TextBox());
			this->GroupNew->SuspendLayout();
			this->SuspendLayout();
			// 
			// GroupNew
			// 
			this->GroupNew->Controls->Add(this->labelTip);
			this->GroupNew->Controls->Add(this->labelTitle);
			this->GroupNew->Controls->Add(this->m_btCancle);
			this->GroupNew->Controls->Add(this->m_btOK);
			this->GroupNew->Controls->Add(this->m_btHighMap);
			this->GroupNew->Controls->Add(this->m_checkHighMap);
			this->GroupNew->Controls->Add(this->labelHighMap);
			this->GroupNew->Controls->Add(this->m_editTitle);
			this->GroupNew->Controls->Add(this->m_editHighMap);
			this->GroupNew->Controls->Add(this->labelLength);
			this->GroupNew->Controls->Add(this->labelWidth);
			this->GroupNew->Controls->Add(this->m_editLength);
			this->GroupNew->Controls->Add(this->m_editWidth);
			this->GroupNew->Location = System::Drawing::Point(5, 5);
			this->GroupNew->Name = L"GroupNew";
			this->GroupNew->Size = System::Drawing::Size(320, 170);
			this->GroupNew->TabIndex = 7;
			this->GroupNew->TabStop = false;
			this->GroupNew->Text = L"新建";
			// 
			// labelTip
			// 
			this->labelTip->AutoSize = true;
			this->labelTip->Location = System::Drawing::Point(9, 145);
			this->labelTip->Name = L"labelTip";
			this->labelTip->Size = System::Drawing::Size(149, 12);
			this->labelTip->TabIndex = 9;
			this->labelTip->Text = L"注:目前长宽设置为4的倍数";
			// 
			// labelTitle
			// 
			this->labelTitle->AutoSize = true;
			this->labelTitle->Location = System::Drawing::Point(9, 28);
			this->labelTitle->Name = L"labelTitle";
			this->labelTitle->Size = System::Drawing::Size(35, 12);
			this->labelTitle->TabIndex = 8;
			this->labelTitle->Text = L"标题:";
			// 
			// m_btCancle
			// 
			this->m_btCancle->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->m_btCancle->Location = System::Drawing::Point(262, 135);
			this->m_btCancle->Name = L"m_btCancle";
			this->m_btCancle->Size = System::Drawing::Size(45, 22);
			this->m_btCancle->TabIndex = 7;
			this->m_btCancle->Text = L"取消";
			this->m_btCancle->UseVisualStyleBackColor = true;
			this->m_btCancle->Click += gcnew System::EventHandler(this, &CNewMapCtrl::m_btCancle_Click);
			// 
			// m_btOK
			// 
			this->m_btOK->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->m_btOK->Location = System::Drawing::Point(211, 135);
			this->m_btOK->Name = L"m_btOK";
			this->m_btOK->Size = System::Drawing::Size(45, 22);
			this->m_btOK->TabIndex = 7;
			this->m_btOK->Text = L"确定";
			this->m_btOK->UseVisualStyleBackColor = true;
			this->m_btOK->Click += gcnew System::EventHandler(this, &CNewMapCtrl::m_btOK_Click);
			// 
			// m_btHighMap
			// 
			this->m_btHighMap->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->m_btHighMap->Location = System::Drawing::Point(269, 84);
			this->m_btHighMap->Name = L"m_btHighMap";
			this->m_btHighMap->Size = System::Drawing::Size(38, 21);
			this->m_btHighMap->TabIndex = 6;
			this->m_btHighMap->Text = L"...";
			this->m_btHighMap->UseVisualStyleBackColor = true;
			this->m_btHighMap->Click += gcnew System::EventHandler(this, &CNewMapCtrl::hmapPathClick);
			// 
			// m_checkHighMap
			// 
			this->m_checkHighMap->AutoSize = true;
			this->m_checkHighMap->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->m_checkHighMap->Location = System::Drawing::Point(10, 115);
			this->m_checkHighMap->Name = L"m_checkHighMap";
			this->m_checkHighMap->Size = System::Drawing::Size(81, 16);
			this->m_checkHighMap->TabIndex = 5;
			this->m_checkHighMap->Text = L"使用高度图";
			this->m_checkHighMap->UseVisualStyleBackColor = true;
			this->m_checkHighMap->CheckedChanged += gcnew System::EventHandler(this, &CNewMapCtrl::m_checkHighMap_CheckedChanged);
			// 
			// labelHighMap
			// 
			this->labelHighMap->AutoSize = true;
			this->labelHighMap->Location = System::Drawing::Point(8, 90);
			this->labelHighMap->Name = L"labelHighMap";
			this->labelHighMap->Size = System::Drawing::Size(41, 12);
			this->labelHighMap->TabIndex = 4;
			this->labelHighMap->Text = L"高度图";
			// 
			// m_editTitle
			// 
			this->m_editTitle->Location = System::Drawing::Point(55, 25);
			this->m_editTitle->Name = L"m_editTitle";
			this->m_editTitle->Size = System::Drawing::Size(252, 21);
			this->m_editTitle->TabIndex = 3;
			this->m_editTitle->Text = L"DefaultWorld";
			// 
			// m_editHighMap
			// 
			this->m_editHighMap->Location = System::Drawing::Point(55, 84);
			this->m_editHighMap->Name = L"m_editHighMap";
			this->m_editHighMap->Size = System::Drawing::Size(252, 21);
			this->m_editHighMap->TabIndex = 3;
			// 
			// labelLength
			// 
			this->labelLength->AutoSize = true;
			this->labelLength->Location = System::Drawing::Point(160, 57);
			this->labelLength->Name = L"labelLength";
			this->labelLength->Size = System::Drawing::Size(47, 12);
			this->labelLength->TabIndex = 2;
			this->labelLength->Text = L"长(米):";
			// 
			// labelWidth
			// 
			this->labelWidth->AutoSize = true;
			this->labelWidth->Location = System::Drawing::Point(8, 57);
			this->labelWidth->Name = L"labelWidth";
			this->labelWidth->Size = System::Drawing::Size(47, 12);
			this->labelWidth->TabIndex = 2;
			this->labelWidth->Text = L"宽(米):";
			// 
			// m_editLength
			// 
			this->m_editLength->Location = System::Drawing::Point(207, 53);
			this->m_editLength->Name = L"m_editLength";
			this->m_editLength->Size = System::Drawing::Size(100, 21);
			this->m_editLength->TabIndex = 1;
			this->m_editLength->Text = L"4";
			this->m_editLength->TextChanged += gcnew System::EventHandler(this, &CNewMapCtrl::m_editLength_TextChanged);
			// 
			// m_editWidth
			// 
			this->m_editWidth->Location = System::Drawing::Point(55, 53);
			this->m_editWidth->Name = L"m_editWidth";
			this->m_editWidth->Size = System::Drawing::Size(100, 21);
			this->m_editWidth->TabIndex = 0;
			this->m_editWidth->Text = L"4";
			this->m_editWidth->TextChanged += gcnew System::EventHandler(this, &CNewMapCtrl::m_editWidth_TextChanged);
			// 
			// CNewMapCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->GroupNew);
			this->Name = L"CNewMapCtrl";
			this->Size = System::Drawing::Size(330, 185);
			this->GroupNew->ResumeLayout(false);
			this->GroupNew->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion		
		public: System::Void HideBt();
		public: System::Void CreatMap();
		public: System::Void m_btOK_Click(System::Object^  sender, System::EventArgs^  e);
		public: System::Void m_btCancle_Click(System::Object^  sender, System::EventArgs^  e);
		public: System::Void hmapPathClick(System::Object^  sender, System::EventArgs^  e);
		public: System::Void m_checkHighMap_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		public: System::Void m_editWidth_TextChanged(System::Object^  sender, System::EventArgs^  e);
		public: System::Void m_editLength_TextChanged(System::Object^  sender, System::EventArgs^  e);
		public: System::String^ GetMapName();
		public: int GetMapWidth();
		public: int GetMapHeight();
		public: System::String^ GetHMapName();
};

//!namespace
}