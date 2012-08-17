#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"
#include "AudioView/CueListCtrl.h"
#include "AudioView/AmbientAudioCtrl.h"

namespace sqr_tools {

	/// <summary>
	/// Summary for AudioViewCtrl
	/// </summary>
	public ref class AudioViewCtrl : public CUIBase
	{
	public:
		AudioViewCtrl(void)
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
		~AudioViewCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: sqr_tools::CueListCtrl^  cueListCtrl;
	protected: 
	private: sqr_tools::AmbientAudioCtrl^  ambientAudioCtrl;
	private: System::Windows::Forms::Button^  createAduio_button;
	private: System::Windows::Forms::RadioButton^  on_radioButton;
	private: System::Windows::Forms::RadioButton^  off_radioButton;
	private: System::Windows::Forms::GroupBox^  groupBox2;





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
			this->cueListCtrl = (gcnew sqr_tools::CueListCtrl());
			this->ambientAudioCtrl = (gcnew sqr_tools::AmbientAudioCtrl());
			this->createAduio_button = (gcnew System::Windows::Forms::Button());
			this->on_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->off_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// cueListCtrl
			// 
			this->cueListCtrl->ContextComponent = nullptr;
			this->cueListCtrl->Location = System::Drawing::Point(12, 1);
			this->cueListCtrl->Name = L"cueListCtrl";
			this->cueListCtrl->Size = System::Drawing::Size(217, 307);
			this->cueListCtrl->TabIndex = 0;
			// 
			// ambientAudioCtrl
			// 
			this->ambientAudioCtrl->ContextComponent = nullptr;
			this->ambientAudioCtrl->Location = System::Drawing::Point(5, 351);
			this->ambientAudioCtrl->Name = L"ambientAudioCtrl";
			this->ambientAudioCtrl->Size = System::Drawing::Size(237, 267);
			this->ambientAudioCtrl->TabIndex = 1;
			// 
			// createAduio_button
			// 
			this->createAduio_button->Location = System::Drawing::Point(19, 12);
			this->createAduio_button->Name = L"createAduio_button";
			this->createAduio_button->Size = System::Drawing::Size(85, 30);
			this->createAduio_button->TabIndex = 0;
			this->createAduio_button->Text = L"´´½¨ÒôÔ´";
			this->createAduio_button->UseVisualStyleBackColor = true;
			this->createAduio_button->Click += gcnew System::EventHandler(this, &AudioViewCtrl::createAduio_button_Click);
			// 
			// on_radioButton
			// 
			this->on_radioButton->AutoSize = true;
			this->on_radioButton->Checked = true;
			this->on_radioButton->Location = System::Drawing::Point(119, 19);
			this->on_radioButton->Name = L"on_radioButton";
			this->on_radioButton->Size = System::Drawing::Size(35, 16);
			this->on_radioButton->TabIndex = 1;
			this->on_radioButton->TabStop = true;
			this->on_radioButton->Text = L"On";
			this->on_radioButton->UseVisualStyleBackColor = true;
			this->on_radioButton->CheckedChanged += gcnew System::EventHandler(this, &AudioViewCtrl::on_radioButton_CheckedChanged);
			// 
			// off_radioButton
			// 
			this->off_radioButton->AutoSize = true;
			this->off_radioButton->Location = System::Drawing::Point(160, 19);
			this->off_radioButton->Name = L"off_radioButton";
			this->off_radioButton->Size = System::Drawing::Size(41, 16);
			this->off_radioButton->TabIndex = 2;
			this->off_radioButton->Text = L"Off";
			this->off_radioButton->UseVisualStyleBackColor = true;
			this->off_radioButton->CheckedChanged += gcnew System::EventHandler(this, &AudioViewCtrl::off_radioButton_CheckedChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->off_radioButton);
			this->groupBox2->Controls->Add(this->on_radioButton);
			this->groupBox2->Controls->Add(this->createAduio_button);
			this->groupBox2->Location = System::Drawing::Point(12, 304);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(217, 50);
			this->groupBox2->TabIndex = 2;
			this->groupBox2->TabStop = false;
			// 
			// AudioViewCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->ambientAudioCtrl);
			this->Controls->Add(this->cueListCtrl);
			this->Name = L"AudioViewCtrl";
			this->Size = System::Drawing::Size(245, 626);
			this->Load += gcnew System::EventHandler(this, &AudioViewCtrl::AudioViewCtrl_Load);
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

		public:  virtual void   Update(void) override;
		public:  virtual char*	GetImpTypeName(void)	override;
		public:  virtual void*	GetImpParam(void)		override;
		private: System::Void	AudioViewCtrl_Load(System::Object^  sender, System::EventArgs^  e);
		private: System::Void	createAduio_button_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void	on_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void	off_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
};
}
