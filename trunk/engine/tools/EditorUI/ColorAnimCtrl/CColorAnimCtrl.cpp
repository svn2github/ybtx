#include "StdAfx.h"
#include "CColorAnimCtrl.h"
#include "ColorAnimCtrl/CColorAnimImp.h"

using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;
using namespace System::Windows::Forms;
using namespace System::Collections;
using namespace System::Collections::Generic;

namespace sqr_tools
{
	const int OFFSCREEN_MULTI   = 2; 
	const int CTRL_POINT_RADIUS = 3*OFFSCREEN_MULTI;

	Color nodeToColor( const ColorAnimNode& node )
	{
		DWORD argb = (BYTE(255) << 24) + (BYTE(255*node.r) << 16) + (BYTE(255*node.g) << 8) + BYTE(255*node.b);
		return Color::FromArgb(argb);
	}

	void getUIColorSize( Color& ui_color, PointF& ui_pos, const ColorAnimNode& node, float key, const RectangleF& rect )
	{
		ui_color = nodeToColor(node);
		ui_pos.X = (const_cast<RectangleF&>(rect).Width-CTRL_POINT_RADIUS*2) * key + CTRL_POINT_RADIUS;
		ui_pos.Y = (const_cast<RectangleF&>(rect).Height-CTRL_POINT_RADIUS*2) * (1.0f-node.a) + CTRL_POINT_RADIUS;
	}

	Color getInverseUIColor( const Color& ui_color )
	{
		Color& color = const_cast<Color&>(ui_color);
		return nodeToColor(ColorAnimNode(1.0f - color.R/255.0f, 1.0f - color.G/255.0f, 1.0f - color.B/255.0f, 1.0f));
	}

	Rectangle getCtrlPointRect( const PointF& point )
	{
		return Rectangle(int(const_cast<PointF&>(point).X-CTRL_POINT_RADIUS), int(const_cast<PointF&>(point).Y-CTRL_POINT_RADIUS), CTRL_POINT_RADIUS*2, CTRL_POINT_RADIUS*2);
	}

	char* CColorAnimCtrl::GetImpTypeName( void )
	{
		return GET_IMP_NAME(CColorAnimImp);
	}

	void* CColorAnimCtrl::GetImpParam( void )
	{
		return this->Handle.ToPointer();
	}

	void CColorAnimCtrl::Update( void )
	{
		CColorAnimImp* pImp = (CColorAnimImp*)GetCoreCtrl();
		OnDraw();
	}

	System::Void CColorAnimCtrl::OnPaint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
	{
		OnDraw();
	}

	System::Void CColorAnimCtrl::OnDoubleClick( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
	{
		if ( e->Button == System::Windows::Forms::MouseButtons::Left )
		{
			ColorDialog^ colorPickDlg = gcnew ColorDialog();
			colorPickDlg->FullOpen = true;
			colorPickDlg->AllowFullOpen = true;
			colorPickDlg->Color = Color::Black;
			if ( colorPickDlg->ShowDialog() == DialogResult::OK )
			{
				ColorAnimNode new_color(colorPickDlg->Color.R/255.0f, colorPickDlg->Color.G/255.0f, colorPickDlg->Color.B/255.0f, 1.0f);
				CColorAnimImp* pImp = (CColorAnimImp*)GetCoreCtrl();
				pImp->SetColorAtPos(new_color, e->X, e->Y);
			}
		}
	}

	System::Void CColorAnimCtrl::OnDraw()
	{
		CColorAnimImp* pImp = (CColorAnimImp*)GetCoreCtrl();
		if ( pImp)
		{
			int keyNum = pImp->GetKeyNum();	
			RectangleF offscreen_rect(0.0f, 0.0f, (float)this->ClientSize.Width*OFFSCREEN_MULTI, (float)this->ClientSize.Height*OFFSCREEN_MULTI);
			Bitmap^			bmp				= gcnew Bitmap((int)offscreen_rect.Width, (int)offscreen_rect.Height);
			Graphics^		back_buf		= Graphics::FromImage(bmp);
			Graphics^		graphic			= CreateGraphics();
			ColorBlend^		colorBlend		= gcnew ColorBlend(keyNum);
			List<Color>^	ui_color_list	= gcnew List<Color>();
			List<PointF>^	ui_pos_list		= gcnew List<PointF>();
			back_buf->SmoothingMode = SmoothingMode::HighQuality;

			vector<ColorAnimNode> node_list;
			node_list.resize(keyNum);
			for ( int i=0; i<keyNum; ++i )
			{
				float cur_key = 0;
				pImp->GetNode(node_list[i], cur_key, i);
				Color	cur_color;
				PointF	cur_pos;
				getUIColorSize(cur_color, cur_pos, node_list[i], cur_key, offscreen_rect);
				ui_color_list->Add(cur_color);
				ui_pos_list->Add(cur_pos);
				colorBlend->Colors[i] = cur_color;
				colorBlend->Positions[i] = cur_key;
			}

			for ( int i=0; i<keyNum; ++i )
			{
				if ( i > 0 )
				{
					float start = int(ui_pos_list[i-1].X+0.5f) - 0.5f;
					float end   = ui_pos_list[i].X;
					if ( i == 1 )
						start = 0.0f;
					if ( i == keyNum-1 )
						end = ui_pos_list[i].X + CTRL_POINT_RADIUS;
					if ( end - start < 1.0f )
						end = start+1.0f;
					RectangleF cur_rect(start, 0.0f, end-start, offscreen_rect.Height);
					LinearGradientBrush^ grad_brush = gcnew LinearGradientBrush(cur_rect, ui_color_list[i-1], ui_color_list[i], LinearGradientMode::Horizontal);
					back_buf->FillRectangle(grad_brush, cur_rect);
					delete grad_brush;
				}
			}

			int sel_idx = 0;
			pImp->GetSelPointIdx(sel_idx);
			Pen^		pen			= gcnew Pen(Color::White);
			SolidBrush^	solid_brush = gcnew SolidBrush(Color::White);
			for ( int i=0; i<keyNum; ++i )
			{
				// draw the control points and the lines between them.
				if ( i > 0 )
				{
					pen->Color = getInverseUIColor(ui_color_list[i-1]);
					back_buf->DrawLine(pen, ui_pos_list[i-1], ui_pos_list[i]);
				}

				solid_brush->Color = getInverseUIColor(ui_color_list[i]);
				if ( sel_idx == i )
					back_buf->FillRectangle(solid_brush, getCtrlPointRect(ui_pos_list[i]));
				else
				{
					pen->Color = getInverseUIColor(ui_color_list[i]);;
					back_buf->DrawRectangle(pen, getCtrlPointRect(ui_pos_list[i]));
				}

				// draw alpha value text.
				RectangleF text_rect(ui_pos_list[i].X, ui_pos_list[i].Y, offscreen_rect.Width, offscreen_rect.Height);
				float txt_width  = 20.0f*OFFSCREEN_MULTI;
				float txt_height = 16.0f*OFFSCREEN_MULTI;
				if ( text_rect.X > offscreen_rect.Width - txt_width )
					text_rect.X -= txt_width;
				if ( text_rect.Top > offscreen_rect.Height - txt_height )
					text_rect.Y -= txt_height;
				System::Drawing::Font^ font = gcnew System::Drawing::Font("Arial", 10*OFFSCREEN_MULTI, FontStyle::Regular, GraphicsUnit::Pixel);
				back_buf->DrawString(String::Format("{0}", int(node_list[i].a*255)), font, solid_brush, text_rect);
				delete font;
			}

			graphic->InterpolationMode = InterpolationMode::HighQualityBicubic;
			graphic->SmoothingMode = SmoothingMode::HighQuality;
			graphic->DrawImage(bmp, this->ClientRectangle, offscreen_rect, GraphicsUnit::Pixel);
			
			delete pen;
			delete solid_brush;
			delete bmp;
			delete graphic;
		}
	}
}
