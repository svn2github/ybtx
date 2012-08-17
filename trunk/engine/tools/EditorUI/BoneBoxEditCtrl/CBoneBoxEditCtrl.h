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
	/// Summary for CBoneBoxEditCtrl
	/// </summary>
	public ref class CBoneBoxEditCtrl : public CUIBase
	{
	public:
		CBoneBoxEditCtrl(void)
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
		~CBoneBoxEditCtrl()
		{
			if (components)
			{
				delete components;
			}
		}

	public:  virtual void Update(void) override;
	public:	 virtual void*	GetImpParam(void)		override;
	public:  virtual char*	GetImpTypeName(void)	override;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	private: System::Windows::Forms::TreeView^  BoneListTreeView;













	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip;
	private: System::Windows::Forms::ToolStripMenuItem^  É¾³ýToolStripMenuItem;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel2;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel3;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::NumericUpDown^  XMaxNumericUpDown;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::NumericUpDown^  XMinNumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  YMaxNumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  YMinNumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  ZMaxNumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  ZMinNumericUpDown;
	private: System::Windows::Forms::CheckBox^  showBox_CheckBox;




	private: System::ComponentModel::IContainer^  components;

	protected: 

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
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->BoneListTreeView = (gcnew System::Windows::Forms::TreeView());
			this->contextMenuStrip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->É¾³ýToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel3 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->XMaxNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->XMinNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->YMaxNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->YMinNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->ZMaxNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->ZMinNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->showBox_CheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox1->SuspendLayout();
			this->tableLayoutPanel1->SuspendLayout();
			this->contextMenuStrip->SuspendLayout();
			this->tableLayoutPanel2->SuspendLayout();
			this->tableLayoutPanel3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->XMaxNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->XMinNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->YMaxNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->YMinNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ZMaxNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ZMinNumericUpDown))->BeginInit();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->tableLayoutPanel1);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->groupBox1->Location = System::Drawing::Point(0, 0);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(296, 245);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"¹Ç÷ÀÏ¸½Ú±à¼­";
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Single;
			this->tableLayoutPanel1->ColumnCount = 2;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				44.9827F)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				55.0173F)));
			this->tableLayoutPanel1->Controls->Add(this->BoneListTreeView, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->tableLayoutPanel2, 1, 0);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(3, 17);
			this->tableLayoutPanel1->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 1;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(290, 225);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// BoneListTreeView
			// 
			this->BoneListTreeView->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->BoneListTreeView->ContextMenuStrip = this->contextMenuStrip;
			this->BoneListTreeView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->BoneListTreeView->Location = System::Drawing::Point(4, 4);
			this->BoneListTreeView->Name = L"BoneListTreeView";
			this->BoneListTreeView->Size = System::Drawing::Size(123, 217);
			this->BoneListTreeView->TabIndex = 0;
			this->BoneListTreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &CBoneBoxEditCtrl::BoneListTreeView_AfterSelect);
			this->BoneListTreeView->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &CBoneBoxEditCtrl::BoneListTreeView_MouseDown);
			// 
			// contextMenuStrip
			// 
			this->contextMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->É¾³ýToolStripMenuItem});
			this->contextMenuStrip->Name = L"contextMenuStrip";
			this->contextMenuStrip->Size = System::Drawing::Size(99, 26);
			// 
			// É¾³ýToolStripMenuItem
			// 
			this->É¾³ýToolStripMenuItem->Name = L"É¾³ýToolStripMenuItem";
			this->É¾³ýToolStripMenuItem->Size = System::Drawing::Size(98, 22);
			this->É¾³ýToolStripMenuItem->Text = L"É¾³ý";
			this->É¾³ýToolStripMenuItem->Click += gcnew System::EventHandler(this, &CBoneBoxEditCtrl::É¾³ýToolStripMenuItem_Click);
			// 
			// tableLayoutPanel2
			// 
			this->tableLayoutPanel2->ColumnCount = 1;
			this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				100)));
			this->tableLayoutPanel2->Controls->Add(this->tableLayoutPanel3, 0, 0);
			this->tableLayoutPanel2->Controls->Add(this->showBox_CheckBox, 0, 1);
			this->tableLayoutPanel2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel2->Location = System::Drawing::Point(134, 4);
			this->tableLayoutPanel2->Name = L"tableLayoutPanel2";
			this->tableLayoutPanel2->RowCount = 3;
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 132)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel2->Size = System::Drawing::Size(152, 217);
			this->tableLayoutPanel2->TabIndex = 1;
			// 
			// tableLayoutPanel3
			// 
			this->tableLayoutPanel3->ColumnCount = 2;
			this->tableLayoutPanel3->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute, 
				50)));
			this->tableLayoutPanel3->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				100)));
			this->tableLayoutPanel3->Controls->Add(this->label1, 0, 0);
			this->tableLayoutPanel3->Controls->Add(this->XMaxNumericUpDown, 1, 0);
			this->tableLayoutPanel3->Controls->Add(this->label2, 0, 1);
			this->tableLayoutPanel3->Controls->Add(this->label3, 0, 2);
			this->tableLayoutPanel3->Controls->Add(this->label4, 0, 3);
			this->tableLayoutPanel3->Controls->Add(this->label5, 0, 4);
			this->tableLayoutPanel3->Controls->Add(this->label6, 0, 5);
			this->tableLayoutPanel3->Controls->Add(this->XMinNumericUpDown, 1, 1);
			this->tableLayoutPanel3->Controls->Add(this->YMaxNumericUpDown, 1, 2);
			this->tableLayoutPanel3->Controls->Add(this->YMinNumericUpDown, 1, 3);
			this->tableLayoutPanel3->Controls->Add(this->ZMaxNumericUpDown, 1, 4);
			this->tableLayoutPanel3->Controls->Add(this->ZMinNumericUpDown, 1, 5);
			this->tableLayoutPanel3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel3->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel3->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel3->Name = L"tableLayoutPanel3";
			this->tableLayoutPanel3->RowCount = 6;
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 22)));
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 22)));
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 22)));
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 22)));
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 22)));
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 22)));
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel3->Size = System::Drawing::Size(152, 132);
			this->tableLayoutPanel3->TabIndex = 2;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label1->Location = System::Drawing::Point(0, 0);
			this->label1->Margin = System::Windows::Forms::Padding(0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(50, 22);
			this->label1->TabIndex = 0;
			this->label1->Text = L"×î´óX:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// XMaxNumericUpDown
			// 
			this->XMaxNumericUpDown->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->XMaxNumericUpDown->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->XMaxNumericUpDown->DecimalPlaces = 5;
			this->XMaxNumericUpDown->Location = System::Drawing::Point(53, 3);
			this->XMaxNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, System::Int32::MinValue});
			this->XMaxNumericUpDown->Name = L"XMaxNumericUpDown";
			this->XMaxNumericUpDown->Size = System::Drawing::Size(96, 17);
			this->XMaxNumericUpDown->TabIndex = 1;
			this->XMaxNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &CBoneBoxEditCtrl::XMaxNumericUpDown_ValueChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label2->Location = System::Drawing::Point(0, 22);
			this->label2->Margin = System::Windows::Forms::Padding(0);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(50, 22);
			this->label2->TabIndex = 2;
			this->label2->Text = L"×îÐ¡X:";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label3->Location = System::Drawing::Point(0, 44);
			this->label3->Margin = System::Windows::Forms::Padding(0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(50, 22);
			this->label3->TabIndex = 3;
			this->label3->Text = L"×î´óY:";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label4->Location = System::Drawing::Point(0, 66);
			this->label4->Margin = System::Windows::Forms::Padding(0);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(50, 22);
			this->label4->TabIndex = 3;
			this->label4->Text = L"×îÐ¡Y:";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label5->Location = System::Drawing::Point(0, 88);
			this->label5->Margin = System::Windows::Forms::Padding(0);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(50, 22);
			this->label5->TabIndex = 3;
			this->label5->Text = L"×î´óZ:";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label6->Location = System::Drawing::Point(0, 110);
			this->label6->Margin = System::Windows::Forms::Padding(0);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(50, 22);
			this->label6->TabIndex = 3;
			this->label6->Text = L"×îÐ¡Z:";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// XMinNumericUpDown
			// 
			this->XMinNumericUpDown->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->XMinNumericUpDown->DecimalPlaces = 5;
			this->XMinNumericUpDown->Dock = System::Windows::Forms::DockStyle::Fill;
			this->XMinNumericUpDown->Location = System::Drawing::Point(53, 25);
			this->XMinNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, System::Int32::MinValue});
			this->XMinNumericUpDown->Name = L"XMinNumericUpDown";
			this->XMinNumericUpDown->Size = System::Drawing::Size(96, 17);
			this->XMinNumericUpDown->TabIndex = 4;
			this->XMinNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &CBoneBoxEditCtrl::XMinNumericUpDown_ValueChanged);
			// 
			// YMaxNumericUpDown
			// 
			this->YMaxNumericUpDown->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->YMaxNumericUpDown->DecimalPlaces = 5;
			this->YMaxNumericUpDown->Dock = System::Windows::Forms::DockStyle::Fill;
			this->YMaxNumericUpDown->Location = System::Drawing::Point(53, 47);
			this->YMaxNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, System::Int32::MinValue});
			this->YMaxNumericUpDown->Name = L"YMaxNumericUpDown";
			this->YMaxNumericUpDown->Size = System::Drawing::Size(96, 17);
			this->YMaxNumericUpDown->TabIndex = 5;
			this->YMaxNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &CBoneBoxEditCtrl::YMaxNumericUpDown_ValueChanged);
			// 
			// YMinNumericUpDown
			// 
			this->YMinNumericUpDown->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->YMinNumericUpDown->DecimalPlaces = 5;
			this->YMinNumericUpDown->Dock = System::Windows::Forms::DockStyle::Fill;
			this->YMinNumericUpDown->Location = System::Drawing::Point(53, 69);
			this->YMinNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, System::Int32::MinValue});
			this->YMinNumericUpDown->Name = L"YMinNumericUpDown";
			this->YMinNumericUpDown->Size = System::Drawing::Size(96, 17);
			this->YMinNumericUpDown->TabIndex = 6;
			this->YMinNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &CBoneBoxEditCtrl::YMinNumericUpDown_ValueChanged);
			// 
			// ZMaxNumericUpDown
			// 
			this->ZMaxNumericUpDown->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->ZMaxNumericUpDown->DecimalPlaces = 5;
			this->ZMaxNumericUpDown->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ZMaxNumericUpDown->Location = System::Drawing::Point(53, 91);
			this->ZMaxNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, System::Int32::MinValue});
			this->ZMaxNumericUpDown->Name = L"ZMaxNumericUpDown";
			this->ZMaxNumericUpDown->Size = System::Drawing::Size(96, 17);
			this->ZMaxNumericUpDown->TabIndex = 7;
			this->ZMaxNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &CBoneBoxEditCtrl::ZMaxNumericUpDown_ValueChanged);
			// 
			// ZMinNumericUpDown
			// 
			this->ZMinNumericUpDown->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->ZMinNumericUpDown->DecimalPlaces = 5;
			this->ZMinNumericUpDown->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ZMinNumericUpDown->Location = System::Drawing::Point(53, 113);
			this->ZMinNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, System::Int32::MinValue});
			this->ZMinNumericUpDown->Name = L"ZMinNumericUpDown";
			this->ZMinNumericUpDown->Size = System::Drawing::Size(96, 17);
			this->ZMinNumericUpDown->TabIndex = 8;
			this->ZMinNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &CBoneBoxEditCtrl::ZMinNumericUpDown_ValueChanged);
			// 
			// showBox_CheckBox
			// 
			this->showBox_CheckBox->AutoSize = true;
			this->showBox_CheckBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->showBox_CheckBox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->showBox_CheckBox->Location = System::Drawing::Point(7, 135);
			this->showBox_CheckBox->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->showBox_CheckBox->Name = L"showBox_CheckBox";
			this->showBox_CheckBox->Size = System::Drawing::Size(142, 24);
			this->showBox_CheckBox->TabIndex = 3;
			this->showBox_CheckBox->Text = L"ÏÔÊ¾°üÎ§ºÐ";
			this->showBox_CheckBox->UseVisualStyleBackColor = true;
			this->showBox_CheckBox->CheckedChanged += gcnew System::EventHandler(this, &CBoneBoxEditCtrl::showBox_CheckBox_CheckedChanged);
			// 
			// CBoneBoxEditCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->groupBox1);
			this->Name = L"CBoneBoxEditCtrl";
			this->Size = System::Drawing::Size(296, 245);
			this->groupBox1->ResumeLayout(false);
			this->tableLayoutPanel1->ResumeLayout(false);
			this->contextMenuStrip->ResumeLayout(false);
			this->tableLayoutPanel2->ResumeLayout(false);
			this->tableLayoutPanel2->PerformLayout();
			this->tableLayoutPanel3->ResumeLayout(false);
			this->tableLayoutPanel3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->XMaxNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->XMinNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->YMaxNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->YMinNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ZMaxNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ZMinNumericUpDown))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
		private: System::Void BoneListTreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
		private: System::Void XMaxNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void XMinNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void YMaxNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void YMinNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void ZMaxNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void ZMinNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void É¾³ýToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void showBox_CheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void BoneListTreeView_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
};
}
