
#include "stdafx.h"
#include "RegionManage.h"


#define ALLOC_UNIT 100


CRegionManage::CRegionManage()
{
}

CRegionManage::~CRegionManage()
{
}

HRGN CRegionManage::BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance)
{
	HRGN hRgn = NULL;
	BITMAP bm;
	HBITMAP holdBmp;

	if (hBmp)
	{
		HDC hMemDC = CreateCompatibleDC(NULL);
		if (hMemDC)
		{
			GetObject(hBmp, sizeof(BITMAP), &bm);

			//创建一个32位色的位图，并选进内存设备环境
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {
				sizeof(BITMAPINFOHEADER),  // biSize
				bm.bmWidth,  // biWidth;
				bm.bmHeight, // biHeight;
				1,           // biPlanes;
				32,          // biBitCount
				BI_RGB,      // biCompression;
				0,           // biSizeImage;
				0,           // biXPelsPerMeter;
				0,           // biYPelsPerMeter;
				0,           // biClrUsed;
				0            // biClrImportant;
			};
			VOID* pbits32;
			HBITMAP hbm32 = CreateDIBSection(hMemDC,(BITMAPINFO*)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

				// Create a DC just to copy the bitmap into the memory DC
				HDC hDC = CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						++bm32.bmWidthBytes;

					// Copy the bitmap into the memory DC
					HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
					BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
					DWORD maxRects = ALLOC_UNIT;
					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
					RGNDATA *pData = (RGNDATA*)GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE* p32 = (BYTE*)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (int y = 0; y < bm.bmHeight; ++y)
					{
						// Scan each bitmap pixel from left to right
						for (int x = 0; x < bm.bmWidth; ++x)
						{
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG* p = (LONG*)p32 + x;
							while (x < bm.bmWidth)
							{
								BYTE b = GetRValue(*p);
								if (b >= lr && b <= hr)
								{
									b = GetGValue(*p);
									if (b >= lg && b <= hg)
									{
										b = GetBValue(*p);
										if (b >= lb && b <= hb) // This pixel is "transparent"
											break;
									}
								}
								++p;
								++x;
							}

							if (x > x0) // Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
							{
								if (pData->rdh.nCount >= maxRects)
								{
									GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)GlobalLock(hData);
								}
								RECT* pr = (RECT*)&pData->Buffer;
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
								if ( x0 < pData->rdh.rcBound.left )
									pData->rdh.rcBound.left = x0;
								if ( y < pData->rdh.rcBound.top )
									pData->rdh.rcBound.top = y;
								if ( x > pData->rdh.rcBound.right )
									pData->rdh.rcBound.right = x;
								if ( y+1 > pData->rdh.rcBound.bottom )
									pData->rdh.rcBound.bottom = y+1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if (pData->rdh.nCount == 2000)
								{
									HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if (hRgn)
									{
										CombineRgn(hRgn, hRgn, h, RGN_OR);
										DeleteObject(h);
									}
									else
									{
										hRgn = h;
									}
									pData->rdh.nCount = 0;
									SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if (hRgn)
					{
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						DeleteObject(h);
					}
					else
					{
						hRgn = h;
					}

					// Clean up
					GlobalFree(hData);
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
				}

				DeleteObject(SelectObject(hMemDC, holdBmp));
			}

			DeleteDC(hMemDC);
		}
	} 

	return hRgn; 
}

