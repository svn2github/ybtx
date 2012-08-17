#include "StdAfx.h"
#include "EStrMapEditCtrl.h"
#include "EStrMapEditCtrlImp.h"
#include "CLRStringHelper.h"

using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;
namespace sqr_tools
{

	char* EStrMapEditCtrl::GetImpTypeName( void )
	{
		return GET_IMP_NAME(EStrMapEditCtrlImp);
	}

	System::Void  EStrMapEditCtrl::InitOtherComponent()
	{
		m_pBursh	= gcnew System::Drawing::SolidBrush(Color::FromArgb(0xFF000000));
		m_pPen		= gcnew System::Drawing::Pen(Color::FromArgb(0xFF000000), 1.0f);
		m_pFont		= gcnew System::Drawing::Font("Arial", 8);
		m_bReSize = false;
	}

	System::Void  EStrMapEditCtrl::DestroyOtherComponent()
	{
	}

	void	EStrMapEditCtrl::Update(void)
	{
		EStrMapEditCtrlImp* EStrMapimp = (EStrMapEditCtrlImp*)m_pCtrl;

		this->TextureMapPanel->Location = System::Drawing::Point(6, 4);
		this->Size = System::Drawing::Size(471, 31);

		_DrawEStrMapPanel();
	}

	void EStrMapEditCtrl::_DrawEStrMapPanel()
	{
		// 完成画
		System::Drawing::Bitmap^ tempNumMap = gcnew System::Drawing::Bitmap(TextureMapPanel->Width,TextureMapPanel->Height);
		Graphics^ Gra = Graphics::FromImage(tempNumMap);
		Gra->InterpolationMode = InterpolationMode::HighQualityBicubic;
		Gra->SmoothingMode = SmoothingMode::HighQuality;
		// 背景
		m_pBursh->Color = System::Drawing::Color::DarkSeaGreen;
		Gra->FillRectangle(m_pBursh, 0, 0, TextureMapPanel->Width, TextureMapPanel->Height);

		EStrMapEditCtrlImp* EStrMapimp = (EStrMapEditCtrlImp*)m_pCtrl;
		int ct = EStrMapimp->GetEStrMapCount();
		if (ct <= 0)
		{
			TextureMapPanel->BackgroundImage = tempNumMap;
			return;
		}
		// 第一个方框
		int x1, x2;
		for (int i = 0; i < ct; ++i)
		{
			_GetXPositon(i, x1);
			if (i+1 < ct)
				_GetXPositon(i+1, x2);
			else
				x2 = _PercentToX(1.0f);

			_DrawRectInfo(Gra,x1, x2, i);
		}
		TextureMapPanel->BackgroundImage = tempNumMap;
	}

	void  EStrMapEditCtrl::_GetXPositon(int i, int& X )
	{
		EStrMapEditCtrlImp* EStrMapimp = (EStrMapEditCtrlImp*)m_pCtrl;
		float x = EStrMapimp->GetEStrMapPer(i);
		X = _PercentToX(x);
	}

	void  EStrMapEditCtrl::_DrawRectInfo(Graphics^ Gra,int X1, int X2, int n)
	{
		EStrMapEditCtrlImp* EStrMapimp = (EStrMapEditCtrlImp*)m_pCtrl;
		float idx = EStrMapimp->GetEStrMapPer(n);
		float sel = EStrMapimp->GetCurPercent();
		if (idx == sel)
			m_pBursh->Color = Color::FromArgb(0xCC808080);
		else
			m_pBursh->Color = Color::FromArgb(0xCCCCCCCC);

		int iHeight = TextureMapPanel->Height - 2;
		Gra->FillRectangle(m_pBursh, X1, 0, X2 - X1, iHeight);
		m_pPen->Color = Color::FromArgb(0xCC222222);
		Gra->DrawRectangle(m_pPen, X1, 0, X2 - X1, iHeight);
		m_pPen->Color = Color::FromArgb(0xCC505050);
		Gra->DrawRectangle(m_pPen, X1, 0, X2 - X1 - 1, iHeight - 1);

		// 文字
		m_pBursh->Color = Color::FromArgb(0xFF000000);
		System::Drawing::Rectangle rtt = System::Drawing::Rectangle( X1, 6, X2-X1, iHeight - 12);
		if (EStrMapimp->GetEStrNode(idx))
		{
			System::String^ drawStr = gcnew String(EStrMapimp->GetEStrNode(idx));
			Gra->DrawString(drawStr, m_pFont, m_pBursh, rtt);
		}
	}

	void EStrMapEditCtrl::_SetCurXPercent(int X)
	{
		float per = _XToPercent(X);
		// 找到当前选中的节点
		float selper = _GetCurSelectNode(per);
		EStrMapEditCtrlImp* EStrMapimp = (EStrMapEditCtrlImp*)m_pCtrl;
		EStrMapimp->SetCurPercent(selper);
	}

	float EStrMapEditCtrl::_GetCurSelectNode(float per)
	{
		EStrMapEditCtrlImp* EStrMapimp = (EStrMapEditCtrlImp*)m_pCtrl;
		int ct = EStrMapimp->GetEStrMapCount();
		int i = 0;
		for (; i < ct; ++i)
		{
			float nodePer = EStrMapimp->GetEStrMapPer(i);
			float dis = nodePer - per;
			dis < 0.0f ? dis = -dis : dis = dis;
			// 选中边缘
			if( dis*TextureMapPanel->Width < 4.0f )
				return nodePer;
			// 选中中间
			else if(per < nodePer)
				return EStrMapimp->GetEStrMapPer(i-1);
		}
		return EStrMapimp->GetEStrMapPer(i-1);
	}

	float EStrMapEditCtrl::_XToPercent(int X)
	{
		int tempW = TextureMapPanel->Width;	
		float percent = X * 1.0f / tempW;
		if( percent < 0.01f )
			percent = 0.0f;
		if( percent > 0.99f )
			percent = 1.0f;
		return percent;
	}

	int EStrMapEditCtrl::_PercentToX(float x)
	{
		int tempW = TextureMapPanel->Width;
		tempW = (int)(tempW * x);
		return tempW;
	}

	System::Void EStrMapEditCtrl::TextureMapPanel_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		// 选中状态
		_SetCurXPercent(e->Location.X);
		Update();
		this->Focus();
	}

	System::Void EStrMapEditCtrl::TextureMapPanel_MouseMove( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		// 移动
		float per = _XToPercent(e->Location.X);
		// 确定鼠标形状
		EStrMapEditCtrlImp* EStrMapimp = (EStrMapEditCtrlImp*)m_pCtrl;
		int ct = EStrMapimp->GetEStrMapCount();
		bool reSize = false;
		for (int i = 0; i < ct; ++i)
		{
			float nodePer = EStrMapimp->GetEStrMapPer(i);
			float dis = nodePer - per;
			dis < 0.0f ? dis = -dis : dis = dis;
			// 选中边缘
			if( i != 0 && dis*TextureMapPanel->Width < 4.0f )
			{
				reSize = true;
				if( e->Button == System::Windows::Forms::MouseButtons::Left )
				{
					if (m_bReSize == false)
						_SetCurXPercent(e->Location.X);	
					m_bReSize = true;
				}
				break;
			}
		}
		if (e->Button != System::Windows::Forms::MouseButtons::Left)
			m_bReSize = false;

		if (reSize)
			this->TextureMapPanel->Cursor = System::Windows::Forms::Cursors::SizeWE;
		else
			this->TextureMapPanel->Cursor = System::Windows::Forms::Cursors::Default;

		if( m_bReSize )
		{
			// 设置鼠标形状
			this->TextureMapPanel->Cursor = System::Windows::Forms::Cursors::SizeWE;

			// 操作贴图
			float CurPer = EStrMapimp->GetCurPercent();
			// 一种是把前面的吃光，一种是把自己吃光
			for (int i = 0; i < ct; ++i)
			{
				float nodePer = EStrMapimp->GetEStrMapPer(i);
				if (CurPer == nodePer)
					continue;
				float dis = nodePer - per;
				dis < 0.0f ? dis = -dis : dis = dis;
				// 选中边缘
				if( dis*TextureMapPanel->Width < 4.0f )
				{
					if (nodePer < CurPer)
					{
						// 删除找到的节点，当前节点移动到鼠标定义位置
						EStrMapimp->DelEStrNode(nodePer);
						EStrMapimp->MoveCurNodeToPer(per);
						Update();
						return;
					}
					else if (nodePer > CurPer)
					{
						// 删除自己，当前节点设置为找到的
						EStrMapimp->DelCurPerNode();
						EStrMapimp->SetCurPercent(nodePer);
						Update();
						return;
					}
					
				}
			}
			EStrMapimp->MoveCurNodeToPer(per);
			Update();
		}
	}

	System::Void EStrMapEditCtrl::EStrMapEditCtrl_KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
	{
		EStrMapEditCtrlImp* EStrMapimp = (EStrMapEditCtrlImp*)m_pCtrl;
		if( e->KeyCode == Keys::C )
		{
			string str = "None";
			EStrMapimp->SetNodeEStr(EStrMapimp->GetCurPercent(), str.c_str());
			Update();
		}
		else if(e->KeyCode == Keys::Delete && EStrMapimp->GetEStrMapCount() > 1 && EStrMapimp->GetCurPercent() > 0.01f)
		{
			// 删掉当前节点
			EStrMapimp->DelCurPerNode();
			Update();
		}
		else if(e->KeyCode == Keys::Insert)
		{
			string str = "None";
			float inPer = EStrMapimp->GetCurPercent();
			float moPer = 0.0f;
			int ct = EStrMapimp->GetEStrMapCount();
			int i = 0;
			for (; i < ct; ++i)
			{
				float nodePer = EStrMapimp->GetEStrMapPer(i);
				if (inPer < nodePer)
				{
					moPer = (inPer + nodePer)/2.0f;
					break;
				}
			}
			if (i == ct)	// 没找到
				moPer = (inPer + 1.0f)/2.0f;

			EStrMapimp->MoveCurNodeToPer(moPer);
			EStrMapimp->InsertNewNode(inPer, str.c_str());
			Update();
		}
	}

	System::Void EStrMapEditCtrl::TextureMapPanel_MouseClick( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		if( e->Button == System::Windows::Forms::MouseButtons::Right )
		{
			// 打开编辑贴图	
			EStrMapEditCtrlImp* EStrMapimp = (EStrMapEditCtrlImp*)m_pCtrl;
			EStrMapimp->OpenCurTexture();
		}
	}

	System::Void EStrMapEditCtrl::TextureMapPanel_MouseDoubleClick( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		if( e->Button == System::Windows::Forms::MouseButtons::Left )
		{
			// 弹出贴图选择窗口
			if (OpenTexFileDialog->ShowDialog() == DialogResult::OK)
			{
				EStrMapEditCtrlImp* EStrMapimp = (EStrMapEditCtrlImp*)m_pCtrl;
				float curPer = EStrMapimp->GetCurPercent();
				float nextPer = 1.0f;
				int ct = EStrMapimp->GetEStrMapCount();
				for (int i = 0; i < ct; ++i)
				{
					float nodePer = EStrMapimp->GetEStrMapPer(i);
					if (curPer == nodePer)
					{
						if (i == ct - 1)
						{
							nextPer = 1.0f;
							break;
						}
						else
						{
							nextPer = EStrMapimp->GetEStrMapPer(i + 1);
							break;
						}
					}
				}
				int lt = OpenTexFileDialog->FileNames->Length;
				for ( int j = 0; j < lt; ++j )
				{
					float insPer = 0.0f;
					String^ strClr = OpenTexFileDialog->FileNames[j];
					std::string strCPlus;
					CLRMarshalString(strClr, strCPlus);
					
					insPer = curPer + j * (nextPer - curPer)/lt;
					EStrMapimp->InsertRoleTexNode(insPer, strCPlus.c_str());		
				}
			}
		}
	}
}