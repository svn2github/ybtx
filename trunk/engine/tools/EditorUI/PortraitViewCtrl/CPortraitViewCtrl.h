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
	/// Summary for CPortraitViewCtrl
	/// </summary>
	public ref class CPortraitViewCtrl : public CUIBase//public System::Windows::Forms::UserControl
	{
	public:
		CPortraitViewCtrl(void)
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
		~CPortraitViewCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox;
	private: System::Windows::Forms::Button^  BrowseButton;
	private: System::Windows::Forms::TextBox^  PhotoFrametextBox;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  canclebutton;
	private: System::Windows::Forms::Button^  okbutton;
	private: System::Windows::Forms::CheckBox^  showPortraitcheckBox;
	private: System::Windows::Forms::CheckBox^  showFramecheckBox;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog;

	protected: 

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;


		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void);

		private: System::Void BrowseButton_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void canclebutton_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void okbutton_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void showFramecheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void showPortraitcheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void CPortraitViewCtrl_VisibleChanged(System::Object^  sender, System::EventArgs^  e) {
				 }

				 public:  virtual void Update(void) override;
	public:  virtual char*	GetImpTypeName(void)	override;
	};
}
