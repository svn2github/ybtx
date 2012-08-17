#include "StdAfx.h"
#include "CFrameScaleCtrl.h"
#include "FrameScaleCtrl/CFrameScaleCtrlImp.h"

#define		SLIDERHEIGHT 16
#define		SLIDERLENGHT 100
#define		GRIDSHORT    8
#define		GRIDLONG     18

#define FrameScaleCtrlImp ((CFrameScaleCtrlImp*)GetCoreCtrl())

void CFrameScaleCtrl::InitDrawEnv()
{
	this->pBursh		= gcnew System::Drawing::SolidBrush(Color::FromArgb(0xff000000));
	this->pPen			= gcnew System::Drawing::Pen(Color::FromArgb(0xff000000), 1.0f);
	this->pFont			= gcnew System::Drawing::Font("Arial", 9);

	System::Drawing::Rectangle rt = this->scale_label->ClientRectangle;
	this->pScaleBitMap	= gcnew System::Drawing::Bitmap(rt.Width, rt.Height);
	this->scale_label->BackgroundImage = this->pScaleBitMap;

	this->bLButtonDown	= false;
}

void CFrameScaleCtrl::ReleaseRes()
{
	if( this->pBursh )
		delete this->pBursh;

	if( this->pPen )
		delete this->pPen;

	if( this->pFont )
		delete this->pFont;

	if( this->pScaleBitMap )
		delete this->pScaleBitMap;
}

void CFrameScaleCtrl::DrawFrameScale()
{
	if (FrameScaleCtrlImp)
	{

		map<int, int> KeyFrame	= FrameScaleCtrlImp->GetKeyFrame();
		int FramesPerGrid		= FrameScaleCtrlImp->GetFramesPerGrid();
		int PixPerGrid			= FrameScaleCtrlImp->GetPixPerGrid();
		Int32 FrameNum			= FrameScaleCtrlImp->GetAniFrameNum();
		float CurFrame			= FrameScaleCtrlImp->GetUpdateFrame();
		float PixPerFrame		= (float)PixPerGrid/FramesPerGrid;

		//image
		System::Drawing::Rectangle rt = this->scale_label->ClientRectangle;
		if (rt.Width == 0 || rt.Height == 0)//出现rt.Width=0的情况，gcnew bitmap会报掉。
			return;

		System::Drawing::Bitmap^ tempBitMap =  gcnew System::Drawing::Bitmap(rt.Width, rt.Height);
		Graphics^ Gra = Graphics::FromImage(tempBitMap);

		// 画背景
		Gra->Clear(Color::FromArgb(0xffc8d0d4));

		//draw something
		int top = rt.Top + SLIDERHEIGHT;
		System::Drawing::Rectangle toprt;
		toprt.X = rt.X; toprt.Y = rt.Y; toprt.Width = rt.Width; toprt.Height = top;

		//////////////////////////////////////////////////////////////////////////
		//标尺上部分
		System::Drawing::SolidBrush^ pBrush = gcnew System::Drawing::SolidBrush(Color::FromArgb(0x80c0c0c0));
		Gra->FillRectangle(pBrush, rt);

		pBrush->Color = Color::FromArgb(0xa0a0a0a0);
		Gra->FillRectangle(pBrush, toprt);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		System::Drawing::Pen^ pPen = gcnew System::Drawing::Pen(Color::FromArgb(0xff000000), 1.0f);
		Gra->DrawRectangle(pPen, rt.X, rt.Y, rt.Width-1, rt.Height-1);

		pPen->Color = Color::FromArgb(0xa00000ff);
		Gra->DrawRectangle(pPen, toprt.X, toprt.Y, toprt.Width-1, toprt.Height);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//画关键帧
		pBrush->Color = Color::FromArgb(0xa000ff00);
		for( map< int, int >::iterator it = KeyFrame.begin(); it != KeyFrame.end(); ++it )
		{
			int x = (int)( it->first*PixPerFrame ) + SLIDERLENGHT/2;
			int y = top;
			int width  = (int)PixPerFrame;
			int height = GRIDLONG;
			Gra->FillRectangle(pBrush, x, y, width, height);
		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//画刻度
		int nNum = ( rt.Width - SLIDERLENGHT )/PixPerGrid + 1;
		for( int i = 0; i <= nNum; ++i )
		{
			int Length = GRIDSHORT;
			if( i%10 == 0 )
				Length = GRIDLONG;

			int start = i*PixPerGrid + SLIDERLENGHT/2;
			Point posStart = Point(start, toprt.Bottom);
			// end
			Point posEnd   = Point(start, toprt.Bottom + Length);

			Gra->DrawLine(pPen, posStart, posEnd );
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//标示帧号
		pBrush->Color = Color::FromArgb(0xffff0000);
		System::Drawing::Rectangle rtText;
		for( System::Int32 i = 0; i <= nNum; ++i )
		{
			//System::String^ sysStr1 =  i.ToString();
			if( i%10 == 0  )
			{
				System::String^ sysStr = System::Convert::ToString(i*FramesPerGrid); 
				rtText.X = i*PixPerGrid + SLIDERLENGHT/2+2;
				rtText.Y = toprt.Bottom;
				rtText.Width = GRIDLONG;
				rtText.Height = rt.Bottom - toprt.Bottom;

				Gra->DrawString(sysStr, this->pFont, pBrush, rtText );
				delete sysStr;
			}
		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//画指针
		int PosX = (long)( ( CurFrame / FramesPerGrid )*PixPerGrid ) + SLIDERLENGHT/2;
		int PosY = toprt.Bottom;
		pPen->Color = Color::FromArgb(0xa0ff0000);
		Point posStart = Point(PosX, PosY);
		// end
		Point posEnd   = Point(PosX, PosY + SLIDERHEIGHT - 6);
		Gra->DrawLine(pPen, posStart, posEnd );

		pPen->Color = Color::FromArgb(0xa0fefefe);
		Gra->DrawRectangle(pPen, PosX - 3, rt.Y + SLIDERHEIGHT - 2, 7, 16);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//滑块
		System::Drawing::Rectangle BlockRect = toprt;
		BlockRect.X = (long)( ( CurFrame / FramesPerGrid ) * PixPerGrid ) + 1;
		String^ buttonText = gcnew String("");
		Int32 iCurFrame = (int)CurFrame;
		buttonText = iCurFrame.ToString() + "/" + FrameNum.ToString();

		this->move_button->Left = BlockRect.X+2;
		this->move_button->Text = buttonText;
		this->move_button->Refresh();
		//////////////////////////////////////////////////////////////////////////

		this->scale_label->BackgroundImage = tempBitMap;
		delete this->pScaleBitMap;
		this->pScaleBitMap	= tempBitMap;
	}
}

void CFrameScaleCtrl::Update( void )
{
	DrawFrameScale();
}

System::Void CFrameScaleCtrl::scale_label_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	if (FrameScaleCtrlImp)
	{
		System::Windows::Forms::MouseButtons mbt = e->Button;
		if( mbt == System::Windows::Forms::MouseButtons::Left )
		{
			this->bLButtonDown = true;
			if( FrameScaleCtrlImp )
			{
				Point point = e->Location;
				float fCurPosX = point.X - SLIDERLENGHT/2.0f;
				FrameScaleCtrlImp->CalCurrentFrame(fCurPosX);
				DrawFrameScale();
			}
		}
	}
}

System::Void CFrameScaleCtrl::scale_label_MouseUp( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	System::Windows::Forms::MouseButtons mbt = e->Button;
	if( mbt == System::Windows::Forms::MouseButtons::Left )
	{
		this->bLButtonDown = false;
	}
}

System::Void CFrameScaleCtrl::scale_label_MouseMove( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	if (FrameScaleCtrlImp)
	{
		Point point = e->Location;
		if(/*::GetKeyState( 0x41)&0x8000 &&*/ (e->Button == System::Windows::Forms::MouseButtons::Left))
		{
			if( this->bLButtonDown )
			{
				float fCurPosX = point.X - SLIDERLENGHT/2.0f;
				FrameScaleCtrlImp->CalCurrentFrame(fCurPosX);
			}
		}
	}
}

char* CFrameScaleCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CFrameScaleCtrlImp);
}

System::Void sqr_tools::CFrameScaleCtrl::CFrameScaleCtrl_Load( System::Object^ sender, System::EventArgs^ e )
{
	if (FrameScaleCtrlImp)
	{
		FrameScaleCtrlImp->SetFrameScaleLen(this->scale_label->ClientRectangle.Width - 50);//临时
		FrameScaleCtrlImp->SetAniStartFrame(0);
		FrameScaleCtrlImp->SetAniEndFrame(0);
		DrawFrameScale();
	}
}

System::Void sqr_tools::CFrameScaleCtrl::PlayBtn_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (FrameScaleCtrlImp)
		FrameScaleCtrlImp->Play();
}

System::Void sqr_tools::CFrameScaleCtrl::StopBtn_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (FrameScaleCtrlImp)
		FrameScaleCtrlImp->Stop();
}

System::Void sqr_tools::CFrameScaleCtrl::CFrameScaleCtrl_Resize(System::Object^  sender, System::EventArgs^  e)
{
	if (FrameScaleCtrlImp)
	{
		FrameScaleCtrlImp->SetFrameScaleLen(this->scale_label->ClientRectangle.Width - 50);//临时
		FrameScaleCtrlImp->CalFramePerGrid();
	}
}
