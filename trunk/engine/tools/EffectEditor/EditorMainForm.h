#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace EffectEditor {

	/// <summary>
	/// Summary for EditorMainForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	class EditorMainFormImp;
	public ref class EditorMainForm : public System::Windows::Forms::Form
	{
	public:
		EditorMainForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			InitMainFormImp();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~EditorMainForm()
		{
			if (components)
			{
				delete components;
			}
			DestroyMainFormImp();
		}
	private: System::Windows::Forms::MenuStrip^  MainMenuStrip;
	private: System::Windows::Forms::ToolStripMenuItem^  FileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  EditToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SetToolStripMenuItem;




	private: sqr_tools::CViewCtrl^  MainViewCtrl;
	private: sqr_tools::CContextComponent^  MainContextComponent;
	private: System::Windows::Forms::Panel^  HelpEditPanel;
	private: System::Windows::Forms::ToolStripMenuItem^  ExitToolStripMenuItem;
	private: sqr_tools::CEfxFileCtrl^  CEfxFileCtrl;
	private: sqr_tools::CRTSCameraCom^  MainCameraCom;
	private: sqr_tools::CEfxEditProp^  CEfxEditPropCtrl;




	protected: 

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
			this->MainMenuStrip = (gcnew System::Windows::Forms::MenuStrip());
			this->FileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ExitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->EditToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SetToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->HelpEditPanel = (gcnew System::Windows::Forms::Panel());
			this->CEfxEditPropCtrl = (gcnew sqr_tools::CEfxEditProp());
			this->MainContextComponent = (gcnew sqr_tools::CContextComponent());
			this->CEfxFileCtrl = (gcnew sqr_tools::CEfxFileCtrl());
			this->MainViewCtrl = (gcnew sqr_tools::CViewCtrl());
			this->MainCameraCom = (gcnew sqr_tools::CRTSCameraCom());
			this->MainMenuStrip->SuspendLayout();
			this->SuspendLayout();
			// 
			// MainMenuStrip
			// 
			this->MainMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->FileToolStripMenuItem, 
				this->EditToolStripMenuItem, this->SetToolStripMenuItem});
			this->MainMenuStrip->Location = System::Drawing::Point(0, 0);
			this->MainMenuStrip->Name = L"MainMenuStrip";
			this->MainMenuStrip->Size = System::Drawing::Size(1264, 25);
			this->MainMenuStrip->TabIndex = 0;
			// 
			// FileToolStripMenuItem
			// 
			this->FileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->ExitToolStripMenuItem});
			this->FileToolStripMenuItem->Name = L"FileToolStripMenuItem";
			this->FileToolStripMenuItem->Size = System::Drawing::Size(58, 21);
			this->FileToolStripMenuItem->Text = L"文件(F)";
			// 
			// ExitToolStripMenuItem
			// 
			this->ExitToolStripMenuItem->Name = L"ExitToolStripMenuItem";
			this->ExitToolStripMenuItem->Size = System::Drawing::Size(100, 22);
			this->ExitToolStripMenuItem->Text = L"退出";
			this->ExitToolStripMenuItem->Click += gcnew System::EventHandler(this, &EditorMainForm::ExitToolStripMenuItem_Click);
			// 
			// EditToolStripMenuItem
			// 
			this->EditToolStripMenuItem->Name = L"EditToolStripMenuItem";
			this->EditToolStripMenuItem->Size = System::Drawing::Size(59, 21);
			this->EditToolStripMenuItem->Text = L"编辑(E)";
			// 
			// SetToolStripMenuItem
			// 
			this->SetToolStripMenuItem->Name = L"SetToolStripMenuItem";
			this->SetToolStripMenuItem->Size = System::Drawing::Size(59, 21);
			this->SetToolStripMenuItem->Text = L"设置(S)";
			// 
			// HelpEditPanel
			// 
			this->HelpEditPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->HelpEditPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->HelpEditPanel->Location = System::Drawing::Point(0, 560);
			this->HelpEditPanel->Name = L"HelpEditPanel";
			this->HelpEditPanel->Size = System::Drawing::Size(752, 362);
			this->HelpEditPanel->TabIndex = 3;
			// 
			// CEfxEditPropCtrl
			// 
			this->CEfxEditPropCtrl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->CEfxEditPropCtrl->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->CEfxEditPropCtrl->ContextComponent = this->MainContextComponent;
			this->CEfxEditPropCtrl->Location = System::Drawing::Point(755, 28);
			this->CEfxEditPropCtrl->Name = L"CEfxEditPropCtrl";
			this->CEfxEditPropCtrl->Size = System::Drawing::Size(509, 891);
			this->CEfxEditPropCtrl->TabIndex = 4;
			// 
			// MainContextComponent
			// 
			this->MainContextComponent->AmbientColor = System::Drawing::Color::White;
			this->MainContextComponent->SceneName = nullptr;
			this->MainContextComponent->ShowBlock = false;
			this->MainContextComponent->ShowGrid = true;
			this->MainContextComponent->SunColor = System::Drawing::Color::White;
			// 
			// CEfxFileCtrl
			// 
			this->CEfxFileCtrl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->CEfxFileCtrl->BackColor = System::Drawing::SystemColors::Control;
			this->CEfxFileCtrl->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->CEfxFileCtrl->ContextComponent = this->MainContextComponent;
			this->CEfxFileCtrl->Location = System::Drawing::Point(523, 564);
			this->CEfxFileCtrl->Name = L"CEfxFileCtrl";
			this->CEfxFileCtrl->Size = System::Drawing::Size(226, 355);
			this->CEfxFileCtrl->TabIndex = 0;
			// 
			// MainViewCtrl
			// 
			this->MainViewCtrl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->MainViewCtrl->BackColor = System::Drawing::SystemColors::AppWorkspace;
			this->MainViewCtrl->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->MainViewCtrl->ContextComponent = this->MainContextComponent;
			this->MainViewCtrl->Location = System::Drawing::Point(0, 28);
			this->MainViewCtrl->Name = L"MainViewCtrl";
			this->MainViewCtrl->Size = System::Drawing::Size(752, 526);
			this->MainViewCtrl->TabIndex = 2;
			// 
			// MainCameraCom
			// 
			this->MainCameraCom->ContextComponent = this->MainContextComponent;
			// 
			// EditorMainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1264, 922);
			this->Controls->Add(this->CEfxEditPropCtrl);
			this->Controls->Add(this->CEfxFileCtrl);
			this->Controls->Add(this->HelpEditPanel);
			this->Controls->Add(this->MainViewCtrl);
			this->Controls->Add(this->MainMenuStrip);
			this->Name = L"EditorMainForm";
			this->Text = L"EditorMainForm";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &EditorMainForm::EditorMainForm_FormClosed);
			this->MainMenuStrip->ResumeLayout(false);
			this->MainMenuStrip->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
private: EditorMainFormImp*	m_pMainFormImp;
private: System::Void InitMainFormImp();
private: System::Void DestroyMainFormImp();
protected: virtual System::Void _Load(System::Object^  sender, System::EventArgs^  e);

private: System::Void ExitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void EditorMainForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e);
};
}
