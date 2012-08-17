#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace sqr_tools {

	/// <summary>
	/// Summary for Vector3EditDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	class Vector3EditCtrlImp;
	public ref class Vector3EditDialog : public System::Windows::Forms::Form
	{
	public:
		Vector3EditDialog(void)
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
		~Vector3EditDialog()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Panel^  XZPanel;
	private: System::Windows::Forms::Panel^  XYPanel;
	private: System::Windows::Forms::NumericUpDown^  MaxNumericUpDown;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  MaxSetButton;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::NumericUpDown^  XNumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  YNumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  ZNumericUpDown;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::NumericUpDown^  LengthnumericUpDown;
	private: System::Windows::Forms::TrackBar^  YTrackBar;
	private: System::Windows::Forms::TrackBar^  ZTrackBar;
	private: System::Windows::Forms::TrackBar^  XTrackBar;

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
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->XZPanel = (gcnew System::Windows::Forms::Panel());
			this->XYPanel = (gcnew System::Windows::Forms::Panel());
			this->MaxNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->MaxSetButton = (gcnew System::Windows::Forms::Button());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->XNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->YNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->ZNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->LengthnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->XTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->YTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->ZTrackBar = (gcnew System::Windows::Forms::TrackBar());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MaxNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->XNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->YNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ZNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LengthnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->XTrackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->YTrackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ZTrackBar))->BeginInit();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(8, 13);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(83, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"俯视图【X-Z】";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(140, 13);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(83, 12);
			this->label2->TabIndex = 1;
			this->label2->Text = L"正视图【X-Y】";
			// 
			// XZPanel
			// 
			this->XZPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->XZPanel->Location = System::Drawing::Point(10, 38);
			this->XZPanel->Name = L"XZPanel";
			this->XZPanel->Size = System::Drawing::Size(120, 120);
			this->XZPanel->TabIndex = 2;
			// 
			// XYPanel
			// 
			this->XYPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->XYPanel->Location = System::Drawing::Point(142, 38);
			this->XYPanel->Name = L"XYPanel";
			this->XYPanel->Size = System::Drawing::Size(120, 120);
			this->XYPanel->TabIndex = 3;
			// 
			// MaxNumericUpDown
			// 
			this->MaxNumericUpDown->Location = System::Drawing::Point(341, 10);
			this->MaxNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, 0});
			this->MaxNumericUpDown->Name = L"MaxNumericUpDown";
			this->MaxNumericUpDown->Size = System::Drawing::Size(85, 21);
			this->MaxNumericUpDown->TabIndex = 4;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(279, 15);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(53, 12);
			this->label3->TabIndex = 5;
			this->label3->Text = L"最大值：";
			// 
			// MaxSetButton
			// 
			this->MaxSetButton->Location = System::Drawing::Point(441, 8);
			this->MaxSetButton->Name = L"MaxSetButton";
			this->MaxSetButton->Size = System::Drawing::Size(75, 23);
			this->MaxSetButton->TabIndex = 6;
			this->MaxSetButton->Text = L"设置";
			this->MaxSetButton->UseVisualStyleBackColor = true;
			this->MaxSetButton->Click += gcnew System::EventHandler(this, &Vector3EditDialog::MaxSetButton_Click);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(281, 48);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(17, 12);
			this->label4->TabIndex = 7;
			this->label4->Text = L"X:";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(281, 75);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(17, 12);
			this->label5->TabIndex = 8;
			this->label5->Text = L"Y:";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(281, 102);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(17, 12);
			this->label6->TabIndex = 9;
			this->label6->Text = L"Z:";
			// 
			// XNumericUpDown
			// 
			this->XNumericUpDown->DecimalPlaces = 2;
			this->XNumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 131072});
			this->XNumericUpDown->Location = System::Drawing::Point(304, 44);
			this->XNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, 0});
			this->XNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, System::Int32::MinValue});
			this->XNumericUpDown->Name = L"XNumericUpDown";
			this->XNumericUpDown->Size = System::Drawing::Size(53, 21);
			this->XNumericUpDown->TabIndex = 10;
			this->XNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &Vector3EditDialog::XNumericUpDown_ValueChanged);
			// 
			// YNumericUpDown
			// 
			this->YNumericUpDown->DecimalPlaces = 2;
			this->YNumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 131072});
			this->YNumericUpDown->Location = System::Drawing::Point(304, 71);
			this->YNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, 0});
			this->YNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, System::Int32::MinValue});
			this->YNumericUpDown->Name = L"YNumericUpDown";
			this->YNumericUpDown->Size = System::Drawing::Size(53, 21);
			this->YNumericUpDown->TabIndex = 11;
			this->YNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &Vector3EditDialog::YNumericUpDown_ValueChanged);
			// 
			// ZNumericUpDown
			// 
			this->ZNumericUpDown->DecimalPlaces = 2;
			this->ZNumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 131072});
			this->ZNumericUpDown->Location = System::Drawing::Point(304, 97);
			this->ZNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, 0});
			this->ZNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, System::Int32::MinValue});
			this->ZNumericUpDown->Name = L"ZNumericUpDown";
			this->ZNumericUpDown->Size = System::Drawing::Size(53, 21);
			this->ZNumericUpDown->TabIndex = 12;
			this->ZNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &Vector3EditDialog::ZNumericUpDown_ValueChanged);
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(281, 134);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(65, 12);
			this->label7->TabIndex = 13;
			this->label7->Text = L"矢量长度：";
			// 
			// LengthnumericUpDown
			// 
			this->LengthnumericUpDown->DecimalPlaces = 4;
			this->LengthnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 262144});
			this->LengthnumericUpDown->Location = System::Drawing::Point(341, 130);
			this->LengthnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, 0});
			this->LengthnumericUpDown->Name = L"LengthnumericUpDown";
			this->LengthnumericUpDown->Size = System::Drawing::Size(85, 21);
			this->LengthnumericUpDown->TabIndex = 14;
			this->LengthnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &Vector3EditDialog::LengthnumericUpDown_ValueChanged);
			// 
			// XTrackBar
			// 
			this->XTrackBar->AutoSize = false;
			this->XTrackBar->Location = System::Drawing::Point(363, 38);
			this->XTrackBar->Maximum = 50000;
			this->XTrackBar->Minimum = -50000;
			this->XTrackBar->Name = L"XTrackBar";
			this->XTrackBar->Size = System::Drawing::Size(153, 22);
			this->XTrackBar->TabIndex = 15;
			this->XTrackBar->TickFrequency = 4000;
			this->XTrackBar->ValueChanged += gcnew System::EventHandler(this, &Vector3EditDialog::XTrackBar_ValueChanged);
			// 
			// YTrackBar
			// 
			this->YTrackBar->AutoSize = false;
			this->YTrackBar->Location = System::Drawing::Point(363, 69);
			this->YTrackBar->Maximum = 50000;
			this->YTrackBar->Minimum = -50000;
			this->YTrackBar->Name = L"YTrackBar";
			this->YTrackBar->Size = System::Drawing::Size(153, 22);
			this->YTrackBar->TabIndex = 16;
			this->YTrackBar->TickFrequency = 4000;
			this->YTrackBar->ValueChanged += gcnew System::EventHandler(this, &Vector3EditDialog::YTrackBar_ValueChanged);
			// 
			// ZTrackBar
			// 
			this->ZTrackBar->AutoSize = false;
			this->ZTrackBar->Location = System::Drawing::Point(363, 97);
			this->ZTrackBar->Maximum = 50000;
			this->ZTrackBar->Minimum = -50000;
			this->ZTrackBar->Name = L"ZTrackBar";
			this->ZTrackBar->Size = System::Drawing::Size(153, 22);
			this->ZTrackBar->TabIndex = 17;
			this->ZTrackBar->TickFrequency = 4000;
			this->ZTrackBar->ValueChanged += gcnew System::EventHandler(this, &Vector3EditDialog::ZTrackBar_ValueChanged);
			// 
			// Vector3EditDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(528, 167);
			this->Controls->Add(this->ZTrackBar);
			this->Controls->Add(this->YTrackBar);
			this->Controls->Add(this->XTrackBar);
			this->Controls->Add(this->LengthnumericUpDown);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->ZNumericUpDown);
			this->Controls->Add(this->YNumericUpDown);
			this->Controls->Add(this->XNumericUpDown);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->MaxSetButton);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->MaxNumericUpDown);
			this->Controls->Add(this->XYPanel);
			this->Controls->Add(this->XZPanel);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->DoubleBuffered = true;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"Vector3EditDialog";
			this->Text = L"Vector3EditDialog";
			this->TopMost = true;
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Vector3EditDialog::Vector3EditDialog_FormClosing);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MaxNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->XNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->YNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ZNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LengthnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->XTrackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->YTrackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ZTrackBar))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: 
			Vector3EditCtrlImp*					m_pVecEditCtrl;
			float								m_VecX;
			float								m_VecY;
			float								m_VecZ;
			int									m_fMaxLength;
			float								m_fVecLength;
			bool								m_bNeedUpdateCtrl;
			System::Drawing::SolidBrush^		m_pBursh;
			System::Drawing::Pen^				m_pPen;

		public:	void	SetVecEditCtrl(void* ctrl);
		private: System::Void	InitOtherComponent();
		private: System::Void	DestroyOtherComponent();
		private: void			_RefreshSetingBars();
		private: void			_SetSetingBarsVal();
		private: void			_SetVectorXYZ();
		private: void			_SetVectorLength();
		private: void			_DrawXYZPanel();
		private: void			_DrawPanel(Graphics^ Gra, int w, int h);
		private: void			_CaculateVec(float& l, float& x, float& y, float& z);

private: System::Void MaxSetButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void LengthnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void ZNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void YNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void XNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void XTrackBar_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void YTrackBar_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void ZTrackBar_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void Vector3EditDialog_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
};
}
