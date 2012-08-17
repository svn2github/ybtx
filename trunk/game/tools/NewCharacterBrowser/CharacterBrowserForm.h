#pragma once


namespace CharacterBrowser {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	class CCharacterAttributes;

	/// <summary>
	/// Summary for CharacterBrowserForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CharacterBrowserForm : public System::Windows::Forms::Form
	{
	private:
		CCharacterAttributes* pCharacterAttributes;
	public:
		CharacterBrowserForm(void);

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CharacterBrowserForm();

	protected: TreeNode ^lastNpcSeleceNode;
	protected: TreeNode ^lastObjSeleceNode;
	protected: String	^lastModelInfo;
	protected: Color	^treeNodeBackColor;
	protected: Color	^treeNodeSelectBackColor;












































































































private: sqr_tools::CContextComponent^  cContextComponent;










private: sqr_tools::CRTSCameraCom^  crtsCameraCom;
private: System::Windows::Forms::ToolStripMenuItem^  TOOL_ToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  CHECK_ANIMATE_ToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  SHOW_BRUSH_ToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  ONEPOINTFIVE_ToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  TWOPOINT_ToolStripMenuItem1;
private: System::Windows::Forms::ToolStripMenuItem^  SELFDEFINE_ToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  HIDEAPERTURE_ToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  SET_ToolStripMenuItem;

private: System::Windows::Forms::ToolStripMenuItem^  PATH_SET_ToolStripMenuItem;

private: System::Windows::Forms::MenuStrip^  menuStrip;
private: System::Windows::Forms::TabPage^  tabPage2;
private: System::Windows::Forms::TextBox^  Equip_Info_textBox;
private: System::Windows::Forms::GroupBox^  groupBox3;
private: System::Windows::Forms::TextBox^  Speed_textBox;
private: System::Windows::Forms::TrackBar^  Ani_Speed_trackBar;
private: System::Windows::Forms::Label^  label16;
private: System::Windows::Forms::ComboBox^  Player_ANI_comboBox;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::ComboBox^  Player_AGP_comboBox;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::GroupBox^  groupBox2;
private: System::Windows::Forms::ComboBox^  SubWeapon_Adv_comboBox;
private: System::Windows::Forms::ComboBox^  Player_SubWeapon_comboBox;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::ComboBox^  MainWeapon_Adv_comboBox;
private: System::Windows::Forms::ComboBox^  Player_MainWeapon_comboBox;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::ComboBox^  Boot_Adv_comboBox;
private: System::Windows::Forms::ComboBox^  Player_Boot_comboBox;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::ComboBox^  Cuff_Adv_comboBox;
private: System::Windows::Forms::ComboBox^  Player_Cuff_comboBox;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::ComboBox^  Mantle_Adv_comboBox;
private: System::Windows::Forms::ComboBox^  Player_Mantle_comboBox;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::ComboBox^  Body_Adv_comboBox;
private: System::Windows::Forms::ComboBox^  Player_Body_comboBox;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::ComboBox^  Shoulder_Adv_comboBox;
private: System::Windows::Forms::ComboBox^  Player_Shoulder_comboBox;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::ComboBox^  Equip_Group_comboBox;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::ComboBox^  Head_Adv_comboBox;
private: System::Windows::Forms::ComboBox^  Player_Head_comboBox;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::GroupBox^  groupBox1;
private: System::Windows::Forms::ComboBox^  comboBox1;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::ComboBox^  Player_Hair_comboBox;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::ComboBox^  Player_Face_comboBox;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::ComboBox^  PlayType_comboBox;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::TreeView^  OBJ_treeView;
private: System::Windows::Forms::TreeView^  NPC_treeView;
private: System::Windows::Forms::Button^  Search_Button;
private: System::Windows::Forms::TextBox^  Search_textBox;
private: System::Windows::Forms::GroupBox^  npcobj_groupBox;
private: System::Windows::Forms::RadioButton^  OBJ_radioButton;
private: System::Windows::Forms::RadioButton^  NPC_radioButton;
private: System::Windows::Forms::TabControl^  tabControl1;
private: sqr_tools::CViewCtrl^  cViewCtrl;















	private: System::ComponentModel::IContainer^  components;





















	protected: 



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


		void InitializeComponent(void);
		void InitComponentValue(void);
		void InitCtrlFormLua(void);
		void InitEquipForm();
		void CreateEquipGroup();
		void UpdateEquipGroup(const string& strEquipName);
 		void UpdateComboBox( ComboBox^ cb, const vector<string>& texts);
		void UpdateAdvComboBox( ComboBox^ cb, const int itemIndex, const int pieceItemIndex);

		void UpdateSelchangePlayerType(const int nPlayerType);
		void UpdateMainPlayer();
		void UpdatePlayerAllAttribute();
		void UpdatePlayerModel( const int nSelectItemIndex, const int ItemTypeIndex, vector<string>& hidePieceNames );
		void UpdatePlayerAnimationGroup();
		void UpdateMainPlayerAni();

		void UpdatePlayerEffect(ComboBox^ cb, const int itemIndex, const int pieceItemIndex);

		void UpdatePlayerEquipInfo();

	private: System::Void CharacterBrowserForm_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void CHECK_ANIMATE_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ONEPOINTFIVE_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void TWOPOINT_ToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void SELFDEFINE_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void HIDEAPERTURE_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void PATH_SET_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void NPC_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void OBJ_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Search_Button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void PlayType_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Player_Face_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Player_Hair_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Equip_Group_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Player_Head_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Head_Adv_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Player_Shoulder_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Shoulder_Adv_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Player_Body_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Body_Adv_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Player_Mantle_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Mantle_Adv_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Player_Cuff_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Cuff_Adv_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Player_Boot_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Boot_Adv_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Main_Weapon_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MainWeapon_Adv_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Sub_Weapon_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void SubWeapon_Adv_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Player_AGP_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Player_ANI_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Ani_Speed_trackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Speed_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void CharacterBrowserForm_SizeChanged(System::Object^  sender, System::EventArgs^  e);

    public:  System::Void OnIdle(System::Object^ sender,System::EventArgs^ e);
	private: System::Void NPC_treeView_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e);
	private: System::Void OBJ_treeView_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e);
	private: System::Void tabControl1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Search_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
};
}

