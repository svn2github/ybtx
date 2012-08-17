#include "StdAfx.h"
#include "CEfxKeyUintCtrl.h"
#include "CEfxKeyUintCtrlImp.h"
#include "EffectTrack/CEfxTrackCtrl.h"

namespace sqr_tools {

	char* CEfxKeyUintCtrl::GetImpTypeName( void )
	{
		return GET_IMP_NAME(CEfxKeyUintCtrlImp);
	}

	void CEfxKeyUintCtrl::SetKeyContent(void* content)
	{
		CEfxKeyUintCtrlImp* EfxEditKeyimp = (CEfxKeyUintCtrlImp*)m_pCtrl;
		EfxEditKeyimp->SetEditKeyContent(content);

		InitKeyName();
	}

	System::Void  CEfxKeyUintCtrl::InitOtherComponent()
	{
		m_uCtrlState = KEY_STATE_NORMAL;
	}

	System::Void  CEfxKeyUintCtrl::DestroyOtherComponent()
	{

	}


	void  CEfxKeyUintCtrl::RefreshKeyState()
	{
		
		if (m_uCtrlState & KEY_STATE_SELPRE)
		{
			m_uCtrlState = (m_uCtrlState & (~KEY_STATE_SELPRE)) | KEY_STATE_SEL;
		}
		else
			m_uCtrlState = m_uCtrlState & (~KEY_STATE_SEL);
	
		CEfxTrackCtrl^ trackCtrl = static_cast<CEfxTrackCtrl^>(this->Parent);
		int trackState = trackCtrl->GetTrackState();
		// 设置颜色
		if (m_uCtrlState & KEY_STATE_SEL)
		{
			// 选中的
			if ((trackState & TRACKCTRL_STATE_HIDE) && (trackState & TRACKCTRL_STATE_SEL))
			{
				// 父亲隐藏的  选中的
				this->BackColor = System::Drawing::Color::ForestGreen;
			}
			else if (trackState & TRACKCTRL_STATE_HIDE)
			{
				// 隐藏的 未选中的
				this->BackColor = System::Drawing::Color::Chartreuse;
			}
			else if(trackState & TRACKCTRL_STATE_SEL)
			{
				// 不隐藏，选中
				this->BackColor = System::Drawing::Color::ForestGreen;
			}
			else
			{
				// 不隐藏，未选中
				this->BackColor = System::Drawing::Color::Chartreuse;
			}
		}
		else
		{
			// 未选中的
			if ((trackState & TRACKCTRL_STATE_HIDE) && (trackState & TRACKCTRL_STATE_SEL))
			{
				// 父亲隐藏的  选中的
				this->BackColor = System::Drawing::Color::DarkSeaGreen;
			}
			if ((trackState & TRACKCTRL_STATE_HIDE) && (trackState & TRACKCTRL_STATE_SELS))
			{
				// 父亲隐藏的  多选选中的
				this->BackColor = System::Drawing::Color::DarkSeaGreen;
			}
			else if (trackState & TRACKCTRL_STATE_HIDE)
			{
				// 隐藏的 未选中的
				this->BackColor = System::Drawing::Color::Chartreuse;
			}
			else if(trackState & TRACKCTRL_STATE_SEL)
			{
				// 不隐藏，选中
				this->BackColor = System::Drawing::Color::DarkSeaGreen;
			}
			else if(trackState & TRACKCTRL_STATE_SELS)
			{
				// 不隐藏，多选选中
				this->BackColor = System::Drawing::Color::DarkSeaGreen;
			}
			else
			{
				// 不隐藏，未选中
				this->BackColor = System::Drawing::Color::Chartreuse;
			}
		}
	}
	
	void CEfxKeyUintCtrl::InitKeyName()
	{
		// 完成控件名字的设置
		CEfxKeyUintCtrlImp* EfxEditKeyimp = (CEfxKeyUintCtrlImp*)m_pCtrl;
		String^ clrKeyText = gcnew String(EfxEditKeyimp->GetEditKeyText().c_str());
		
		KeyNameLable->Text = clrKeyText;

		CKeytoolTip->SetToolTip(this, clrKeyText);
		CKeytoolTip->SetToolTip(this->KeyNameLable, clrKeyText);
	}

	void  CEfxKeyUintCtrl::OnKeySelected()
	{
		m_uCtrlState |= KEY_STATE_SELPRE;

		CEfxTrackCtrl^ trackCtrl = static_cast<CEfxTrackCtrl^>(this->Parent);
		trackCtrl->OnTrackSelected();
	}

	void  CEfxKeyUintCtrl::OnKeySelectedLots()
	{
		m_uCtrlState |= KEY_STATE_SELPRE;

		CEfxTrackCtrl^ trackCtrl = static_cast<CEfxTrackCtrl^>(this->Parent);
		trackCtrl->OnTrackSelectedLots();
	}

	void  CEfxKeyUintCtrl::OnKeyUintEdit()
	{
		CEfxTrackCtrl^ trackCtrl = static_cast<CEfxTrackCtrl^>(this->Parent);
		CEfxKeyUintCtrlImp* EfxEditKeyimp = (CEfxKeyUintCtrlImp*)m_pCtrl;
		trackCtrl->OnKeyUintEdit(EfxEditKeyimp->GetEditKeyContent());
	}

	System::Void CEfxKeyUintCtrl::CEfxKeyUintCtrl_MouseClick( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		System::Windows::Forms::MouseButtons mbt = e->Button;
		switch(mbt)
		{
		case System::Windows::Forms::MouseButtons::Middle:

			break;
		case System::Windows::Forms::MouseButtons::Right:

			break;
		case System::Windows::Forms::MouseButtons::Left:
			if (System::Windows::Forms::Control::ModifierKeys == Keys::Control)
				OnKeySelectedLots();
			else
				OnKeySelected();
			break;
		}
	}

	System::Void CEfxKeyUintCtrl::CEfxKeyUintCtrl_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
		OnKeyUintEdit();
	}

	System::Void CEfxKeyUintCtrl::KeyNameLable_MouseClick( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		System::Windows::Forms::MouseButtons mbt = e->Button;
		switch(mbt)
		{
		case System::Windows::Forms::MouseButtons::Middle:

			break;
		case System::Windows::Forms::MouseButtons::Right:

			break;
		case System::Windows::Forms::MouseButtons::Left:
			if (System::Windows::Forms::Control::ModifierKeys == Keys::Control)
				OnKeySelectedLots();
			else
				OnKeySelected();
			break;
		}
	}

	System::Void CEfxKeyUintCtrl::KeyNameLable_MouseDoubleClick( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		OnKeyUintEdit();
	}

	System::Void CEfxKeyUintCtrl::CEfxKeyUintCtrl_KeyPress( System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e )
	{
		
	}


}
