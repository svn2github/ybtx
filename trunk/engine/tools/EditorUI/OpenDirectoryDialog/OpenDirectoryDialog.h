#pragma once
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace sqr_tools 
{
	public ref class OpenDirectoryDialog
	{
		public:OpenDirectoryDialog(void)
		  {
			   dlg.CheckFileExists = false;
			   dlg.FileName = L"文件夹";
			   dlg.Filter = L"文件夹|*.";
			   dlg.Title = L"选择文件夹";
			   dlg.ValidateNames = false;
			   dlg.FileOk += gcnew System::ComponentModel::CancelEventHandler(this,&OpenDirectoryDialog::OpenDirectory_FileOk);
		   }

		protected: System::Void OpenDirectory_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			String^ tmp = dlg.FileName;
			tmp = tmp->Replace('/','\\');
			dlg.FileName  = tmp->Substring(0,tmp->LastIndexOf('\\') + 1);
		}

		public: OpenFileDialog^ operator->()
		{
			return %dlg;
		}
	
	protected:
		OpenFileDialog dlg;
	};
}