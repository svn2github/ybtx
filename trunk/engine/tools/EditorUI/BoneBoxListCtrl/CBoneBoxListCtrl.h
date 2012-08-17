#pragma once

#include "CUIBase.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace sqr_tools {

	/// <summary>
	/// Summary for CBoneBoxListCtrl
	/// </summary>
	public ref class CBoneBoxListCtrl : public CUIBase
	{
	public:
		CBoneBoxListCtrl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

		virtual char*	GetImpTypeName(void)	override;
		virtual void*	GetImpParam(void)		override;
		virtual void	Update(void) override;

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CBoneBoxListCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;

	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip;
	private: System::Windows::Forms::ToolStripMenuItem^  删除ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  增加点选盒子ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  加载默认骨骼ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  头骨包围盒ToolStripMenuItem;
	private: System::Windows::Forms::ListView^  BoneBoxTreeView;
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
			this->BoneBoxTreeView = (gcnew System::Windows::Forms::ListView());
			this->contextMenuStrip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->删除ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->增加点选盒子ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->加载默认骨骼ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->头骨包围盒ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->groupBox1->SuspendLayout();
			this->contextMenuStrip->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->BoneBoxTreeView);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->groupBox1->Location = System::Drawing::Point(0, 0);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(235, 259);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"带有包围盒的骨骼";
			// 
			// BoneBoxTreeView
			// 
			this->BoneBoxTreeView->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->BoneBoxTreeView->ContextMenuStrip = this->contextMenuStrip;
			this->BoneBoxTreeView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->BoneBoxTreeView->Location = System::Drawing::Point(3, 17);
			this->BoneBoxTreeView->Name = L"BoneBoxTreeView";
			this->BoneBoxTreeView->Size = System::Drawing::Size(229, 239);
			this->BoneBoxTreeView->TabIndex = 0;
			this->BoneBoxTreeView->UseCompatibleStateImageBehavior = false;
			this->BoneBoxTreeView->View = System::Windows::Forms::View::List;
			this->BoneBoxTreeView->SelectedIndexChanged += gcnew System::EventHandler(this, &CBoneBoxListCtrl::BoneBoxTreeView_SelectedIndexChanged);
			// 
			// contextMenuStrip
			// 
			this->contextMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->删除ToolStripMenuItem, 
				this->增加点选盒子ToolStripMenuItem, this->加载默认骨骼ToolStripMenuItem, this->头骨包围盒ToolStripMenuItem});
			this->contextMenuStrip->Name = L"contextMenuStrip";
			this->contextMenuStrip->Size = System::Drawing::Size(147, 92);
			// 
			// 删除ToolStripMenuItem
			// 
			this->删除ToolStripMenuItem->Name = L"删除ToolStripMenuItem";
			this->删除ToolStripMenuItem->Size = System::Drawing::Size(146, 22);
			this->删除ToolStripMenuItem->Text = L"删除";
			this->删除ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CBoneBoxListCtrl::删除ToolStripMenuItem_Click);
			// 
			// 增加点选盒子ToolStripMenuItem
			// 
			this->增加点选盒子ToolStripMenuItem->Name = L"增加点选盒子ToolStripMenuItem";
			this->增加点选盒子ToolStripMenuItem->Size = System::Drawing::Size(146, 22);
			this->增加点选盒子ToolStripMenuItem->Text = L"增加点选盒子";
			this->增加点选盒子ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CBoneBoxListCtrl::增加点选盒子ToolStripMenuItem_Click);
			// 
			// 加载默认骨骼ToolStripMenuItem
			// 
			this->加载默认骨骼ToolStripMenuItem->Name = L"加载默认骨骼ToolStripMenuItem";
			this->加载默认骨骼ToolStripMenuItem->Size = System::Drawing::Size(146, 22);
			this->加载默认骨骼ToolStripMenuItem->Text = L"加载默认骨骼";
			this->加载默认骨骼ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CBoneBoxListCtrl::加载默认骨骼ToolStripMenuItem_Click);
			// 
			// 头骨包围盒ToolStripMenuItem
			// 
			this->头骨包围盒ToolStripMenuItem->Name = L"头骨包围盒ToolStripMenuItem";
			this->头骨包围盒ToolStripMenuItem->Size = System::Drawing::Size(146, 22);
			this->头骨包围盒ToolStripMenuItem->Text = L"头骨包围盒";
			this->头骨包围盒ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CBoneBoxListCtrl::头骨包围盒ToolStripMenuItem_Click);
			// 
			// CBoneBoxListCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->groupBox1);
			this->Name = L"CBoneBoxListCtrl";
			this->Size = System::Drawing::Size(235, 259);
			this->groupBox1->ResumeLayout(false);
			this->contextMenuStrip->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: ListViewItem^ FindNode(String^ value);
	private: System::Void BoneBoxTreeView_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void 删除ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void 增加点选盒子ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void 加载默认骨骼ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void 头骨包围盒ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
};
}
