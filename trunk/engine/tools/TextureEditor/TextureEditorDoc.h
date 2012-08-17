// TextureEditorDoc.h :  CTextureEditorDoc 类的接口
//

#include "texture.h"
#include "D3dd.h"
#pragma once

class CTextureEditorDoc : public CDocument
{
	CD3d*     m_D3d;
	CTexture  m_Text;
	float     m_Scale;

	CString   m_PathStr;

protected: // 仅从序列化创建
	CTextureEditorDoc();
	DECLARE_DYNCREATE(CTextureEditorDoc)

// 属性
public:
	float GetScale()                 { return m_Scale;  }
	void  SetScale( float Scale )    { m_Scale = Scale; }

// 操作
public:
	CPoint SetTexture();
	void   SetD3d( CD3d* D3d ){ m_D3d=D3d; }
	CString GetFormat();
	virtual void SetTitle(LPCTSTR lpszTitle);

// 重写
	public:
	virtual BOOL OnNewDocument();

// 实现
public:
	virtual ~CTextureEditorDoc();

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	afx_msg void OnFileAutobat();
	afx_msg void OnFileMaptrans();
	afx_msg void OnEditAddframe();
	afx_msg void OnExportTga();
	afx_msg void OnDDSConvert();
};


