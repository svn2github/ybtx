#include "StdAfx.h"
#include "CUIBase.h"
#include "ClrMsgDefine.h"
#include "CCtrlBase.h"
#include "ICtrlImpFactory.h"
#include "ContextCtrl/CContextCtrl.h"

using namespace sqr;

System::Void  CUIBase::Initialize(void* ImpFactory)
{
	st_pImpFact = (ICtrlImpFactory*)ImpFactory;
}

void BaseUICallBack(void* pUI)
{
	IntPtr ptr = (IntPtr)pUI;
	CUIBase^ pUIBase = (CUIBase^)System::Runtime::InteropServices::Marshal::GetObjectForIUnknown(ptr);	
	if(pUIBase)
		pUIBase->Update();
}

CContextComponent^ CUIBase::ContextComponent::get()
{
	return m_pContextCom;	
}

void CUIBase::ContextComponent::set(CContextComponent^ val)
{
	m_pContextCom = val;		
	if(m_pContextCom!=nullptr && GetCoreCtrl())
	{
		CContextCtrlImp* pContextImp = (CContextCtrlImp*)m_pContextCom->GetCoreCtrl();
		if(pContextImp)
			m_pCtrl->SetContext(pContextImp->GetContext());
	}
}

CUIBase::CUIBase(void)
{
	m_pCtrl=NULL;
	m_pContextCom = nullptr;
	this->Load += gcnew System::EventHandler(this, &CUIBase::_Load);		
}

void* CUIBase::GetImpParam(void)
{
	return NULL;
}


void* CUIBase::CreateCtrlImp( char* TypeName,void* Param )
{
	if(st_pImpFact)
		return st_pImpFact->CreateImp( TypeName, Param);
	return NULL;
}

System::Void CUIBase::_Load(System::Object^  sender, System::EventArgs^  e)
{
	char* ImpName = GetImpTypeName();
	if( ImpName == NULL )
		return;
	
	m_pCtrl=(CCtrlBase*)CreateCtrlImp( ImpName, GetImpParam() );
	if(m_pCtrl)
	{
		m_pCtrl->RegisterUICallBack( System::Runtime::InteropServices::Marshal::GetIUnknownForObject(this).ToPointer() , BaseUICallBack);
		if(m_pContextCom!=nullptr)
		{
			CContextCtrlImp* pContextImp = (CContextCtrlImp*)m_pContextCom->GetCoreCtrl();
			if(pContextImp)
				m_pCtrl->SetContext(pContextImp->GetContext());
		}
		_InitCtrl();
	}
}

System::Void CUIBase::_InitCtrl(void)
{}

CUIBase::~CUIBase(void)
{
	if(m_pCtrl)
	{
		m_pCtrl->RegisterUICallBack( System::Runtime::InteropServices::Marshal::GetIUnknownForObject(this).ToPointer() ,NULL);
		m_pCtrl->Destroy();
	}
	m_pCtrl=NULL;
}

void* CUIBase::GetCoreCtrl()
{
	return m_pCtrl;
}

void CUIBase::Update(void)
{
}

System::Void CUIBase::_KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
{
	MSG_INFO info;
	info._iinfo = e->KeyValue;
	sqr::CMsg msg(MGS_KEYMSG,MGT_KEY_DOWN,info);	
	if(m_pCtrl)
	{
		m_pCtrl->Collect(msg);
	}
}

System::Void CUIBase::_KeyUp( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
{
	MSG_INFO info;
	info._iinfo = e->KeyValue;
	sqr::CMsg msg(MGS_KEYMSG,MGT_KEY_UP,info);	
	if(m_pCtrl)
	{
		m_pCtrl->Collect(msg);
	}
}

System::Void CUIBase::WndProc(System::Windows::Forms::Message% m)
{
	if(m_pCtrl)
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source=MGS_NULL;
		type.sys_type=MGT_SYS_RUN;
		info._iinfo=0;
		
		//ÏûÏ¢·â×°
		switch(m.Msg)
		{
		case WM_KEYDOWN:
			{
				source=MGS_KEYMSG;
				type.key_type=MGT_KEY_DOWN;
				info._iinfo=m.WParam.ToInt32();    //ÐéÄâ¼üÂë
				break;
			}
		case WM_KEYUP:
			{
				source=MGS_KEYMSG;
				type.key_type=MGT_KEY_UP;
				info._iinfo=m.WParam.ToInt32();    //ÐéÄâ¼üÂë
				break;
			}
		case WM_MOUSEMOVE:
			{
				source=MGS_MOUSEMSG;
				type.mouse_type=MGT_MOUSE_MOVE;
				info.mouse_info.X=m.LParam.ToInt32()&0xffff; //µÍ16 X
				info.mouse_info.Y=m.LParam.ToInt32()>>16;    //¸ß16 Y
				break;
			}
		case WM_LBUTTONDOWN:
			{
				source=MGS_MOUSEMSG;
				type.mouse_type=MGT_MOUSE_LBNDOWN;
				info.mouse_info.X=m.LParam.ToInt32()&0xffff;
				info.mouse_info.Y=m.LParam.ToInt32()>>16;
				break;
			}
		case WM_LBUTTONUP:
			{
				source=MGS_MOUSEMSG;
				type.mouse_type=MGT_MOUSE_LBNUP;
				info.mouse_info.X=m.LParam.ToInt32()&0xffff;
				info.mouse_info.Y=m.LParam.ToInt32()>>16;
				break;
			}
		case WM_LBUTTONDBLCLK:
			{
				source=MGS_MOUSEMSG;
				type.mouse_type=MGT_MOUSE_LBNDCLK;
				info.mouse_info.X=m.LParam.ToInt32()&0xffff;
				info.mouse_info.Y=m.LParam.ToInt32()>>16;
				break;
			}
		case WM_RBUTTONDOWN:
			{
				source=MGS_MOUSEMSG;
				type.mouse_type=MGT_MOUSE_RBNDOWN;
				info.mouse_info.X=m.LParam.ToInt32()&0xffff;
				info.mouse_info.Y=m.LParam.ToInt32()>>16;
				break;
			}
		case WM_RBUTTONUP:
			{
				source=MGS_MOUSEMSG;
				type.mouse_type=MGT_MOUSE_RBNUP;
				info.mouse_info.X=m.LParam.ToInt32()&0xffff;
				info.mouse_info.Y=m.LParam.ToInt32()>>16;
				break;
			}
		case WM_RBUTTONDBLCLK:
			{
				source=MGS_MOUSEMSG;
				type.mouse_type=MGT_MOUSE_RBNDCLK;
				info.mouse_info.X=m.LParam.ToInt32()&0xffff;
				info.mouse_info.Y=m.LParam.ToInt32()>>16;
				break;
			}
		case WM_MBUTTONDOWN:
			{
				source=MGS_MOUSEMSG;
				type.mouse_type=MGT_MOUSE_MBNDOWN;
				info.mouse_info.X=m.LParam.ToInt32()&0xffff;
				info.mouse_info.Y=m.LParam.ToInt32()>>16;
				break;
			}
		case WM_MBUTTONUP:
			{
				source=MGS_MOUSEMSG;
				type.mouse_type=MGT_MOUSE_MBNUP;
				info.mouse_info.X=m.LParam.ToInt32()&0xffff;
				info.mouse_info.Y=m.LParam.ToInt32()>>16;
				break;
			}
		case WM_MBUTTONDBLCLK :
			{
				source=MGS_MOUSEMSG;
				type.mouse_type=MGT_MOUSE_MBNDCLK;
				info.mouse_info.X=m.LParam.ToInt32()&0xffff;
				info.mouse_info.Y=m.LParam.ToInt32()>>16;
				break;
			}
		case WM_MOUSEWHEEL:
			{
				source=MGS_MOUSEMSG;
				type.mouse_type=MGT_MOUSE_WHEEL;
				info._iinfo=(m.WParam.ToInt32()>>16)/60;
				break;
			}
		case WM_SIZE:
			{
				source=MGS_UIMSG;
				type.ui_type=MGT_UI_SIZE;
				info._iinfo=m.LParam.ToInt32();
				break;
			}
		case WM_CREATE:
			{
				source=MGS_UIMSG;
				type.ui_type=MGT_UI_CREATE;
				break;
			}
		case WM_CLOSE:
			{
				source=MGS_UIMSG;
				type.ui_type=MGT_UI_CLOSE;
				break;
			}
		case WM_PAINT:
			{
				source=MGS_UIMSG;
				type.ui_type=MGT_UI_PAINT;
				break;
			}
		case WM_QUIT:
			{
				source=MGS_UIMSG;
				type.ui_type=MGT_UI_QUIT;
				break;
			}
		}
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}

	System::Windows::Forms::UserControl::WndProc(m);
}

CContextComponent^ CUIBase::GetContextComponet( void )
{
	return m_pContextCom;
}