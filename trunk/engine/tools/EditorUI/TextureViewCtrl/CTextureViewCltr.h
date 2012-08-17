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
	/// Summary for CTextureViewCltr
	/// </summary>
	public ref class CTextureViewCltr : public CUIBase
	{
	public:
		CTextureViewCltr(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->brushStrenth_trackBar->Minimum = 1;
			this->brushStrenth_trackBar->Maximum = 32;
			this->brushStrenth_trackBar->LargeChange = 1;
			this->brushStrenth_trackBar->SmallChange = 1;
			this->brushStrenth_trackBar->TickFrequency = 1;//每次移动的刻度
			this->brushStrenth_trackBar->Value = 16; //初始位置
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CTextureViewCltr()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::RadioButton^  layerTwo_radioButton;
	protected: 

	private: System::Windows::Forms::RadioButton^  layerOne_radioButton;
	private: System::Windows::Forms::CheckBox^  layerTwoCliff_checkBox;
	private: System::Windows::Forms::CheckBox^  layerOneCliff_checkBox;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::TrackBar^  brushStrenth_trackBar;



	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::RadioButton^  dirXZ_radioButton;

	private: System::Windows::Forms::RadioButton^  dirZ_radioButton;

	private: System::Windows::Forms::RadioButton^  dirX_radioButton;

	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::RadioButton^  eraseAlpha_radioButton;

	private: System::Windows::Forms::RadioButton^  smoothAlpha_radioButton;

	private: System::Windows::Forms::RadioButton^  reduceAlpha_radioButton;

	private: System::Windows::Forms::RadioButton^  addAlpha_radioButton;

	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::TreeView^  texture_treeView;
	private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::Button^  deleteGroupTexture_button;

	private: System::Windows::Forms::Button^  deleteSingleTexture_button;

	private: System::Windows::Forms::Button^  SingleTextureToGroup_button;

	private: System::Windows::Forms::Button^  changeSingleTexture_button;

	private: System::Windows::Forms::TextBox^  newTextureGroup_textBox;

	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  oldTextureGroup_textBox;

	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  newTextureName_textBox;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  oldTextureName_textBox;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  GroupTextureToGroup_button;
	private: System::Windows::Forms::ImageList^  textureTree_imageList;
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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(CTextureViewCltr::typeid));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->layerTwoCliff_checkBox = (gcnew System::Windows::Forms::CheckBox());
			this->layerOneCliff_checkBox = (gcnew System::Windows::Forms::CheckBox());
			this->layerTwo_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->layerOne_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->brushStrenth_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->dirXZ_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->dirZ_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->dirX_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->eraseAlpha_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->smoothAlpha_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->reduceAlpha_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->addAlpha_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->texture_treeView = (gcnew System::Windows::Forms::TreeView());
			this->textureTree_imageList = (gcnew System::Windows::Forms::ImageList(this->components));
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->GroupTextureToGroup_button = (gcnew System::Windows::Forms::Button());
			this->deleteGroupTexture_button = (gcnew System::Windows::Forms::Button());
			this->deleteSingleTexture_button = (gcnew System::Windows::Forms::Button());
			this->SingleTextureToGroup_button = (gcnew System::Windows::Forms::Button());
			this->changeSingleTexture_button = (gcnew System::Windows::Forms::Button());
			this->newTextureGroup_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->oldTextureGroup_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->newTextureName_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->oldTextureName_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->brushStrenth_trackBar))->BeginInit();
			this->groupBox4->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->groupBox6->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->layerTwoCliff_checkBox);
			this->groupBox1->Controls->Add(this->layerOneCliff_checkBox);
			this->groupBox1->Controls->Add(this->layerTwo_radioButton);
			this->groupBox1->Controls->Add(this->layerOne_radioButton);
			this->groupBox1->Location = System::Drawing::Point(23, 2);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(251, 63);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"HotPoint";
			// 
			// layerTwoCliff_checkBox
			// 
			this->layerTwoCliff_checkBox->AutoSize = true;
			this->layerTwoCliff_checkBox->Enabled = false;
			this->layerTwoCliff_checkBox->Location = System::Drawing::Point(116, 39);
			this->layerTwoCliff_checkBox->Name = L"layerTwoCliff_checkBox";
			this->layerTwoCliff_checkBox->Size = System::Drawing::Size(96, 16);
			this->layerTwoCliff_checkBox->TabIndex = 3;
			this->layerTwoCliff_checkBox->Text = L"开启悬崖贴图";
			this->layerTwoCliff_checkBox->UseVisualStyleBackColor = true;
			// 
			// layerOneCliff_checkBox
			// 
			this->layerOneCliff_checkBox->AutoSize = true;
			this->layerOneCliff_checkBox->Location = System::Drawing::Point(116, 18);
			this->layerOneCliff_checkBox->Name = L"layerOneCliff_checkBox";
			this->layerOneCliff_checkBox->Size = System::Drawing::Size(96, 16);
			this->layerOneCliff_checkBox->TabIndex = 2;
			this->layerOneCliff_checkBox->Text = L"开启悬崖贴图";
			this->layerOneCliff_checkBox->UseVisualStyleBackColor = true;
			this->layerOneCliff_checkBox->CheckedChanged += gcnew System::EventHandler(this, &CTextureViewCltr::layerOneCliff_checkBox_CheckedChanged);
			// 
			// layerTwo_radioButton
			// 
			this->layerTwo_radioButton->AutoSize = true;
			this->layerTwo_radioButton->Location = System::Drawing::Point(38, 39);
			this->layerTwo_radioButton->Name = L"layerTwo_radioButton";
			this->layerTwo_radioButton->Size = System::Drawing::Size(59, 16);
			this->layerTwo_radioButton->TabIndex = 1;
			this->layerTwo_radioButton->TabStop = true;
			this->layerTwo_radioButton->Text = L"第二层";
			this->layerTwo_radioButton->UseVisualStyleBackColor = true;
			this->layerTwo_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CTextureViewCltr::layerTwo_radioButton_CheckedChanged);
			// 
			// layerOne_radioButton
			// 
			this->layerOne_radioButton->AutoSize = true;
			this->layerOne_radioButton->Location = System::Drawing::Point(38, 17);
			this->layerOne_radioButton->Name = L"layerOne_radioButton";
			this->layerOne_radioButton->Size = System::Drawing::Size(59, 16);
			this->layerOne_radioButton->TabIndex = 0;
			this->layerOne_radioButton->TabStop = true;
			this->layerOne_radioButton->Text = L"第一层";
			this->layerOne_radioButton->UseVisualStyleBackColor = true;
			this->layerOne_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CTextureViewCltr::layerOne_radioButton_CheckedChanged);
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->brushStrenth_trackBar);
			this->groupBox3->Controls->Add(this->label1);
			this->groupBox3->Controls->Add(this->groupBox4);
			this->groupBox3->Controls->Add(this->groupBox2);
			this->groupBox3->Location = System::Drawing::Point(15, 71);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(266, 171);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"笔刷属性";
			// 
			// brushStrenth_trackBar
			// 
			this->brushStrenth_trackBar->Location = System::Drawing::Point(36, 118);
			this->brushStrenth_trackBar->Name = L"brushStrenth_trackBar";
			this->brushStrenth_trackBar->Size = System::Drawing::Size(197, 45);
			this->brushStrenth_trackBar->TabIndex = 5;
			this->brushStrenth_trackBar->Scroll += gcnew System::EventHandler(this, &CTextureViewCltr::brushStrenth_trackBar_Scroll);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(96, 104);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(53, 12);
			this->label1->TabIndex = 4;
			this->label1->Text = L"画刷强度";
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->dirXZ_radioButton);
			this->groupBox4->Controls->Add(this->dirZ_radioButton);
			this->groupBox4->Controls->Add(this->dirX_radioButton);
			this->groupBox4->Location = System::Drawing::Point(8, 57);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(250, 41);
			this->groupBox4->TabIndex = 3;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"笔刷方向";
			// 
			// dirXZ_radioButton
			// 
			this->dirXZ_radioButton->AutoSize = true;
			this->dirXZ_radioButton->Checked = true;
			this->dirXZ_radioButton->Location = System::Drawing::Point(179, 16);
			this->dirXZ_radioButton->Name = L"dirXZ_radioButton";
			this->dirXZ_radioButton->Size = System::Drawing::Size(47, 16);
			this->dirXZ_radioButton->TabIndex = 2;
			this->dirXZ_radioButton->TabStop = true;
			this->dirXZ_radioButton->Text = L"双向";
			this->dirXZ_radioButton->UseVisualStyleBackColor = true;
			this->dirXZ_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CTextureViewCltr::dirXZ_radioButton_CheckedChanged);
			// 
			// dirZ_radioButton
			// 
			this->dirZ_radioButton->AutoSize = true;
			this->dirZ_radioButton->Location = System::Drawing::Point(102, 16);
			this->dirZ_radioButton->Name = L"dirZ_radioButton";
			this->dirZ_radioButton->Size = System::Drawing::Size(53, 16);
			this->dirZ_radioButton->TabIndex = 1;
			this->dirZ_radioButton->Text = L"Z方向";
			this->dirZ_radioButton->UseVisualStyleBackColor = true;
			this->dirZ_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CTextureViewCltr::dirZ_radioButton_CheckedChanged);
			// 
			// dirX_radioButton
			// 
			this->dirX_radioButton->AutoSize = true;
			this->dirX_radioButton->Location = System::Drawing::Point(27, 16);
			this->dirX_radioButton->Name = L"dirX_radioButton";
			this->dirX_radioButton->Size = System::Drawing::Size(53, 16);
			this->dirX_radioButton->TabIndex = 0;
			this->dirX_radioButton->Text = L"X方向";
			this->dirX_radioButton->UseVisualStyleBackColor = true;
			this->dirX_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CTextureViewCltr::dirX_radioButton_CheckedChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->eraseAlpha_radioButton);
			this->groupBox2->Controls->Add(this->smoothAlpha_radioButton);
			this->groupBox2->Controls->Add(this->reduceAlpha_radioButton);
			this->groupBox2->Controls->Add(this->addAlpha_radioButton);
			this->groupBox2->Location = System::Drawing::Point(8, 16);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(251, 39);
			this->groupBox2->TabIndex = 2;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"功能";
			// 
			// eraseAlpha_radioButton
			// 
			this->eraseAlpha_radioButton->AutoSize = true;
			this->eraseAlpha_radioButton->Location = System::Drawing::Point(190, 16);
			this->eraseAlpha_radioButton->Name = L"eraseAlpha_radioButton";
			this->eraseAlpha_radioButton->Size = System::Drawing::Size(47, 16);
			this->eraseAlpha_radioButton->TabIndex = 3;
			this->eraseAlpha_radioButton->Text = L"擦除";
			this->eraseAlpha_radioButton->UseVisualStyleBackColor = true;
			this->eraseAlpha_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CTextureViewCltr::eraseAlpha_radioButton_CheckedChanged);
			// 
			// smoothAlpha_radioButton
			// 
			this->smoothAlpha_radioButton->AutoSize = true;
			this->smoothAlpha_radioButton->Location = System::Drawing::Point(134, 16);
			this->smoothAlpha_radioButton->Name = L"smoothAlpha_radioButton";
			this->smoothAlpha_radioButton->Size = System::Drawing::Size(47, 16);
			this->smoothAlpha_radioButton->TabIndex = 2;
			this->smoothAlpha_radioButton->Text = L"平滑";
			this->smoothAlpha_radioButton->UseVisualStyleBackColor = true;
			this->smoothAlpha_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CTextureViewCltr::smoothAlpha_radioButton_CheckedChanged);
			// 
			// reduceAlpha_radioButton
			// 
			this->reduceAlpha_radioButton->AutoSize = true;
			this->reduceAlpha_radioButton->Location = System::Drawing::Point(75, 16);
			this->reduceAlpha_radioButton->Name = L"reduceAlpha_radioButton";
			this->reduceAlpha_radioButton->Size = System::Drawing::Size(47, 16);
			this->reduceAlpha_radioButton->TabIndex = 1;
			this->reduceAlpha_radioButton->Text = L"降低";
			this->reduceAlpha_radioButton->UseVisualStyleBackColor = true;
			this->reduceAlpha_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CTextureViewCltr::reduceAlpha_radioButton_CheckedChanged);
			// 
			// addAlpha_radioButton
			// 
			this->addAlpha_radioButton->AutoSize = true;
			this->addAlpha_radioButton->Checked = true;
			this->addAlpha_radioButton->Location = System::Drawing::Point(17, 16);
			this->addAlpha_radioButton->Name = L"addAlpha_radioButton";
			this->addAlpha_radioButton->Size = System::Drawing::Size(47, 16);
			this->addAlpha_radioButton->TabIndex = 0;
			this->addAlpha_radioButton->TabStop = true;
			this->addAlpha_radioButton->Text = L"增加";
			this->addAlpha_radioButton->UseVisualStyleBackColor = true;
			this->addAlpha_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CTextureViewCltr::addAlpha_radioButton_CheckedChanged);
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->texture_treeView);
			this->groupBox5->Location = System::Drawing::Point(15, 249);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(266, 244);
			this->groupBox5->TabIndex = 3;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"纹理列表";
			// 
			// texture_treeView
			// 
			this->texture_treeView->ImageIndex = 0;
			this->texture_treeView->ImageList = this->textureTree_imageList;
			this->texture_treeView->Location = System::Drawing::Point(6, 20);
			this->texture_treeView->Name = L"texture_treeView";
			this->texture_treeView->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->texture_treeView->SelectedImageIndex = 0;
			this->texture_treeView->Size = System::Drawing::Size(254, 218);
			this->texture_treeView->TabIndex = 0;
			this->texture_treeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &CTextureViewCltr::texture_treeView_AfterSelect);
			this->texture_treeView->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &CTextureViewCltr::texture_treeView_NodeMouseClick);

			// 
			// textureTree_imageList
			// 
			this->textureTree_imageList->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"textureTree_imageList.ImageStream")));
			this->textureTree_imageList->TransparentColor = System::Drawing::Color::Transparent;
			this->textureTree_imageList->Images->SetKeyName(0, L"ico00001.ico");
			this->textureTree_imageList->Images->SetKeyName(1, L"flodersel.ico");
			this->textureTree_imageList->Images->SetKeyName(2, L"2dunit.ico");
			this->textureTree_imageList->Images->SetKeyName(3, L"2dunitsel.ico");
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->GroupTextureToGroup_button);
			this->groupBox6->Controls->Add(this->deleteGroupTexture_button);
			this->groupBox6->Controls->Add(this->deleteSingleTexture_button);
			this->groupBox6->Controls->Add(this->SingleTextureToGroup_button);
			this->groupBox6->Controls->Add(this->changeSingleTexture_button);
			this->groupBox6->Controls->Add(this->newTextureGroup_textBox);
			this->groupBox6->Controls->Add(this->label5);
			this->groupBox6->Controls->Add(this->oldTextureGroup_textBox);
			this->groupBox6->Controls->Add(this->label4);
			this->groupBox6->Controls->Add(this->newTextureName_textBox);
			this->groupBox6->Controls->Add(this->label3);
			this->groupBox6->Controls->Add(this->oldTextureName_textBox);
			this->groupBox6->Controls->Add(this->label2);
			this->groupBox6->Location = System::Drawing::Point(15, 496);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(265, 212);
			this->groupBox6->TabIndex = 4;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"地表纹理替换";
			// 
			// GroupTextureToGroup_button
			// 
			this->GroupTextureToGroup_button->Location = System::Drawing::Point(62, 154);
			this->GroupTextureToGroup_button->Name = L"GroupTextureToGroup_button";
			this->GroupTextureToGroup_button->Size = System::Drawing::Size(149, 25);
			this->GroupTextureToGroup_button->TabIndex = 12;
			this->GroupTextureToGroup_button->Text = L"四方连续包->四方连续图素包";
			this->GroupTextureToGroup_button->UseVisualStyleBackColor = true;
			this->GroupTextureToGroup_button->Click += gcnew System::EventHandler(this, &CTextureViewCltr::GroupTextureToGroup_button_Click);
			// 
			// deleteGroupTexture_button
			// 
			this->deleteGroupTexture_button->Location = System::Drawing::Point(124, 181);
			this->deleteGroupTexture_button->Name = L"deleteGroupTexture_button";
			this->deleteGroupTexture_button->Size = System::Drawing::Size(134, 25);
			this->deleteGroupTexture_button->TabIndex = 11;
			this->deleteGroupTexture_button->Text = L"四方图素包删除";
			this->deleteGroupTexture_button->UseVisualStyleBackColor = true;
			this->deleteGroupTexture_button->Click += gcnew System::EventHandler(this, &CTextureViewCltr::deleteGroupTexture_button_Click);
			// 
			// deleteSingleTexture_button
			// 
			this->deleteSingleTexture_button->Location = System::Drawing::Point(14, 181);
			this->deleteSingleTexture_button->Name = L"deleteSingleTexture_button";
			this->deleteSingleTexture_button->Size = System::Drawing::Size(91, 25);
			this->deleteSingleTexture_button->TabIndex = 10;
			this->deleteSingleTexture_button->Text = L"单个纹理删除";
			this->deleteSingleTexture_button->UseVisualStyleBackColor = true;
			this->deleteSingleTexture_button->Click += gcnew System::EventHandler(this, &CTextureViewCltr::deleteSingleTexture_button_Click);
			// 
			// SingleTextureToGroup_button
			// 
			this->SingleTextureToGroup_button->Location = System::Drawing::Point(124, 127);
			this->SingleTextureToGroup_button->Name = L"SingleTextureToGroup_button";
			this->SingleTextureToGroup_button->Size = System::Drawing::Size(134, 25);
			this->SingleTextureToGroup_button->TabIndex = 9;
			this->SingleTextureToGroup_button->Text = L"纹理->四方连续图素包";
			this->SingleTextureToGroup_button->UseVisualStyleBackColor = true;
			this->SingleTextureToGroup_button->Click += gcnew System::EventHandler(this, &CTextureViewCltr::SingleTextureToGroup_button_Click);
			// 
			// changeSingleTexture_button
			// 
			this->changeSingleTexture_button->Location = System::Drawing::Point(14, 127);
			this->changeSingleTexture_button->Name = L"changeSingleTexture_button";
			this->changeSingleTexture_button->Size = System::Drawing::Size(91, 25);
			this->changeSingleTexture_button->TabIndex = 8;
			this->changeSingleTexture_button->Text = L"单个纹理替换";
			this->changeSingleTexture_button->UseVisualStyleBackColor = true;
			this->changeSingleTexture_button->Click += gcnew System::EventHandler(this, &CTextureViewCltr::changeSingleTexture_button_Click);
			// 
			// newTextureGroup_textBox
			// 
			this->newTextureGroup_textBox->Location = System::Drawing::Point(75, 97);
			this->newTextureGroup_textBox->Multiline = true;
			this->newTextureGroup_textBox->Name = L"newTextureGroup_textBox";
			this->newTextureGroup_textBox->Size = System::Drawing::Size(184, 20);
			this->newTextureGroup_textBox->TabIndex = 7;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(5, 101);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(65, 12);
			this->label5->TabIndex = 6;
			this->label5->Text = L"新图素包名";
			// 
			// oldTextureGroup_textBox
			// 
			this->oldTextureGroup_textBox->Location = System::Drawing::Point(75, 72);
			this->oldTextureGroup_textBox->Multiline = true;
			this->oldTextureGroup_textBox->Name = L"oldTextureGroup_textBox";
			this->oldTextureGroup_textBox->Size = System::Drawing::Size(183, 20);
			this->oldTextureGroup_textBox->TabIndex = 5;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(4, 76);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(65, 12);
			this->label4->TabIndex = 4;
			this->label4->Text = L"旧图素包名";
			// 
			// newTextureName_textBox
			// 
			this->newTextureName_textBox->Location = System::Drawing::Point(76, 46);
			this->newTextureName_textBox->Multiline = true;
			this->newTextureName_textBox->Name = L"newTextureName_textBox";
			this->newTextureName_textBox->Size = System::Drawing::Size(182, 20);
			this->newTextureName_textBox->TabIndex = 3;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(4, 50);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(53, 12);
			this->label3->TabIndex = 2;
			this->label3->Text = L"新纹理名";
			// 
			// oldTextureName_textBox
			// 
			this->oldTextureName_textBox->Location = System::Drawing::Point(76, 20);
			this->oldTextureName_textBox->Multiline = true;
			this->oldTextureName_textBox->Name = L"oldTextureName_textBox";
			this->oldTextureName_textBox->Size = System::Drawing::Size(183, 20);
			this->oldTextureName_textBox->TabIndex = 1;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(5, 24);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(53, 12);
			this->label2->TabIndex = 0;
			this->label2->Text = L"旧纹理名";
			// 
			// CTextureViewCltr
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->Controls->Add(this->groupBox6);
			this->Controls->Add(this->groupBox5);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox1);
			this->Name = L"CTextureViewCltr";
			this->Size = System::Drawing::Size(300, 711);
			this->Load += gcnew System::EventHandler(this, &CTextureViewCltr::CTextureViewCltr_Load);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->brushStrenth_trackBar))->EndInit();
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox5->ResumeLayout(false);
			this->groupBox6->ResumeLayout(false);
			this->groupBox6->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void layerOne_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void layerOneCliff_checkBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void layerTwo_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void addAlpha_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void reduceAlpha_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void smoothAlpha_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void eraseAlpha_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void dirX_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void dirZ_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void dirXZ_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void brushStrenth_trackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
	private: System::Void changeSingleTexture_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void SingleTextureToGroup_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void GroupTextureToGroup_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void deleteSingleTexture_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void deleteGroupTexture_button_Click(System::Object^  sender, System::EventArgs^  e);
	public: void InsertTextureResetTreeView(String^ resetname);

	public:  virtual void   Update(void) override;
	public:  virtual char*	GetImpTypeName(void)	override;
	public:  virtual void*	GetImpParam(void)		override;
	private: System::Void CTextureViewCltr_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void texture_treeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
	private: System::Void texture_treeView_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e);
	private: System::Void SelectTreeNode(TreeNode ^seleceNode);
};
}
