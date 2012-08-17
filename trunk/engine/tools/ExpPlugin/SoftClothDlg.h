#pragma once

#include "inode.h"

// CSoftClothDlg dialog
struct SoftClothExpProp{
	uint8   Width;
	uint8   Height;
	uint16  MassA;
	uint16  MassB;
	uint16  MassC;
	uint16  WindMin;
	uint16  WindMax;  
	uint16  WindMaxH;    
	float   Gravity;
	float   GassForce;
	float   CoefficientK;
	float   LongRatio;
	float   ShortRatio;
	float   TimePass;
    INode * MaxNode;
	int	HengXiang;
};

struct SoftClothExpPropSave{
	uint8   Width;
	uint8   Height;
	uint16  MassA;
	uint16  MassB;
	uint16  MassC;
	uint16  WindMin;
	uint16  WindMax;    
	uint16  WindMaxH;    
	float   Gravity;
	float   GassForce;
	float   CoefficientK;
	float   LongRatio;
	float   ShortRatio;
	float   TimePass;
};


class CSoftClothDlg : public CDialog
{
	DECLARE_DYNAMIC(CSoftClothDlg)

// Dialog Data
private:
    SoftClothExpProp * Exp;

public:
	CSoftClothDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSoftClothDlg();

// Dialog Data
	enum { IDD = 3005 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    virtual INT_PTR DoModal( SoftClothExpProp * Exp);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
