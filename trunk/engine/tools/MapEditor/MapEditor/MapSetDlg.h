#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MapEditor {

	/// <summary>
	/// Summary for NewMapDlg
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class MapSetDlg : public System::Windows::Forms::Form
	{
	public:
		MapSetDlg(void)
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
		~MapSetDlg()
		{
			if (components)
			{
				delete components;
			}
		}
	public: sqr_tools::CNewMapCtrl^  MapCtrl;
	private: System::Windows::Forms::Button^  btNewOK;
	private: System::Windows::Forms::Button^  btNewCancle;
	public: sqr_tools::CExMapCtrl^  ExMapCtrl;
	private: System::Windows::Forms::Button^  btExOK;
	private: System::Windows::Forms::Button^  btExCancle;
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
			this->MapCtrl = (gcnew sqr_tools::CNewMapCtrl());
			this->btNewOK = (gcnew System::Windows::Forms::Button());
			this->btNewCancle = (gcnew System::Windows::Forms::Button());
			this->ExMapCtrl = (gcnew sqr_tools::CExMapCtrl());
			this->btExOK = (gcnew System::Windows::Forms::Button());
			this->btExCancle = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// MapCtrl
			// 
			this->MapCtrl->Location = System::Drawing::Point(3, 4);
			this->MapCtrl->Name = L"MapCtrl";
			this->MapCtrl->Size = System::Drawing::Size(330, 183);
			this->MapCtrl->TabIndex = 0;
			// 
			// btNewOK
			// 
			this->btNewOK->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btNewOK->Location = System::Drawing::Point(194, 142);
			this->btNewOK->Name = L"btNewOK";
			this->btNewOK->Size = System::Drawing::Size(58, 27);
			this->btNewOK->TabIndex = 1;
			this->btNewOK->Text = L"确定";
			this->btNewOK->UseVisualStyleBackColor = true;
			this->btNewOK->Click += gcnew System::EventHandler(this, &MapSetDlg::btNewOK_Click);
			// 
			// btNewCancle
			// 
			this->btNewCancle->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btNewCancle->Location = System::Drawing::Point(258, 142);
			this->btNewCancle->Name = L"btNewCancle";
			this->btNewCancle->Size = System::Drawing::Size(58, 27);
			this->btNewCancle->TabIndex = 1;
			this->btNewCancle->Text = L"取消";
			this->btNewCancle->UseVisualStyleBackColor = true;
			this->btNewCancle->Click += gcnew System::EventHandler(this, &MapSetDlg::btNewCancle_Click);
			// 
			// ExMapCtrl
			// 
			this->ExMapCtrl->Location = System::Drawing::Point(3, 190);
			this->ExMapCtrl->Name = L"ExMapCtrl";
			this->ExMapCtrl->Size = System::Drawing::Size(330, 258);
			this->ExMapCtrl->TabIndex = 2;
			// 
			// btExOK
			// 
			this->btExOK->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btExOK->Location = System::Drawing::Point(194, 407);
			this->btExOK->Name = L"btExOK";
			this->btExOK->Size = System::Drawing::Size(58, 23);
			this->btExOK->TabIndex = 3;
			this->btExOK->Text = L"确定";
			this->btExOK->UseVisualStyleBackColor = true;
			this->btExOK->Click += gcnew System::EventHandler(this, &MapSetDlg::btExOK_Click);
			// 
			// btExCancle
			// 
			this->btExCancle->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btExCancle->Location = System::Drawing::Point(258, 407);
			this->btExCancle->Name = L"btExCancle";
			this->btExCancle->Size = System::Drawing::Size(58, 23);
			this->btExCancle->TabIndex = 3;
			this->btExCancle->Text = L"取消";
			this->btExCancle->UseVisualStyleBackColor = true;
			this->btExCancle->Click += gcnew System::EventHandler(this, &MapSetDlg::btExCancle_Click);
			// 
			// MapSetDlg
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(336, 453);
			this->Controls->Add(this->btExCancle);
			this->Controls->Add(this->btExOK);
			this->Controls->Add(this->ExMapCtrl);
			this->Controls->Add(this->btNewCancle);
			this->Controls->Add(this->btNewOK);
			this->Controls->Add(this->MapCtrl);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"MapSetDlg";
			this->Text = L"地图创建与拓展";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void btNewCancle_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void btNewOK_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void btExOK_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void btExCancle_Click(System::Object^  sender, System::EventArgs^  e);
};
}
