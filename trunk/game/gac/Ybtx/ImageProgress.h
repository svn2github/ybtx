#pragma once


#include "ImageManage.h"


class CImageProgress : public CButton
{
public:
	CImageProgress();
	virtual ~CImageProgress();

	BOOL LoadProgressImage( wstring strBackImage, wstring strForeImage );
	BOOL LoadProgressImage(int BackImage, int ForeImage);
	void SetPos(int percent);

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	DECLARE_MESSAGE_MAP()

private:
	int    m_Pos;

	CImageManage     m_ForeImage;
	CImageManage     m_BackImage;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

