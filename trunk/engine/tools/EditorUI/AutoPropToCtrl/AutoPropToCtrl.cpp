#include "StdAfx.h"
#include "AutoPropToCtrl.h"
#include "AutoPropToCtrlImp.h"
#include "CUIPropBase.h"
#include "BaseDataCtrl/BoolEditCtrl.h"
#include "BaseDataCtrl/NumEditCtrl.h"
#include "BaseDataCtrl/SingleStringEditCtrl.h"
#include "BaseDataCtrl/ComBoEditCtrl.h"
#include "BaseDataCtrl/Vector3EditCtrl.h"
#include "BaseDataCtrl/NumMapEditCtrl.h"
#include "BaseDataCtrl/Vector3MapCtrl.h"
#include "BaseDataCtrl/EStrMapEditCtrl.h"
#include "BaseDataCtrl/CueListDialog.h"
#include "ColorAnimCtrl/CColorAnimCtrl.h"
#include "AutoDataUintCtrl.h"
#include "CUIPropBase.h"

namespace sqr_tools {

	char* AutoPropToCtrl::GetImpTypeName( void )
	{
		return GET_IMP_NAME(AutoPropToCtrlImp);
	}

	void AutoPropToCtrl::SetPropContent(void* content)
	{
		this->FourceButton->Focus();
		// 清空CHILD
		ClearKeyPropUint();
		// 设置属性
		if (m_pCtrl)
		{
			AutoPropToCtrlImp* Autoimp = (AutoPropToCtrlImp*)m_pCtrl;
			Autoimp->SetEditPropContent(content);
			// 更新CHILD
			UpdateKeyPropUint();
		}
	}

	System::Void  AutoPropToCtrl::InitOtherComponent()
	{
		m_CueListDlg = gcnew sqr_tools::CueListDialog();
		m_KeyPropUint = gcnew System::Collections::Generic::List<AutoDataUintCtrl^>();
		m_PropUintBase = gcnew System::Collections::Generic::List<CUIPropBase^>();
	}

	System::Void  AutoPropToCtrl::DestroyOtherComponent()
	{
		
	}

	void  AutoPropToCtrl::ClearKeyPropUint()
	{
		this->SuspendLayout();
		for( int32 i = 0; i < m_KeyPropUint->Count; ++i )
			delete m_KeyPropUint[i];
		m_KeyPropUint->Clear();
		for( int32 i = 0; i < m_PropUintBase->Count; ++i )
			delete m_PropUintBase[i];
		m_PropUintBase->Clear();
		this->ResumeLayout(false);
		this->AutoScroll = true;
	}

	void  AutoPropToCtrl::_CreatPropUintUI(int tp, void* ct, const char* title)
	{
		ECtrlDataType etp = (ECtrlDataType)tp;
		CUIPropBase^		m_PropUintUI;
		AutoDataUintCtrl^	m_PropCtrl =  gcnew AutoDataUintCtrl();
		this->SuspendLayout();

		bool isLayoutAuto = true;
		switch(tp)
		{
		case E_STRING:
		case E_ESTRING:
		case E_STRINGBUTTON:
		case E_ESTRINGBUTTON:
		case E_ESETESTRING:
		case E_EVECESTRING:
		case E_COLORSTRING:
		case E_CHARPTR:
		case E_CUEESTRING:
			{
				m_PropUintUI = gcnew SingleStringEditCtrl();
				static_cast<SingleStringEditCtrl^>(m_PropUintUI)->SetCueListDlg(m_CueListDlg);
				isLayoutAuto = true;
			}
			break;
		case E_INTEGER:
		case E_FLOAT:
		case E_BYTE:
			{
				m_PropUintUI = gcnew NumEditCtrl();
				isLayoutAuto = true;
			}
			break;
		case E_VECTOR:
		case E_VECTOR2F:
			{
				m_PropUintUI = gcnew Vector3EditCtrl();
				isLayoutAuto = true;
			}
			break;
		case E_COMBO:
			{
				m_PropUintUI = gcnew ComBoEditCtrl();
				isLayoutAuto = true;
			}
			break;
		case E_DBWORD:
			delete m_PropCtrl;
			this->ResumeLayout(false);
			return;
			break;
		case E_FloatMap:
		case E_EFloatMap:
		case E_BYTEMap:
		case E_EBYTEMap:
			{
				m_PropUintUI = gcnew NumMapEditCtrl();
				isLayoutAuto = false;
			}
			break;
		case E_VectorMap:
		case E_EVectorMap:
			{
				m_PropUintUI = gcnew Vector3MapCtrl();
				isLayoutAuto = false;
			}
			break;
		case E_BOOLValue:
			{
				m_PropUintUI = gcnew BoolEditCtrl();
				isLayoutAuto = true;
			}
			break;
		case E_EEStrMap:
			{
				m_PropUintUI = gcnew EStrMapEditCtrl();
				isLayoutAuto = false;
			}
			break;
		case E_COLOR4Map:
		case E_COLORMap:
		case E_UINT32CLMap:
		case E_FLOATCLMap:
		case E_ECOLORMap:
			{
				m_PropUintUI = gcnew CColorAnimCtrl();
				m_PropUintUI->Size	= System::Drawing::Size(471, 110);
				isLayoutAuto = false;
			}
			break;
		default:
			delete m_PropCtrl;
			this->ResumeLayout(false);
			return;
		}
		
		
		this->Controls->Add(m_PropCtrl);
		m_PropCtrl->LayoutAutoDataCtrl(isLayoutAuto, title, m_PropUintUI);
		m_PropUintUI->SetContentData(ct);
		m_PropUintUI->ContextComponent = this->ContextComponent;

		m_KeyPropUint->Add( m_PropCtrl );
		m_PropUintBase->Add(m_PropUintUI);
		this->ResumeLayout(false);
	}

	void  AutoPropToCtrl::_RefReshPropUintPos()
	{
		this->SuspendLayout();
		int posX = 0, posY = 0;
		for( int32 i = 0; i < m_KeyPropUint->Count; ++i )
		{
			m_KeyPropUint[i]->Location = System::Drawing::Point(posX, posY);
			posY += m_KeyPropUint[i]->Height;
		}
		
		this->ResumeLayout(false);
	}

	void  AutoPropToCtrl::Update()
	{
		// UPDATE的是内容
		AutoPropToCtrlImp* Autoimp = (AutoPropToCtrlImp*)m_pCtrl;
		int ct = Autoimp->GetEditPropUintCount(); 
		if (ct == 0)
		{
			ClearKeyPropUint();
			return;
		}
		// 更新每个节点的内容
		for( int32 i = 0; i < m_PropUintBase->Count; ++i )
			m_PropUintBase[i]->Update();
	}
	
	void  AutoPropToCtrl::UpdateKeyPropUint()
	{
		ClearKeyPropUint();
		AutoPropToCtrlImp* Autoimp = (AutoPropToCtrlImp*)m_pCtrl;
		int ct = Autoimp->GetEditPropUintCount(); 
		for (int i = 0; i < ct; ++i)
		{
			ECtrlDataType tp = Autoimp->GetEditPropUintType(i);
			void*	ct = Autoimp->GetEditPropUintData(i);
			const char* title = Autoimp->GetEditPropTitle(i);
			
			_CreatPropUintUI(tp, ct, title);
		}
		_RefReshPropUintPos();
		this->AutoScroll = true;
	}

	System::Void AutoPropToCtrl::AutoPropToCtrl_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		FourceButton->Location = e->Location;
		this->ActiveControl = FourceButton; 
	}

	System::Void AutoPropToCtrl::FourceButton_KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
	{
		_KeyDown(sender,e);
	}

	System::Void AutoPropToCtrl::FourceButton_KeyUp( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
	{
		_KeyUp(sender,e);
	}
}