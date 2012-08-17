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
	/// Summary for CueListCtrl
	/// </summary>
	public ref class CueListCtrl : CUIBase
	{
	public:
		CueListCtrl(void)
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
		~CueListCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox;
	protected: 
	private: System::Windows::Forms::TreeView^  cue_treeView;

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
			this->groupBox = (gcnew System::Windows::Forms::GroupBox());
			this->cue_treeView = (gcnew System::Windows::Forms::TreeView());
			this->groupBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox
			// 
			this->groupBox->Controls->Add(this->cue_treeView);
			this->groupBox->Location = System::Drawing::Point(0, 6);
			this->groupBox->Name = L"groupBox";
			this->groupBox->Size = System::Drawing::Size(214, 298);
			this->groupBox->TabIndex = 0;
			this->groupBox->TabStop = false;
			this->groupBox->Text = L"ÒôÔ´°ü";
			// 
			// cue_treeView
			// 
			this->cue_treeView->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
			this->cue_treeView->Location = System::Drawing::Point(6, 20);
			this->cue_treeView->Name = L"cue_treeView";
			this->cue_treeView->Size = System::Drawing::Size(202, 272);
			this->cue_treeView->TabIndex = 0;
			this->cue_treeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &CueListCtrl::cue_treeView_AfterSelect);
			// 
			// CueListCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->groupBox);
			this->Name = L"CueListCtrl";
			this->Size = System::Drawing::Size(215, 305);
			this->Load += gcnew System::EventHandler(this, &CueListCtrl::CueListCtrl_Load);
			this->groupBox->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

			public:  virtual void   Update(void) override;
			public:  virtual char*	GetImpTypeName(void)	override;
			public:  virtual void*	GetImpParam(void)		override;
			public:  virtual void	SetImpCueName(System::String^ str);
			public:  virtual const char*	GetImpCueName();

			private: void			InitCueTreeView();
			private: System::Void	cue_treeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
			private: System::Void CueListCtrl_Load(System::Object^  sender, System::EventArgs^  e);
	};
}
