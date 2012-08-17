#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Design;
#include "CContextCom.h"
//This is the base class of all the controls in Clr dll;

namespace sqr_tools
{
	class ICtrlImpFactory;
	public ref class CUIBase : public System::Windows::Forms::UserControl
	{
	public:
		CUIBase(void);
		~CUIBase(void);
	protected:	CCtrlBase*			m_pCtrl;
	private:	CContextComponent^	m_pContextCom;

	public:
		[Category("控件集属性"),Description("选择控件对应的Context"),TypeConverterAttribute(ReferenceConverter::typeid)]
		property CContextComponent^ ContextComponent 
		{
			CContextComponent^ get();		//{ return m_pContextCom;		}
			void set(CContextComponent^ val);//	{ m_pContextCom = val;		}
		}

	public:
		void*				GetCoreCtrl(void);
		CContextComponent^	GetContextComponet(void);
		virtual void	Update(void);
		virtual char*	GetImpTypeName(void){return 0;};
		virtual void*	GetImpParam(void);
	public:
		virtual System::Void WndProc(System::Windows::Forms::Message% m) override;
	protected: 
		virtual System::Void _Load(System::Object^  sender, System::EventArgs^  e);
		virtual System::Void _InitCtrl(void);

	public:		System::Void _KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	public:		System::Void _KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	public:
		static	System::Void Initialize(void* ImpFactory);
		static	void*	CreateCtrlImp(char* TypeName,void* Param);
	private:
		static	ICtrlImpFactory*	st_pImpFact = NULL;
	};

}