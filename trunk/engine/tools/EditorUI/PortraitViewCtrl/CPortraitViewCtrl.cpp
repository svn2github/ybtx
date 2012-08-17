#include "StdAfx.h"
#include "CPortraitViewCtrl.h"
#include "PortraitView\PortraitViewCtrlImp.h"

#pragma region Windows Form Designer generated code
void CPortraitViewCtrl::InitializeComponent( void )
{
	this->groupBox = (gcnew System::Windows::Forms::GroupBox());
	this->canclebutton = (gcnew System::Windows::Forms::Button());
	this->okbutton = (gcnew System::Windows::Forms::Button());
	this->showPortraitcheckBox = (gcnew System::Windows::Forms::CheckBox());
	this->showFramecheckBox = (gcnew System::Windows::Forms::CheckBox());
	this->BrowseButton = (gcnew System::Windows::Forms::Button());
	this->PhotoFrametextBox = (gcnew System::Windows::Forms::TextBox());
	this->label1 = (gcnew System::Windows::Forms::Label());
	this->openFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
	this->groupBox->SuspendLayout();
	this->SuspendLayout();
	// 
	// groupBox
	// 
	this->groupBox->Controls->Add(this->canclebutton);
	this->groupBox->Controls->Add(this->okbutton);
	this->groupBox->Controls->Add(this->showPortraitcheckBox);
	this->groupBox->Controls->Add(this->showFramecheckBox);
	this->groupBox->Controls->Add(this->BrowseButton);
	this->groupBox->Controls->Add(this->PhotoFrametextBox);
	this->groupBox->Controls->Add(this->label1);
	this->groupBox->Location = System::Drawing::Point(6, 9);
	this->groupBox->Name = L"groupBox";
	this->groupBox->Size = System::Drawing::Size(361, 79);
	this->groupBox->TabIndex = 0;
	this->groupBox->TabStop = false;
	this->groupBox->Text = L"相框设置";
	// 
	// canclebutton
	// 
	this->canclebutton->Location = System::Drawing::Point(290, 52);
	this->canclebutton->Name = L"canclebutton";
	this->canclebutton->Size = System::Drawing::Size(57, 21);
	this->canclebutton->TabIndex = 6;
	this->canclebutton->Text = L"取消";
	this->canclebutton->UseVisualStyleBackColor = true;
	this->canclebutton->Click += gcnew System::EventHandler(this, &CPortraitViewCtrl::canclebutton_Click);
	// 
	// okbutton
	// 
	this->okbutton->Location = System::Drawing::Point(195, 52);
	this->okbutton->Name = L"okbutton";
	this->okbutton->Size = System::Drawing::Size(57, 21);
	this->okbutton->TabIndex = 5;
	this->okbutton->Text = L"确定";
	this->okbutton->UseVisualStyleBackColor = true;
	this->okbutton->Click += gcnew System::EventHandler(this, &CPortraitViewCtrl::okbutton_Click);
	// 
	// showPortraitcheckBox
	// 
	this->showPortraitcheckBox->AutoSize = true;
	this->showPortraitcheckBox->Location = System::Drawing::Point(113, 55);
	this->showPortraitcheckBox->Name = L"showPortraitcheckBox";
	this->showPortraitcheckBox->Size = System::Drawing::Size(48, 16);
	this->showPortraitcheckBox->TabIndex = 4;
	this->showPortraitcheckBox->Text = L"显示";
	this->showPortraitcheckBox->UseVisualStyleBackColor = true;
	this->showPortraitcheckBox->CheckedChanged += gcnew System::EventHandler(this, &CPortraitViewCtrl::showPortraitcheckBox_CheckedChanged);
	// 
	// showFramecheckBox
	// 
	this->showFramecheckBox->AutoSize = true;
	this->showFramecheckBox->Location = System::Drawing::Point(13, 55);
	this->showFramecheckBox->Name = L"showFramecheckBox";
	this->showFramecheckBox->Size = System::Drawing::Size(72, 16);
	this->showFramecheckBox->TabIndex = 3;
	this->showFramecheckBox->Text = L"显示相框";
	this->showFramecheckBox->UseVisualStyleBackColor = true;
	this->showFramecheckBox->CheckedChanged += gcnew System::EventHandler(this, &CPortraitViewCtrl::showFramecheckBox_CheckedChanged);
	// 
	// BrowseButton
	// 
	this->BrowseButton->Location = System::Drawing::Point(290, 22);
	this->BrowseButton->Name = L"BrowseButton";
	this->BrowseButton->Size = System::Drawing::Size(57, 21);
	this->BrowseButton->TabIndex = 2;
	this->BrowseButton->Text = L"...浏览";
	this->BrowseButton->UseVisualStyleBackColor = true;
	this->BrowseButton->Click += gcnew System::EventHandler(this, &CPortraitViewCtrl::BrowseButton_Click);
	// 
	// PhotoFrametextBox
	// 
	this->PhotoFrametextBox->Location = System::Drawing::Point(65, 22);
	this->PhotoFrametextBox->Name = L"PhotoFrametextBox";
	this->PhotoFrametextBox->ReadOnly = true;
	this->PhotoFrametextBox->Size = System::Drawing::Size(218, 21);
	this->PhotoFrametextBox->TabIndex = 1;
	// 
	// label1
	// 
	this->label1->AutoSize = true;
	this->label1->Location = System::Drawing::Point(6, 28);
	this->label1->Name = L"label1";
	this->label1->Size = System::Drawing::Size(53, 12);
	this->label1->TabIndex = 0;
	this->label1->Text = L"相框纹理";
	// 
	// openFileDialog
	// 
	this->openFileDialog->FileName = L"openFileDialog";
	// 
	// CPortraitViewCtrl
	// 
	this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
	this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	this->Controls->Add(this->groupBox);
	this->Name = L"CPortraitViewCtrl";
	this->Size = System::Drawing::Size(373, 93);
	this->VisibleChanged += gcnew System::EventHandler(this, &CPortraitViewCtrl::CPortraitViewCtrl_VisibleChanged);
	this->groupBox->ResumeLayout(false);
	this->groupBox->PerformLayout();
	this->ResumeLayout(false);

}
#pragma endregion

System::Void CPortraitViewCtrl::BrowseButton_Click( System::Object^ sender, System::EventArgs^ e )
{
	openFileDialog->Title			 = "选择相框";
	openFileDialog->Filter			 = "DDS文件|*.dds||";
	openFileDialog->RestoreDirectory = true;
	openFileDialog->ShowDialog();
	this->PhotoFrametextBox->Text = openFileDialog->FileName;
}

System::Void CPortraitViewCtrl::canclebutton_Click( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void CPortraitViewCtrl::okbutton_Click( System::Object^ sender, System::EventArgs^ e )
{
	std::string strPhotoFrameTexPath = MagStringToUnmagString(this->PhotoFrametextBox->Text);

	return;
	//CPortraitViewCtrlImp *pImp = (CPortraitViewCtrlImp*)m_pCtrl;
	//pImp->SetPhotoFrameTexPath(strPhotoFrameTexPath);
	//pImp->SetIsShowPhotoFrame(showFramecheckBox->Checked);
	//pImp->SetIsShowPortraitView(showPortraitcheckBox->Checked);
}	

System::Void CPortraitViewCtrl::showFramecheckBox_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	bool bCheck = showFramecheckBox->Checked;
}

System::Void CPortraitViewCtrl::showPortraitcheckBox_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	bool bCheck = showPortraitcheckBox->Checked;
}

void CPortraitViewCtrl::Update( void )
{

}

char* CPortraitViewCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CPortraitViewCtrlImp);
}