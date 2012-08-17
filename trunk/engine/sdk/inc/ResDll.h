// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RESDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RESDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef RESDLL_EXPORTS
#define RESDLL_API __declspec(dllexport)
#else
#define RESDLL_API __declspec(dllimport)
#endif

// This class is exported from the ResDll.dll
class RESDLL_API CResDll {
public:
	CResDll(void);
	// TODO: add your methods here.
};


HINSTANCE hInst = NULL;

extern RESDLL_API int nResDll;

RESDLL_API int fnResDll(void);

RESDLL_API HINSTANCE GetResDllHInstance(void);

RESDLL_API HWND ShowDlg(DLGPROC Proc);

RESDLL_API HWND GetListChild(HWND parentHWnd);
