#include "StdAfx.h"
#include "Vector3EditDialog.h"
#include "CCtrlPropBase.h"
#include "Vector3EditCtrlImp.h" 
#include <math.h>

#define STAND_MAX_LENGTH 500

namespace sqr_tools {
	System::Void  Vector3EditDialog::InitOtherComponent()
	{
		m_pVecEditCtrl = NULL;
		m_fMaxLength = 500;
		m_fVecLength = 0.0f;
		m_bNeedUpdateCtrl = true;

		m_pBursh	= gcnew System::Drawing::SolidBrush(Color::FromArgb(0xFFFFFFFF));
		m_pPen		= gcnew System::Drawing::Pen(Color::FromArgb(0xFFFFFFFF), 1.5f);
	}

	System::Void  Vector3EditDialog::DestroyOtherComponent()
	{
	}
	
	void	Vector3EditDialog::SetVecEditCtrl(void* ctrl)
	{
		m_pVecEditCtrl = (Vector3EditCtrlImp*)ctrl;
		// 根据值计算出合适的最大值
		float tempMax = m_pVecEditCtrl->GetImpXValue();
		tempMax <= m_pVecEditCtrl->GetImpYValue() ? tempMax = m_pVecEditCtrl->GetImpYValue() : tempMax = tempMax ;
		tempMax <= m_pVecEditCtrl->GetImpYValue() ? tempMax = m_pVecEditCtrl->GetImpYValue() : tempMax = tempMax ;
		if (tempMax < STAND_MAX_LENGTH)
			m_fMaxLength = STAND_MAX_LENGTH;
		else
			m_fMaxLength = (int)(2 * tempMax);

		Int32 dt = m_fMaxLength;
		System::String^ Textstr = dt.ToString();
		MaxNumericUpDown->Text = Textstr;
		m_bNeedUpdateCtrl = false;
		// 刷新缩放条
		_RefreshSetingBars();
		_SetSetingBarsVal();
		_SetVectorXYZ();
		_SetVectorLength();

		m_bNeedUpdateCtrl = true;
	}

	void Vector3EditDialog::_RefreshSetingBars()
	{
		this->XTrackBar->Maximum = m_fMaxLength * 100;
		this->XTrackBar->Minimum = -m_fMaxLength * 100;
		this->YTrackBar->Maximum = m_fMaxLength * 100;
		this->YTrackBar->Minimum = -m_fMaxLength * 100;
		this->ZTrackBar->Maximum = m_fMaxLength * 100;
		this->ZTrackBar->Minimum = -m_fMaxLength * 100;

		this->XNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {m_fMaxLength, 0, 0, 0});
		this->XNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {m_fMaxLength, 0, 0, System::Int32::MinValue});
		this->YNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {m_fMaxLength, 0, 0, 0});
		this->YNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {m_fMaxLength, 0, 0, System::Int32::MinValue});
		this->ZNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {m_fMaxLength, 0, 0, 0});
		this->ZNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {m_fMaxLength, 0, 0, System::Int32::MinValue});

		
		float temp = (m_fMaxLength * m_fMaxLength + m_fMaxLength * m_fMaxLength + m_fMaxLength * m_fMaxLength)*1.0f;
		int MaxL = (int)(sqrtf(temp));
		this->LengthnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {MaxL, 0, 0, 0});
	}

	void Vector3EditDialog::_SetSetingBarsVal()
	{
		this->XTrackBar->Value = (int)((m_pVecEditCtrl->GetImpXValue())*100);
		this->YTrackBar->Value = (int)((m_pVecEditCtrl->GetImpYValue())*100);
		this->ZTrackBar->Value = (int)((m_pVecEditCtrl->GetImpYValue())*100);
	}

	void Vector3EditDialog::_SetVectorXYZ()
	{
		Double dt = m_pVecEditCtrl->GetImpXValue();
		System::String^ Textstr = dt.ToString();	
		this->XNumericUpDown->Text =Textstr;

		dt = m_pVecEditCtrl->GetImpYValue();
		Textstr = dt.ToString();	
		this->YNumericUpDown->Text =Textstr;

		dt = m_pVecEditCtrl->GetImpYValue();
		Textstr = dt.ToString();	
		this->ZNumericUpDown->Text =Textstr;
		
		// 更新两个texture
		_DrawXYZPanel();
		// 数据变，关联显示变
		if (m_pVecEditCtrl)
			static_cast<CCtrlPropBase*>(m_pVecEditCtrl)->OnDataReFresh();
	}


	void Vector3EditDialog::_DrawXYZPanel()
	{
		System::Drawing::Bitmap^ tempMapXZ = gcnew System::Drawing::Bitmap(XZPanel->Width,XZPanel->Height);
		System::Drawing::Bitmap^ tempMapXY = gcnew System::Drawing::Bitmap(XYPanel->Width,XYPanel->Height);
		float l, x, y, z;
		_CaculateVec(l, x,  y,  z);
		int Len = XZPanel->Width/2 - 3;
		int w,h;
		w = (int)(XZPanel->Width/2 + x * Len);
		h = (int)(XZPanel->Width/2 - y * Len);
		Graphics^ Gra = Graphics::FromImage(tempMapXY);
		_DrawPanel(Gra, w, h);	// 平视
		XYPanel->BackgroundImage = tempMapXY;

		w = (int)(XZPanel->Width/2 + x * Len);
		h = (int)(XZPanel->Width/2 - z * Len);
		Gra = Graphics::FromImage(tempMapXZ);
		_DrawPanel(Gra, w, h);	// 俯视
		XZPanel->BackgroundImage = tempMapXZ;
	}



	void Vector3EditDialog::_DrawPanel(Graphics^ Gra, int w, int h)
	{
		// 背景
		m_pBursh->Color = Color::FromArgb(0xFF000000);
		Gra->FillRectangle(m_pBursh, 0, 0, XZPanel->Width, XZPanel->Height);
		// 方框
		m_pBursh->Color = Color::FromArgb(0xFFFFFFFF);
		Gra->FillRectangle(m_pBursh, w-2, h-2, 4, 4);
		// 线
		Point posStart = Point( XZPanel->Width/2, XZPanel->Height/2 );
		Point posEnd = Point( w, h );
		m_pPen->Color = Color::FromArgb(0xFFFFFFFF);
		Gra->DrawLine(m_pPen,posStart, posEnd);
	}

	void Vector3EditDialog::_CaculateVec(float& l, float& x, float& y, float& z)
	{
		l = sqrtf((m_pVecEditCtrl->GetImpXValue()) * (m_pVecEditCtrl->GetImpXValue()) + (m_pVecEditCtrl->GetImpYValue()) * (m_pVecEditCtrl->GetImpYValue()) + (m_pVecEditCtrl->GetImpYValue()) * (m_pVecEditCtrl->GetImpYValue()));
		if (l > 0.0001)
		{
			x = (m_pVecEditCtrl->GetImpXValue())/l;
			y = (m_pVecEditCtrl->GetImpYValue())/l;
			z = (m_pVecEditCtrl->GetImpYValue())/l;
		}
		else
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}
	}

	void Vector3EditDialog::_SetVectorLength()
	{
		float l, x, y, z;
		_CaculateVec(l, x, y, z);
		m_fVecLength = l;	m_VecX = x; m_VecY = y; m_VecZ = z;
		Double dt = m_fVecLength;
		System::String^ Textstr = dt.ToString();	
		this->LengthnumericUpDown->Text =Textstr;
	}

	System::Void Vector3EditDialog::MaxSetButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		bool tempUpdate = false;
		m_fMaxLength = (int)(Convert::ToInt32(MaxNumericUpDown->Value));
		if (abs(m_pVecEditCtrl->GetImpXValue()) > m_fMaxLength)
		{
			if (m_pVecEditCtrl->GetImpXValue() > 0)
				m_pVecEditCtrl->SetImpXValue( m_fMaxLength*1.0f );
			else
				m_pVecEditCtrl->SetImpXValue(  -m_fMaxLength*1.0f );
			tempUpdate = true;
		}
		if (abs(m_pVecEditCtrl->GetImpYValue()) > m_fMaxLength)
		{
			if (m_pVecEditCtrl->GetImpYValue() > 0)
				m_pVecEditCtrl->SetImpYValue( m_fMaxLength*1.0f );
			else
				m_pVecEditCtrl->SetImpYValue( -m_fMaxLength*1.0f );
			tempUpdate = true;
		}
		if (abs(m_pVecEditCtrl->GetImpYValue()) > m_fMaxLength)
		{
			if (m_pVecEditCtrl->GetImpYValue() > 0)
				m_pVecEditCtrl->SetImpYValue( m_fMaxLength*1.0f );
			else
				m_pVecEditCtrl->SetImpYValue( -m_fMaxLength*1.0f );
			tempUpdate = true;
		}
		m_bNeedUpdateCtrl = false;
		_RefreshSetingBars();
		if (tempUpdate)
		{
			_SetSetingBarsVal();
			_SetVectorXYZ();
			_SetVectorLength();
		}
		
		m_bNeedUpdateCtrl = true;

		if (tempUpdate && m_pVecEditCtrl)
			static_cast<CCtrlPropBase*>(m_pVecEditCtrl)->OnDataReFresh();
	}

	System::Void Vector3EditDialog::LengthnumericUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		if(!m_bNeedUpdateCtrl)
			return;
		m_bNeedUpdateCtrl = false;
		float val = (float)(Convert::ToDouble(LengthnumericUpDown->Value));

		m_pVecEditCtrl->SetImpXValue( m_VecX * val );
		m_pVecEditCtrl->SetImpYValue( m_VecY * val );
		m_pVecEditCtrl->SetImpYValue( m_VecZ * val );

		_SetSetingBarsVal();
		_SetVectorXYZ();

		m_bNeedUpdateCtrl = true;
		if (m_pVecEditCtrl)
			static_cast<CCtrlPropBase*>(m_pVecEditCtrl)->OnDataReFresh();
	}

	System::Void Vector3EditDialog::ZNumericUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		if(!m_bNeedUpdateCtrl)
			return;
		m_bNeedUpdateCtrl = false;
		m_pVecEditCtrl->SetImpYValue((float)(Convert::ToDouble(ZNumericUpDown->Value)));

		_SetSetingBarsVal();
		_SetVectorLength();
		// 更新两个texture
		_DrawXYZPanel();

		m_bNeedUpdateCtrl = true;
		if (m_pVecEditCtrl)
			static_cast<CCtrlPropBase*>(m_pVecEditCtrl)->OnDataReFresh();
	}

	System::Void Vector3EditDialog::YNumericUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		if(!m_bNeedUpdateCtrl)
			return;
		m_bNeedUpdateCtrl = false;
		m_pVecEditCtrl->SetImpYValue((float)(Convert::ToDouble(YNumericUpDown->Value)));

		_SetSetingBarsVal();
		_SetVectorLength();
		// 更新两个texture
		_DrawXYZPanel();

		m_bNeedUpdateCtrl = true;
		if (m_pVecEditCtrl)
			static_cast<CCtrlPropBase*>(m_pVecEditCtrl)->OnDataReFresh();
	}

	System::Void Vector3EditDialog::XNumericUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		if(!m_bNeedUpdateCtrl)
			return;
		m_bNeedUpdateCtrl = false;
		m_pVecEditCtrl->SetImpXValue((float)(Convert::ToDouble(XNumericUpDown->Value)));

		_SetSetingBarsVal();
		_SetVectorLength();
		// 更新两个texture
		_DrawXYZPanel();

		m_bNeedUpdateCtrl = true;
		if (m_pVecEditCtrl)
			static_cast<CCtrlPropBase*>(m_pVecEditCtrl)->OnDataReFresh();
	}

	System::Void Vector3EditDialog::XTrackBar_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		if(!m_bNeedUpdateCtrl)
			return;
		m_bNeedUpdateCtrl = false;
		
		m_pVecEditCtrl->SetImpXValue((float)(Convert::ToDouble(XTrackBar->Value))/100.0f);
		_SetVectorXYZ();
		_SetVectorLength();
		m_bNeedUpdateCtrl = true;
		if (m_pVecEditCtrl)
			static_cast<CCtrlPropBase*>(m_pVecEditCtrl)->OnDataReFresh();
	}

	System::Void Vector3EditDialog::YTrackBar_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		if(!m_bNeedUpdateCtrl)
			return;
		m_bNeedUpdateCtrl = false;

		m_pVecEditCtrl->SetImpYValue((float)(Convert::ToDouble(YTrackBar->Value))/100.0f);
		_SetVectorXYZ();
		_SetVectorLength();
		m_bNeedUpdateCtrl = true;
		if (m_pVecEditCtrl)
			static_cast<CCtrlPropBase*>(m_pVecEditCtrl)->OnDataReFresh();
	}

	System::Void Vector3EditDialog::ZTrackBar_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		if(!m_bNeedUpdateCtrl)
			return;
		m_bNeedUpdateCtrl = false;

		m_pVecEditCtrl->SetImpYValue((float)(Convert::ToDouble(ZTrackBar->Value))/100.0f);
		_SetVectorXYZ();
		_SetVectorLength();
		m_bNeedUpdateCtrl = true;
		if (m_pVecEditCtrl)
			static_cast<CCtrlPropBase*>(m_pVecEditCtrl)->OnDataReFresh();
	}

	System::Void Vector3EditDialog::Vector3EditDialog_FormClosing( System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e )
	{
		if(this->Visible)
		{
			e->Cancel=true;
			this->Hide();
		}
	}
}