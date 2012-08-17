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
	/// Summary for CPieceUnitEditCtrl
	/// </summary>
	public ref class CPieceUnitEditCtrl : public CUIBase
	{
	public:
		CPieceUnitEditCtrl(void)
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
		~CPieceUnitEditCtrl()
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


	private: System::Windows::Forms::TreeView^  PieceUnitTreeView;







	private: System::Windows::Forms::ImageList^  pieceUnit_imageList;
	private: System::Windows::Forms::GroupBox^  groupBox1;































	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip;
	private: System::Windows::Forms::ToolStripMenuItem^  增加ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  删除ToolStripMenuItem;



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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(CPieceUnitEditCtrl::typeid));
			this->PieceUnitTreeView = (gcnew System::Windows::Forms::TreeView());
			this->contextMenuStrip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->增加ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->删除ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pieceUnit_imageList = (gcnew System::Windows::Forms::ImageList(this->components));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->contextMenuStrip->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// PieceUnitTreeView
			// 
			this->PieceUnitTreeView->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->PieceUnitTreeView->ContextMenuStrip = this->contextMenuStrip;
			this->PieceUnitTreeView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->PieceUnitTreeView->ImageIndex = 0;
			this->PieceUnitTreeView->ImageList = this->pieceUnit_imageList;
			this->PieceUnitTreeView->LabelEdit = true;
			this->PieceUnitTreeView->Location = System::Drawing::Point(3, 17);
			this->PieceUnitTreeView->Name = L"PieceUnitTreeView";
			this->PieceUnitTreeView->SelectedImageIndex = 0;
			this->PieceUnitTreeView->Size = System::Drawing::Size(173, 223);
			this->PieceUnitTreeView->TabIndex = 1;
			this->PieceUnitTreeView->AfterLabelEdit += gcnew System::Windows::Forms::NodeLabelEditEventHandler(this, &CPieceUnitEditCtrl::PieceUnitTreeView_AfterLabelEdit);
			this->PieceUnitTreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &CPieceUnitEditCtrl::TreeNode_AfterSelect);
			this->PieceUnitTreeView->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &CPieceUnitEditCtrl::PieceUnitTreeView_KeyDown);
			this->PieceUnitTreeView->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &CPieceUnitEditCtrl::PieceUnitTreeView_MouseDown);
			// 
			// contextMenuStrip
			// 
			this->contextMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->增加ToolStripMenuItem, 
				this->删除ToolStripMenuItem});
			this->contextMenuStrip->Name = L"contextMenuStrip";
			this->contextMenuStrip->Size = System::Drawing::Size(99, 48);
			// 
			// 增加ToolStripMenuItem
			// 
			this->增加ToolStripMenuItem->Name = L"增加ToolStripMenuItem";
			this->增加ToolStripMenuItem->Size = System::Drawing::Size(98, 22);
			this->增加ToolStripMenuItem->Text = L"增加";
			this->增加ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CPieceUnitEditCtrl::增加ToolStripMenuItem_Click);
			// 
			// 删除ToolStripMenuItem
			// 
			this->删除ToolStripMenuItem->Name = L"删除ToolStripMenuItem";
			this->删除ToolStripMenuItem->Size = System::Drawing::Size(98, 22);
			this->删除ToolStripMenuItem->Text = L"删除";
			this->删除ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CPieceUnitEditCtrl::删除ToolStripMenuItem_Click);
			// 
			// pieceUnit_imageList
			// 
			this->pieceUnit_imageList->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"pieceUnit_imageList.ImageStream")));
			this->pieceUnit_imageList->TransparentColor = System::Drawing::Color::Transparent;
			this->pieceUnit_imageList->Images->SetKeyName(0, L"piecegro.png");
			this->pieceUnit_imageList->Images->SetKeyName(1, L"piecers.PNG");
			this->pieceUnit_imageList->Images->SetKeyName(2, L"singlepi.PNG");
			this->pieceUnit_imageList->Images->SetKeyName(3, L"rstexture.PNG");
			this->pieceUnit_imageList->Images->SetKeyName(4, L"VERANI.PNG");
			this->pieceUnit_imageList->Images->SetKeyName(5, L"verani1.PNG");
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->PieceUnitTreeView);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->groupBox1->Location = System::Drawing::Point(0, 0);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(179, 243);
			this->groupBox1->TabIndex = 5;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Piece列表";
			// 
			// CPieceUnitEditCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->groupBox1);
			this->Margin = System::Windows::Forms::Padding(0);
			this->Name = L"CPieceUnitEditCtrl";
			this->Size = System::Drawing::Size(179, 243);
			this->contextMenuStrip->ResumeLayout(false);
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void UpdateNodes(void);
	private: System::Void TreeNode_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
	private: System::Void PieceUnitTreeView_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	private: System::Void PieceUnitTreeView_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void 增加ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void 删除ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void PieceUnitTreeView_AfterLabelEdit(System::Object^  sender, System::Windows::Forms::NodeLabelEditEventArgs^  e);
};
}
