#pragma once

class CGUICheckBox;
class CSharedTexture;

class CTexturedBox : public CButton
{
	DECLARE_DYNAMIC(CTexturedBox)

private:
	CGUICheckBox * m_pNode;

	CSharedTexture * m_pTexture;

	DWORD m_id;

public:
	CTexturedBox(DWORD id);
	virtual ~CTexturedBox();
	static CTexturedBox * ms_pHot;
protected:
	DECLARE_MESSAGE_MAP()
public:
	void Init(DWORD dwWidth, DWORD dwHeight, const char * strTex);
	void ResetTexture(const char * strTex);
	void SetHot(bool b);
	void SetMove(bool b);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};