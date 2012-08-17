#include "stdafx.h"
#include "ClrCmd.h"
#include "CUIBase.h"
#include "CCallBackCmd.h"

void ExecuteCall(void* pCmd)
{
	IntPtr ptr = (IntPtr)pCmd;
	ClrCmd^ cmd = (ClrCmd^)System::Runtime::InteropServices::Marshal::GetObjectForIUnknown(ptr);	
	if(cmd)
		cmd->_Execute();
}

void UnExecuteCall(void* pCmd)
{
	IntPtr ptr = (IntPtr)pCmd;
	ClrCmd^ cmd = (ClrCmd^)System::Runtime::InteropServices::Marshal::GetObjectForIUnknown(ptr);	
	if(cmd)
		cmd->_UnExecute();
}


ClrCmd::ClrCmd(void)
{
	m_pCallBackCmd = static_cast<CCallBackCmd*>((CCtrlBase*)CUIBase::CreateCtrlImp(GET_IMP_NAME(CCallBackCmd),System::Runtime::InteropServices::Marshal::GetIUnknownForObject(this).ToPointer()));
	m_pCallBackCmd->SetCallBack(ExecuteCall,UnExecuteCall);
}

ClrCmd::~ClrCmd()
{
	m_pCallBackCmd->Destroy();
}

void	ClrCmd::Execute(void)
{
	m_pCallBackCmd->Execute();
}

void ClrCmd::_Execute(void)
{

}

void ClrCmd::_UnExecute(void)
{

}


void ClrState::ActiveState(void)
{

}

void ClrState::_Execute(void)
{
	ActiveState();
}

void ClrState::_UnExecute(void)
{
	ActiveState();
}