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
	/// Summary for CMapEffectCtrl
	/// </summary>
	public ref class CMapEffectCtrl : public CUIBase
	{
	public:
		CMapEffectCtrl(void)
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
		~CMapEffectCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ImageList^  effectTree_imageList;
	private: System::Windows::Forms::TreeView^  effect_treeView;

	protected: 
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		bool	bOnlyShowLocalEffect;
		String^ bActiveEffectName;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(CMapEffectCtrl::typeid));
			this->effectTree_imageList = (gcnew System::Windows::Forms::ImageList(this->components));
			this->effect_treeView = (gcnew System::Windows::Forms::TreeView());
			this->SuspendLayout();
			// 
			// effectTree_imageList
			// 
			this->effectTree_imageList->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"effectTree_imageList.ImageStream")));
			this->effectTree_imageList->TransparentColor = System::Drawing::Color::Transparent;
			this->effectTree_imageList->Images->SetKeyName(0, L"flodersel.ico");
			this->effectTree_imageList->Images->SetKeyName(1, L"ico00001.ico");
			this->effectTree_imageList->Images->SetKeyName(2, L"ico00003.ico");
			this->effectTree_imageList->Images->SetKeyName(3, L"ico00007.ico");
			this->effectTree_imageList->Images->SetKeyName(4, L"ico00008.ico");
			// 
			// effect_treeView
			// 
			this->effect_treeView->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->effect_treeView->ImageIndex = 0;
			this->effect_treeView->ImageList = this->effectTree_imageList;
			this->effect_treeView->Location = System::Drawing::Point(3, 3);
			this->effect_treeView->Name = L"effect_treeView";
			this->effect_treeView->SelectedImageIndex = 0;
			this->effect_treeView->Size = System::Drawing::Size(232, 324);
			this->effect_treeView->TabIndex = 0;
			this->effect_treeView->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &CMapEffectCtrl::effect_treeView_MouseUp);
			this->effect_treeView->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &CMapEffectCtrl::effect_treeView_KeyDown);
			// 
			// CMapEffectCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->effect_treeView);
			this->Name = L"CMapEffectCtrl";
			this->Size = System::Drawing::Size(238, 331);
			this->Load += gcnew System::EventHandler(this, &CMapEffectCtrl::CMapEffectCtrl_Load);
			this->ResumeLayout(false);

		}
#pragma endregion

		public:  virtual void   Update(void) override;
		public:  virtual char*	GetImpTypeName(void)	override;
		public:  virtual void*	GetImpParam(void)		override;
		private: System::Void CMapEffectCtrl_Load(System::Object^  sender, System::EventArgs^  e);

		public:  void SetIsOnlyShowLocalEffect(bool b);
		private: void InsertLocalEffectToTreeView();
		private: void InsertAmbientEffectToTreeView();
		private: System::Void effect_treeView_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		private: System::Void effect_treeView_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);

		public: String^ GetActiveEffectName();
	};
}
