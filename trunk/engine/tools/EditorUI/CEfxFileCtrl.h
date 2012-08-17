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
	/// Summary for CEfxFileCtrl
	/// </summary>
	public ref class CEfxFileCtrl :  public CUIBase//public System::Windows::Forms::UserControl
	{
	public:
		CEfxFileCtrl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			InitOtherComponent();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CEfxFileCtrl()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}
	private: System::Windows::Forms::TreeView^  EfxFiletreeView;
	private: System::Windows::Forms::Button^  OpenEfxFileFolder;
	private: System::Windows::Forms::Button^  UintTheEffect;
	private: System::Windows::Forms::Button^  ClearFillTree;
	private: System::Windows::Forms::Button^  SaveFile;
	private: System::Windows::Forms::Button^  SaveFileAs;
	private: System::Windows::Forms::Button^  CreateButton;
	private: System::Windows::Forms::TextBox^  NewNodeEditBox;
	private: System::Windows::Forms::OpenFileDialog^  EfxOpenFileDialog;
	private: System::Windows::Forms::TreeNode^ m_RootTreeNode;
	private: System::Windows::Forms::SaveFileDialog^  EfxSaveFileDialog;
	private: System::Windows::Forms::FolderBrowserDialog^  EfxFolderBrowserDialog;

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
			System::Windows::Forms::Button^  OpenEfxFile;
			this->EfxFiletreeView = (gcnew System::Windows::Forms::TreeView());
			this->OpenEfxFileFolder = (gcnew System::Windows::Forms::Button());
			this->UintTheEffect = (gcnew System::Windows::Forms::Button());
			this->ClearFillTree = (gcnew System::Windows::Forms::Button());
			this->SaveFile = (gcnew System::Windows::Forms::Button());
			this->SaveFileAs = (gcnew System::Windows::Forms::Button());
			this->CreateButton = (gcnew System::Windows::Forms::Button());
			this->NewNodeEditBox = (gcnew System::Windows::Forms::TextBox());
			this->EfxOpenFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->EfxSaveFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->EfxFolderBrowserDialog = (gcnew System::Windows::Forms::FolderBrowserDialog());
			OpenEfxFile = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// OpenEfxFile
			// 
			OpenEfxFile->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			OpenEfxFile->Location = System::Drawing::Point(7, 364);
			OpenEfxFile->Name = L"OpenEfxFile";
			OpenEfxFile->Size = System::Drawing::Size(49, 25);
			OpenEfxFile->TabIndex = 1;
			OpenEfxFile->Text = L"导入";
			OpenEfxFile->UseVisualStyleBackColor = true;
			OpenEfxFile->Click += gcnew System::EventHandler(this, &CEfxFileCtrl::OpenEfxFile_Click);
			// 
			// EfxFiletreeView
			// 
			this->EfxFiletreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->EfxFiletreeView->Location = System::Drawing::Point(3, 3);
			this->EfxFiletreeView->Name = L"EfxFiletreeView";
			this->EfxFiletreeView->Size = System::Drawing::Size(219, 328);
			this->EfxFiletreeView->TabIndex = 0;
			this->EfxFiletreeView->DoubleClick += gcnew System::EventHandler(this, &CEfxFileCtrl::EfxFiletreeView_DoubleClick);
			this->EfxFiletreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &CEfxFileCtrl::EfxFiletreeView_AfterSelect);
			this->EfxFiletreeView->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &CEfxFileCtrl::EfxFiletreeView_KeyDown);
			// 
			// OpenEfxFileFolder
			// 
			this->OpenEfxFileFolder->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->OpenEfxFileFolder->Location = System::Drawing::Point(7, 393);
			this->OpenEfxFileFolder->Name = L"OpenEfxFileFolder";
			this->OpenEfxFileFolder->Size = System::Drawing::Size(102, 25);
			this->OpenEfxFileFolder->TabIndex = 2;
			this->OpenEfxFileFolder->Text = L"导入特效文件夹";
			this->OpenEfxFileFolder->UseVisualStyleBackColor = true;
			this->OpenEfxFileFolder->Click += gcnew System::EventHandler(this, &CEfxFileCtrl::OpenEfxFileFolder_Click);
			// 
			// UintTheEffect
			// 
			this->UintTheEffect->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->UintTheEffect->Location = System::Drawing::Point(115, 393);
			this->UintTheEffect->Name = L"UintTheEffect";
			this->UintTheEffect->Size = System::Drawing::Size(107, 25);
			this->UintTheEffect->TabIndex = 3;
			this->UintTheEffect->Text = L"当前文件拆分";
			this->UintTheEffect->UseVisualStyleBackColor = true;
			this->UintTheEffect->Click += gcnew System::EventHandler(this, &CEfxFileCtrl::UintTheEffect_Click);
			// 
			// ClearFillTree
			// 
			this->ClearFillTree->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->ClearFillTree->Location = System::Drawing::Point(62, 364);
			this->ClearFillTree->Name = L"ClearFillTree";
			this->ClearFillTree->Size = System::Drawing::Size(47, 25);
			this->ClearFillTree->TabIndex = 4;
			this->ClearFillTree->Text = L"清空";
			this->ClearFillTree->UseVisualStyleBackColor = true;
			this->ClearFillTree->Click += gcnew System::EventHandler(this, &CEfxFileCtrl::ClearFillTree_Click);
			// 
			// SaveFile
			// 
			this->SaveFile->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->SaveFile->Location = System::Drawing::Point(115, 364);
			this->SaveFile->Name = L"SaveFile";
			this->SaveFile->Size = System::Drawing::Size(44, 25);
			this->SaveFile->TabIndex = 5;
			this->SaveFile->Text = L"保存";
			this->SaveFile->UseVisualStyleBackColor = true;
			this->SaveFile->Click += gcnew System::EventHandler(this, &CEfxFileCtrl::SaveFile_Click);
			// 
			// SaveFileAs
			// 
			this->SaveFileAs->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->SaveFileAs->Location = System::Drawing::Point(165, 364);
			this->SaveFileAs->Name = L"SaveFileAs";
			this->SaveFileAs->Size = System::Drawing::Size(57, 25);
			this->SaveFileAs->TabIndex = 6;
			this->SaveFileAs->Text = L"另存为";
			this->SaveFileAs->UseVisualStyleBackColor = true;
			this->SaveFileAs->Click += gcnew System::EventHandler(this, &CEfxFileCtrl::SaveFileAs_Click);
			// 
			// CreateButton
			// 
			this->CreateButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->CreateButton->Location = System::Drawing::Point(165, 337);
			this->CreateButton->Name = L"CreateButton";
			this->CreateButton->Size = System::Drawing::Size(57, 25);
			this->CreateButton->TabIndex = 7;
			this->CreateButton->Text = L"新建";
			this->CreateButton->UseVisualStyleBackColor = true;
			this->CreateButton->Click += gcnew System::EventHandler(this, &CEfxFileCtrl::CreateButton_Click);
			// 
			// NewNodeEditBox
			// 
			this->NewNodeEditBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->NewNodeEditBox->Location = System::Drawing::Point(7, 337);
			this->NewNodeEditBox->Name = L"NewNodeEditBox";
			this->NewNodeEditBox->Size = System::Drawing::Size(152, 21);
			this->NewNodeEditBox->TabIndex = 8;
			// 
			// EfxOpenFileDialog
			// 
			this->EfxOpenFileDialog->Filter = L"特效文件(*.efx)|*.efx";
			this->EfxOpenFileDialog->Multiselect = true;
			this->EfxOpenFileDialog->Title = L"特效文件";
			// 
			// EfxSaveFileDialog
			// 
			this->EfxSaveFileDialog->Filter = L"特效文件(*.efx)|*.efx";
			this->EfxSaveFileDialog->RestoreDirectory = true;
			// 
			// CEfxFileCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->NewNodeEditBox);
			this->Controls->Add(this->CreateButton);
			this->Controls->Add(this->SaveFileAs);
			this->Controls->Add(this->SaveFile);
			this->Controls->Add(this->ClearFillTree);
			this->Controls->Add(this->UintTheEffect);
			this->Controls->Add(this->OpenEfxFileFolder);
			this->Controls->Add(OpenEfxFile);
			this->Controls->Add(this->EfxFiletreeView);
			this->Name = L"CEfxFileCtrl";
			this->Size = System::Drawing::Size(225, 421);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
private: System::Void CreateButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void OpenEfxFile_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void OpenEfxFileFolder_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void UintTheEffect_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void ClearFillTree_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void SaveFile_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void SaveFileAs_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void EfxFiletreeView_DoubleClick(System::Object^  sender, System::EventArgs^  e);
private: System::Void EfxFiletreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
private: System::Void EfxFiletreeView_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);


private: System::Void InitOtherComponent();
private: System::Void DestroyOtherComponent();
private: System::Void SetSelectNode(bool refresh);
		 
public:  virtual char*	GetImpTypeName(void)	override;
		 virtual void	Update(void)	override;


};
}
