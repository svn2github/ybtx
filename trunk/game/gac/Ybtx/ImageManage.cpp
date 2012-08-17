
#include "stdafx.h"
#include "ImageManage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

wstring CImageManage::ms_wstrCurLan;
wstring CImageManage::ms_wstrOriLan;

wstring&   replace_all_distinct(wstring&   str,const   wstring&   old_value,const   wstring&   new_value,wstring::size_type pos)   
{   
	for( pos;   pos!=wstring::npos;   pos+=new_value.length())  
	{   
		if( (pos=str.find(old_value,pos)) != wstring::npos )   
			str.replace(pos,old_value.length(),new_value);   
		else
			break;   
	}   
	return   str;   
}

CImageManage::CImageManage()
: m_hMemDC(NULL)
{
}


CImageManage::~CImageManage()
{
}


void CImageManage::SetMultiLang(const wchar_t* wszCurLan, const wchar_t* wszOriLan)
{
	ms_wstrCurLan = wszCurLan;
	ms_wstrOriLan = wszOriLan;
}

void CImageManage::LoadImage(const wchar_t* ImagePath)
{
	if (m_hMemDC == NULL)
		m_hMemDC = CreateCompatibleDC(NULL);

	HBITMAP hBmp = (HBITMAP)::LoadImage(NULL, ImagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if(!hBmp)
	{
		wstring strOriImagePath = ImagePath;
		replace_all_distinct(strOriImagePath, ms_wstrCurLan, ms_wstrOriLan, 0);
		hBmp = (HBITMAP)::LoadImage(NULL, strOriImagePath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	m_cBmp.DeleteObject();
	m_cBmp.Attach(hBmp);

	GetObject(hBmp, sizeof(BITMAP), &m_bm);

	//BITMAPINFOHEADER RGB32BITSBITMAPINFO = {
	//	sizeof(BITMAPINFOHEADER),  // biSize
	//	m_bm.bmWidth,  // biWidth;
	//	m_bm.bmHeight, // biHeight;
	//	1,           // biPlanes;
	//	32,          // biBitCount
	//	BI_RGB,      // biCompression;
	//	0,           // biSizeImage;
	//	0,           // biXPelsPerMeter;
	//	0,           // biYPelsPerMeter;
	//	0,           // biClrUsed;
	//	0            // biClrImportant;
	//};

	//VOID* pbits32; 
	//HBITMAP hbm32 = CreateDIBSection(m_hMemDC,(BITMAPINFO*)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
}

void CImageManage::LoadImage(int ImageID)
{
	if (m_hMemDC == NULL)
		m_hMemDC = CreateCompatibleDC(NULL);

	m_cBmp.DeleteObject();
	m_cBmp.LoadBitmap(ImageID);
	HBITMAP hBmp = (HBITMAP)m_cBmp.GetSafeHandle();

	GetObject(hBmp, sizeof(BITMAP), &m_bm);

	//BITMAPINFOHEADER RGB32BITSBITMAPINFO = {
	//	sizeof(BITMAPINFOHEADER),  // biSize
	//	m_bm.bmWidth,  // biWidth;
	//	m_bm.bmHeight, // biHeight;
	//	1,           // biPlanes;
	//	32,          // biBitCount
	//	BI_RGB,      // biCompression;
	//	0,           // biSizeImage;
	//	0,           // biXPelsPerMeter;
	//	0,           // biYPelsPerMeter;
	//	0,           // biClrUsed;
	//	0            // biClrImportant;
	//};

	//VOID* pbits32; 
	//HBITMAP hbm32 = CreateDIBSection(m_hMemDC,(BITMAPINFO*)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
}


HDC CImageManage::GetMemDC()
{
	return m_hMemDC;
}


HBITMAP CImageManage::GetBitmapHandle()
{
	return (HBITMAP)m_cBmp.GetSafeHandle();
}

BITMAP CImageManage::GetBitmapInfo()
{
	return m_bm;
}

