#include "stdafx.h"
#include "CWordInterface.h"

#pragma warning(disable:4996)

bool CWordInterface::ConvertSCTC(LPCTSTR szSrc, TCHAR*& szTgt, size_t& uSize)
{
	m_wordSelection.TypeText(szSrc);

	HRESULT hr;
	//OLECHAR FAR* szMethod[4];
	//DISPID dispid[4];
	//szMethod[0] = OLESTR("ToolsSCTCTranslate");
	//szMethod[1] = OLESTR("Direction");
	//szMethod[2] = OLESTR("Varients");
	//szMethod[3] = OLESTR("TranslateCommon");
	//m_wordBasic = m_wordApp.get_WordBasic();
	//hr = m_wordBasic.m_lpDispatch->GetIDsOfNames(IID_NULL, szMethod, 4, LOCALE_USER_DEFAULT, dispid);

	//VARIANT vArgs[3];
	//DISPPARAMS dp;
	//dp.cArgs = 3;
	//dp.cNamedArgs = 3;
	//dp.rgvarg = vArgs;
	//dp.rgdispidNamedArgs = &(dispid[1]);

	//for(size_t i x= 0; i < 3; ++i)
	//{
	//	vArgs[i].vt = VT_I2;
	//	vArgs[i].iVal = 0;
	//}

	//hr = m_wordBasic.m_lpDispatch->Invoke(dispid[0], IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dp, NULL, NULL, NULL);
	hr = m_wordBasic.m_lpDispatch->Invoke(m_dispid[0], IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &m_dp, NULL, NULL, NULL);

	//COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	//m_wordDoc.PrintOut(COleVariant((short)false), vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt,
	//	vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt);

	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	m_wordSelection.WholeStory();
	CString& csResult = m_wordSelection.get_Text();
	m_wordSelection.Delete(vOpt, vOpt);
	uSize = csResult.GetLength();
	//csResult = csResult.Left(uSize -1);
	szTgt = new TCHAR[uSize];
	strncpy(szTgt, csResult, uSize);
	szTgt[uSize - 1] = '\0';
	
	return true;
}

CWordInterface::CWordInterface()
{
	if(!(m_wordApp.CreateDispatch("Word.Application", NULL)))
	{
		AfxMessageBox("启动Word失败，请检查是否安装了Office", MB_SETFOREGROUND);
		return;
	}
	m_wordDocs = m_wordApp.get_Documents();
	//wordDoc = wordDocs.Open(COleVariant("C))

	//wordDocs.Add()

	CComVariant tp1(_T("")), Visible, DocType(0), NewTemplate(false);
	m_wordDoc = m_wordDocs.Add(&tp1, &NewTemplate, &DocType, &Visible);

	m_wordSelection = m_wordApp.get_Selection();



	HRESULT hr;
	m_szMethod[0] = OLESTR("ToolsSCTCTranslate");
	m_szMethod[1] = OLESTR("Direction");
	m_szMethod[2] = OLESTR("Varients");
	m_szMethod[3] = OLESTR("TranslateCommon");
	m_wordBasic = m_wordApp.get_WordBasic();
	hr = m_wordBasic.m_lpDispatch->GetIDsOfNames(IID_NULL, m_szMethod, 4, LOCALE_USER_DEFAULT, m_dispid);

	m_dp.cArgs = 3;
	m_dp.cNamedArgs = 3;
	m_dp.rgvarg = m_vArgs;
	m_dp.rgdispidNamedArgs = &(m_dispid[1]);

	for(size_t i = 0; i < 3; ++i)
	{
		m_vArgs[i].vt = VT_I2;
		m_vArgs[i].iVal = 0;
	}

}

void CWordInterface::Release()
{
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	m_wordDoc.Close(COleVariant((short)false), vOpt, vOpt);
	m_wordApp.Quit(COleVariant((short)false), vOpt, vOpt);
	m_wordApp.ReleaseDispatch();
	m_wordDocs.ReleaseDispatch();
	m_wordDoc.ReleaseDispatch();
	m_wordSelection.ReleaseDispatch();
	m_wordBasic.ReleaseDispatch();
}

CWordInterface::~CWordInterface()
{
}

