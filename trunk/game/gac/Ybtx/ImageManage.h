#pragma once


class CImageManage
{
public:
	CImageManage();
	~CImageManage();

	void LoadImage(const wchar_t* ImagePath);
	void LoadImage(int ImageID);
	static void SetMultiLang(const wchar_t* wszCurLan, const wchar_t* wszOriLan);
	static wstring& GetCurLan() {return ms_wstrCurLan;}
	static wstring& GetOriLan() {return ms_wstrOriLan;}

	HDC GetMemDC();
	HBITMAP GetBitmapHandle();
	BITMAP GetBitmapInfo();

private:
	HDC     m_hMemDC;
	BITMAP  m_bm;
	CBitmap m_cBmp;

	static wstring ms_wstrCurLan;
	static wstring ms_wstrOriLan;
};

extern wstring&   replace_all_distinct(wstring&   str,const   wstring&   old_value,const   wstring&   new_value,wstring::size_type pos);


