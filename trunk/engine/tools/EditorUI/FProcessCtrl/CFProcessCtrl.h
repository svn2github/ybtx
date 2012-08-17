#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"
#include "ProcessCtrlEx/CProcessCtrlEx.h"

namespace sqr_tools {

	/// <summary>
	/// Summary for CFProcessCtrl
	/// </summary>
	public ref class CFProcessCtrl : public CUIBase 
	{
	public:
		CFProcessCtrl(void)
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
		~CFProcessCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::ListBox^  USE_PROCESSER_ListBox;
	protected: 

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::ListBox^  ALL_PROCESSER_ListBox;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  RemoveAll_Button;

	private: System::Windows::Forms::Button^  AddAll_Button;

	private: System::Windows::Forms::Button^  Remove_Button;

	private: System::Windows::Forms::Button^  Add_Button;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::ListBox^  Path_ListBox;
	private: System::Windows::Forms::Button^  Remove_Path_Button;


	private: System::Windows::Forms::Button^  Add_Path_Button;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Button^  Process_Button;

	private: System::Windows::Forms::Label^  label4;

	private: System::Windows::Forms::Label^  label3;





	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
	private: sqr_tools::CProcessCtrlEx^  cTotal_ProcessCtrlEx;
	private: sqr_tools::CProcessCtrlEx^  cCur_ProcessCtrlEx;
	private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialog;

			 /// <summary>
			 /// Required method for Designer support - do not modify
			 /// the contents of this method with the code editor.
			 /// </summary>
#pragma region Windows Form Designer generated code
			 void InitializeComponent( void )
			 {
				 this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
				 this->RemoveAll_Button = (gcnew System::Windows::Forms::Button());
				 this->AddAll_Button = (gcnew System::Windows::Forms::Button());
				 this->Remove_Button = (gcnew System::Windows::Forms::Button());
				 this->Add_Button = (gcnew System::Windows::Forms::Button());
				 this->USE_PROCESSER_ListBox = (gcnew System::Windows::Forms::ListBox());
				 this->label2 = (gcnew System::Windows::Forms::Label());
				 this->ALL_PROCESSER_ListBox = (gcnew System::Windows::Forms::ListBox());
				 this->label1 = (gcnew System::Windows::Forms::Label());
				 this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
				 this->Remove_Path_Button = (gcnew System::Windows::Forms::Button());
				 this->Add_Path_Button = (gcnew System::Windows::Forms::Button());
				 this->Path_ListBox = (gcnew System::Windows::Forms::ListBox());
				 this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
				 this->cTotal_ProcessCtrlEx = (gcnew sqr_tools::CProcessCtrlEx());
				 this->cCur_ProcessCtrlEx = (gcnew sqr_tools::CProcessCtrlEx());
				 this->Process_Button = (gcnew System::Windows::Forms::Button());
				 this->label4 = (gcnew System::Windows::Forms::Label());
				 this->label3 = (gcnew System::Windows::Forms::Label());
				 this->folderBrowserDialog = (gcnew System::Windows::Forms::FolderBrowserDialog());
				 this->groupBox1->SuspendLayout();
				 this->groupBox2->SuspendLayout();
				 this->groupBox3->SuspendLayout();
				 this->SuspendLayout();
				 // 
				 // groupBox1
				 // 
				 this->groupBox1->Controls->Add(this->RemoveAll_Button);
				 this->groupBox1->Controls->Add(this->AddAll_Button);
				 this->groupBox1->Controls->Add(this->Remove_Button);
				 this->groupBox1->Controls->Add(this->Add_Button);
				 this->groupBox1->Controls->Add(this->USE_PROCESSER_ListBox);
				 this->groupBox1->Controls->Add(this->label2);
				 this->groupBox1->Controls->Add(this->ALL_PROCESSER_ListBox);
				 this->groupBox1->Controls->Add(this->label1);
				 this->groupBox1->Location = System::Drawing::Point(4, 9);
				 this->groupBox1->Name = L"groupBox1";
				 this->groupBox1->Size = System::Drawing::Size(492, 214);
				 this->groupBox1->TabIndex = 0;
				 this->groupBox1->TabStop = false;
				 this->groupBox1->Text = L"过滤器";
				 // 
				 // RemoveAll_Button
				 // 
				 this->RemoveAll_Button->Location = System::Drawing::Point(208, 169);
				 this->RemoveAll_Button->Name = L"RemoveAll_Button";
				 this->RemoveAll_Button->Size = System::Drawing::Size(75, 24);
				 this->RemoveAll_Button->TabIndex = 7;
				 this->RemoveAll_Button->Text = L"<<==";
				 this->RemoveAll_Button->UseVisualStyleBackColor = true;
				 this->RemoveAll_Button->Click += gcnew System::EventHandler(this, &CFProcessCtrl::RemoveAll_Button_Click);
				 // 
				 // AddAll_Button
				 // 
				 this->AddAll_Button->Location = System::Drawing::Point(208, 130);
				 this->AddAll_Button->Name = L"AddAll_Button";
				 this->AddAll_Button->Size = System::Drawing::Size(75, 24);
				 this->AddAll_Button->TabIndex = 6;
				 this->AddAll_Button->Text = L"==>>";
				 this->AddAll_Button->UseVisualStyleBackColor = true;
				 this->AddAll_Button->Click += gcnew System::EventHandler(this, &CFProcessCtrl::AddAll_Button_Click);
				 // 
				 // Remove_Button
				 // 
				 this->Remove_Button->Location = System::Drawing::Point(208, 91);
				 this->Remove_Button->Name = L"Remove_Button";
				 this->Remove_Button->Size = System::Drawing::Size(75, 24);
				 this->Remove_Button->TabIndex = 5;
				 this->Remove_Button->Text = L"<<--";
				 this->Remove_Button->UseVisualStyleBackColor = true;
				 this->Remove_Button->Click += gcnew System::EventHandler(this, &CFProcessCtrl::Remove_Button_Click);
				 // 
				 // Add_Button
				 // 
				 this->Add_Button->Location = System::Drawing::Point(208, 52);
				 this->Add_Button->Name = L"Add_Button";
				 this->Add_Button->Size = System::Drawing::Size(75, 24);
				 this->Add_Button->TabIndex = 4;
				 this->Add_Button->Text = L"-->>";
				 this->Add_Button->UseVisualStyleBackColor = true;
				 this->Add_Button->Click += gcnew System::EventHandler(this, &CFProcessCtrl::Add_Button_Click);
				 // 
				 // USE_PROCESSER_ListBox
				 // 
				 this->USE_PROCESSER_ListBox->FormattingEnabled = true;
				 this->USE_PROCESSER_ListBox->ItemHeight = 12;
				 this->USE_PROCESSER_ListBox->Location = System::Drawing::Point(322, 36);
				 this->USE_PROCESSER_ListBox->Name = L"USE_PROCESSER_ListBox";
				 this->USE_PROCESSER_ListBox->Size = System::Drawing::Size(162, 172);
				 this->USE_PROCESSER_ListBox->Sorted = true;
				 this->USE_PROCESSER_ListBox->TabIndex = 3;
				 // 
				 // label2
				 // 
				 this->label2->AutoSize = true;
				 this->label2->Location = System::Drawing::Point(320, 17);
				 this->label2->Name = L"label2";
				 this->label2->Size = System::Drawing::Size(125, 12);
				 this->label2->TabIndex = 2;
				 this->label2->Text = L"需要使用的文件过滤器";
				 // 
				 // ALL_PROCESSER_ListBox
				 // 
				 this->ALL_PROCESSER_ListBox->FormattingEnabled = true;
				 this->ALL_PROCESSER_ListBox->ItemHeight = 12;
				 this->ALL_PROCESSER_ListBox->Location = System::Drawing::Point(8, 38);
				 this->ALL_PROCESSER_ListBox->Name = L"ALL_PROCESSER_ListBox";
				 this->ALL_PROCESSER_ListBox->Size = System::Drawing::Size(162, 172);
				 this->ALL_PROCESSER_ListBox->Sorted = true;
				 this->ALL_PROCESSER_ListBox->TabIndex = 1;
				 // 
				 // label1
				 // 
				 this->label1->AutoSize = true;
				 this->label1->Location = System::Drawing::Point(6, 19);
				 this->label1->Name = L"label1";
				 this->label1->Size = System::Drawing::Size(89, 12);
				 this->label1->TabIndex = 0;
				 this->label1->Text = L"所有文件过滤器";
				 // 
				 // groupBox2
				 // 
				 this->groupBox2->Controls->Add(this->Remove_Path_Button);
				 this->groupBox2->Controls->Add(this->Add_Path_Button);
				 this->groupBox2->Controls->Add(this->Path_ListBox);
				 this->groupBox2->Location = System::Drawing::Point(7, 233);
				 this->groupBox2->Name = L"groupBox2";
				 this->groupBox2->Size = System::Drawing::Size(488, 140);
				 this->groupBox2->TabIndex = 1;
				 this->groupBox2->TabStop = false;
				 this->groupBox2->Text = L"批处理路径";
				 // 
				 // Remove_Path_Button
				 // 
				 this->Remove_Path_Button->Location = System::Drawing::Point(273, 107);
				 this->Remove_Path_Button->Name = L"Remove_Path_Button";
				 this->Remove_Path_Button->Size = System::Drawing::Size(75, 24);
				 this->Remove_Path_Button->TabIndex = 6;
				 this->Remove_Path_Button->Text = L"移除路径";
				 this->Remove_Path_Button->UseVisualStyleBackColor = true;
				 this->Remove_Path_Button->Click += gcnew System::EventHandler(this, &CFProcessCtrl::Remove_Path_Button_Click);
				 // 
				 // Add_Path_Button
				 // 
				 this->Add_Path_Button->Location = System::Drawing::Point(117, 107);
				 this->Add_Path_Button->Name = L"Add_Path_Button";
				 this->Add_Path_Button->Size = System::Drawing::Size(75, 24);
				 this->Add_Path_Button->TabIndex = 5;
				 this->Add_Path_Button->Text = L"添加路径";
				 this->Add_Path_Button->UseVisualStyleBackColor = true;
				 this->Add_Path_Button->Click += gcnew System::EventHandler(this, &CFProcessCtrl::Add_Path_Button_Click);
				 // 
				 // Path_ListBox
				 // 
				 this->Path_ListBox->FormattingEnabled = true;
				 this->Path_ListBox->ItemHeight = 12;
				 this->Path_ListBox->Location = System::Drawing::Point(10, 25);
				 this->Path_ListBox->Name = L"Path_ListBox";
				 this->Path_ListBox->Size = System::Drawing::Size(470, 76);
				 this->Path_ListBox->TabIndex = 0;
				 // 
				 // groupBox3
				 // 
				 this->groupBox3->Controls->Add(this->cTotal_ProcessCtrlEx);
				 this->groupBox3->Controls->Add(this->cCur_ProcessCtrlEx);
				 this->groupBox3->Controls->Add(this->Process_Button);
				 this->groupBox3->Controls->Add(this->label4);
				 this->groupBox3->Controls->Add(this->label3);
				 this->groupBox3->Location = System::Drawing::Point(10, 384);
				 this->groupBox3->Name = L"groupBox3";
				 this->groupBox3->Size = System::Drawing::Size(484, 120);
				 this->groupBox3->TabIndex = 2;
				 this->groupBox3->TabStop = false;
				 this->groupBox3->Text = L"处理进度";
				 // 
				 // cTotal_ProcessCtrlEx
				 // 
				 this->cTotal_ProcessCtrlEx->Location = System::Drawing::Point(61, 49);
				 this->cTotal_ProcessCtrlEx->Name = L"cTotal_ProcessCtrlEx";
				 this->cTotal_ProcessCtrlEx->Size = System::Drawing::Size(411, 37);
				 this->cTotal_ProcessCtrlEx->TabIndex = 6;
				 // 
				 // cCur_ProcessCtrlEx
				 // 
				 this->cCur_ProcessCtrlEx->Location = System::Drawing::Point(61, 16);
				 this->cCur_ProcessCtrlEx->Name = L"cCur_ProcessCtrlEx";
				 this->cCur_ProcessCtrlEx->Size = System::Drawing::Size(411, 37);
				 this->cCur_ProcessCtrlEx->TabIndex = 5;
				 // 
				 // Process_Button
				 // 
				 this->Process_Button->Location = System::Drawing::Point(200, 88);
				 this->Process_Button->Name = L"Process_Button";
				 this->Process_Button->Size = System::Drawing::Size(76, 26);
				 this->Process_Button->TabIndex = 4;
				 this->Process_Button->Text = L"开始处理";
				 this->Process_Button->UseVisualStyleBackColor = true;
				 this->Process_Button->Click += gcnew System::EventHandler(this, &CFProcessCtrl::Process_Button_Click);
				 // 
				 // label4
				 // 
				 this->label4->AutoSize = true;
				 this->label4->Location = System::Drawing::Point(7, 62);
				 this->label4->Name = L"label4";
				 this->label4->Size = System::Drawing::Size(53, 12);
				 this->label4->TabIndex = 2;
				 this->label4->Text = L"处理文件";
				 // 
				 // label3
				 // 
				 this->label3->AutoSize = true;
				 this->label3->Location = System::Drawing::Point(6, 28);
				 this->label3->Name = L"label3";
				 this->label3->Size = System::Drawing::Size(53, 12);
				 this->label3->TabIndex = 0;
				 this->label3->Text = L"过滤文件";
				 // 
				 // CFProcessCtrl
				 // 
				 this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
				 this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				 this->Controls->Add(this->groupBox3);
				 this->Controls->Add(this->groupBox2);
				 this->Controls->Add(this->groupBox1);
				 this->Name = L"CFProcessCtrl";
				 this->Size = System::Drawing::Size(506, 512);
				 this->Load += gcnew System::EventHandler(this, &CFProcessCtrl::CFProcessCtrl_Load);
				 this->groupBox1->ResumeLayout(false);
				 this->groupBox1->PerformLayout();
				 this->groupBox2->ResumeLayout(false);
				 this->groupBox3->ResumeLayout(false);
				 this->groupBox3->PerformLayout();
				 this->ResumeLayout(false);

			 }
#pragma endregion


	private: System::Void Add_Button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Remove_Button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void AddAll_Button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void RemoveAll_Button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Add_Path_Button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Remove_Path_Button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Process_Button_Click(System::Object^  sender, System::EventArgs^  e);

			 void InitCtrlValue();

	public: void virtual SetCurProgressCtrl(void* ctrl)
			{
				//this->cCurProgressCtrl->SetCoreCtrl(ctrl);
			}
	public: void virtual SetTotalProgressCtrl(void* ctrl)
			{
				//this->cTotalProgressCtrl->SetCoreCtrl(ctrl);
			}
	public:  virtual void Update(void) override;
	public:  virtual char*	GetImpTypeName(void)	override;
	private: System::Void CFProcessCtrl_Load(System::Object^  sender, System::EventArgs^  e);
};
}
