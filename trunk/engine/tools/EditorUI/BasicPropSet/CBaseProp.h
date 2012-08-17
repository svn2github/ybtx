#pragma once
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;

namespace sqr_tools 
{

	class CtrlBase;
	public ref class CBaseProp
	{
	public:
		CBaseProp(void* ctrl);
		~CBaseProp(void);
	public: 	
		void*		GetCoreCtrl(void);
		virtual	void UpdateProp(){};		//属性全部更新
		virtual	void UpdateTimerProp(){};	//属性部分更新(实时属性)
		virtual float ShowModify(float value);
	protected:	
		void*		m_pCtrl;
	};

//!namespace
}
