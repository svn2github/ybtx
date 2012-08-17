#pragma once


#include "ImageManage.h"


enum BtnState {
	eBS_NORMAL,
	eBS_DISABLE,
	eBS_HOVER,
	eBS_LEAVE,
	eBS_CLICK,
};


#define  STATE_NUM  2


class CImageButton : public CButton
{
public:
	// 如果覆盖了构造函数就不能把drawitem事件注册过去
	CImageButton();
	virtual ~CImageButton();

	void LoadButtonImage( wstring strImageFile );
	void LoadHoverImage( wstring strImageFile );
	void LoadDisableImage( wstring strImageFile );
	void LoadClickImage( wstring strImageFile );

	void LoadButtonImage(int ImageFile);
	void LoadHoverImage(int ImageFile);
	void LoadDisableImage(int ImageFile);
	void LoadClickImage(int ImageFile);
	void SetState(BtnState state);
	BtnState GetState();

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	BtnState  m_state;
	bool      m_enable;

	CImageManage     m_NormalImage;
	CImageManage     m_HoverImage;
	CImageManage     m_DisableImage;
	CImageManage     m_ClickImage;

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

