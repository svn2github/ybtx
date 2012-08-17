#include "StdAfx.h"
#include "CEfxEditProp.h"
#include "CEfxEditPropImp.h"
#include "AutoPropToCtrl/AutoPropToCtrl.h"
#include "EffectTrack/CEfxTrackCtrl.h" 

namespace sqr_tools
{
	char* CEfxEditProp::GetImpTypeName( void )
	{
		return GET_IMP_NAME(CEfxEditPropImp);
	}

	System::Void  CEfxEditProp::InitOtherComponent()
	{
		this->SuspendLayout();

		m_TrackCtrls = gcnew System::Collections::Generic::List<CEfxTrackCtrl^>();
		this->m_AutoPropToCtrl = gcnew AutoPropToCtrl();
		this->m_AutoPropToCtrl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
			| System::Windows::Forms::AnchorStyles::Left) 
			| System::Windows::Forms::AnchorStyles::Right));
		this->m_AutoPropToCtrl->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
		this->m_AutoPropToCtrl->Location = System::Drawing::Point(0, 0);
		this->m_AutoPropToCtrl->Size = System::Drawing::Size(500, 651);

		this->Controls->Add(this->m_AutoPropToCtrl);
		this->m_AutoPropToCtrl->Hide();

		this->ResumeLayout(false);
		m_fScale = 1.0f;
	}

	System::Void  CEfxEditProp::DestroyOtherComponent()
	{
		
	}

	System::Void CEfxEditProp::TimeSetTextBox_KeyPress( System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e )
	{
		e->Handled = true;
		if (e->KeyChar >= '0' && e->KeyChar <= '9' || e->KeyChar == '\b')
			e->Handled = false;
		return;
	}

	void CEfxEditProp::ClearTracks(void)
	{
		this->SuspendLayout();
		for( int32 i = 0; i < m_TrackCtrls->Count; ++i )
			delete m_TrackCtrls[i];
		m_TrackCtrls->Clear();
		this->ResumeLayout(false);
	}

	void CEfxEditProp::ReSetTracks(void)
	{
		OnBackTrack();
		ClearTracks();

		this->SuspendLayout();
		CEfxEditPropImp* EfxPropimp = (CEfxEditPropImp*)m_pCtrl;
		for (uint32 i = 0; i < EfxPropimp->GetTracksNum(); ++i)
		{
			CEfxTrackCtrl^ pTrackCtrl = gcnew CEfxTrackCtrl();
			EfxEditPanel->Controls->Add(pTrackCtrl);
			// 完成一些属性设置
			void* editTrack = NULL;
			EfxPropimp->GetEditTrack( i, &editTrack );
			void* EditProp = EfxPropimp->GetEditEffectProp();
			pTrackCtrl->SetTrackContent(editTrack,EditProp);

			m_TrackCtrls->Add( pTrackCtrl );
		}
		this->ResumeLayout(false);
	}

	float CEfxEditProp::GetCurScale()
	{
		return m_fScale;
	}

	void CEfxEditProp::UpdateTracksBG(bool ctrl)
	{
		for( int32 i = 0; i < m_TrackCtrls->Count; ++i )
		{
			m_TrackCtrls[i]->RefreshTrackState(ctrl);
		}
	}

	void CEfxEditProp::UpdateTrackContent(void)
	{
		// 更新所有的子TRACK显示
		for( int32 i = 0; i < m_TrackCtrls->Count; ++i )
		{
			// 完成TRACK控件内容刷新,更新位置
			m_TrackCtrls[i]->RefreshTrack();
		}
		UpdateTrackPosAndScale();
	}

	void CEfxEditProp::UpdateTrackPosAndScale(void)
	{
		int ptx = 0, pty = 0;
		int iWidth = (int)(EfxEditPanel->Width * m_fScale);
		for( int32 i = 0; i < m_TrackCtrls->Count; ++i )
		{
			int iHeight = m_TrackCtrls[i]->Height;
			m_TrackCtrls[i]->Location = System::Drawing::Point(ptx, pty);
			m_TrackCtrls[i]->Size = System::Drawing::Size(iWidth, iHeight);
			pty += (iHeight + 1);

			// 更新TRACK内部KEY
			m_TrackCtrls[i]->RefreshTrackPos();
		}
	}

	void CEfxEditProp::OnKeyUintEdit(void* content)
	{
		EfxEditPanel->Hide();
		m_AutoPropToCtrl->Show();
		// 初始化内容
		m_AutoPropToCtrl->SetPropContent(content);
	}


	void CEfxEditProp::OnBackTrack()
	{
		m_AutoPropToCtrl->Hide();
		m_AutoPropToCtrl->SetPropContent(NULL);
		EfxEditPanel->Show();
	}

	void CEfxEditProp::Update(void)
	{
		// 根据数据创建更新轨道，这个走大范围的全面更新
		ReSetTracks();
		UpdateTrackContent();
	}
}
