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
	/// Summary for CSetViewCtrl
	/// </summary>
	public ref class CSetViewCtrl : public CUIBase
	{
	public:
		CSetViewCtrl(void)
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
		~CSetViewCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::TreeView^  objectReset_treeView;
	protected: 

	private: System::Windows::Forms::Button^  deleteReset_button;

	private: System::Windows::Forms::Button^  insertReset_button;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::RadioButton^  noMirror_radioButton;

	private: System::Windows::Forms::RadioButton^  mirrorZ_radioButton;

	private: System::Windows::Forms::RadioButton^  mirrorX_radioButton;

	private: System::Windows::Forms::Button^  editRS_button;

	private: System::Windows::Forms::Button^  nextRS_button;

	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::RadioButton^  copyAll_radioButton;


	private: System::Windows::Forms::RadioButton^  copyTerrain_radioButton;

	private: System::Windows::Forms::RadioButton^  copyObject_radioButton;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog;

	private: System::Windows::Forms::ImageList^  modelTree_imageList;
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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(CSetViewCtrl::typeid));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->objectReset_treeView = (gcnew System::Windows::Forms::TreeView());
			this->modelTree_imageList = (gcnew System::Windows::Forms::ImageList(this->components));
			this->deleteReset_button = (gcnew System::Windows::Forms::Button());
			this->insertReset_button = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->noMirror_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->mirrorZ_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->mirrorX_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->editRS_button = (gcnew System::Windows::Forms::Button());
			this->nextRS_button = (gcnew System::Windows::Forms::Button());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->copyAll_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->copyTerrain_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->copyObject_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->openFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->objectReset_treeView);
			this->groupBox1->Controls->Add(this->deleteReset_button);
			this->groupBox1->Controls->Add(this->insertReset_button);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Location = System::Drawing::Point(4, 3);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(292, 346);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"图素包";
			// 
			// objectReset_treeView
			// 
			this->objectReset_treeView->ImageIndex = 0;
			this->objectReset_treeView->ImageList = this->modelTree_imageList;
			this->objectReset_treeView->Location = System::Drawing::Point(4, 56);
			this->objectReset_treeView->Name = L"objectReset_treeView";
			this->objectReset_treeView->SelectedImageIndex = 0;
			this->objectReset_treeView->Size = System::Drawing::Size(285, 285);
			this->objectReset_treeView->TabIndex = 3;
			this->objectReset_treeView->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &CSetViewCtrl::objectReset_treeView_MouseUp);

			// 
			// modelTree_imageList
			// 
			this->modelTree_imageList->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"modelTree_imageList.ImageStream")));
			this->modelTree_imageList->TransparentColor = System::Drawing::Color::Transparent;
			this->modelTree_imageList->Images->SetKeyName(0, L"ico00001.ico");
			this->modelTree_imageList->Images->SetKeyName(1, L"flodersel.ico");
			this->modelTree_imageList->Images->SetKeyName(2, L"3dunit.ico");
			this->modelTree_imageList->Images->SetKeyName(3, L"3dunitsel.ico");
			// 
			// deleteReset_button
			// 
			this->deleteReset_button->Location = System::Drawing::Point(183, 17);
			this->deleteReset_button->Name = L"deleteReset_button";
			this->deleteReset_button->Size = System::Drawing::Size(84, 33);
			this->deleteReset_button->TabIndex = 2;
			this->deleteReset_button->Text = L"删除图素包";
			this->deleteReset_button->UseVisualStyleBackColor = true;
			this->deleteReset_button->Click += gcnew System::EventHandler(this, &CSetViewCtrl::deleteReset_button_Click);
			// 
			// insertReset_button
			// 
			this->insertReset_button->Location = System::Drawing::Point(93, 17);
			this->insertReset_button->Name = L"insertReset_button";
			this->insertReset_button->Size = System::Drawing::Size(84, 33);
			this->insertReset_button->TabIndex = 1;
			this->insertReset_button->Text = L"添加图素包";
			this->insertReset_button->UseVisualStyleBackColor = true;
			this->insertReset_button->Click += gcnew System::EventHandler(this, &CSetViewCtrl::insertReset_button_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(14, 27);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(77, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"地图图素包：";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->noMirror_radioButton);
			this->groupBox2->Controls->Add(this->mirrorZ_radioButton);
			this->groupBox2->Controls->Add(this->mirrorX_radioButton);
			this->groupBox2->Controls->Add(this->editRS_button);
			this->groupBox2->Controls->Add(this->nextRS_button);
			this->groupBox2->Location = System::Drawing::Point(4, 351);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(292, 72);
			this->groupBox2->TabIndex = 1;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Props";
			// 
			// noMirror_radioButton
			// 
			this->noMirror_radioButton->AutoSize = true;
			this->noMirror_radioButton->Checked = true;
			this->noMirror_radioButton->Location = System::Drawing::Point(196, 49);
			this->noMirror_radioButton->Name = L"noMirror_radioButton";
			this->noMirror_radioButton->Size = System::Drawing::Size(59, 16);
			this->noMirror_radioButton->TabIndex = 4;
			this->noMirror_radioButton->TabStop = true;
			this->noMirror_radioButton->Text = L"无镜像";
			this->noMirror_radioButton->UseVisualStyleBackColor = true;
			this->noMirror_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CSetViewCtrl::noMirror_radioButton_CheckedChanged);
			// 
			// mirrorZ_radioButton
			// 
			this->mirrorZ_radioButton->AutoSize = true;
			this->mirrorZ_radioButton->Location = System::Drawing::Point(106, 49);
			this->mirrorZ_radioButton->Name = L"mirrorZ_radioButton";
			this->mirrorZ_radioButton->Size = System::Drawing::Size(71, 16);
			this->mirrorZ_radioButton->TabIndex = 3;
			this->mirrorZ_radioButton->Text = L"左右镜像";
			this->mirrorZ_radioButton->UseVisualStyleBackColor = true;
			this->mirrorZ_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CSetViewCtrl::mirrorZ_radioButton_CheckedChanged);
			// 
			// mirrorX_radioButton
			// 
			this->mirrorX_radioButton->AutoSize = true;
			this->mirrorX_radioButton->Location = System::Drawing::Point(16, 49);
			this->mirrorX_radioButton->Name = L"mirrorX_radioButton";
			this->mirrorX_radioButton->Size = System::Drawing::Size(71, 16);
			this->mirrorX_radioButton->TabIndex = 2;
			this->mirrorX_radioButton->Text = L"前后镜像";
			this->mirrorX_radioButton->UseVisualStyleBackColor = true;
			this->mirrorX_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CSetViewCtrl::mirrorX_radioButton_CheckedChanged);
			// 
			// editRS_button
			// 
			this->editRS_button->Enabled = false;
			this->editRS_button->Location = System::Drawing::Point(172, 18);
			this->editRS_button->Name = L"editRS_button";
			this->editRS_button->Size = System::Drawing::Size(95, 25);
			this->editRS_button->TabIndex = 1;
			this->editRS_button->Text = L"编辑RS";
			this->editRS_button->UseVisualStyleBackColor = true;
			this->editRS_button->Click += gcnew System::EventHandler(this, &CSetViewCtrl::editRS_button_Click);
			// 
			// nextRS_button
			// 
			this->nextRS_button->Location = System::Drawing::Point(16, 18);
			this->nextRS_button->Name = L"nextRS_button";
			this->nextRS_button->Size = System::Drawing::Size(95, 25);
			this->nextRS_button->TabIndex = 0;
			this->nextRS_button->Text = L"切换渲染类型";
			this->nextRS_button->UseVisualStyleBackColor = true;
			this->nextRS_button->Click += gcnew System::EventHandler(this, &CSetViewCtrl::nextRS_button_Click);
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->copyAll_radioButton);
			this->groupBox3->Controls->Add(this->copyTerrain_radioButton);
			this->groupBox3->Controls->Add(this->copyObject_radioButton);
			this->groupBox3->Location = System::Drawing::Point(7, 428);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(288, 46);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Copy";
			// 
			// copyAll_radioButton
			// 
			this->copyAll_radioButton->AutoSize = true;
			this->copyAll_radioButton->Location = System::Drawing::Point(193, 20);
			this->copyAll_radioButton->Name = L"copyAll_radioButton";
			this->copyAll_radioButton->Size = System::Drawing::Size(71, 16);
			this->copyAll_radioButton->TabIndex = 2;
			this->copyAll_radioButton->Text = L"复制全部";
			this->copyAll_radioButton->UseVisualStyleBackColor = true;
			this->copyAll_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CSetViewCtrl::copyAll_radioButton_CheckedChanged);
			// 
			// copyTerrain_radioButton
			// 
			this->copyTerrain_radioButton->AutoSize = true;
			this->copyTerrain_radioButton->Location = System::Drawing::Point(103, 20);
			this->copyTerrain_radioButton->Name = L"copyTerrain_radioButton";
			this->copyTerrain_radioButton->Size = System::Drawing::Size(71, 16);
			this->copyTerrain_radioButton->TabIndex = 1;
			this->copyTerrain_radioButton->Text = L"复制地表";
			this->copyTerrain_radioButton->UseVisualStyleBackColor = true;
			this->copyTerrain_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CSetViewCtrl::copyTerrain_radioButton_CheckedChanged);
			// 
			// copyObject_radioButton
			// 
			this->copyObject_radioButton->AutoSize = true;
			this->copyObject_radioButton->Checked = true;
			this->copyObject_radioButton->Location = System::Drawing::Point(13, 20);
			this->copyObject_radioButton->Name = L"copyObject_radioButton";
			this->copyObject_radioButton->Size = System::Drawing::Size(71, 16);
			this->copyObject_radioButton->TabIndex = 0;
			this->copyObject_radioButton->TabStop = true;
			this->copyObject_radioButton->Text = L"复制物件";
			this->copyObject_radioButton->UseVisualStyleBackColor = true;
			this->copyObject_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CSetViewCtrl::copyObject_radioButton_CheckedChanged);
			// 
			// openFileDialog
			// 
			this->openFileDialog->FileName = L"openFileDialog";
			// 
			// CSetViewCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Name = L"CSetViewCtrl";
			this->Size = System::Drawing::Size(300, 480);
			this->Load += gcnew System::EventHandler(this, &CSetViewCtrl::CSetViewCtrl_Load);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void insertReset_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void deleteReset_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void selectObjectTreeViewNode(TreeNode ^seleceNode);
	private: System::Void nextRS_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void editRS_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void mirrorX_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void mirrorZ_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void noMirror_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void copyObject_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void copyTerrain_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void copyAll_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

	private: void InsertObjectResetTreeView(String^ resetname);
	private: void RemoveOldSet(String^ resetname);

	public:  virtual void   Update(void) override;
	public:  virtual char*	GetImpTypeName(void)	override;
	public:  virtual void*	GetImpParam(void)		override;
	private: System::Void	CSetViewCtrl_Load(System::Object^  sender, System::EventArgs^  e);
private: System::Void objectReset_treeView_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
};

}
