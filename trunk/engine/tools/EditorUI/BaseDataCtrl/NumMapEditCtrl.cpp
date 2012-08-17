#include "StdAfx.h"
#include "NumMapEditCtrl.h"
#include "NumMapEditCtrlImp.h"
#include "MaxMinNumEditDialog.h"

using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;
namespace sqr_tools
{
	char* NumMapEditCtrl::GetImpTypeName( void )
	{
		return GET_IMP_NAME(NumMapEditCtrlImp);
	}

	System::Void  NumMapEditCtrl::InitOtherComponent()
	{
		m_pBursh	= gcnew System::Drawing::SolidBrush(Color::FromArgb(0xFF000000));
		m_pPen		= gcnew System::Drawing::Pen(Color::FromArgb(0xFF000000), 1.0f);
		m_pFont		= gcnew System::Drawing::Font("Arial", 8);
		// System::Drawing::Color::LightCyan
	}

	System::Void  NumMapEditCtrl::DestroyOtherComponent()
	{
	}

	float  NumMapEditCtrl::_XToPercent(int X)
	{
		int tempW = NumMapPanel->Width;	
		X -= 3;
		float percent = X / (tempW - 6.0f);
		if( percent < 0.01f )
			percent = 0.0f;
		if( percent > 0.99f )
			percent = 1.0f;
		return percent;
	}

	float  NumMapEditCtrl::_YToPercent(int Y)
	{
		int tempH = NumMapPanel->Height;	
		Y -= 3;
		float percent = Y / (tempH - 6.0f);
		if( percent < 0.01f )
			percent = 0.0f;
		if( percent > 0.99f )
			percent = 1.0f;
		return percent;
	}

	int  NumMapEditCtrl::_PercentToX(float x)
	{
		int tempW = NumMapPanel->Width - 6;
		tempW = (int)(tempW * x) + 3;
		return tempW;
	}

	int  NumMapEditCtrl::_PercentToY(float y)
	{
		int tempH = NumMapPanel->Height - 6;
		tempH = (int)(tempH * y) + 3;
		return tempH;
	}

	void	NumMapEditCtrl::Update(void)
	{
		NumMapEditCtrlImp* NumMapimp = (NumMapEditCtrlImp*)m_pCtrl;

		this->NumMapPanel->Location = System::Drawing::Point(6, 4);
		this->Size = System::Drawing::Size(471, 112);


		_DrawNumMapPanel();
	}

	void	NumMapEditCtrl::_DrawNumMapPanel()
	{
		System::Drawing::Bitmap^ tempNumMap = gcnew System::Drawing::Bitmap(NumMapPanel->Width,NumMapPanel->Height);
		Graphics^ Gra = Graphics::FromImage(tempNumMap);
		Gra->InterpolationMode = InterpolationMode::HighQualityBicubic;
		Gra->SmoothingMode = SmoothingMode::HighQuality;

		// 背景
		m_pBursh->Color = Color::FromArgb(0xFFFFFFFF);
		Gra->FillRectangle(m_pBursh, 0, 0, NumMapPanel->Width, NumMapPanel->Height);

		NumMapEditCtrlImp* NumMapimp = (NumMapEditCtrlImp*)m_pCtrl;
		int ct = NumMapimp->GetNumMapCount();
		if (ct <= 0)
		{
			NumMapPanel->BackgroundImage = tempNumMap;
			return;
		}
		// 0线
		float zeroper = NumMapimp->GetZeroPercent();
		if (zeroper > 0.0f && zeroper < 1.0f)
		{
			int IzeroPer = _PercentToY(zeroper);
			Point pStart = Point( 0, IzeroPer );
			Point pEnd = Point( NumMapPanel->Width, IzeroPer );
			m_pPen->Color = Color::FromArgb(0xFF000000);
			Gra->DrawLine(m_pPen,pStart, pEnd);
		}
		// 第一个方框
		int x1, y1, x2, y2;
		_GetXYPositon(0, x1, y1);
		_DrawRect(Gra, x1-2, y1-2, 0);
		float val = NumMapimp->GetNumMapVal(0);
		_DrawFloatString(Gra, x1 + 2, y1 + 2, val);
		
		// 线 框
		for (int i = 1; i < ct; ++i)
		{
			_GetXYPositon(i, x2, y2);
			_DrawRect(Gra, x2-2, y2-2, i);	
			// 画线
			Point posStart = Point( x1, y1 );
			Point posEnd = Point( x2, y2 );
			m_pPen->Color = Color::FromArgb(0xFF000000);
			Gra->DrawLine(m_pPen,posStart, posEnd);
			// 文字
			val = NumMapimp->GetNumMapVal(i);
			_DrawFloatString(Gra, x2 + 2, y2 + 2, val);

			x1 = x2; 
			y1 = y2;
		}
		NumMapPanel->BackgroundImage = tempNumMap;
	}

	void	NumMapEditCtrl::_DrawRect(Graphics^ Gra, int X, int Y, int i)
	{
		NumMapEditCtrlImp* NumMapimp = (NumMapEditCtrlImp*)m_pCtrl;
		float idx = NumMapimp->GetNumMapIdx(i);
		float sel = NumMapimp->GetCurPercent();
		if (idx == sel)
		{
			m_pBursh->Color = Color::FromArgb(0xFF000000);
			Gra->FillRectangle(m_pBursh, X, Y, 4, 4);
		}
		else
		{
			m_pPen->Color = Color::FromArgb(0xFF000000);
			Gra->DrawRectangle(m_pPen, X, Y, 4, 4);
		}
		
	}

	void	NumMapEditCtrl::_DrawFloatString(Graphics^ Gra, int X, int Y, float val)
	{
		Double Dval = val;
		System::String^ drawStr = Dval.ToString("F2");
		
		int lt = drawStr->Length * 7;
		int ht = 12;
		if (X + lt > NumMapPanel->Width - 3)
			X = NumMapPanel->Width - lt - 3;
		if (Y + ht > NumMapPanel->Height - 3)
			Y = NumMapPanel->Height - ht - 3;
		m_pBursh->Color = Color::FromArgb(0xFF000000);
		System::Drawing::Rectangle rtt = System::Drawing::Rectangle( X, Y, lt, ht);
		Gra->DrawString(drawStr, m_pFont, m_pBursh, rtt);
	}


	void	NumMapEditCtrl::_GetXYPositon(int i, int& X, int& Y)
	{
		NumMapEditCtrlImp* NumMapimp = (NumMapEditCtrlImp*)m_pCtrl;
		float x = NumMapimp->GetNumMapIdx(i);
		float y = NumMapimp->GetNumMapValPercent(i);
		X = _PercentToX(x);
		Y = _PercentToY(y);
	}

	void	NumMapEditCtrl::_SetCurXPercent(int X)
	{
		bool beFind = false;
		int tempW = NumMapPanel->Width - 6;
		float percent = _XToPercent(X);
		NumMapEditCtrlImp* NumMapimp = (NumMapEditCtrlImp*)m_pCtrl;
		int ct = NumMapimp->GetNumMapCount();
		for (int i = 0; i < ct; ++i)
		{
			float per = NumMapimp->GetNumMapIdx(i);
			float tempDis = per - percent;
			tempDis < 0.0f ? tempDis = -tempDis : tempDis = tempDis;
			if ( tempDis < 3.0f/(tempW*1.0f) )
			{
				NumMapimp->SetCurPercent(per);
				beFind = true;
				break;
			}
		}
		if (!beFind)
			NumMapimp->SetCurPercent(percent);
	}
	void	NumMapEditCtrl::_SetCurYPercent(int Y)
	{
		NumMapEditCtrlImp* NumMapimp = (NumMapEditCtrlImp*)m_pCtrl;
		float per = _YToPercent(Y);
		NumMapimp->SetCurNodeVal(per);
	}

	System::Void NumMapEditCtrl::NumMapPanel_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		System::Windows::Forms::MouseButtons mbt = e->Button;
		switch(mbt)
		{
		case System::Windows::Forms::MouseButtons::Middle:
			break;
		case System::Windows::Forms::MouseButtons::Right:
			{
				NumMapEditCtrlImp* NumMapimp = (NumMapEditCtrlImp*)m_pCtrl;
				MaxMinNumEditDialog m_MaxMinDlg;
				m_MaxMinDlg.SetMaxMinEditData(NumMapimp);
				m_MaxMinDlg.ShowDialog();
			}
			
			break;
		case System::Windows::Forms::MouseButtons::Left:
			int X = e->Location.X;
			if ( X > NumMapPanel->Width)
				X = NumMapPanel->Width;	
			_SetCurXPercent(X);
			Update();
			break;
		}
		this->Focus();	
	}

	System::Void NumMapEditCtrl::NumMapPanel_MouseMove( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		if( e->Button == System::Windows::Forms::MouseButtons::Left )
		{
			NumMapEditCtrlImp* NumMapimp = (NumMapEditCtrlImp*)m_pCtrl;
			if (NumMapimp->GetCurPercent() > 0.01f && NumMapimp->GetCurPercent() < 0.99f 
				&& System::Windows::Forms::Control::ModifierKeys != Keys::Control)
			{
				NumMapimp->DelCurPerNode();
				int X = e->Location.X;
				if ( X > NumMapPanel->Width)
					X = NumMapPanel->Width;	
				_SetCurXPercent(X);
			}
			int Y = e->Location.Y;
			_SetCurYPercent(Y);
			if ( Y > NumMapPanel->Height)
				Y = NumMapPanel->Height;	
			Update();
		}
	}

	System::Void NumMapEditCtrl::NumMapEditCtrl_KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
	{
		if( e->KeyCode == Keys::Delete )
		{
			// 删除当前选中的节点
			NumMapEditCtrlImp* NumMapimp = (NumMapEditCtrlImp*)m_pCtrl;
			NumMapimp->DelCurPerNode();
			Update();
		}
	}
}
