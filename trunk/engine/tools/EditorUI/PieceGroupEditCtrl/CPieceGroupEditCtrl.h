#pragma once

#include "CUIBase.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;
using namespace System::Text;
using namespace System::Runtime::InteropServices;

namespace sqr_tools {

	/// <summary>
	/// Summary for CPieceGroupEditCtrl
	/// </summary>
	public ref class CPieceGroupEditCtrl : public CUIBase
	{
	public:
		CPieceGroupEditCtrl(void)
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
		~CPieceGroupEditCtrl()
		{
			if (components)
			{
				delete components;
			}
		}

	public:
		virtual char*	GetImpTypeName(void)	override;
		virtual void*	GetImpParam(void)		override;
		virtual void	Update(void) override;

	private: System::Windows::Forms::TreeView^  PieceGroupTree;



	private: System::Windows::Forms::ImageList^  pieceGroup_imageList;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip;
	private: System::Windows::Forms::ToolStripMenuItem^  打开模型ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  保存模型ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  保存模型为ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  保存全部ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  关闭所有模型ToolStripMenuItem;

	private: System::ComponentModel::IContainer^  components;

	protected: 

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(CPieceGroupEditCtrl::typeid));
			this->PieceGroupTree = (gcnew System::Windows::Forms::TreeView());
			this->contextMenuStrip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->打开模型ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->保存模型ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->保存模型为ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->保存全部ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->关闭所有模型ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pieceGroup_imageList = (gcnew System::Windows::Forms::ImageList(this->components));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->contextMenuStrip->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// PieceGroupTree
			// 
			this->PieceGroupTree->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->PieceGroupTree->ContextMenuStrip = this->contextMenuStrip;
			this->PieceGroupTree->Dock = System::Windows::Forms::DockStyle::Fill;
			this->PieceGroupTree->ImageIndex = 0;
			this->PieceGroupTree->ImageList = this->pieceGroup_imageList;
			this->PieceGroupTree->Location = System::Drawing::Point(3, 17);
			this->PieceGroupTree->Name = L"PieceGroupTree";
			this->PieceGroupTree->SelectedImageIndex = 0;
			this->PieceGroupTree->Size = System::Drawing::Size(173, 223);
			this->PieceGroupTree->TabIndex = 0;
			this->PieceGroupTree->BeforeExpand += gcnew System::Windows::Forms::TreeViewCancelEventHandler(this, &CPieceGroupEditCtrl::PieceGroupTree_BeforeExpand);
			this->PieceGroupTree->DoubleClick += gcnew System::EventHandler(this, &CPieceGroupEditCtrl::TreeNode_DoubleClick);
			this->PieceGroupTree->BeforeCollapse += gcnew System::Windows::Forms::TreeViewCancelEventHandler(this, &CPieceGroupEditCtrl::PieceGroupTree_BeforeCollapse);
			this->PieceGroupTree->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &CPieceGroupEditCtrl::PieceGroupTree_MouseDown);
			// 
			// contextMenuStrip
			// 
			this->contextMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->打开模型ToolStripMenuItem, 
				this->保存模型ToolStripMenuItem, this->保存模型为ToolStripMenuItem, this->保存全部ToolStripMenuItem, this->关闭所有模型ToolStripMenuItem});
			this->contextMenuStrip->Name = L"contextMenuStrip";
			this->contextMenuStrip->Size = System::Drawing::Size(147, 114);
			// 
			// 打开模型ToolStripMenuItem
			// 
			this->打开模型ToolStripMenuItem->Name = L"打开模型ToolStripMenuItem";
			this->打开模型ToolStripMenuItem->Size = System::Drawing::Size(146, 22);
			this->打开模型ToolStripMenuItem->Text = L"打开模型";
			this->打开模型ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CPieceGroupEditCtrl::打开模型ToolStripMenuItem_Click);
			// 
			// 保存模型ToolStripMenuItem
			// 
			this->保存模型ToolStripMenuItem->Name = L"保存模型ToolStripMenuItem";
			this->保存模型ToolStripMenuItem->Size = System::Drawing::Size(146, 22);
			this->保存模型ToolStripMenuItem->Text = L"保存模型";
			this->保存模型ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CPieceGroupEditCtrl::保存模型ToolStripMenuItem_Click);
			// 
			// 保存模型为ToolStripMenuItem
			// 
			this->保存模型为ToolStripMenuItem->Name = L"保存模型为ToolStripMenuItem";
			this->保存模型为ToolStripMenuItem->Size = System::Drawing::Size(146, 22);
			this->保存模型为ToolStripMenuItem->Text = L"保存模型为";
			this->保存模型为ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CPieceGroupEditCtrl::保存模型为ToolStripMenuItem_Click);
			// 
			// 保存全部ToolStripMenuItem
			// 
			this->保存全部ToolStripMenuItem->Name = L"保存全部ToolStripMenuItem";
			this->保存全部ToolStripMenuItem->Size = System::Drawing::Size(146, 22);
			this->保存全部ToolStripMenuItem->Text = L"保存全部";
			this->保存全部ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CPieceGroupEditCtrl::保存全部ToolStripMenuItem_Click);
			// 
			// 关闭所有模型ToolStripMenuItem
			// 
			this->关闭所有模型ToolStripMenuItem->Name = L"关闭所有模型ToolStripMenuItem";
			this->关闭所有模型ToolStripMenuItem->Size = System::Drawing::Size(146, 22);
			this->关闭所有模型ToolStripMenuItem->Text = L"关闭所有模型";
			this->关闭所有模型ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CPieceGroupEditCtrl::关闭所有模型ToolStripMenuItem_Click);
			// 
			// pieceGroup_imageList
			// 
			this->pieceGroup_imageList->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"pieceGroup_imageList.ImageStream")));
			this->pieceGroup_imageList->TransparentColor = System::Drawing::Color::Transparent;
			this->pieceGroup_imageList->Images->SetKeyName(0, L"PGRoot.PNG");
			this->pieceGroup_imageList->Images->SetKeyName(1, L"piecegro.png");
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->PieceGroupTree);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->groupBox1->Location = System::Drawing::Point(0, 0);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(179, 243);
			this->groupBox1->TabIndex = 2;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"PieceGroup列表";
			// 
			// CPieceGroupEditCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->groupBox1);
			this->Margin = System::Windows::Forms::Padding(0);
			this->Name = L"CPieceGroupEditCtrl";
			this->Size = System::Drawing::Size(179, 243);
			this->contextMenuStrip->ResumeLayout(false);
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void TreeNode_DoubleClick(System::Object^  sender, System::EventArgs^  e);
	private: System::Void UpdateNode( std::vector<string> strVector, bool isReadOnly );
	public:  System::Void AddMesh(void);
	public:  System::Void SaveMesh(void);
	public:  System::Void SaveMeshAs(void);
	public:	 System::Void SaveAllMesh(void);
	public:  System::Void ClearAllMesh(void);
	private: System::Void PieceGroupTree_BeforeExpand(System::Object^  sender, System::Windows::Forms::TreeViewCancelEventArgs^  e);
	private: System::Void PieceGroupTree_BeforeCollapse(System::Object^  sender, System::Windows::Forms::TreeViewCancelEventArgs^  e);
	private: System::Void PieceGroupTree_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void 打开模型ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void 保存模型ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void 保存模型为ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void 保存全部ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void 关闭所有模型ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Boolean m_bExpand;
};
}
