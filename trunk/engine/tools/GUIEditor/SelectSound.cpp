// SelectSound.cpp : implementation file
//

#include "stdafx.h"
#include "GUIEditor.h"
#include "SelectSound.h"
#include "IAudioModule.h"
#include "CodeCvs.h"


// CSelectSound dialog

IMPLEMENT_DYNAMIC(CSelectSound, CDialog)

CSelectSound::CSelectSound(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectSound::IDD, pParent)
{
	m_bCancle = false;
}

CSelectSound::~CSelectSound()
{
}

void CSelectSound::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CUES, m_treeCues);
}


BEGIN_MESSAGE_MAP(CSelectSound, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CUES, &CSelectSound::OnTvnSelchangedTreeCues)
	ON_BN_CLICKED(IDC_OK, &CSelectSound::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CANCEL, &CSelectSound::OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL CSelectSound::OnInitDialog()
{
	::CDialog::OnInitDialog();

 	// init the tree with soundbank/cues
 	IAudioSystem* audioSystem = GetAudioModule()->GetAudioSystem();
 	if (NULL != audioSystem)
 	{
 		TV_INSERTSTRUCT treeItem;
 		HTREEITEM hTreeItem;
 		treeItem.hInsertAfter = TVI_LAST;
 		treeItem.item.mask = TVIF_TEXT | TVIF_PARAM;
 
 		size_t nSoundBankCnt = audioSystem->GetNumSoundBanks();
 		for (index_t i = 0; i < nSoundBankCnt; i++)
 		{
 			const wstring& name = gbk_to_utf16(audioSystem->GetSoundBankName(i));
 			treeItem.hParent = TVI_ROOT;
 			treeItem.item.pszText = const_cast<wchar_t*>(name.c_str());
 			treeItem.item.lParam = 1;
 			hTreeItem = m_treeCues.InsertItem(&treeItem);
 
 			size_t nSoundBankCueCnt = audioSystem->GetNumCues(i);
 			for (index_t j = 0; j < nSoundBankCueCnt; j++)
 			{
 				const wstring& cueName = gbk_to_utf16(audioSystem->GetCueName(i, j));
 				treeItem.hParent = hTreeItem;
 				treeItem.item.lParam = 10;
 				treeItem.item.pszText = const_cast<wchar_t*>(cueName.c_str());
 				HTREEITEM cueItem = m_treeCues.InsertItem(&treeItem);
  				if (m_strCurCueName == cueName)
  					m_treeCues.SelectItem(cueItem);
 			}
 		}
 	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CSelectSound message handlers

void CSelectSound::OnTvnSelchangedTreeCues(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_strCurCueName = m_treeCues.GetItemText(m_treeCues.GetSelectedItem());
	if (-1 != m_strCurCueName.find('/'))
		return;

	//if( m_strCurCueName != m_strLastCueName )
	{
		if( m_strLastCueName != L"" )
			StopCue(utf16_to_gbk(m_strLastCueName).c_str());

		PlayCue(utf16_to_gbk(m_strCurCueName).c_str());

		m_strLastCueName = m_strCurCueName;
	}

	*pResult = 0;
}

void CSelectSound::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_bCancle = false;
	this->EndDialog(0);
}

void CSelectSound::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_bCancle = true;
	this->EndDialog(0);
}

wstring CSelectSound::GetSelectSoundName() const
{
	if( m_bCancle )
		return L"";
	else
		return m_strCurCueName;
}