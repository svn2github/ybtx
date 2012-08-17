#include "StdAfx.h"
#include "AutoDataUintCtrl.h"

namespace sqr_tools
{
	void AutoDataUintCtrl::LayoutAutoDataCtrl(bool Hover, const char* title, CUIPropBase^ ctrl)
	{
		this->Controls->Add(ctrl);
		if (title)
		{
			PropTitlelabel->Text = gcnew System::String(title);
			PropTitlelabel->Show();

			if (Hover)
			{
				// ºáÏò
				ctrl->Location = System::Drawing::Point(120, 0);
				this->Size	= System::Drawing::Size(120 + ctrl->Width, ctrl->Height);
			}
			else
			{
				ctrl->Location = System::Drawing::Point(0, 20);
				this->Size	= System::Drawing::Size(ctrl->Width, 20 + ctrl->Height);
			}
		}
		else
		{
			PropTitlelabel->Hide();
			ctrl->Location = System::Drawing::Point(0, 0);
			this->Size	= ctrl->Size;
		}
	}
}
