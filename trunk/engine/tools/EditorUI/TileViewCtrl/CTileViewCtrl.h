#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"
#include "MapEffectCtrl/CMapEffectCtrl.h"

namespace sqr_tools {
	/// <summary>
	/// Summary for CTileViewCtrl
	/// </summary>
	public ref class CTileViewCtrl : public CUIBase
	{
	public:
		CTileViewCtrl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->cMapEffectCtrl->SetIsOnlyShowLocalEffect(true);
			this->nUnitTreeSelectImageInex = -1;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CTileViewCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	protected: 
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::ListBox^  resets_listBox;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  resetName_textBox;
	private: System::Windows::Forms::Button^  createReset_button;
	private: System::Windows::Forms::Button^  SaveReset_button;

	private: System::Windows::Forms::Button^  importReset_button;
	private: System::Windows::Forms::Button^  deleteReset_button;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::Button^  createGroup_button;
	private: System::Windows::Forms::ComboBox^  groupType_comboBox;


	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  deleteGroup_button;
	private: System::Windows::Forms::Button^  PlantHideGroup_button2;
	private: System::Windows::Forms::GroupBox^  groupBox7;
	private: System::Windows::Forms::Button^  unbindUnitEffect_button;

	private: System::Windows::Forms::Button^  unbindUnitAni_button;

	private: System::Windows::Forms::Button^  bindUnitAni_button3;
	private: System::Windows::Forms::Button^  deleteUnit_button;
	private: System::Windows::Forms::Button^  createSubUnits_button;
	private: System::Windows::Forms::Button^  createUnit_button;
	private: System::Windows::Forms::Button^  bindGroupAni_button;
	private: System::Windows::Forms::Button^  patchSave_button;

	private: System::Windows::Forms::TreeView^  unit_treeView;

	private: System::Windows::Forms::GroupBox^  groupBox8;

	private: System::Windows::Forms::GroupBox^  groupBox9;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::ComboBox^  cueMat_comboBox;
	private: System::Windows::Forms::TextBox^  selectMatCueIndex_textBox;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Button^  setMat_button;


	private: System::Windows::Forms::TextBox^  cueMatIndex_textBox;
	private: System::Windows::Forms::ImageList^  imageList;
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog;

	private: System::Windows::Forms::TextBox^  tileGroupName_textBox;

	private: System::Windows::Forms::Label^  label5;
	private: sqr_tools::CMapEffectCtrl^  cMapEffectCtrl;



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		int				  nUnitTreeSelectImageInex;
	private: sqr_tools::CContextComponent^  cContextComponent;
			 TreeNode^		  lastSelectGroupNode;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(CTileViewCtrl::typeid));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->patchSave_button = (gcnew System::Windows::Forms::Button());
			this->SaveReset_button = (gcnew System::Windows::Forms::Button());
			this->importReset_button = (gcnew System::Windows::Forms::Button());
			this->deleteReset_button = (gcnew System::Windows::Forms::Button());
			this->createReset_button = (gcnew System::Windows::Forms::Button());
			this->resetName_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->resets_listBox = (gcnew System::Windows::Forms::ListBox());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
			this->unbindUnitEffect_button = (gcnew System::Windows::Forms::Button());
			this->unbindUnitAni_button = (gcnew System::Windows::Forms::Button());
			this->bindUnitAni_button3 = (gcnew System::Windows::Forms::Button());
			this->deleteUnit_button = (gcnew System::Windows::Forms::Button());
			this->createSubUnits_button = (gcnew System::Windows::Forms::Button());
			this->createUnit_button = (gcnew System::Windows::Forms::Button());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->unit_treeView = (gcnew System::Windows::Forms::TreeView());
			this->imageList = (gcnew System::Windows::Forms::ImageList(this->components));
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->tileGroupName_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->bindGroupAni_button = (gcnew System::Windows::Forms::Button());
			this->deleteGroup_button = (gcnew System::Windows::Forms::Button());
			this->PlantHideGroup_button2 = (gcnew System::Windows::Forms::Button());
			this->createGroup_button = (gcnew System::Windows::Forms::Button());
			this->groupType_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->groupBox8 = (gcnew System::Windows::Forms::GroupBox());
			this->cMapEffectCtrl = (gcnew sqr_tools::CMapEffectCtrl());
			this->cContextComponent = (gcnew sqr_tools::CContextComponent());
			this->groupBox9 = (gcnew System::Windows::Forms::GroupBox());
			this->selectMatCueIndex_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->setMat_button = (gcnew System::Windows::Forms::Button());
			this->cueMatIndex_textBox = (gcnew System::Windows::Forms::TextBox());
			this->cueMat_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->openFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->groupBox1->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox7->SuspendLayout();
			this->groupBox6->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->groupBox8->SuspendLayout();
			this->groupBox9->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->groupBox3);
			this->groupBox1->Controls->Add(this->groupBox2);
			this->groupBox1->Location = System::Drawing::Point(5, 3);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(319, 215);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"图素包";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->patchSave_button);
			this->groupBox3->Controls->Add(this->SaveReset_button);
			this->groupBox3->Controls->Add(this->importReset_button);
			this->groupBox3->Controls->Add(this->deleteReset_button);
			this->groupBox3->Controls->Add(this->createReset_button);
			this->groupBox3->Controls->Add(this->resetName_textBox);
			this->groupBox3->Controls->Add(this->label1);
			this->groupBox3->Location = System::Drawing::Point(6, 20);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(113, 190);
			this->groupBox3->TabIndex = 1;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"图素包操作";
			// 
			// patchSave_button
			// 
			this->patchSave_button->Location = System::Drawing::Point(8, 165);
			this->patchSave_button->Name = L"patchSave_button";
			this->patchSave_button->Size = System::Drawing::Size(98, 24);
			this->patchSave_button->TabIndex = 6;
			this->patchSave_button->Text = L"批量存";
			this->patchSave_button->UseVisualStyleBackColor = true;
			this->patchSave_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::patchSave_button_Click);
			// 
			// SaveReset_button
			// 
			this->SaveReset_button->Location = System::Drawing::Point(6, 140);
			this->SaveReset_button->Name = L"SaveReset_button";
			this->SaveReset_button->Size = System::Drawing::Size(101, 24);
			this->SaveReset_button->TabIndex = 5;
			this->SaveReset_button->Text = L"保存图素包";
			this->SaveReset_button->UseVisualStyleBackColor = true;
			this->SaveReset_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::SaveReset_button_Click);
			// 
			// importReset_button
			// 
			this->importReset_button->Location = System::Drawing::Point(6, 115);
			this->importReset_button->Name = L"importReset_button";
			this->importReset_button->Size = System::Drawing::Size(101, 24);
			this->importReset_button->TabIndex = 4;
			this->importReset_button->Text = L"导入图素包";
			this->importReset_button->UseVisualStyleBackColor = true;
			this->importReset_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::importReset_button_Click);
			// 
			// deleteReset_button
			// 
			this->deleteReset_button->Location = System::Drawing::Point(6, 90);
			this->deleteReset_button->Name = L"deleteReset_button";
			this->deleteReset_button->Size = System::Drawing::Size(101, 24);
			this->deleteReset_button->TabIndex = 3;
			this->deleteReset_button->Text = L"删除图素包";
			this->deleteReset_button->UseVisualStyleBackColor = true;
			this->deleteReset_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::deleteReset_button_Click);
			// 
			// createReset_button
			// 
			this->createReset_button->Location = System::Drawing::Point(6, 65);
			this->createReset_button->Name = L"createReset_button";
			this->createReset_button->Size = System::Drawing::Size(101, 24);
			this->createReset_button->TabIndex = 2;
			this->createReset_button->Text = L"创建图素包";
			this->createReset_button->UseVisualStyleBackColor = true;
			this->createReset_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::createReset_button_Click);
			// 
			// resetName_textBox
			// 
			this->resetName_textBox->Location = System::Drawing::Point(6, 42);
			this->resetName_textBox->Name = L"resetName_textBox";
			this->resetName_textBox->Size = System::Drawing::Size(101, 21);
			this->resetName_textBox->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 23);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(101, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"请输入图素包名：";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->resets_listBox);
			this->groupBox2->Location = System::Drawing::Point(119, 20);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(179, 190);
			this->groupBox2->TabIndex = 0;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"图素包列表";
			// 
			// resets_listBox
			// 
			this->resets_listBox->FormattingEnabled = true;
			this->resets_listBox->ItemHeight = 12;
			this->resets_listBox->Location = System::Drawing::Point(3, 13);
			this->resets_listBox->Name = L"resets_listBox";
			this->resets_listBox->Size = System::Drawing::Size(174, 172);
			this->resets_listBox->TabIndex = 0;
			this->resets_listBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CTileViewCtrl::resets_listBox_SelectedIndexChanged);
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->groupBox7);
			this->groupBox4->Controls->Add(this->groupBox6);
			this->groupBox4->Controls->Add(this->groupBox5);
			this->groupBox4->Location = System::Drawing::Point(6, 219);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(318, 416);
			this->groupBox4->TabIndex = 1;
			this->groupBox4->TabStop = false;
			// 
			// groupBox7
			// 
			this->groupBox7->Controls->Add(this->unbindUnitEffect_button);
			this->groupBox7->Controls->Add(this->unbindUnitAni_button);
			this->groupBox7->Controls->Add(this->bindUnitAni_button3);
			this->groupBox7->Controls->Add(this->deleteUnit_button);
			this->groupBox7->Controls->Add(this->createSubUnits_button);
			this->groupBox7->Controls->Add(this->createUnit_button);
			this->groupBox7->Location = System::Drawing::Point(6, 194);
			this->groupBox7->Name = L"groupBox7";
			this->groupBox7->Size = System::Drawing::Size(117, 214);
			this->groupBox7->TabIndex = 2;
			this->groupBox7->TabStop = false;
			this->groupBox7->Text = L"图素操作";
			// 
			// unbindUnitEffect_button
			// 
			this->unbindUnitEffect_button->Location = System::Drawing::Point(5, 180);
			this->unbindUnitEffect_button->Name = L"unbindUnitEffect_button";
			this->unbindUnitEffect_button->Size = System::Drawing::Size(107, 27);
			this->unbindUnitEffect_button->TabIndex = 5;
			this->unbindUnitEffect_button->Text = L"删除特效绑定";
			this->unbindUnitEffect_button->UseVisualStyleBackColor = true;
			this->unbindUnitEffect_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::unbindUnitEffect_button_Click);
			// 
			// unbindUnitAni_button
			// 
			this->unbindUnitAni_button->Location = System::Drawing::Point(3, 147);
			this->unbindUnitAni_button->Name = L"unbindUnitAni_button";
			this->unbindUnitAni_button->Size = System::Drawing::Size(109, 27);
			this->unbindUnitAni_button->TabIndex = 4;
			this->unbindUnitAni_button->Text = L"删除动画绑定";
			this->unbindUnitAni_button->UseVisualStyleBackColor = true;
			this->unbindUnitAni_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::unbindUnitAni_button_Click);
			// 
			// bindUnitAni_button3
			// 
			this->bindUnitAni_button3->Location = System::Drawing::Point(4, 114);
			this->bindUnitAni_button3->Name = L"bindUnitAni_button3";
			this->bindUnitAni_button3->Size = System::Drawing::Size(108, 27);
			this->bindUnitAni_button3->TabIndex = 3;
			this->bindUnitAni_button3->Text = L"绑定动画";
			this->bindUnitAni_button3->UseVisualStyleBackColor = true;
			this->bindUnitAni_button3->Click += gcnew System::EventHandler(this, &CTileViewCtrl::bindUnitAni_button3_Click);
			// 
			// deleteUnit_button
			// 
			this->deleteUnit_button->Location = System::Drawing::Point(5, 80);
			this->deleteUnit_button->Name = L"deleteUnit_button";
			this->deleteUnit_button->Size = System::Drawing::Size(107, 27);
			this->deleteUnit_button->TabIndex = 2;
			this->deleteUnit_button->Text = L"删除图素";
			this->deleteUnit_button->UseVisualStyleBackColor = true;
			this->deleteUnit_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::deleteUnit_button_Click);
			// 
			// createSubUnits_button
			// 
			this->createSubUnits_button->Location = System::Drawing::Point(4, 47);
			this->createSubUnits_button->Name = L"createSubUnits_button";
			this->createSubUnits_button->Size = System::Drawing::Size(109, 27);
			this->createSubUnits_button->TabIndex = 1;
			this->createSubUnits_button->Text = L"添加复合图素";
			this->createSubUnits_button->UseVisualStyleBackColor = true;
			this->createSubUnits_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::createSubUnits_button_Click);
			// 
			// createUnit_button
			// 
			this->createUnit_button->Location = System::Drawing::Point(4, 13);
			this->createUnit_button->Name = L"createUnit_button";
			this->createUnit_button->Size = System::Drawing::Size(108, 27);
			this->createUnit_button->TabIndex = 0;
			this->createUnit_button->Text = L"添加独立图素";
			this->createUnit_button->UseVisualStyleBackColor = true;
			this->createUnit_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::createUnit_button_Click);
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->unit_treeView);
			this->groupBox6->Location = System::Drawing::Point(123, 13);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(193, 399);
			this->groupBox6->TabIndex = 1;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"图素组列表";
			// 
			// unit_treeView
			// 
			this->unit_treeView->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
			this->unit_treeView->ImageIndex = 0;
			this->unit_treeView->ImageList = this->imageList;
			this->unit_treeView->Location = System::Drawing::Point(5, 19);
			this->unit_treeView->Name = L"unit_treeView";
			this->unit_treeView->SelectedImageIndex = 0;
			this->unit_treeView->Size = System::Drawing::Size(184, 376);
			this->unit_treeView->TabIndex = 0;
			this->unit_treeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &CTileViewCtrl::unit_treeView_AfterSelect);
			// 
			// imageList
			// 
			this->imageList->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"imageList.ImageStream")));
			this->imageList->TransparentColor = System::Drawing::Color::Transparent;
			this->imageList->Images->SetKeyName(0, L"ico00001.ico");
			this->imageList->Images->SetKeyName(1, L"fxunit1.ico");
			this->imageList->Images->SetKeyName(2, L"2dunit.ico");
			this->imageList->Images->SetKeyName(3, L"2dunitsel.ico");
			this->imageList->Images->SetKeyName(4, L"3dunit.ico");
			this->imageList->Images->SetKeyName(5, L"3dunitsel.ico");
			this->imageList->Images->SetKeyName(6, L"Floder.ico");
			this->imageList->Images->SetKeyName(7, L"flodersel.ico");
			this->imageList->Images->SetKeyName(8, L"ico00007.ico");
			this->imageList->Images->SetKeyName(9, L"ico00008.ico");
			this->imageList->Images->SetKeyName(10, L"PGRoot.PNG");
			this->imageList->Images->SetKeyName(11, L"piecegro.png");
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->tileGroupName_textBox);
			this->groupBox5->Controls->Add(this->label5);
			this->groupBox5->Controls->Add(this->bindGroupAni_button);
			this->groupBox5->Controls->Add(this->deleteGroup_button);
			this->groupBox5->Controls->Add(this->PlantHideGroup_button2);
			this->groupBox5->Controls->Add(this->createGroup_button);
			this->groupBox5->Controls->Add(this->groupType_comboBox);
			this->groupBox5->Controls->Add(this->label2);
			this->groupBox5->Location = System::Drawing::Point(6, 13);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(116, 178);
			this->groupBox5->TabIndex = 0;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"图素组操作";
			// 
			// tileGroupName_textBox
			// 
			this->tileGroupName_textBox->Location = System::Drawing::Point(6, 35);
			this->tileGroupName_textBox->Name = L"tileGroupName_textBox";
			this->tileGroupName_textBox->Size = System::Drawing::Size(100, 21);
			this->tileGroupName_textBox->TabIndex = 7;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(6, 19);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(101, 12);
			this->label5->TabIndex = 6;
			this->label5->Text = L"请输入图素组名：";
			// 
			// bindGroupAni_button
			// 
			this->bindGroupAni_button->Location = System::Drawing::Point(4, 149);
			this->bindGroupAni_button->Name = L"bindGroupAni_button";
			this->bindGroupAni_button->Size = System::Drawing::Size(108, 26);
			this->bindGroupAni_button->TabIndex = 5;
			this->bindGroupAni_button->Text = L"图素包绑骨骼";
			this->bindGroupAni_button->UseVisualStyleBackColor = true;
			this->bindGroupAni_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::bindGroupAni_button_Click);
			// 
			// deleteGroup_button
			// 
			this->deleteGroup_button->Location = System::Drawing::Point(5, 123);
			this->deleteGroup_button->Name = L"deleteGroup_button";
			this->deleteGroup_button->Size = System::Drawing::Size(108, 26);
			this->deleteGroup_button->TabIndex = 4;
			this->deleteGroup_button->Text = L"删除图素组";
			this->deleteGroup_button->UseVisualStyleBackColor = true;
			this->deleteGroup_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::deleteGroup_button_Click);
			// 
			// PlantHideGroup_button2
			// 
			this->PlantHideGroup_button2->Location = System::Drawing::Point(4, 97);
			this->PlantHideGroup_button2->Name = L"PlantHideGroup_button2";
			this->PlantHideGroup_button2->Size = System::Drawing::Size(108, 26);
			this->PlantHideGroup_button2->TabIndex = 3;
			this->PlantHideGroup_button2->Text = L"植物隐藏图素组";
			this->PlantHideGroup_button2->UseVisualStyleBackColor = true;
			this->PlantHideGroup_button2->Click += gcnew System::EventHandler(this, &CTileViewCtrl::PlantHideGroup_button2_Click);
			// 
			// createGroup_button
			// 
			this->createGroup_button->Location = System::Drawing::Point(69, 75);
			this->createGroup_button->Name = L"createGroup_button";
			this->createGroup_button->Size = System::Drawing::Size(41, 20);
			this->createGroup_button->TabIndex = 2;
			this->createGroup_button->Text = L"创建";
			this->createGroup_button->UseVisualStyleBackColor = true;
			this->createGroup_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::createGroup_button_Click);
			// 
			// groupType_comboBox
			// 
			this->groupType_comboBox->FormattingEnabled = true;
			this->groupType_comboBox->Items->AddRange(gcnew cli::array< System::Object^  >(5) {L"普通", L"npc", L"树", L"灌木", L"草"});
			this->groupType_comboBox->Location = System::Drawing::Point(5, 75);
			this->groupType_comboBox->Name = L"groupType_comboBox";
			this->groupType_comboBox->Size = System::Drawing::Size(63, 20);
			this->groupType_comboBox->TabIndex = 1;
			this->groupType_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CTileViewCtrl::groupType_comboBox_SelectedIndexChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(4, 60);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(65, 12);
			this->label2->TabIndex = 0;
			this->label2->Text = L"图素组类型";
			// 
			// groupBox8
			// 
			this->groupBox8->Controls->Add(this->cMapEffectCtrl);
			this->groupBox8->Location = System::Drawing::Point(7, 638);
			this->groupBox8->Name = L"groupBox8";
			this->groupBox8->Size = System::Drawing::Size(248, 294);
			this->groupBox8->TabIndex = 2;
			this->groupBox8->TabStop = false;
			this->groupBox8->Text = L"特效列表";
			// 
			// cMapEffectCtrl
			// 
			this->cMapEffectCtrl->ContextComponent = this->cContextComponent;
			this->cMapEffectCtrl->Location = System::Drawing::Point(5, 20);
			this->cMapEffectCtrl->Name = L"cMapEffectCtrl";
			this->cMapEffectCtrl->Size = System::Drawing::Size(241, 264);
			this->cMapEffectCtrl->TabIndex = 0;
			// 
			// groupBox9
			// 
			this->groupBox9->Controls->Add(this->selectMatCueIndex_textBox);
			this->groupBox9->Controls->Add(this->label4);
			this->groupBox9->Controls->Add(this->setMat_button);
			this->groupBox9->Controls->Add(this->cueMatIndex_textBox);
			this->groupBox9->Controls->Add(this->cueMat_comboBox);
			this->groupBox9->Controls->Add(this->label3);
			this->groupBox9->Location = System::Drawing::Point(8, 936);
			this->groupBox9->Name = L"groupBox9";
			this->groupBox9->Size = System::Drawing::Size(284, 73);
			this->groupBox9->TabIndex = 3;
			this->groupBox9->TabStop = false;
			this->groupBox9->Text = L"地表音效材质设置";
			// 
			// selectMatCueIndex_textBox
			// 
			this->selectMatCueIndex_textBox->Enabled = false;
			this->selectMatCueIndex_textBox->Location = System::Drawing::Point(121, 47);
			this->selectMatCueIndex_textBox->Name = L"selectMatCueIndex_textBox";
			this->selectMatCueIndex_textBox->Size = System::Drawing::Size(48, 21);
			this->selectMatCueIndex_textBox->TabIndex = 5;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(9, 50);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(113, 12);
			this->label4->TabIndex = 4;
			this->label4->Text = L"选中材质音效索引：";
			// 
			// setMat_button
			// 
			this->setMat_button->Location = System::Drawing::Point(198, 19);
			this->setMat_button->Name = L"setMat_button";
			this->setMat_button->Size = System::Drawing::Size(79, 22);
			this->setMat_button->TabIndex = 3;
			this->setMat_button->Text = L"设置材质";
			this->setMat_button->UseVisualStyleBackColor = true;
			this->setMat_button->Click += gcnew System::EventHandler(this, &CTileViewCtrl::setMat_button_Click);
			// 
			// cueMatIndex_textBox
			// 
			this->cueMatIndex_textBox->Enabled = false;
			this->cueMatIndex_textBox->Location = System::Drawing::Point(155, 21);
			this->cueMatIndex_textBox->Name = L"cueMatIndex_textBox";
			this->cueMatIndex_textBox->Size = System::Drawing::Size(38, 21);
			this->cueMatIndex_textBox->TabIndex = 2;
			// 
			// cueMat_comboBox
			// 
			this->cueMat_comboBox->FormattingEnabled = true;
			this->cueMat_comboBox->Location = System::Drawing::Point(64, 21);
			this->cueMat_comboBox->Name = L"cueMat_comboBox";
			this->cueMat_comboBox->Size = System::Drawing::Size(85, 20);
			this->cueMat_comboBox->TabIndex = 1;
			this->cueMat_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CTileViewCtrl::cueMat_comboBox_SelectedIndexChanged);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(11, 24);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(53, 12);
			this->label3->TabIndex = 0;
			this->label3->Text = L"材质名：";
			// 
			// openFileDialog
			// 
			this->openFileDialog->FileName = L"openFileDialog";
			// 
			// CTileViewCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->Controls->Add(this->groupBox9);
			this->Controls->Add(this->groupBox8);
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->groupBox1);
			this->Name = L"CTileViewCtrl";
			this->Size = System::Drawing::Size(257, 476);
			this->Load += gcnew System::EventHandler(this, &CTileViewCtrl::CTileViewCtrl_Load);
			this->VisibleChanged += gcnew System::EventHandler(this, &CTileViewCtrl::CTileViewCtrl_VisibleChanged);
			this->groupBox1->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox7->ResumeLayout(false);
			this->groupBox6->ResumeLayout(false);
			this->groupBox5->ResumeLayout(false);
			this->groupBox5->PerformLayout();
			this->groupBox8->ResumeLayout(false);
			this->groupBox9->ResumeLayout(false);
			this->groupBox9->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void createReset_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void deleteReset_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void importReset_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void SaveReset_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void patchSave_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void resets_listBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void groupType_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void createGroup_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void PlantHideGroup_button2_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void deleteGroup_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void bindGroupAni_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void unit_treeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
	private: System::Void createUnit_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void createSubUnits_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void deleteUnit_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void bindUnitAni_button3_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void unbindUnitAni_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void unbindUnitEffect_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void cueMat_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void setMat_button_Click(System::Object^  sender, System::EventArgs^  e);
	private:  void InitCueMaterialCombox();
	private: void CreateTreeRoot();
	private: void ResetUnitsTree();
	private: void CreateResGroup( const BYTE bResGroupMark, const bool bVisibleByConfigue );
	private: void CreatTerrainSingleModelUnit();
	private: void CreatTerraiMultiModelUnit();
	private: void CreatTerrainTextureUnit();
	private: void CreateResSetGroupNode(String^ clrGroupName, bool mark);
	private: System::Void CTileViewCtrl_VisibleChanged(System::Object^  sender, System::EventArgs^  e);
	public:  virtual void   Update(void) override;
	public:  virtual char*	GetImpTypeName(void)	override;
	public:  virtual void*	GetImpParam(void)		override;
	private: System::Void CTileViewCtrl_Load(System::Object^  sender, System::EventArgs^  e);

	private: void SelectGroupNode();
	private: void SelectGroupUnit();
	private: void SelectGroupMultiUnit();
};
}
