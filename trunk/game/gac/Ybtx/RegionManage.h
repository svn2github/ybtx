#pragma once


class CRegionManage
{
public:
	CRegionManage();
	~CRegionManage();
	HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance);
};

