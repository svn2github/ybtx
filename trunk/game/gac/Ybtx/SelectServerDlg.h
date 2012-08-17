#pragma once

#include "ImageManage.h"
#include "ImageButton.h"
#include "ImageListCtrl.h"
#include <map>
#include <hash_map>
#include <string>

using namespace stdext;
using namespace std;

struct CServerInfo
{
	string		server;
	string		version;
	int			icon;
	COLORREF	color;
	wstring		tip;
	string		param;
	string		name;
};

class CSelectServerDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectServerDlg)

public:
	CSelectServerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectServerDlg();
	void InitServerGroupList(wstring wstrServerListPath);
	void InitServerList(const wstring strSelectedServerGroup);
	void ReadSelectedServerStr();
	void SaveSelectServer();
	wstring GetServerName();
	const CServerInfo* GetServerInfo();

// Dialog Datai
private:
	enum { IDD = IDD_SELECTSERVER_DIALOG };
	CImageManage m_BGImage;

	CImageListCtrl m_ServerGroupListCtrl;
	CImageListCtrl m_ServerItemListCtrl;

	CImageButton m_SelectOKBtn;
	CImageButton m_SelectCancelBtn;

	wstring m_strSelectedServerGroup;
	wstring m_strSelectedServerGroupTemp;
	wstring m_strSelectedServer;
	
	typedef vector<pair<wstring,CServerInfo>> ServerInfoVector;
	typedef vector<pair<wstring,ServerInfoVector>> ServerGroupInfoVector;

	ServerGroupInfoVector m_VectorServerGroupInfo;

private:
	void ReviseServerName();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnNMClickListServerGroup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSelectCancel();
	afx_msg void OnBnClickedButtonSelectOK();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNMDBLCLKListServer(NMHDR *pNMHDR, LRESULT *pResult);
};
