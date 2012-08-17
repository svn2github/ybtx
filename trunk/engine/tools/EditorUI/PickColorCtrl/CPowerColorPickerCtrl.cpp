#include "StdAfx.h"
#include "CPowerColorPickerCtrl.h"
#include "PickColorCtrl/PowerColorPickerImp.h"

#define DEFAULT_LUMINANCE				0.50f

void CPowerColorPickerCtrl::Update( void )
{
}

char* CPowerColorPickerCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CPowerColorPickerImp);
}

void CPowerColorPickerCtrl::GenerateColorPicker()
{
	System::Drawing::Size size = this->colorPicker_label->Size;
	System::Drawing::Bitmap^ tempBitMap =  gcnew System::Drawing::Bitmap(size.Width, size.Height);
	Graphics^ Gra = Graphics::FromImage(tempBitMap);
	// »­±³¾°
	Gra->Clear(Color::FromArgb(0xffc8d0d4));

	double dWidthDivisor  = 1.0f / size.Width;
	double dHeightDivisor = 1.0f / size.Height;
	for ( int i= 0; i < size.Width; ++i )
	{
		for ( int j=0; j < size.Height; ++j )
		{
			 Drawing::Color color = this->HLStoRGB(j * dHeightDivisor, DEFAULT_LUMINANCE, i * dWidthDivisor);
			 tempBitMap->SetPixel(i,j,color);
		}
	}

	this->colorPicker_label->BackgroundImage = tempBitMap;
	//this->colorPicker_label->Visible = false;
}

void CPowerColorPickerCtrl::GenerateLuminanceBar()
{
	System::Drawing::Size size = this->luminance_label->Size;
	System::Drawing::Bitmap^ tempBitMap =  gcnew System::Drawing::Bitmap(size.Width, size.Height);
	Graphics^ Gra = Graphics::FromImage(tempBitMap);
	Gra->Clear(Color::FromArgb(0xffff));
	for (int i= 0; i<size.Height; i++)
	{
		for (int j = 0; j < size.Width; j++)
		{
			Drawing::Color color = HLStoRGB(currentHue, (double)i/(double)size.Height, currentSaturation);
			tempBitMap->SetPixel(j,i,color);
		}
	}

	this->luminance_label->BackgroundImage = tempBitMap;
}

void CPowerColorPickerCtrl::UpdateFinaleColorBar()
{
	this->finaleColor_label->BackColor = currentPickColor;
}

double CPowerColorPickerCtrl::HuetoRGB( double m1, double m2, double h )
{
	if( h < 0 ) h += 1.0;
	if( h > 1 ) h -= 1.0;
	if( 6.0*h < 1 )
		return (m1+(m2-m1)*h*6.0);
	if( 2.0*h < 1 )
		return m2;
	if( 3.0*h < 2.0 )
		return (m1+(m2-m1)*((2.0/3.0)-h)*6.0);
	return m1;
}

Drawing::Color CPowerColorPickerCtrl::HLStoRGB( double H, double L, double S )
{
	double r,g,b;
	double m1, m2;

	if(S==0) {
		r=g=b=L;
	} else {
		if(L <=0.5)
			m2 = L*(1.0+S);
		else
			m2 = L+S-L*S;
		m1 = 2.0*L-m2;
		r = HuetoRGB(m1,m2,H+1.0/3.0);
		g = HuetoRGB(m1,m2,H);
		b = HuetoRGB(m1,m2,H-1.0/3.0);
	}
	
	if( r > 1.0f )
		r = 1.0f;
	if( r < 0.0f )
		r = 0.0f;

	if( g > 1.0f )
		g = 1.0f;
	if( g < 0.0f )
		g = 0.0f;

	if( b > 1.0f )
		b = 1.0f;
	if( b < 0.0f )
		b = 0.0f;

	Drawing::Color color = Color::FromArgb((int)(r*255), (int)(g*255), (int)(b*255));
	return color;
}

void CPowerColorPickerCtrl::RGBtoHSL( Drawing::Color rgb, double *H, double *S, double *L )
{
	double delta;
	double r = (double)rgb.R/255;
	double g = (double)rgb.G/255;
	double b = (double)rgb.B/255;   
	double cmax = System::Math::Max(r, System::Math::Max(g,b));
	double cmin = System::Math::Min(r, System::Math::Min(g,b));   
	*L=(cmax+cmin)/2.0;   

	if(cmax==cmin) 
	{
		*S = 0;      
		*H = 0; // it's really undefined   
	} else 
	{
		if(*L < 0.5) 
			*S = (cmax-cmin)/(cmax+cmin);      
		else
			*S = (cmax-cmin)/(2.0-cmax-cmin);      

		delta = cmax - cmin;
		if(r==cmax) 
			*H = (g-b)/delta;      
		else if(g==cmax)
			*H = 2.0 +(b-r)/delta;
		else          
			*H=4.0+(r-g)/delta;
		*H /= 6.0; 
		if(*H < 0.0)
			*H += 1;  
	}
}

System::Void CPowerColorPickerCtrl::colorPicker_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	System::Windows::Forms::MouseButtons mbt = e->Button;
	if( mbt == System::Windows::Forms::MouseButtons::Left )
	{
		this->bPickerColorButtonDown = true;
	}
}

System::Void CPowerColorPickerCtrl::colorPicker_MouseUp( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	System::Windows::Forms::MouseButtons mbt = e->Button;
	if( mbt == System::Windows::Forms::MouseButtons::Left )
	{
		this->bPickerColorButtonDown = false;
	}
}

System::Void CPowerColorPickerCtrl::colorPicker_MouseMove( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	if((e->Button == System::Windows::Forms::MouseButtons::Left) && this->bPickerColorButtonDown)
	{
		curColorPickerPoint = e->Location;
		System::Drawing::Size size = this->colorPicker_label->Size;

		currentSaturation = (double)curColorPickerPoint.X / (double)size.Width;
		currentHue = (double)curColorPickerPoint.Y / (double)size.Height;
		currentPickColor = this->HLStoRGB(currentHue,currentLuminance,currentSaturation);

		this->UpdateFinaleColorBar();
		this->finaleColor_label->Refresh();

		this->GenerateLuminanceBar();
		this->luminance_label->Refresh();

		this->colorPicker_label->Refresh();

		this->SetCurPickerColorToImp();
	}
}

System::Void CPowerColorPickerCtrl::colorPicker_label_Paint( System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e )
{
	System::Drawing::Size size;
	System::Drawing::Graphics^ Gra = e->Graphics;

	Gra->DrawRectangle(pPen, curColorPickerPoint.X, curColorPickerPoint.Y, 6, 6);
}

System::Void CPowerColorPickerCtrl::luminanceBar_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	System::Windows::Forms::MouseButtons mbt = e->Button;
	if( mbt == System::Windows::Forms::MouseButtons::Left )
	{
		this->bLuminanceButtonDown = true;
	}
}

System::Void CPowerColorPickerCtrl::luminanceBar_MouseUp( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	System::Windows::Forms::MouseButtons mbt = e->Button;
	if( mbt == System::Windows::Forms::MouseButtons::Left )
	{
		this->bLuminanceButtonDown = false;
	}
}

System::Void CPowerColorPickerCtrl::luminanceBar_MouseMove( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	if((e->Button == System::Windows::Forms::MouseButtons::Left) && this->bLuminanceButtonDown)
	{
		curLuminancePoint = e->Location;

		currentLuminance = ((double)curLuminancePoint.Y)/((double)this->luminance_label->Size.Height);
		currentPickColor = HLStoRGB(currentHue,currentLuminance,currentSaturation);

		this->UpdateFinaleColorBar();
		this->finaleColor_label->Refresh();
		this->luminance_label->Refresh();

		this->SetCurPickerColorToImp();
	}
}

System::Void CPowerColorPickerCtrl::luminance_label_Paint( System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e )
{
	System::Drawing::Size size;
	System::Drawing::Graphics^ Gra = e->Graphics;

	Gra->DrawRectangle(pPen, 0, curLuminancePoint.Y, this->luminance_label->Size.Width, 6);
}

void CPowerColorPickerCtrl::SetCurPickerColorToImp()
{
	CPowerColorPickerImp *pImp = (CPowerColorPickerImp*)m_pCtrl;
	if( pImp )
	{
		pImp->SetCurPickColor(this->currentPickColor.R, this->currentPickColor.G, this->currentPickColor.B);
	}
}