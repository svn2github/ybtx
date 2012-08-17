#include "StdAfx.h"
#include "Vector3MapCtrl.h"
#include "Vector3MapCtrlImp.h"

using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;
namespace sqr_tools
{
	char* Vector3MapCtrl::GetImpTypeName( void )
	{
		return GET_IMP_NAME(Vector3MapCtrlImp);
	}

	System::Void  Vector3MapCtrl::InitOtherComponent()
	{
		m_pBursh	= gcnew System::Drawing::SolidBrush(Color::FromArgb(0xFF000000));
		m_pPen		= gcnew System::Drawing::Pen(Color::FromArgb(0xFF000000), 1.0f);
		m_pFont		= gcnew System::Drawing::Font("Arial", 8);
		// System::Drawing::Color::LightCyan
	}

	System::Void  Vector3MapCtrl::DestroyOtherComponent()
	{
	}

	void	Vector3MapCtrl::Update(void)
	{
		Vector3MapCtrlImp* VecMapimp = (Vector3MapCtrlImp*)m_pCtrl;
		this->Vector3Mappanel->Location = System::Drawing::Point(6, 4);
		this->Size = System::Drawing::Size(471, 66);

		_DrawVecMapPanel();
	}

	void	Vector3MapCtrl::_DrawVecMapPanel()
	{
		// 完成绘制
		System::Drawing::Bitmap^ tempNumMap = gcnew System::Drawing::Bitmap(Vector3Mappanel->Width,Vector3Mappanel->Height);
		Graphics^ Gra = Graphics::FromImage(tempNumMap);
		Gra->InterpolationMode = InterpolationMode::HighQualityBicubic;
		Gra->SmoothingMode = SmoothingMode::HighQuality;
		// 背景
		m_pBursh->Color = Color::FromArgb(0xFFFFFFFF);
		Gra->FillRectangle(m_pBursh, 0, 0, Vector3Mappanel->Width, Vector3Mappanel->Height);

		Vector3MapCtrlImp* VecMapimp = (Vector3MapCtrlImp*)m_pCtrl;
		int ct = VecMapimp->GetVector3MapCount();
		if (ct <= 0)
		{
			Vector3Mappanel->BackgroundImage = tempNumMap;
			return;
		}
		// 第一个方框
		int x1, x2;
		_GetXPositon(0, x1 );
		_DrawRectInfo(Gra,x1-2, 0);

		// 线 框
		for (int i = 1; i < ct; ++i)
		{
			_GetXPositon(i, x2 );
			_DrawRectInfo(Gra,x2-2, i);
			// 画线
			Point posStart = Point( x1, 5 );
			Point posEnd = Point( x2, 5 );
			m_pPen->Color = Color::FromArgb(0xFF000000);
			Gra->DrawLine(m_pPen,posStart, posEnd);
			x1 = x2; 
		}
		Vector3Mappanel->BackgroundImage = tempNumMap;
	}

	void  Vector3MapCtrl::_GetXPositon(int i, int& X )
	{
		Vector3MapCtrlImp* VecMapimp = (Vector3MapCtrlImp*)m_pCtrl;
		float x = VecMapimp->GetVector3MapPer(i);
		X = _PercentToX(x);
	}

	void  Vector3MapCtrl::_DrawRectInfo(Graphics^ Gra,int X, int n)
	{
		Vector3MapCtrlImp* VecMapimp = (Vector3MapCtrlImp*)m_pCtrl;
		float idx = VecMapimp->GetVector3MapPer(n);
		float sel = VecMapimp->GetCurPercent();
		if (idx == sel)
		{
			m_pBursh->Color = Color::FromArgb(0xFF000000);
			Gra->FillRectangle(m_pBursh, X, 2, 5, 5);
			// string
			int Y = 8;
			// X
			Double Dvalx = VecMapimp->GetVector3MapXVal(idx);
			System::String^ drawStr = Dvalx.ToString("F2");

			int lt = drawStr->Length * 7;
			int ht = 12;
			if (X + lt > Vector3Mappanel->Width - 3)
				X = Vector3Mappanel->Width - lt - 3;
			m_pBursh->Color = Color::FromArgb(0xFF000000);
			System::Drawing::Rectangle rtt = System::Drawing::Rectangle( X, Y, lt, ht);
			Gra->DrawString(drawStr, m_pFont, m_pBursh, rtt);
			Y = Y + ht + 2;
			// Y
			Double Dvaly = VecMapimp->GetVector3MapYVal(idx);
			drawStr = Dvaly.ToString("F2");
			lt = drawStr->Length * 7;
			if (X + lt > Vector3Mappanel->Width - 3)
				X = Vector3Mappanel->Width - lt - 3;
			rtt = System::Drawing::Rectangle( X, Y, lt, ht);
			Gra->DrawString(drawStr, m_pFont, m_pBursh, rtt);
			Y = Y + ht + 2;
			// Z
			Double Dvalz = VecMapimp->GetVector3MapZVal(idx);
			drawStr = Dvalz.ToString("F2");
			lt = drawStr->Length * 7;
			if (X + lt > Vector3Mappanel->Width - 3)
				X = Vector3Mappanel->Width - lt - 3;
			rtt = System::Drawing::Rectangle( X, Y, lt, ht);
			Gra->DrawString(drawStr, m_pFont, m_pBursh, rtt);
		}
		else
		{
			m_pPen->Color = Color::FromArgb(0xFF000000);
			Gra->DrawRectangle(m_pPen, X, 2, 5, 5);
		}
	}

	float  Vector3MapCtrl::_XToPercent(int X)
	{
		int tempW = Vector3Mappanel->Width;	
		X -= 3;
		float percent = X / (tempW - 6.0f);
		if( percent < 0.01f )
			percent = 0.0f;
		if( percent > 0.99f )
			percent = 1.0f;
		return percent;
	}

	int  Vector3MapCtrl::_PercentToX(float x)
	{
		int tempW = Vector3Mappanel->Width - 6;
		tempW = (int)(tempW * x) + 3;
		return tempW;
	}

	void	Vector3MapCtrl::_SetCurXPercent(int X)
	{
		bool beFind = false;
		int tempW = Vector3Mappanel->Width - 6;
		float percent = _XToPercent(X);
		Vector3MapCtrlImp* VecMapimp = (Vector3MapCtrlImp*)m_pCtrl;
		int ct = VecMapimp->GetVector3MapCount();
		for (int i = 0; i < ct; ++i)
		{
			float per = VecMapimp->GetVector3MapPer(i);
			float tempDis = per - percent;
			tempDis < 0.0f ? tempDis = -tempDis : tempDis = tempDis;
			if ( tempDis < 3.0f/(tempW*1.0f) )
			{
				VecMapimp->SetCurPercent(per);
				beFind = true;
				break;
			}
		}
		if (!beFind)
			VecMapimp->SetCurPercent(percent);
	}

	System::Void Vector3MapCtrl::Vector3MapCtrl_KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
	{
		if( e->KeyCode == Keys::Delete )
		{
			// 删除当前选中的节点
			Vector3MapCtrlImp* VecMapimp = (Vector3MapCtrlImp*)m_pCtrl;
			VecMapimp->DelCurPerNode();
			Update();
		}
	}

	System::Void Vector3MapCtrl::Vector3Mappanel_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		System::Windows::Forms::MouseButtons mbt = e->Button;
		switch(mbt)
		{
		case System::Windows::Forms::MouseButtons::Middle:
			break;
		case System::Windows::Forms::MouseButtons::Right:
			break;
		case System::Windows::Forms::MouseButtons::Left:
			_SetCurXPercent(e->Location.X);
			Update();
			break;
		}
		this->Focus();	
	}

	System::Void Vector3MapCtrl::Vector3Mappanel_MouseMove( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		if( e->Button == System::Windows::Forms::MouseButtons::Left )
		{
			Vector3MapCtrlImp* VecMapimp = (Vector3MapCtrlImp*)m_pCtrl;
			if (VecMapimp->GetCurPercent() > 0.01f && VecMapimp->GetCurPercent() < 0.99f 
				&& System::Windows::Forms::Control::ModifierKeys != Keys::Control)
			{
				float curper = VecMapimp->GetCurPercent();
				void* node = VecMapimp->GetVector3Node(curper);
				if (!node)
					VecMapimp->InsertNewNode(curper);

				int X = e->Location.X;
				float percent = _XToPercent(X);
				float findper = 0.0f;
				bool beFind = _GetCurPosNode(percent, curper, findper);
				if (beFind)
				{
					// 删除当前节点，设置找到节点
					VecMapimp->DelCurPerNode();
					VecMapimp->SetCurPercent(findper);
				}
				else
					VecMapimp->SetCurVecToPer(percent);
				Update();
			}	
		}
	}

	bool Vector3MapCtrl::_GetCurPosNode(float per, float exper, float& findper)
	{
		bool beFind = false;
		int tempW = Vector3Mappanel->Width - 6;
		
		Vector3MapCtrlImp* VecMapimp = (Vector3MapCtrlImp*)m_pCtrl;
		int ct = VecMapimp->GetVector3MapCount();
		for (int i = 0; i < ct; ++i)
		{
			float idxper = VecMapimp->GetVector3MapPer(i);
			float tempDis = idxper - per;
			tempDis < 0.0f ? tempDis = -tempDis : tempDis = tempDis;
			if ( tempDis < 3.0f/(tempW*1.0f) && idxper != exper )
			{
				findper = idxper;
				beFind = true;
				break;
			}
		}
		return beFind;	
	}


	System::Void Vector3MapCtrl::Vector3Mappanel_MouseDoubleClick( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		// 双击，先查找节点，有 编辑，没有 插入节点编辑
		Vector3MapCtrlImp* VecMapimp = (Vector3MapCtrlImp*)m_pCtrl;
		if( e->Button == System::Windows::Forms::MouseButtons::Left )
		{
			float idx = VecMapimp->GetCurPercent();
			void* node = VecMapimp->GetVector3Node(idx);
			if (!node)
			{
				VecMapimp->InsertNewNode(idx);
				Update();
			}

			m_Vec3Dlg.SetVecEditCtrl(VecMapimp);
			m_Vec3Dlg.Show();
		}
	}
}
